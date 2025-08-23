/***************************************************************************//**
  @file     encoder.c
  @brief    Driver for the encoder
  @author   Group 2
 ******************************************************************************/

#include "encoder.h"

static encoder_t *encoder_global;

/*******************************************************************************
 *                                FUNCTIONS
 ******************************************************************************/

bool encoder_init(encoder_t *encoder, pin_t senA, pin_t senB)
{
	encoder_global = encoder;

	encoder->enable = 0;
	encoder->pinA = senA;
	encoder->pinB = senB;
	gpioMode(senA, INPUT);
	gpioMode(senB, INPUT);

	gpioSetupISR(encoder->pinA, FLAG_INT_EDGE, encoder_updated);
	gpioSetupISR(encoder->pinB, FLAG_INT_EDGE, encoder_updated);

	return 1;
}

void encoder_enable(encoder_t * encoder, bool enable)
{
	encoder->enable = enable;
}

void encoder_updated(void)
{
	/*lo que quiero hacer con este if es: si el enable esta desactivado entonces
	 * las interrupciones no deberian hacer nada. No se si es la mejor manera de hacerlo*/
	//if(encoder->enable)
//	{
//		getEncoderStatus(encoder); //actualizo los valores de A y B
//		getEncoderDir(encoder);
//	}
	getEncoderStatus(encoder_global);
	getEncoderDir(encoder_global);
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
	uint8_t state = 0;
	state = encoder->prevA<<1 | encoder->prevB;

	switch(state)
	{
	case A1B1:
		if(encoder->ticks == VUELTA_COMPLETA)
		{
			encoder->ticks = 0;
			if(encoder->dir == RIGHT)
			{
				gpioToggle(PIN_LED_RED);
			}
			else
			{
				gpioToggle(PIN_LED_BLUE);
			}
			turns++;
		}
		else
		{
			encoder->ticks = 0;
			turns = 0;
		}
		if(!encoder->actualA)
		{
			if(encoder->dir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks = 1;
			}
			encoder->dir = 1;
			gpioToggle(PIN_LED_RED);
			return 1;
		}
		else if(!encoder->actualB)
		{
			/*La siguiente secuencia de if else solo tiene sentido si se usa
			 * la propiedad ticks del encoder.*/
			if(!encoder->dir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks = 1;
			}
			encoder->dir = 0;
			gpioToggle(PIN_LED_BLUE);
			return 0;
		}
		break;

	case A0B0:
		if(encoder->actualA)
		{
			/*La siguiente secuencia de if else solo tiene sentido si se usa
			 * la propiedad ticks del encoder.*/
			if(encoder->dir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks = 1;
			}
			encoder->dir = 1;
			gpioToggle(PIN_LED_RED);
			return 1;
		}
		else if(encoder->actualB)
		{
			/*La siguiente secuencia de if else solo tiene sentido si se usa
			 * la propiedad ticks del encoder.*/
			if(!encoder->dir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks = 1;
			}
			encoder->dir = 0;
			gpioToggle(PIN_LED_BLUE);
			return 0;
		}
		break;

	case A0B1:
		if(!encoder->actualB)
		{
			/*La siguiente secuencia de if else solo tiene sentido si se usa
			 * la propiedad ticks del encoder.*/
			if(encoder->dir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks = 1;
			}
			encoder->dir = 1;
			gpioToggle(PIN_LED_RED);
			return 1;
		}
		else if(encoder->actualA)
		{
			/*La siguiente secuencia de if else solo tiene sentido si se usa
			 * la propiedad ticks del encoder.*/
			if(!encoder->dir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks = 1;
			}
			encoder->dir = 0;
			gpioToggle(PIN_LED_BLUE);
			return 0;
		}
		break;

	case A1B0:
		if(encoder->actualB)
		{
			/*La siguiente secuencia de if else solo tiene sentido si se usa
			 * la propiedad ticks del encoder.*/
			if(encoder->dir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks = 1;
			}
			encoder->dir = 1;
			gpioToggle(PIN_LED_RED);
			return 1;
		}
		else if(!encoder->actualA)
		{
			/*La siguiente secuencia de if else solo tiene sentido si se usa
			 * la propiedad ticks del encoder.*/
			if(!encoder->dir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks = 1;
			}
			encoder->dir = 0;
			gpioToggle(PIN_LED_BLUE);
			return 0;
		}
		break;



	default:
		//quizas sea conveniente poner algo que avise si entra aca por equivocacion
		break;
	}

	return 1; //temporal
}
