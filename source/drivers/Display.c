/*
 * DIsplay.c
 *
 *  Created on: Sep 1, 2025
 *      Author: jtori
 */

#include "Display.h"
#include "Timer.h"
#include "SerialEncoder.h"

// lo de aca abajo se agrega para debuguear
#include "gpio.h"
#include "board.h"

#include "Binary2BCD.h"

#include <string.h>
#include <stdlib.h>

static char* data;
static uint16_t numCharacters;
static uint16_t currentCharacter;
static uint16_t stringOffset;
static service_id serviceId;

//probando
static uint16_t contador;

#define S2P_BYTES (uint8_t)2
#define NUM_DIGITS 4
#define MS_PER_DIGIT (1000 / (NUM_DIGITS * DIGIT_REFRESH_RATE))

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

void DisplayPISR(void*)
{
	const char currentDigit = data[stringOffset + currentCharacter];

	ParallelBytes data = {};
	data.Dig0 = currentCharacter & 0b01;
	data.Dig1 = (currentCharacter & 0b10) >> 1;

	data.Led0 = 0;
	data.Led1 = 0;

	data.unused0 = 0;
	data.unused1 = 0;
	data.unused2 = 0;
	data.unused3 = 0;

	bcd_data_t bcd = {};
	if (currentDigit >= 0 && currentDigit <= 9)
	{
		bcd = binary_to_bcd(currentDigit);
	}
	else if (currentDigit >= '0' && currentDigit <= '9')
	{
		bcd = binary_to_bcd(currentDigit - '0');
	}
	else if (currentDigit == ' ')
	{
		bcd.A = 0;
		bcd.B = 0;
		bcd.C = 0;
		bcd.D = 0;
		bcd.E = 0;
		bcd.F = 0;
		bcd.G = 0;
	}
	else if ((currentDigit >= 'a' && currentDigit <= 'z') || (currentDigit >= 'A' && currentDigit <= 'Z'))
	{

	}

	data.A = bcd.A;
	data.B = bcd.B;
	data.C = bcd.C;
	data.D = bcd.D;
	data.E = bcd.E;
	data.F = bcd.F;
	data.G = bcd.G;

	currentCharacter = currentCharacter == NUM_DIGITS - 1 ? 0 : currentCharacter + 1;

	if(contador != 125)
	{
		contador++;
	}
	else
	{
		stringOffset = stringOffset == numCharacters - NUM_DIGITS ? 0 : stringOffset + 1;
		contador = 0;
	}

	if(stringOffset == 0)
	{
		gpioWrite(PIN_LED_RED, LOW);
		gpioWrite(PIN_LED_BLUE, HIGH);
	}
	else if(stringOffset == 1)
	{
		gpioWrite(PIN_LED_RED, HIGH);
		gpioWrite(PIN_LED_BLUE, LOW);
	}
	else
	{
		gpioWrite(PIN_LED_RED, HIGH);
		gpioWrite(PIN_LED_BLUE, HIGH);
	}

	WriteSerialData((uint8_t*)&data);
}

void DisplayInit()
{
	InitSerialEncoder(S2P_BYTES, 4*((8 * S2P_BYTES)/MS_PER_DIGIT));
	serviceId = TimerRegisterPeriodicInterruption(&DisplayPISR, MS_TO_TICKS(MS_PER_DIGIT), 0);
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
	data = (char*)malloc(numCharacters);
	strcpy(data, pData);
}
