#include "MagneticStrip.h"            // Header del driver (declara la API y tipos como pin_t)
#include <stdbool.h>                  // Tipos bool, true, false
#include <stdint.h>                   // Tipos enteros de ancho fijo (uint8_t, etc.)



/* ========================= Constantes ====================== */
#define CHAR_BITS       5u            // Cantidad de bits por carácter en Track 2
#define BUFFER_LEN      40u           // Tamaño del buffer de lectura cruda (caracteres)
#define PAN_MAX_LEN     19u           // Máximo de dígitos del PAN según Track 2
#define AD_CHARS_MIN    7u            // Mínimo de caracteres de datos AD (fecha vencimiento + codigo seguridad)
#define NUM_DATA_LEN    37u           // Largo máximo total de datos numéricos en Track 2 (sin SS, ES, LRC)
#define FS_LEN          1u            // Largo del FS ('=')
#define ID_DIGITS		8u

#define SS_CHAR         ';'           // Start Sentinel (SS) de Track 2
#define FS_CHAR         '='           // Field Separator (FS) de Track 2
#define ES_CHAR         '?'           // End Sentinel (ES) de Track 2
#define ES_BITS         0b11111       // Patrón de 5 bits que mapea a '?' (ES) en Track 2

/* Tabla Track-2 (5 bits -> ASCII)
   Se usa durante la ISR para mapear el char de 5 bits
   a su carácter ASCII correspondiente en Track 2. */
#define BITS_TO_CHAR(b) ((b == 0b10000) ? '0' : \
                        ((b == 0b00001) ? '1' : \
                        ((b == 0b00010) ? '2' : \
                        ((b == 0b10011) ? '3' : \
                        ((b == 0b00100) ? '4' : \
                        ((b == 0b10101) ? '5' : \
                        ((b == 0b10110) ? '6' : \
                        ((b == 0b00111) ? '7' : \
                        ((b == 0b01000) ? '8' : \
                        ((b == 0b11001) ? '9' : \
                        ((b == 0b11010) ? ':' : \
                        ((b == 0b01011) ? ';' : \
                        ((b == 0b11100) ? '<' : \
                        ((b == 0b01101) ? '=' : \
                        ((b == 0b01110) ? '>' : \
                        ((b == 0b11111) ? '?' : '\0' \
                        ))))))))))))))))

/* ========================= Pines (se cargan en init) ========================= */
static pin_t pinData, pinClock, pinEnable;  // Pines de DATA, CLOCK y ENABLE del lector
enum { START, CHAR, END }; // Estados del parser

/* ========================= Estado y buffers ========================= */
static volatile bool  swiping   = false;   // true mientras la tarjeta está presente (EN en bajo)
static volatile bool  id_ready  = false;   // true cuando terminó la pasada y hay datos para parsear

static char    data_buffer[BUFFER_LEN]; // Buffer de caracteres crudos decodificados por la ISR
static uint8_t data_buffer_pos;         // Posición de escritura en data_buffer
static uint8_t lrc_check;                  // Acumulador de LRC (XOR de bytes de 5 bits)
static bool    bit_buffer_flag;            // Señal para reiniciar ensamblado de char en próximo CLK

static char    pan_digits[PAN_MAX_LEN]; // Buffer de salida (PAN limpio en ASCII, hasta 19 dígitos)
static uint8_t pan_digits_len;          // Largo válido en pan_digits

/* ========================= Prototipos locales ========================= */
static void isr_clk(void*);                 // ISR de reloj (flanco descendente)
static void isr_en(void*);                  // ISR de enable (ambos flancos)
static void decoder_buffer(char* out_buf, uint8_t* out_len); // Parser fuera de ISR

/* ========================= API ========================= */
void bandaMag_init(pin_t pin_data, pin_t pin_clk, pin_t pin_en)  // Inicializa el driver con los pines
{
    pinData   = pin_data;                 // Guarda pin de DATA
    pinClock  = pin_clk;                  // Guarda pin de CLOCK
    pinEnable = pin_en;                   // Guarda pin de ENABLE

    gpioMode(pinData,   INPUT_PULLUP);    // DATA como entrada con pull-up (activo en bajo)
    gpioMode(pinClock,  INPUT_PULLUP);    // CLOCK como entrada con pull-up
    gpioMode(pinEnable, INPUT_PULLUP);    // ENABLE como entrada con pull-up

    // Registra interrupciones: CLK en flanco descendente, EN en ambos
    gpioSetupISR(pinClock,  FLAG_INT_NEGEDGE, &isr_clk, 0); // Llama isr_clk en bajada de CLOCK
    gpioSetupISR(pinEnable, FLAG_INT_EDGE,    &isr_en, 0); // Llama isr_en en subida y bajada de ENABLE

    bandaMag_reset();                     // Deja el estado interno limpio
}

bool bandaMag_getID(char* out8)           // Devuelve true si entrega 8 dígitos del PAN en out8 + '\0'
{
    if (!id_ready)                        // Si todavía no hay datos listos para parsear
    {
        return false;                     // No hay ID disponible
    }

    // Ejecuta parseo completo (SS→PAN→AD→ES→LRC) a pan_digits/pan_digits_len
    decoder_buffer(pan_digits, &pan_digits_len);

    // Exige al menos 8 dígitos de PAN válidos
    if (pan_digits_len < ID_DIGITS)
    {
        bandaMag_reset();                 // Si no alcanza, resetea estado
        return false;                     // Y reporta falla
    }

    // Copia exactamente los primeros 8 dígitos en ASCII
    for (uint8_t i = 0; i < ID_DIGITS; i++)
    {
        out8[i] = pan_digits[i];          // Copia dígito i
    }
    out8[ID_DIGITS] = '\0';               // Cierra con terminador de cadena

    bandaMag_reset();                     // Limpia estado para próxima pasada
    return true;                          // Éxito
}

void bandaMag_reset(void)                  // Resetea banderas y acumuladores internos
{
    swiping  = false;                      // No estamos en pasada
    id_ready = false;                      // No hay ID listo
    data_buffer_pos = 0u;                  // Vacía posición del buffer de lectura
    lrc_check          = 0u;               // Reinicia acumulador de LRC
    bit_buffer_flag    = true;             // Fuerza reinicio de ensamblado en próximo CLK
}

/* ========================= ISR: ENABLE (ambos flancos) ====================== */
static void isr_en(void*)                   // Maneja inicio/fin de pasada por cambios en ENABLE
{
    bool en_level = gpioRead(pinEnable);   // Lee nivel actual de ENABLE (pull-up → inactivo en alto)
    if (!en_level)                         // Si ENABLE está en bajo (activo)
    {
        // Inicio de pasada
        swiping            = true;         // Marca que estamos leyendo
        id_ready           = false;        // Aún no hay datos listos
        data_buffer_pos = 0u;           // Reinicia el índice del buffer crudo
        lrc_check          = 0u;           // Reinicia LRC (se acumula hasta ES)
        bit_buffer_flag    = true;         // Pedirá rearmar primer char en próximo CLK
    }
    else                                   // ENABLE volvió a alto → fin de pasada
    {
        if (swiping)                       // Solo si efectivamente estábamos en pasada
        {
            swiping  = false;              // Corta la lectura
            id_ready = true;               // Señala que hay datos para parsear
        }
    }
}

/* ========================= ISR: CLOCK (flanco descendente) ================== */
static void isr_clk(void*)                  // Ensambla caracteres de 5 bits en cada flanco descendente de CLK
{
    // Procesa solo si estamos en pasada y todavía no entregamos datos
    if (!swiping || id_ready)
    {
        return;                            // Ignora si no corresponde
    }

    // Variables estáticas para máquina de estados y buffers de bits
    static uint8_t state = START;          // 0=START, 1=CHAR, 2=END (corte por buffer lleno)
    static bool    bit_buffer[CHAR_BITS];  // Buffer temporal de 5 bits (LSB-first al capturar)
    static uint8_t bit_buffer_pos = 0;     // Índice dentro del buffer de bits (0..4)
    static bool    lrc_flag = false;       // true después de haber visto ES ('?') → deja de acumular LRC

    if (bit_buffer_flag)                   // Si se pidió reiniciar armado de char
    {
        state = START;                     // Vuelve a START
        bit_buffer_pos     = 0u;           // Reinicia posición de bit
        data_buffer_pos = 0u;           // (Re)inicia escritura del buffer crudo
        bit_buffer_flag    = false;        // Ya atendido
    }

    // DATA es activo en bajo → mapear a 1 lógico invirtiendo la lectura
    bool data = (bool)(!gpioRead(pinData)); // data=1 si pin está en 0V; data=0 si está en 5V

    switch (state)                          // Máquina de estados del ensamblado
    {
        case START: // START                // Espera el primer '1' para alinear el char
        {
            if (data)                       // Cuando vea un '1' de arranque
            {
                state = CHAR;               // Pasa a recolectar bits del carácter
                bit_buffer[0] = 1;          // Guarda el primer bit (LSB)
                bit_buffer_pos = 1u;        // Próxima posición a escribir
                lrc_check = 0u;             // Reinicia acumulador de LRC
                lrc_flag  = false;          // Aún no vimos ES
            }
        } break;

        case CHAR: // CHAR                        // Recolecta bits hasta completar 5
        {
            bit_buffer[bit_buffer_pos++] = data;  // Guarda el bit actual y avanza índice

            if (bit_buffer_pos == CHAR_BITS)      // ¿Completamos 5 bits?
            {
                uint8_t new_char = 0u;            // Acá armaremos el valor MSB-first
                for (uint8_t i = 0; i < CHAR_BITS; i++)
                {
                    // Invierte el orden (tenemos LSB-first) para armar MSB-first:
                    new_char = (uint8_t)((new_char << 1) |
                               (bit_buffer[CHAR_BITS - 1u - i] ? 1u : 0u));
                }

                bit_buffer_pos = 0u;              // Reinicia índice de bits para el próximo char

                if (data_buffer_pos < BUFFER_LEN)       // Evita overflow del buffer crudo
                {
                    data_buffer[data_buffer_pos++] = BITS_TO_CHAR(new_char); // Guarda ASCII mapeado

                    if (!lrc_flag)                         // Mientras no hayamos visto ES
                    {
                        lrc_check ^= new_char;             // Acumula LRC por XOR de "bytes" de 5 bits
                    }

                    if (new_char == ES_BITS)               // ¿Este char es '?' (fin de datos)?
                    {
                        lrc_flag = true;                   // Marca que ya no se acumula LRC
                    }

                    if (data_buffer_pos == BUFFER_LEN)  // Si llenamos el buffer crudo
                    {
                        state = END;                         // Pasa a END (corta captura)
                    }
                }
            }
        } break;

        case END:                // Estado terminal por buffer lleno (no hace nada)
        	break;
        default:
        	break;
    }
}

/* ========================= Parseo con LRC (fuera de ISR) ==================== */
static void decoder_buffer(char* out, uint8_t* out_len)   // Parsea SS→PAN→AD→ES→LRC
{
    enum { SS, PAN, AD, LRC, SUCCESS } ps = SS;  // Estados del parser
    *out_len = 0u;                               // Inicializa largo de salida
    uint8_t ad_count = 0u;                       // Cuenta de caracteres en la sección AD

    for (uint8_t i = 0; i < data_buffer_pos; i++)  // Recorre lo recibido por la ISR
    {
        switch (ps)                                  // Según estado del parser
        {
            case SS:                                  // Espera ';'
            {
                if (data_buffer[i] != SS_CHAR)     // Si no arranca con SS válido
                {
                    goto done;                        // Corta parseo (falla)
                }
                ps = PAN;                             // Pasa a leer dígitos del PAN
            } break;

            case PAN:                                 // Lee dígitos hasta FS '='
            {
                if (data_buffer[i] != FS_CHAR)     // Mientras no aparezca FS
                {
                    if (*out_len >= PAN_MAX_LEN)      // No exceder 19 dígitos de PAN
                    {
                        goto done;                    // Falla por excedente
                    }
                    if (data_buffer[i] < '0' || data_buffer[i] > '9') // Solo dígitos
                    {
                        goto done;                    // Falla por carácter no numérico
                    }
                    out[(*out_len)++] = data_buffer[i]; // Agrega dígito a la salida
                }
                else                                   // Encontró FS '='
                {
                    ps = AD;                           // Pasa a datos adicionales
                }
            } break;

            case AD:                                   // Consume AD hasta ES '?'
            {
                if (data_buffer[i] == ES_CHAR)      // Si aparece ES
                {
                    if (ad_count < AD_CHARS_MIN)      // Control mínimo de AD
                    {
                        goto done;                     // Falla si es demasiado corto
                    }
                    if (ad_count > (NUM_DATA_LEN - FS_LEN - (*out_len))) // Control básico de totales
                    {
                        goto done;                     // Falla si es demasiado largo
                    }
                    ps = LRC;                          // Pasa a verificar LRC
                }
                else                                   // Mientras no aparezca ES
                {
                    ad_count++;                        // Cuenta caracteres de AD
                }
            } break;

            case LRC:                                   // Verifica LRC
            {
                if (data_buffer[i] != BITS_TO_CHAR(lrc_check)) // Compara LRC esperado vs recibido
                {
                    goto done;                          // Falla si no coincide
                }
                ps = SUCCESS;                           // Éxito de parseo
            } break;

            case SUCCESS:                               // Ya terminó; salir
            {
                goto done;                              // Salida limpia
            }
        }
    }

done:                                                  // Punto común de salida
    if (ps != SUCCESS)                                 // Si no llegó a SUCCESS
    {
        *out_len = 0u;                                  // Invalida salida
    }
}
