/***************************************************************************//**
  @file     Peripherals.h
  @brief    Peripheral configuration
  @author   Group 2
 ******************************************************************************/

#ifndef _PERIPHERALS_
#define _PERIPHERALS_

#include <stdbool.h>
#include <stdint.h>

bool initPeripherals();

// Encoder
void setEncoderEnable(bool value);
bool getEncoderEnable();

// MagneticKey
void setMagneticKeyEnable(bool value);
bool getMagneticKeyEnable();

#endif
