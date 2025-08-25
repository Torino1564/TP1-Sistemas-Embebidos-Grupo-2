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
#include "drivers/encoder.h"
#include "drivers/SysTick.h"

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

/* Función de inicialización */
void App_Init (void)
{
	hw_DisableInterrupts();

	// Arrancar perifericos
	Peripherals_Init();
	gpioMode(PIN_SW3, INPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);
	gpioMode(PIN_LED_RED, OUTPUT);
	gpioSetupISR(PIN_SW3, FLAG_INT_POSEDGE, &ISR);
	SysTick_Init(&SysTickISR);
	encoder_init(ENCODER_A, ENCODER_B);
	encoder_enable(1);

	// Estado inicial de la FSM
	stateMachine.state = ADMIN;

	// Habilito interrupciones por puerto
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTB_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);

	hw_EnableInterrupts();

	gpioWrite(PIN_LED_RED, LOW);
	gpioWrite(PIN_LED_BLUE, LOW);
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
