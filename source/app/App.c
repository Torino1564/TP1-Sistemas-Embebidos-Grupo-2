/*****************************************************************************
  @file     App.c
  @brief    Main Application
  @author   Group 2
 ******************************************************************************/

/*******************************************************************************
 *                                ENCABEZADOS
 ******************************************************************************/
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "StateMachine.h"
#include "hardware.h"
#include "drivers/board.h"
#include "drivers/Timer.h"
#include "drivers/encoder.h"
#include "drivers/Display.h"
#include "drivers/Button.h"

//cositas nuevas
#include "Segurity.h"

/*******************************************************************************
 *                                MACROS
 ******************************************************************************/

#define MAX_STRING_LENGHT 16

/*******************************************************************************
 *                                VARIABLES
 ******************************************************************************/
static StateMachine stateMachine;
static ticks tick_counter;
static char* id_string;
static char* pin_string;
static int currentDigit;
static char currentNum;
static uint16_t buttonEncoder;

/*******************************************************************************
 *                           FUNCIONES GLOBALES
 ******************************************************************************/

void ProcessInput();
void enteringID();

/* Función de inicialización */
void App_Init (void)
{
	hw_DisableInterrupts();

	// Arrancar perifericos
	TimerInit();
	DisplayInit();
	buttonEncoder = NewButton(ENCODER_C, false);
	encoder_init(ENCODER_A, ENCODER_B);

	// Estado inicial de la FSM
	stateMachine.state = IDLE;

	hw_EnableInterrupts();

	id_string = (char*)malloc(MAX_STRING_LENGHT);
	pin_string = (char*)malloc(MAX_STRING_LENGHT);

	memset((void*)id_string, 0, MAX_STRING_LENGHT);
	memset((void*)pin_string, 0, MAX_STRING_LENGHT);

	currentNum = '0';
	currentDigit = 0;
	tick_counter = Now();

	gpioMode(PIN_LED_RED, OUTPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);

	gpioWrite(PIN_LED_RED, HIGH);
	gpioWrite(PIN_LED_BLUE, HIGH);

	gpioMode(PORTNUM2PIN(PC, 10), OUTPUT);

	NVIC_EnableIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn); // ESTA MAL, DESPUES LO HACE JOACO

	WriteDisplay("1234");

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	switch (stateMachine.state)
	{
	case IDLE:
		enteringID();
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
			currentNum = currentNum == '9' ? '0' : currentNum + 1;
		}
		else if (turnDir == LEFT)
		{
			currentNum = currentNum == '0' ? '9' : currentNum - 1;
		}
		else // BUTTON
		{

		}
	}
	//WriteDisplay(&currentNum);
}

void enteringID()
{
	static int state = 0;
	static int firstTurn = true;
	bool encoderStatus = readEncoderStatus();
	bool buttonStatus = readButtonStatus(buttonEncoder);
	uint8_t buttonData = readButtonData(buttonEncoder);

	switch(state)
	{
	case 0: //waiting
		if (encoderStatus || buttonStatus)
		{
			if(encoderStatus)
			{
				if(firstTurn)
				{
					WriteDisplay("0");
					firstTurn = false;
				}
				else
				{
					uint8_t turnDir = readEncoderData();
					if (turnDir == RIGHT && currentNum != '9')
					{
						currentNum = currentNum == '9' ? '0' : currentNum + 1;
						id_string[currentDigit] = currentNum;
					}
					else if (turnDir == LEFT)
					{
						if(currentNum == '0')
						{
							if(currentDigit == 0)
							{
								state = 0; // vuelvo al estado 0
							}
							else
							{
								currentDigit--;
								size_t len = strlen(id_string);  // longitud actual
								if (len > 0) {
									id_string[len - 1] = '\0';   // eliminamos el último carácter
								}
								id_string[currentDigit] = currentNum;
							}
						}
						else
						{
							currentNum = currentNum == '0' ? '9' : currentNum - 1;
							id_string[currentDigit] = currentNum;
						}
					}
				}
			}
			else if (buttonStatus)// BUTTON
			{
				if(buttonData == BUTTON_PRESSED)
				{
					strncat(id_string, &currentNum, 1);
					currentDigit++;
				}
				else if(buttonData == BUTTON_HELD)
				{
					state = 1;
				}
			}
			WriteDisplay(id_string);
		}

		break;

	case 1: //validating
		if(IDSentinel(id_string))
		{
			gpioWrite(PIN_LED_RED, LOW);
			WriteDisplay("   ID valid   ");
			stateMachine.validID = 1;
		}
		else
		{
			WriteDisplay("   ID invalid   ");
		}
		id_string = "0";
		break;
	}

}

/*******************************************************************************
 ******************************************************************************/
