/*
 * Display.h
 *
 *  Created on: Sep 1, 2025
 *      Author: jtori
 */

#ifndef DRIVERS_DISPLAY_H_
#define DRIVERS_DISPLAY_H_

#define DIGIT_REFRESH_RATE 30
#define NUM_DIGITS 4

void InitDisplay();
void WriteDisplay(const char* pData);
void ClearDisplay();
void EnableDisplay();

#endif /* DRIVERS_DISPLAY_H_ */
