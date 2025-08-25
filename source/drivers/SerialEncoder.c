/*
 * SerialEncoder.c
 *
 *  Created on: Aug 25, 2025
 *      Author: jtori
 */

#include "gpio.h"
#include "SerialEncoder.h"
#include "board.h"
#include "hardware.h"

// Global State
static uint8_t* data;
static uint8_t* backBuffer;
static uint8_t 	byteLenght;
static uint8_t 	bitsSent;
static uint32_t serialPeriodUs;

static bool posTick;

static bool newData;



__ISR__ SerialEncoderISR()
{
	if (posTick)
	{

	}
	// Check if end of the word and if there is new data
	if (bitsSent * 8 == byteLenght)
	{
		if (newData)
		{
			newData = 0;
			memcpy(data, backBuffer, byteLenght);
		}
		bitsSent = 0;
	}

	// Send bits
}

bool Setup_SerialEncoder(uint8_t wordByteLenght, uint32_t serialClkKHz)
{
	byteLenght = wordByteLenght;
	serialPeriodUs = serialClkKHz * 1000;
	if (data != 0)
	{
		free(data);
		free(backBuffer);
	}
	data = (uint8_t*)malloc(byteLenght);
	backBuffer = (uint8_t*)malloc(byteLenght);

	// pin setup
	gpioMode(SERIAL_DATA_PIN, OUTPUT);
	gpioMode(SERIAL_CLK_PIN, OUTPUT);
	gpioMode(DATA_READY_PIN, OUTPUT);
}

void WriteData(const uint8_t* pData)
{
	for (int i = 0; i < byteLenght; i++)
	{
		backBuffer[i] = pData[i];
	}
	newData = 1;
}
