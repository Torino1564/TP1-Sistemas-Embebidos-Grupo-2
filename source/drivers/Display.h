/*
 * Display.h
 *
 *  Created on: Sep 1, 2025
 *      Author: jtori
 */

#ifndef DRIVERS_DISPLAY_H_
#define DRIVERS_DISPLAY_H_

#define DIGIT_REFRESH_RATE 120

void DisplayInit();
void WriteDisplay(const char* pData);
void ClearDisplay();
void EnableDisplay();

#endif /* DRIVERS_DISPLAY_H_ */
