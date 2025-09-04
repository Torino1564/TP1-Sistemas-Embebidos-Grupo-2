/*
 * SerialEncoder.c
 *
 *  Created on: Aug 25, 2025
 *      Author: jtori
 */

#include "gpio.h"
#include "SerialEncoder.h"
#include <string.h>
#include <stdlib.h>
#include "Timer.h"
#include "hardware.h"

// Global State
static uint8_t* data;
static uint8_t* backBuffer;
static uint8_t 	byteLenght;
static uint8_t 	bitsSent;
static uint32_t serialPeriodUs;

static bool posTick;

static bool newData;
static bool completedWord;
static service_id serviceId;

void SerialEncoderPISR(void*)
{
	if (completedWord)
	{
		if (newData)
		{
			newData = 0;
			completedWord = 0;
			memcpy(data, backBuffer, byteLenght);
			gpioWrite(DATA_READY_PIN, 0u);
			gpioWrite(SERIAL_CLK_PIN, 1u);
			posTick = 0;
		}
		return;
	}
	if (posTick)
	{
		posTick = 0;
		gpioWrite(SERIAL_CLK_PIN, 1u);
	}
	else
	{
		// clock
		posTick = 1;
		gpioWrite(SERIAL_CLK_PIN, 0u);

		// Check if end of the word
		if (bitsSent == byteLenght * 8)
		{
			bitsSent = 0;
			completedWord = 1;
			gpioWrite(DATA_READY_PIN, 1u);
		}
		const uint8_t byteNumber = bitsSent / 8;
		const uint8_t byteOffset = bitsSent % 8;
		// Send bits
		gpioWrite(SERIAL_DATA_PIN, (data[byteNumber] >> byteOffset) & 0b01);
		bitsSent++;
	}
}

bool InitSerialEncoder(uint8_t wordByteLenght, uint32_t serialClkKHz)
{
	byteLenght = wordByteLenght;
	//cambio la linea comentada por la siguiente
	//serialPeriodUs = serialClkKHz * 1000;
	serialPeriodUs = 1000/serialClkKHz;

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

	// Register service
	serviceId = TimerRegisterPeriodicInterruption(&SerialEncoderPISR, MS_TO_TICKS((float)1/(float)serialClkKHz), 0);
	return 0;
}

bool WriteSerialData(const uint8_t* pData)
{
	if (newData)
		return 0;
	for (int i = 0; i < byteLenght; i++)
	{
		backBuffer[i] = pData[i];
	}
	newData = 1;
	return 1;
}
