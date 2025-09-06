/*****************************************************************************
  @file     encoder.h
  @brief    Header for the driver of the encoder
  @author   jtori & jpla
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
bool encoder_init(pin_t senA, pin_t senB);

/* Es una funcion que habilita o desahabilita el encoder */
void encoder_enable(bool enable);

// Cada vez que se vaya a esperar una entrada del encoder hay que llamar esta funcion
// para que obtenga el estado de A y B. Puede pasar que en algun momento se desactiven
// las interrupciones del encoder y que por eso se pueda mover libremente el mismo y que
// los movimientos no sean detectados y se pierde el valor de A y B.
void getEncoderStatus();

bool getEncoderDir();

bool readEncoderStatus();
/**
 * @return Devuelve RIGHT, LEFT, BUTTON
 */
uint8_t readEncoderData();

/*Funcion para resetear todas las variables del encoder*/
//void encoder_reset(encoder_t * encoder);
