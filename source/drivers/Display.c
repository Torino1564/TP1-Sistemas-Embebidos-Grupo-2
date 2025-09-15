/*****************************************************************************
  @file     Display.c
  @brief    Driver del display
  @author   jtori & jpla
  @version  2.0 - comentando
 ******************************************************************************/

/*******************************************************************************
 *                               ENCABEZADOS
 ******************************************************************************/
// Estandar
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Propios HAL
#include "Display.h" 		// header propio
#include "Timer.h"   		// hace uso de interrupciones periodicas
#include "SerialEncoder.h"  //
#include "AsciiToSeg7.h"	// conversion de datos

// Propios MCAL
#include "gpio.h"			// para configurar

/*******************************************************************************
 *                                VARIABLES
 ******************************************************************************/
static char* data;
static uint16_t numCharacters;
static uint16_t currentCharacter;
static uint16_t stringOffset;
static service_id serviceId;

// Variables del carrusel del display
static uint16_t carruselCounter;
static uint16_t carruselTicks;

// Variable del brillo del display
static uint8_t brightnessCounter;
static int8_t brightnessLevel;

// Variables de los leds
static uint8_t ledsCounter;
static bool ledsArray[4];

/*******************************************************************************
 *                                 MACROS
 ******************************************************************************/
#define S2P_BYTES (uint8_t)2
#define NUM_DIGITS 4
#define MS_PER_DIGIT ((float)1000 / (float)(DIGIT_REFRESH_RATE))


/*******************************************************************************
 *                                OBJETOS
 ******************************************************************************/
typedef struct
{
	// Padding
	uint16_t unused0 	: 1;
	uint16_t unused1 	: 1;
	uint16_t unused2 	: 1;
	uint16_t unused3 	: 1;

	// Leds
	uint16_t Led1 		: 1;
	uint16_t Led0 		: 1;

	// Digit Select
	uint16_t Dig1 		: 1;
	uint16_t Dig0 		: 1;

	// Segment Data
	uint16_t DP 		: 1;
	uint16_t G 			: 1;
	uint16_t F 			: 1;
	uint16_t E 			: 1;
	uint16_t D 			: 1;
	uint16_t C 			: 1;
	uint16_t B 			: 1;
	uint16_t A 			: 1;
} ParallelBytes;

/*******************************************************************************
 *                                PROTOTIPOS
 ******************************************************************************/
void DisplayPISR(void*);

/*******************************************************************************
 *                                FUNCIONES
 ******************************************************************************/
void DisplayInit()
{
	if (data != NULL)
	{
		free(data);
	}
	//InitSerialEncoder(S2P_BYTES, (uint16_t)( (float)(1000)*(3*4*((8 * S2P_BYTES))/(float)MS_PER_DIGIT)));
	InitSerialEncoder(S2P_BYTES, TICKS_PER_SECOND/2);
	serviceId = TimerRegisterPeriodicInterruption(&DisplayPISR, MS_TO_TICKS(MS_PER_DIGIT/(float)4), 0);

	// seteo el tiempo de carrusel por default
	carruselTicks = MS_TO_TICKS(DEFAULT_CARRUSEL_TIME*1000)/MS_TO_TICKS(MS_PER_DIGIT/(float)4);

	// seteo el brillo por default
	brightnessLevel = DEFAULT_BRIGHTNESS_LEVEL;

	// seteo los leds por defecto
	memset(ledsArray, 0, sizeof(ledsArray));
	ledsCounter = 4;

	DisplaySetLeds(3, true);
	DisplaySetLeds(2, true);
	DisplaySetLeds(1, true);
}


void DisplayPISR(void*)
{
	// obtengo el char para mandar al display
	char currentDigit = data[stringOffset + currentCharacter];

	// control de brillo
	brightnessCounter--;
	if(brightnessCounter > 0)
	{
		currentDigit = (char)0;
	}
	else
	{
		brightnessCounter = brightnessLevel;
	}

	// armo el dato para el encoder
	ParallelBytes data = {};
	data.Dig0 = currentCharacter & 0b01;
	data.Dig1 = (currentCharacter & 0b10) >> 1;

	ledsCounter--;
	if(ledsArray[ledsCounter] && ledsCounter != 0)
	{
		data.Led0 = ledsCounter & 0b01;
		data.Led1 = (ledsCounter & 0b10) >> 1;
	}
	else
	{
		ledsCounter = 4;
	}

	data.unused0 = 0;
	data.unused1 = 0;
	data.unused2 = 0;
	data.unused3 = 0;

	// armo el digito en 7 segmentos
	seg7_t bcd = {};
	if (currentDigit >= '0' && currentDigit <= '9')
	{
		bcd = binary_to_seg7(currentDigit - '0');
	}
	else
	{
		bcd = ascii_to_seg7(currentDigit);
	}

	// Guardamos el estado de cada segmento
	data.A = bcd.A;
	data.B = bcd.B;
	data.C = bcd.C;
	data.D = bcd.D;
	data.E = bcd.E;
	data.F = bcd.F;
	data.G = bcd.G;

	// currentCharacter va pasando por cada digito para prender uno a la vez
	currentCharacter = currentCharacter == NUM_DIGITS - 1 ? 0 : currentCharacter + 1;

	// if para el carrusel
	if(carruselCounter != carruselTicks)
	{
		carruselCounter++;
	}
	else
	{
		if(numCharacters - NUM_DIGITS <= 0)
		{
			stringOffset = 0;
		}
		else if (numCharacters - stringOffset <= 4)
		{
			stringOffset = 0;
		}
		else
		{
			stringOffset++;
		}
		carruselCounter = 0;
	}

	// Le digo al serial que info mandar
	WriteSerialData((uint8_t*)&data);
}

void DisplaySetLeds(uint8_t ledNum, bool ledOn)
{
	if(ledNum >= 0 && ledNum <= 3)
	{
		ledsArray[ledNum] = ledOn;
	}
}

void DisplaySetCarruselTime(uint16_t miliSecs)
{
	carruselTicks = MS_TO_TICKS(miliSecs)/MS_TO_TICKS(MS_PER_DIGIT/(float)4);
}

uint8_t DisplayGetBrightnessLevel(void)
{
	switch (brightnessLevel)
	{
	case 5:
		return 1;
		break;
	case 3:
		return 2;
		break;
	case 1:
		return 3;
		break;
	default:
		return 2;
		break;
	}
}

bool DisplaySetBrightnessLevel(uint8_t level)
{
	switch (level){
	case 1:
		brightnessLevel = 5;
		DisplaySetLeds(3, true);
		DisplaySetLeds(2, false);
		DisplaySetLeds(1, false);
		break;
	case 2:
		brightnessLevel = 3;
		DisplaySetLeds(3, true);
		DisplaySetLeds(2, true);
		DisplaySetLeds(1, false);
		break;
	case 3:
		brightnessLevel = 1;
		DisplaySetLeds(3, true);
		DisplaySetLeds(2, true);
		DisplaySetLeds(1, true);
		break;
	default:
		return false;
	}


	return true;
}

void WriteDisplay(const char* pData)
{
	if (data != NULL)
	{
		free(data);
	}

	currentCharacter = 0;
	stringOffset = 0;
	carruselCounter = 0;
	numCharacters = (uint16_t)strlen(pData);
	numCharacters = numCharacters <= 4 ? 4 : numCharacters + 7;
	data = (char*)calloc(numCharacters, 1);

	//strcpy(data, "~~~~");
	strcpy(numCharacters <= 4 ? data : data + 3, pData);
}
