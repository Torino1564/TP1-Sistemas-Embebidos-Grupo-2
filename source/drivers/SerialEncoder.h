/*****************************************************************************
  @file     SerialEncoder.h
  @brief    Parallel to serial Driver
  @author   jtori
  @version  1.0 - coding
 ******************************************************************************/

#ifndef DRIVERS_SERIALENCODER_H_
#define DRIVERS_SERIALENCODER_H_

/*******************************************************************************
 *                                ENCABEZADOS
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 *                                PROTOTIPOS
 ******************************************************************************/

bool InitSerialEncoder(uint8_t wordByteLenght, uint32_t serialClk);
bool WriteSerialData(const uint8_t* pData);

#endif /* DRIVERS_SERIALENCODER_H_ */
