/***************************************************************************//**
  @file     App.c
  @brief    Main Application
  @author   Group 2
 ******************************************************************************/

/*******************************************************************************
 *                        INCLUDE HEADER FILES
 ******************************************************************************/

#include "StateMachine.h"
#include "Peripherals.h"
#include "hardware.h"
#include "drivers/board.h"
#include "drivers/Timer.h"
#include "Binary2BCD.h"

/*******************************************************************************
 *             CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 *                            GLOBAL STATE
 ******************************************************************************/

static StateMachine stateMachine;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static ticks tick_counter;

/* Función de inicialización */
void App_Init (void)
{
	hw_DisableInterrupts();

	// Arrancar perifericos
	Peripherals_Init();
	gpioMode(PIN_SW3, INPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);
	gpioMode(PIN_LED_RED, OUTPUT);
	TimerInit();
	// Estado inicial de la FSM
	stateMachine.state = ADMIN;

	// Habilito interrupciones por puerto
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTB_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);

	hw_EnableInterrupts();

	gpioWrite(PIN_LED_RED, HIGH);
	gpioWrite(PIN_LED_BLUE, HIGH);

	tick_counter = now();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	ticks current_ticks = now();
	if (current_ticks - tick_counter > MS_TO_TICKS(1000))
	{
		gpioToggle(PIN_LED_BLUE);
		tick_counter = current_ticks;
	}

//	switch (stateMachine.state)
//	{
//	case IDLE:
//		if (stateMachine.validID)
//		{
//			stateMachine.state = PIN;
//		}
//		break;
//	case PIN:
//		if (stateMachine.validPIN)
//		{
//			stateMachine.state = OPEN;
//		}
//		break;
//	case COOLDOWN:
//		if (stateMachine.cooldownTicks >= stateMachine.cooldownTime)
//		{
//			stateMachine.state = stateMachine.stateAfterCooldown;
//		}
//		break;
//	// OPCIONAL:
//	case ADMIN:
//	case CHANGE_PIN:
//	default:
//		break;
//	}
}

/*******************************************************************************
 ******************************************************************************/
