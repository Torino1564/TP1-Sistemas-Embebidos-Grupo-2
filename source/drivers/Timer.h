/*
 * Timer.h
 *
 *  Created on: Aug 26, 2025
 *      Author: jtori
 */

#ifndef DRIVERS_TIMER_H_
#define DRIVERS_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

typedef unsigned long long ticks;

#define TICKS_PER_SECOND (ticks)1000000u
#define MS_TO_TICKS(x) (ticks)(x * TICKS_PER_SECOND/1000)

bool TimerInit();

uint32_t RegisterPeriodicInterruption(void (*callback)(), ticks deltaT);
bool UnregisterPeriodicInterruption(uint32_t service_id);

ticks now();

#endif /* DRIVERS_TIMER_H_ */
