/*****************************************************************************
  @file     Display.h
  @brief    Driver del display
  @author   jtori & jpla
 ******************************************************************************/

#ifndef DRIVERS_DISPLAY_H_
#define DRIVERS_DISPLAY_H_


/*******************************************************************************
 *                               DEFINICIONES
 ******************************************************************************/
#define DIGIT_REFRESH_RATE 50 * 5
#define BRIGHTNESS_LEVEL 1

/*******************************************************************************
 *                                PROTOTIPOS
 ******************************************************************************/
void DisplayInit();
void WriteDisplay(const char* pData);
void ClearDisplay();
void EnableDisplay();

#endif /* DRIVERS_DISPLAY_H_ */
