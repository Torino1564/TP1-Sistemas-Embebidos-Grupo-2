/*
 * Button.c
 *
 *  Created on: Sep 3, 2025
 *      Author: jtori
 */
#include "Button.h"
#include <stdlib.h>
#include <string.h>
#include "Timer.h"

#define START_MAX_BUTTON_COUNT (int)8;

static uint16_t buttonCounter = 0;
static uint16_t maxButtonCount = START_MAX_BUTTON_COUNT;
static bool initialized = 0;
static bool semaphore = 0;

typedef struct
{
	pin_t pin;
	uint8_t buttonId;
	uint8_t inputMode;
	uint8_t isrType;

	bool pressed;
	uint16_t tickInterval;
	service_id debouncingIsrId;
} Button;

static Button* buttonArray;

void ButtonISR(void* user_data)
{
	if (semaphore)
		return;

	Button* pButton = (Button*)(user_data);
	// Hacer cosas con el boton
	pButton->pressed = 1;
	gpioSetupISR(pButton->pin, NO_INT, &ButtonISR, pButton);
	TimerSetEnable(pButton->debouncingIsrId, true);
}

void DebouncingISR(void* user_data)
{
	if (semaphore)
		return;


	Button* pButton = (Button*)(user_data);
	gpioSetupISR(pButton->pin, pButton->isrType, &ButtonISR, pButton);
	TimerSetEnable(pButton->debouncingIsrId,false);
}

void InitButtonDriver()
{
	initialized = true;
	buttonArray = (Button*)calloc(maxButtonCount, sizeof(Button));
}

uint16_t NewButton(pin_t pin, bool activeHigh)
{
	if (!initialized)
	{
		InitButtonDriver();
	}

	if (buttonCounter + 1 >= maxButtonCount)
	{
		semaphore = 1;
		maxButtonCount += START_MAX_BUTTON_COUNT;
		Button* tempArray = (Button*)calloc(maxButtonCount, sizeof(Button));
		memcpy(buttonArray, tempArray, buttonCounter * sizeof(Button));
		free(buttonArray);
		buttonArray = tempArray;
		for (int i = 0; i < buttonCounter; i++)
		{
			gpioSetupISR(buttonArray[i].pin, buttonArray[i].isrType, &ButtonISR, &buttonArray[i]);
			TimerSetUserData(buttonArray[i].debouncingIsrId, &buttonArray[i]);
			TimerSetEnable(buttonArray[i].debouncingIsrId, 0);
		}
		semaphore = 0;
	}
	const uint16_t buttonId = buttonCounter;
	Button* pButton = &buttonArray[buttonCounter++];

	pButton->pin = pin;
	pButton->inputMode = activeHigh ? INPUT_PULLDOWN : INPUT_PULLUP;
	pButton->isrType = activeHigh ? FLAG_INT_POSEDGE : FLAG_INT_NEGEDGE;
	pButton->tickInterval = 0;
	pButton->debouncingIsrId = 0;
	pButton->pressed = 0;
	pButton->debouncingIsrId = TimerRegisterPeriodicInterruption(&DebouncingISR, MS_TO_TICKS(400), pButton);
	TimerSetEnable(pButton->debouncingIsrId, false);

	gpioMode(pButton->pin, pButton->inputMode);
	gpioSetupISR(pButton->pin, pButton->isrType, &ButtonISR, &buttonArray[buttonId]);

	return buttonId;
}

bool readButtonStatus(uint16_t buttonId)
{
	return buttonArray[buttonId].pressed;
}

bool readButtonData(uint16_t buttonId)
{
	bool temp = (bool)buttonArray[buttonId].pressed;
	buttonArray[buttonId].pressed = 0;
	return temp;
}
