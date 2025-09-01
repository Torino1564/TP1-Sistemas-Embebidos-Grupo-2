/*
 * DIsplay.c
 *
 *  Created on: Sep 1, 2025
 *      Author: jtori
 */

#include "Display.h"
#include "Timer.h"
#include "SerialEncoder.h"

#include <string.h>
#include <stdlib.h>

static char* data;
static uint16_t numCharacters;
static uint16_t currentCharacter;
static uint16_t stringOffset;
static uint32_t service_id;

#define S2P_BYTES (uint8_t)2

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

void DisplayPISR()
{
	const char currentDigit = data[stringOffset + currentCharacter];

	ParallelBytes data = {};
	data.Dig0 = (3 - currentCharacter) & 0b01;
	data.Dig1 = (3 - currentCharacter) & 0b10;
}

void InitDisplay()
{
	InitSerialEncoder(S2P_BYTES, (8 * S2P_BYTES)/MS_PER_DIGIT);

	service_id = RegisterPeriodicInterruption(&DisplayPISR, MS_TO_TICKS(MS_PER_DIGIT));
}


void WriteDisplay(const char* pData)
{
	if (data != NULL)
	{
		free(data);
	}

	currentCharacter = 0;
	stringOffset = 0;
	numCharacters = (uint16_t)strlen(pData);
	data = (char*)malloc(numCharacters);
	strcpy(data, pData);
}
