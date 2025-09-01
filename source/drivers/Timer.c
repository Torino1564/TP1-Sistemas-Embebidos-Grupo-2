/*
 * Timer.c
 *
 *  Created on: Aug 26, 2025
 *      Author: jtori
 */


#include "Timer.h"
#include "SysTick.h"
#include <stdlib.h>

#define INITIAL_SERVICE_CAPACITY 16u

typedef struct
{
	void (*pCallback)();
	ticks tickInterval;
	ticks tickCount;
} PeriodicService;

static PeriodicService* pServices;
static uint32_t registeredServicesCount;
static uint32_t maxCapacity;

static ticks current_ticks = 0;

void TimerPISR()
{
	for (int i = 0; i < registeredServicesCount; i++)
	{
		PeriodicService* pService = &pServices[i];
		pService->tickCount--;
		if (pService->tickCount == 0)
		{
			pService->tickCount = pService->tickInterval;
			pService->pCallback();
		}
	}
	current_ticks++;
}

bool TimerInit()
{
	// Init systick
	return SysTick_Init(&TimerPISR, (uint64_t)TICKS_PER_SECOND);
}

uint32_t RegisterPeriodicInterruption(void (*callback)(), ticks deltaT)
{
	if (maxCapacity == 0)
	{
		pServices = (PeriodicService*)malloc(sizeof(PeriodicService) * INITIAL_SERVICE_CAPACITY);
		maxCapacity = INITIAL_SERVICE_CAPACITY;
	}

	if (registeredServicesCount >= maxCapacity)
	{
		maxCapacity = maxCapacity << 1;
		pServices = (PeriodicService*)realloc(pServices, sizeof(PeriodicService) * maxCapacity);
	}

	PeriodicService* pService = &pServices[registeredServicesCount];

	pService->pCallback = callback;
	pService->tickInterval = deltaT;
	pService->tickCount = 0;

	return registeredServicesCount++;
}

bool UnregisterPeriodicInterruption(uint32_t service_id)
{
	if (service_id >= registeredServicesCount)
	{
		return false;
	}

	PeriodicService temp = pServices[registeredServicesCount--];
	pServices[service_id] = temp;
	return true;
}

ticks Now()
{
	return current_ticks;
}

void Sleep(ticks dt)
{
	const ticks start = Now();
	while (Now() - start < dt)
	{
		// Nothing
	}
	return;
}
