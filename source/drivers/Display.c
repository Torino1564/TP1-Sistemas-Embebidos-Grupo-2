/*****************************************************************************
  @file     Display.c
  @brief    Driver del display
  @author   jtori & jpla
 ******************************************************************************/

/*******************************************************************************
 *                               ENCABEZADOS
 ******************************************************************************/
#include "Display.h" // header del driver Display
#include "Timer.h"   //
#include "SerialEncoder.h"
#include "AsciiToSeg7.h"
#include <string.h>
#include <stdlib.h>
#include "gpio.h"

/*******************************************************************************
 *                                VARIABLES
 ******************************************************************************/
static char* data;
static uint16_t numCharacters;
static uint16_t currentCharacter;
static uint16_t stringOffset;
static service_id serviceId;

// este contador es para usar el carrusel
static uint16_t contador;

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
 *                                FUNCIONES
 ******************************************************************************/
void DisplayPISR(void*)
{
	static uint8_t brillo = BRIGHTNESS_LEVEL;

	brillo --;

	gpioToggle(PORTNUM2PIN(PC, 10));
	char currentDigit = data[stringOffset + currentCharacter];

	if(brillo > 0)
	{
		currentDigit = (char)0;
	}
	else
	{
		brillo = BRIGHTNESS_LEVEL;
	}

	ParallelBytes data = {};
	data.Dig0 = currentCharacter & 0b01;
	data.Dig1 = (currentCharacter & 0b10) >> 1;

	data.Led0 = 0;
	data.Led1 = 0;

	data.unused0 = 0;
	data.unused1 = 0;
	data.unused2 = 0;
	data.unused3 = 0;

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

	/* El siguiente if else es para hacer el carrusel. Hay que mejorarlo
	 * */
	if(contador != 500)
	{
		contador++;
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
		contador = 0;
	}

	WriteSerialData((uint8_t*)&data); // Le digo al serial que info mandar
	gpioToggle(PORTNUM2PIN(PC, 10));
}

void DisplayInit()
{
	if (data != NULL)
	{
		free(data);
	}
	//InitSerialEncoder(S2P_BYTES, (uint16_t)( (float)(1000)*(3*4*((8 * S2P_BYTES))/(float)MS_PER_DIGIT)));
	InitSerialEncoder(S2P_BYTES, TICKS_PER_SECOND/2);
	serviceId = TimerRegisterPeriodicInterruption(&DisplayPISR, MS_TO_TICKS(MS_PER_DIGIT/(float)4), 0);
}


void WriteDisplay(const char* pData)
{
	if (data != NULL)
	{
		free(data);
	}

	currentCharacter = 0;
	stringOffset = 0;
	contador = 0;
	numCharacters = (uint16_t)strlen(pData);
	numCharacters = numCharacters < 4 ? 4 : numCharacters + 8;
	data = (char*)malloc(numCharacters);

	strcpy(data, "~~~~");
	strcpy(data, pData);
}
