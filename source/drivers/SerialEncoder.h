/*
 * SerialEncoder.h
 *
 *  Created on: Aug 25, 2025
 *      Author: jtori
 */

#ifndef DRIVERS_SERIALENCODER_H_
#define DRIVERS_SERIALENCODER_H_

#include <stdint.h>
#include <stdbool.h>

bool Setup_SerialEncoder(uint8_t wordByteLenght, uint32_t serialClkKHz);
void WriteData(const uint8_t* pData);


#endif /* DRIVERS_SERIALENCODER_H_ */
