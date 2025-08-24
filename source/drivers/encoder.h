/***************************************************************************//**
  @file     encoder.h
  @brief    Header for the driver of the encoder
  @author   Group 2
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "gpio.h"

//esta bien agregar el header board.h?
#include "board.h"


#define VUELTA_COMPLETA 4
#define RIGHT 1
#define LEFT 0

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum StatesEncoder
{
	A0B0,
	A0B1,
	A1B0,
	A1B1
};

typedef struct {
	pin_t pinA;
	pin_t pinB;
	unsigned int actualA:1; // actual state of A
	unsigned int actualB:1; // actual state of B
	unsigned int prevA:1; // previous state of A
	unsigned int prevB:1; // previous state of B
	unsigned int enable:1; // interrupts enable
	unsigned int ticksDir:1; // 0 counter clockwise / 1 clockwise
	unsigned int prevTicksDir:1;
	unsigned int turnsDir:1;
	unsigned int prevTurnsDir:1;
	uint8_t ticks; // number of ticks in the same dir
	uint8_t turns;
} encoder_t;


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

//para inicializar el encoder
bool encoder_init(encoder_t *encoder, pin_t senA, pin_t senB);

/* Es una funcion que habilita o desahabilita el encoder */
void encoder_enable(encoder_t * encoder, bool enable);

void encoder_updated(void);

//cada vez que se vaya a esperar una entrada del encoder hay que llamar esta funcion
//para que obtenga el estado de A y B. Puede pasar que en algun momento se desactiven
// las interrupciones del encoder y que por eso se pueda mover libremente el mismo y que
// los movimientos no sean detectados y se pierde el valor de A y B.
void getEncoderStatus(encoder_t *encoder);

bool getEncoderDir(encoder_t *encoder);

/*Funcion para resetear todas las variables del encoder*/
//void encoder_reset(encoder_t * encoder);
