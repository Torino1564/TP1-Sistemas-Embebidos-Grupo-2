/***************************************************************************//**
  @file     encoder.c
  @brief    Driver for the encoder
  @author   Group 2
 ******************************************************************************/

#include "encoder.h"

/*******************************************************************************
 *                                FUNCTIONS
 ******************************************************************************/

bool encoder_init(encoder_t *encoder, pin_t senA, pin_t senB)
{
	encoder->enable = 0;
	encoder->pinA = senA;
	encoder->pinB = senB;
	gpioMode(senA, INPUT);
	gpioMode(senB, INPUT);

	gpioSetupISR(encoder->pinA, FLAG_INT_EDGE, &encoder_updated);
	gpioSetupISR(encoder->pinB, FLAG_INT_EDGE, &encoder_updated);

	return 1;
}

void encoder_enable(encoder_t * encoder, bool enable)
{
	encoder->enable = enable;
}

void encoder_updated(encoder_t * encoder)
{
	/*lo que quiero hacer con este if es: si el enable esta desactivado entonces
	 * las interrupciones no deberian hacer nada. No se si es la mejor manera de hacerlo*/
	if(encoder->enable)
	{
		getEncoderStatus(encoder); //actualizo los valores de A y B
	}
}

void getEncoderStatus(encoder_t *encoder)
{
	encoder->prevA = encoder->actualA;
	encoder->prevB = encoder->actualB;

	encoder->actualA = gpioRead(encoder->pinA);
	encoder->actualB = gpioRead(encoder->pinB);
}

bool getEncoderDir(encoder_t *encoder)
{


//	switch()
//	{
//
//	}

	return 1;
}
