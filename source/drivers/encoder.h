/*****************************************************************************
  @file     encoder.h
  @brief    Header for the driver of the encoder
  @author   jtori & jpla
  @version  2.0 - comentando
 ******************************************************************************/

/*******************************************************************************
 *                               ENCABEZADOS
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "board.h"

/*******************************************************************************
 *                                MACROS
 ******************************************************************************/
#define LEFT 0
#define RIGHT 1
#define BUTTON 2
#define VUELTA_COMPLETA 4

/*******************************************************************************
 *                               PROTOTIPOS
 ******************************************************************************/

//para inicializar el encoder
bool EncoderInit(pin_t senA, pin_t senB);

bool readEncoderStatus();

uint8_t readEncoderData();
