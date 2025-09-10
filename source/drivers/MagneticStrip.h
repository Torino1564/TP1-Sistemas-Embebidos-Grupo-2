/* ========================= driverBandaMag.h ========================= */
#ifndef DRIVER_BANDA_MAG_H
#define DRIVER_BANDA_MAG_H

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"   // pin_t, gpioMode, gpioRead, gpioSetupISR, etc.

/*
 * Lector de banda magnética (Track 2: DATA, CLK, EN) para FRDM-K64F.
 * - 5 bits por carácter (4 datos LSB-first + 1 paridad). La validación final se hace con LRC.
 * - Busca Start Sentinel ';' y termina en End Sentinel '?'.
 * - Devuelve 8 dígitos ASCII (PAN truncado a 8) en out8[9] con '\0'.
 */

/* Inicializa pines e interrupciones (CLK flanco descendente, EN ambos) */
void bandaMag_init(pin_t pin_data, pin_t pin_clk, pin_t pin_en);

/* Copia los 8 dígitos (ASCII) a out8 y agrega '\0'. Devuelve true si hay uno listo. */
bool bandaMag_getID(char* out8);

/* Resetea la máquina de estados interna para una nueva lectura */
void bandaMag_reset(void);

#endif /* DRIVER_BANDA_MAG_H */
