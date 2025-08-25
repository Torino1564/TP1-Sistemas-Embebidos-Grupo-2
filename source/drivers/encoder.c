/***************************************************************************//**
  @file     encoder.c
  @brief    Driver for the encoder
  @author   Group 2
 ******************************************************************************/

#include "encoder.h"

static encoder_t *encoder_global;


enum StatesEncoder
{
	A0B0,
	A0B1,
	A1B0,
	A1B1
};


/*******************************************************************************
 *                                FUNCTIONS
 ******************************************************************************/

bool encoder_init(encoder_t *encoder, pin_t senA, pin_t senB)
{
	encoder_global = encoder;

	encoder->enable = 0;
	encoder->pinA = senA;
	encoder->pinB = senB;
	encoder->actualA = 1;
	encoder->actualB = 1;
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

//void getEncoderStatus(encoder_t *encoder)
//{
//
//	return;
//}
//
//bool getEncoderDir(encoder_t *encoder)
//{
//	gpioWrite(PIN_LED_RED, gpioRead(encoder->pinA));
//	gpioWrite(PIN_LED_BLUE, gpioRead(encoder->pinB));
//	return 1;
//}

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
	case A1B1: // Si llego a 11 entonces puede haber una vuelta completa
		if(encoder->ticks == VUELTA_COMPLETA)
		{
			if(encoder->turns == 0) // Primera vuelta?
			{
				encoder->turns++;
				encoder->turnsDir = encoder->ticksDir;
			}
			else //No es la primera vuelta
			{
				encoder->prevTurnsDir = encoder->turnsDir;
				encoder->turnsDir = encoder->ticksDir;
				if(encoder->prevTurnsDir == encoder->turnsDir)
				{
					encoder->turns++;
				}
				else
				{
					encoder->turns--;
				}
			}
			if(encoder->turnsDir == RIGHT)
			{
				gpioToggle(PIN_LED_RED);
			}
			else
			{
				gpioToggle(PIN_LED_BLUE);
			}
			encoder->ticks = 0;
		}
		else //Si no hubo vuelta completa no hago nada
		{
			encoder->ticks = 0;
			if(encoder->actualA == 0) // Si estando en AB = 11 pasa a AB = 01
			{
				encoder->prevTicksDir = encoder->ticksDir;
				encoder->ticksDir = RIGHT;
				encoder->ticks++;
				return 1;
			}
			else if(encoder->actualB == 0) //Si estando en AB = 11 pasa a AB = 10
			{
				encoder->prevTicksDir = encoder->ticksDir;
				encoder->ticksDir = LEFT;
				encoder->ticks++;
				return 0;
			}
		}

		break;

	case A0B1:
		if(encoder->actualB == 0) // Si estando en AB = 01 pasa a AB = 00
		{
			encoder->prevTicksDir = encoder->ticksDir;
			encoder->ticksDir = RIGHT;
			encoder->ticks++;
			return 1;
		}
		else if(encoder->actualA == 1) //Si estando en AB = 01 pasa a AB = 11
		{
			encoder->prevTicksDir = encoder->ticksDir;
			encoder->ticksDir = LEFT;
			encoder->ticks++;
			if (encoder->ticks == VUELTA_COMPLETA)
			{
				if(encoder->turns == 0) // Primera vuelta?
				{
					encoder->turns++;
					encoder->turnsDir = encoder->ticksDir;
				}
				else //No es la primera vuelta
				{
					encoder->prevTurnsDir = encoder->turnsDir;
					encoder->turnsDir = encoder->ticksDir;
					if(encoder->prevTurnsDir == encoder->turnsDir)
					{
						encoder->turns++;
					}
					else
					{
						encoder->turns--;
					}
				}
				gpioToggle(PIN_LED_BLUE);
				encoder->ticks = 0;
			}
			return 0;
		}
		break;


	case A0B0:
		if(encoder->actualA == 1) //Si estando en AB = 00 pasa a AB = 10
		{
			encoder->prevTicksDir = encoder->ticksDir;
			encoder->ticksDir = RIGHT;
			if(encoder->prevTicksDir == encoder->ticksDir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks--;
			}
			return 1;
		}
		else if(encoder->actualB == 1) //Si estando en AB = 00 pasa a AB = 01
		{
			encoder->prevTicksDir = encoder->ticksDir;
			encoder->ticksDir = LEFT;
			if(encoder->prevTicksDir == encoder->ticksDir)
			{
				encoder->ticks++;
			}
			else
			{
				encoder->ticks--;
			}
			return 0;
		}
		break;



	case A1B0:
		if(encoder->actualB == 1) //Si estando en AB = 10 pasa a AB = 11
		{
			encoder->prevTicksDir = encoder->ticksDir;
			encoder->ticksDir = RIGHT;
			encoder->ticks++;
			if (encoder->ticks == VUELTA_COMPLETA)
			{
				if(encoder->turns == 0) // Primera vuelta?
				{
					encoder->turns++;
					encoder->turnsDir = encoder->ticksDir;
				}
				else //No es la primera vuelta
				{
					encoder->prevTurnsDir = encoder->turnsDir;
					encoder->turnsDir = encoder->ticksDir;
					if(encoder->prevTurnsDir == encoder->turnsDir)
					{
						encoder->turns++;
					}
					else
					{
						encoder->turns--;
					}
				}
				gpioToggle(PIN_LED_RED);
				encoder->ticks = 0;
			}
			return 1;
		}
		else if(encoder->actualA == 0) //Si estando en AB = 10 pasa a AB = 00
		{
			encoder->prevTicksDir = encoder->ticksDir;
			encoder->ticksDir = LEFT;
			encoder->ticks++;
			return 0;
		}
		break;



	default:
		//quizas sea conveniente poner algo que avise si entra aca por equivocacion
		break;
	}

	return 1; //temporal
}

bool readEncoderStatus(encoder_t* encoder)
{
	return encoder->newData;
}


uint8_t readEncoderData(encoder_t* encoder)
{
	return encoder->turnsDir;
}
