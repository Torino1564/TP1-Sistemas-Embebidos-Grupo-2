/***************************************************************************//**
  @file     App.c
  @brief    Main Application
  @author   Group 2
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/board.h"
#include "StateMachine.h"
#include "Peripherals.h"
#include "drivers/SysTick.h"

#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * GLOBAL STATE
 ******************************************************************************/

static StateMachine stateMachine;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void ISR()
{
	gpioToggle(PIN_LED_BLUE);
}

void SysTickISR()
{
	int a = 0;
	for (int i = 0; i < 100; i++) {
		a += i;
	}
}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	hw_DisableInterrupts();

	NVIC_EnableIRQ(PORTA_IRQn);

	stateMachine.state = ADMIN;
	// Arrancar perifericos
	initPeripherals();

	gpioMode(BUTTON_PIN, INPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);

	gpioSetupISR(BUTTON_PIN, FLAG_INT_POSEDGE, &ISR);
	SysTick_Init(&SysTickISR);

	hw_EnableInterrupts();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
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
