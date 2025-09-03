/*
 * Button.h
 *
 *  Created on: Sep 3, 2025
 *      Author: jtori
 */

#ifndef DRIVERS_BUTTON_H_
#define DRIVERS_BUTTON_H_

#include <stdbool.h>
#include "Timer.h"
#include "gpio.h"
uint16_t NewButton(pin_t pin, bool activeHigh);
bool SetDebouncing(uint16_t buttonId, ticks dt);

void DeleteButton();

#endif /* DRIVERS_BUTTON_H_ */
