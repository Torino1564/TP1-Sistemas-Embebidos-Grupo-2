/*****************************************************************************
  @file     Display.h
  @brief    Driver del display
  @author   jtori & jpla
  @version  2.0 - comentando
 ******************************************************************************/

#ifndef DRIVERS_DISPLAY_H_
#define DRIVERS_DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 *                               DEFINICIONES
 ******************************************************************************/
#define DIGIT_REFRESH_RATE 50 * 5
#define DEFAULT_BRIGHTNESS_LEVEL 1
#define DEFAULT_CARRUSEL_TIME (float)0.3

/*******************************************************************************
 *                                PROTOTIPOS
 ******************************************************************************/
void DisplayInit();									// starting driver
void WriteDisplay(const char* pData);				// writing segments
void ClearDisplay();								// all segments and leds off
void EnableDisplay();								//
void DisplaySetLeds(uint8_t ledNum, bool ledOn);	// writing board leds
void DisplaySetCarruselTime(uint16_t miliSecs);		// settine carrusel time

/*
 * @param Level from 1 to 3. 1 is most intense
 * @return true if the change was successfull
 */
bool DisplaySetBrightnessLevel(uint8_t level);		// setting brightness level


#endif /* DRIVERS_DISPLAY_H_ */
