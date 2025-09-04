/***************************************************************************//**
  @file     encoder.c
  @brief    Driver for the encoder
  @author   Group 2
 ******************************************************************************/

#include "encoder.h"
#include <stdbool.h>

typedef struct {
	pin_t pinA;
	pin_t pinB;
	uint16_t actualA		:1; 		// actual state of A
	uint16_t actualB		:1; 		// actual state of B
	uint16_t prevA			:1; 		// previous state of A
	uint16_t prevB			:1; 		// previous state of B
	uint16_t enable			:1; 		// interrupts enable
	uint16_t ticksDir		:1; 		// 0 counter clockwise / 1 clockwise
	uint16_t prevTicksDir	:1;
	uint16_t turnsDir		:1;
	uint16_t prevTurnsDir	:1;
	uint16_t newData		:1;
	uint8_t ticks; 						// number of ticks in the same dir
	uint8_t turns;
} encoder_t;

static encoder_t global_encoder;

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

void encoder_enable(bool enable)
{
	global_encoder.enable = enable;
}

void encoder_updated(void* user_data)
{
	getEncoderStatus(global_encoder);
	getEncoderDir(global_encoder);
}

bool encoder_init(pin_t senA, pin_t senB)
{
	global_encoder.enable = 0;
	global_encoder.pinA = senA;
	global_encoder.pinB = senB;
	global_encoder.actualA = 1;
	global_encoder.actualB = 1;
	gpioMode(senA, INPUT);
	gpioMode(senB, INPUT);

	gpioSetupISR(global_encoder.pinA, FLAG_INT_EDGE, encoder_updated, 0);
	gpioSetupISR(global_encoder.pinB, FLAG_INT_EDGE, encoder_updated, 0);

	return 1;
}

void getEncoderStatus()
{
	global_encoder.prevA = global_encoder.actualA;
	global_encoder.prevB = global_encoder.actualB;

	global_encoder.actualA = gpioRead(global_encoder.pinA);
	global_encoder.actualB = gpioRead(global_encoder.pinB);
}

bool getEncoderDir()
{
	uint8_t state = 0;
	state = global_encoder.prevA<<1 | global_encoder.prevB;

	switch(state)
	{
	case A1B1: // Si llego a 11 entonces puede haber una vuelta completa
		//Si no hubo vuelta completa no hago nada
		global_encoder.ticks = 0;
		if(global_encoder.actualA == 0) // Si estando en AB = 11 pasa a AB = 01
		{
			global_encoder.prevTicksDir = global_encoder.ticksDir;
			global_encoder.ticksDir = LEFT; // cambiar a left
			global_encoder.ticks++;
			return 1;
		}
		else if(global_encoder.actualB == 0) //Si estando en AB = 11 pasa a AB = 10
		{
			global_encoder.prevTicksDir = global_encoder.ticksDir;
			global_encoder.ticksDir = RIGHT; // cambiar a right
			global_encoder.ticks++;
			return 0;
		}
		break;

	case A0B1:
		if(global_encoder.actualB == 0) // Si estando en AB = 01 pasa a AB = 00
		{
			global_encoder.prevTicksDir = global_encoder.ticksDir;
			global_encoder.ticksDir = LEFT; // cambiar a left
			global_encoder.ticks++;
			return 1;
		}
		else if(global_encoder.actualA == 1) //Si estando en AB = 01 pasa a AB = 11
		{
			global_encoder.prevTicksDir = global_encoder.ticksDir;
			global_encoder.ticksDir = RIGHT; // cambiar a right
			global_encoder.ticks++;
			if (global_encoder.ticks == VUELTA_COMPLETA)
			{
				if(global_encoder.turns == 0) // Primera vuelta?
				{
					global_encoder.turns++;
					global_encoder.turnsDir = global_encoder.ticksDir;
				}
				else //No es la primera vuelta
				{
					global_encoder.prevTurnsDir = global_encoder.turnsDir;
					global_encoder.turnsDir = global_encoder.ticksDir;
					if(global_encoder.prevTurnsDir == global_encoder.turnsDir)
					{
						global_encoder.turns++;
					}
					else
					{
						global_encoder.turns--;
					}
				}
				gpioToggle(PIN_LED_BLUE);
				global_encoder.newData = 1;
				global_encoder.ticks = 0;
			}
			return 0;
		}
		break;

	case A0B0:
		if(global_encoder.actualA == 1) //Si estando en AB = 00 pasa a AB = 10
		{
			global_encoder.prevTicksDir = global_encoder.ticksDir;
			global_encoder.ticksDir = LEFT; // cambiar a left
			if(global_encoder.prevTicksDir == global_encoder.ticksDir)
			{
				global_encoder.ticks++;
			}
			else
			{
				global_encoder.ticks--;
			}
			return 1;
		}
		else if(global_encoder.actualB == 1) //Si estando en AB = 00 pasa a AB = 01
		{
			global_encoder.prevTicksDir = global_encoder.ticksDir;
			global_encoder.ticksDir = RIGHT; // cambiar a right
			if(global_encoder.prevTicksDir == global_encoder.ticksDir)
			{
				global_encoder.ticks++;
			}
			else
			{
				global_encoder.ticks--;
			}
			return 0;
		}
		break;

	case A1B0:
		if(global_encoder.actualB == 1) //Si estando en AB = 10 pasa a AB = 11
		{
			global_encoder.prevTicksDir = global_encoder.ticksDir;
			global_encoder.ticksDir = LEFT; // cambiar a left
			global_encoder.ticks++;
			if (global_encoder.ticks == VUELTA_COMPLETA)
			{
				if(global_encoder.turns == 0) // Primera vuelta?
				{
					global_encoder.turns++;
					global_encoder.turnsDir = global_encoder.ticksDir;
				}
				else //No es la primera vuelta
				{
					global_encoder.prevTurnsDir = global_encoder.turnsDir;
					global_encoder.turnsDir = global_encoder.ticksDir;
					if(global_encoder.prevTurnsDir == global_encoder.turnsDir)
					{
						global_encoder.turns++;
					}
					else
					{
						global_encoder.turns--;
					}
				}
				gpioToggle(PIN_LED_RED);
				global_encoder.newData = 1;
				global_encoder.ticks = 0;
			}
			return 1;
		}
		else if(global_encoder.actualA == 0) //Si estando en AB = 10 pasa a AB = 00
		{
			global_encoder.prevTicksDir = global_encoder.ticksDir;
			global_encoder.ticksDir = RIGHT; // cambiar a right
			global_encoder.ticks++;
			return 0;
		}
		break;

	default:
		//quizas sea conveniente poner algo que avise si entra aca por equivocacion
		break;
	}

	return 1; //temporal
}

bool readEncoderStatus()
{
	const bool temp = global_encoder.newData;
	if ((bool)global_encoder.newData)
	{
		global_encoder.newData = 0;
	}
	return temp;
}


uint8_t readEncoderData()
{
	return global_encoder.turnsDir;
}
