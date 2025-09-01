/***************************************************************************//**
  @file     App.c
  @brief    Main Application
  @author   Group 2
 ******************************************************************************/

/*******************************************************************************
 *                        INCLUDE HEADER FILES
 ******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "StateMachine.h"
#include "hardware.h"
#include "drivers/board.h"
#include "drivers/Timer.h"
#include "drivers/encoder.h"
#include "drivers/Display.h"

/*******************************************************************************
 *             CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_STRING_LENGHT 16

/*******************************************************************************
 *                            GLOBAL STATE
 ******************************************************************************/

static StateMachine stateMachine;

static ticks tick_counter;

static char* id_string;
static char* pin_string;

static char currentDigit;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void ProcessInput();

/* Función de inicialización */
void App_Init (void)
{
	hw_DisableInterrupts();

	// Arrancar perifericos
	TimerInit();
	DisplayInit();
	encoder_init(ENCODER_A, ENCODER_B);

	// Estado inicial de la FSM
	stateMachine.state = ADMIN;

	hw_EnableInterrupts();

	id_string = (char*)malloc(MAX_STRING_LENGHT);
	pin_string = (char*)malloc(MAX_STRING_LENGHT);

	memset((void*)id_string, 0, MAX_STRING_LENGHT);
	memset((void*)pin_string, 0, MAX_STRING_LENGHT);

	currentDigit = '0';
	tick_counter = Now();

	WriteDisplay("1234");
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	switch (stateMachine.state)
	{
	case IDLE:
		ProcessInput();
		if (stateMachine.validID)
		{
			stateMachine.state = PIN;
		}
		break;
	case PIN:
		if (stateMachine.validPIN)
		{
			stateMachine.state = OPEN;
		}
		break;
	case COOLDOWN:
		if (stateMachine.cooldownTicks >= stateMachine.cooldownTime)
		{
			stateMachine.state = stateMachine.stateAfterCooldown;
		}
		break;
	// OPCIONAL:
	case ADMIN:
	case CHANGE_PIN:
	default:
		break;
	}
}

void ProcessInput()
{
	if (readEncoderStatus())
	{
		uint8_t turnDir = readEncoderData();
		if (turnDir == RIGHT)
		{
			currentDigit = currentDigit == '9' ? '0' : currentDigit + 1;
		}
		else if (turnDir == LEFT)
		{

			currentDigit = currentDigit == '0' ? '9' : currentDigit - 1;
		}
		else // BUTTON
		{

		}
	}
}

/*******************************************************************************
 ******************************************************************************/
