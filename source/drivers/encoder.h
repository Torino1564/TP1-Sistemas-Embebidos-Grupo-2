/***************************************************************************//**
  @file     encoder.h
  @brief    Header for the driver of the encoder
  @author   Group 2
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "gpio.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	bool encoderA; // pin A of the channel
	bool encoderB; // pin B of the channel
} encoderChannel_t;

typedef struct {
	bool enable; // interrupts enable
	pin_t pinA;
	pin_t pinB;
	bool dir; // 0 counter clockwise / 1 clockwise
	uint8_t ticks; // number of ticks in the same dir
	encoderChannel_t actual; // actual state of the encoder
	encoderChannel_t prev; // previous state of the encoder
} encoder_t;


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

//para inicializar el encoder
bool encoder_init(encoder_t *encoder, pin_t senA, pin_t senB);

/* Es una funcion que habilita o desahabilita el encoder */
void encoder_enable(encoder_t * encoder, bool enable);

void encoder_updated(encoder_t * encoder);

//cada vez que se vaya a esperar una entrada del encoder hay que llamar esta funcion
//para que obtenga el estado de A y B. Puede pasar que en algun momento se desactiven
// las interrupciones del encoder y que por eso se pueda mover libremente el mismo y que
// los movimientos no sean detectados y se pierde el valor de A y B.
bool* getEncoderStatus(encoder_t *encoder);

bool getEncoderDir(encoder_t *encoder);
