/*****************************************************************************
  @file     App.c
  @brief    Main Application
  @author   Group 2
  @version  1.0 - coding
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
#include "drivers/MagneticStrip.h"

//cositas nuevas
#include "Segurity.h"

enum {
	CANCEL,
	BRIGHTNESS,
	RETURN
};

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
static char* input_string;
static int currentDigit;
static char currentNum;
static uint16_t buttonEncoder;

/*******************************************************************************
 *                           FUNCIONES GLOBALES
 ******************************************************************************/

void EnteringData(void);		// data input read
void ProcessingData(void);		// data input process
void ClearInputVariables(void); // clean input variables
void ChangeBrightnessLevel(void);

/* Función de inicialización */
void App_Init (void)
{
	hw_DisableInterrupts();

	// Arranco los perisfericos
	TimerInit();
	DisplayInit();
	MagBandInit(MAG_DATA, MAG_CLOCK, MAG_ENABLE);
	buttonEncoder = NewButton(ENCODER_C, false);
	EncoderInit(ENCODER_A, ENCODER_B);

	// Estado inicial de la FSM
	stateMachine.state = IDLE;

	hw_EnableInterrupts();

	id_string = (char*)malloc(MAX_STRING_LENGHT);
	pin_string = (char*)malloc(MAX_STRING_LENGHT);
	input_string = (char*)malloc(MAX_STRING_LENGHT);

	memset((void*)id_string, 0, MAX_STRING_LENGHT);
	memset((void*)pin_string, 0, MAX_STRING_LENGHT);
	memset((void*)input_string, 0, MAX_STRING_LENGHT);

	currentNum = '0';
	currentDigit = 0;
	tick_counter = Now();

	gpioMode(PIN_LED_GREEN, OUTPUT);

	gpioWrite(PIN_LED_RED, HIGH);
	gpioWrite(PIN_LED_BLUE, HIGH);
	gpioWrite(PIN_LED_GREEN, HIGH);

	gpioMode(PORTNUM2PIN(PC, 10), OUTPUT);

	NVIC_EnableIRQ(PORTE_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn); // ESTA MAL, DESPUES LO HACE JOACO
	NVIC_EnableIRQ(PORTB_IRQn); // ESTA MAL, DESPUES LO HACE JOACO
	NVIC_EnableIRQ(PORTA_IRQn); // ESTA MAL, DESPUES LO HACE JOACO

	WriteDisplay("Ingrese ID");

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	switch (stateMachine.state)
	{
	case IDLE:
		EnteringData();
		if (stateMachine.validID)
		{
			stateMachine.state = PIN;
			MagBandEnableInt(false);
		}
		break;
	case PIN:
		EnteringData();
		if (stateMachine.validPIN)
		{
			stateMachine.state = OPEN;
		}
		break;

	case OPEN:

		gpioWrite(PIN_LED_GREEN, LOW);
		Sleep(MS_TO_TICKS(5000));
		gpioWrite(PIN_LED_GREEN, HIGH);
		ClearInputVariables();
		stateMachine.state = IDLE;
		MagBandEnableInt(true);
		stateMachine.validID = false;
		stateMachine.validPIN = false;
		WriteDisplay("Ingrese ID");

		break;

	case USER_MENU:

		static uint8_t menuState = CANCEL;
		static uint8_t turnDir;
		bool encoderStatus = readEncoderStatus();
		bool buttonStatus = readButtonStatus(buttonEncoder);
		uint8_t buttonData = readButtonData(buttonEncoder);
		MagBandEnableInt(false);
		if(encoderStatus || buttonStatus)
		{
			if(encoderStatus)
			{
				// encoder
				turnDir = readEncoderData();
				if(turnDir && menuState < RETURN)
				{
					// derecha
					menuState++;
				}
				else if(!turnDir && menuState > CANCEL)
				{
					// izquierda
					menuState--;
				}
			}
			else
			{
				// boton
				if(buttonData == BUTTON_PRESSED)
				{

				}
				else if(buttonData == BUTTON_HELD)
				{

				}
				else if(buttonData == BUTTON_LONG_HELD)
				{

				}
			}


		}


		switch(menuState)
		{
		case CANCEL: 		// cancelar

			break;
		case BRIGHTNESS: 	// brillo

			break;
		case RETURN: 		// vuelvo

			break;
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

void EnteringData(void)
{
	static int firstTurn = true;
	bool encoderStatus = readEncoderStatus();
	bool buttonStatus = readButtonStatus(buttonEncoder);
	bool cardStatus = MagBandGetStatus();

	if (encoderStatus || buttonStatus) // if encoder or button new data
	{
		if(encoderStatus)
		{
			if(firstTurn)
			{
				strcpy(input_string, "0");
				WriteDisplay(input_string);
				firstTurn = false;
			}
			else
			{
				uint8_t turnDir = readEncoderData();
				if (turnDir == RIGHT && currentNum != '9')
				{
					currentNum = currentNum == '9' ? '0' : currentNum + 1;
					input_string[currentDigit] = currentNum;
				}
				else if (turnDir == LEFT)
				{
					if(currentNum == '0')
					{
						if(currentDigit != 0)
						{
							currentDigit--;
							size_t len = strlen(input_string);  // longitud actual
							if (len > 0) {
								input_string[len - 1] = '\0';   // eliminamos el último carácter
							}
							input_string[currentDigit] = currentNum;
						}
					}
					else
					{
						currentNum = currentNum == '0' ? '9' : currentNum - 1;
						input_string[currentDigit] = currentNum;
					}
				}
			}
			WriteDisplay(input_string);
		}
		else if (buttonStatus)// BUTTON
		{
			uint8_t buttonData = readButtonData(buttonEncoder);
			if(buttonData == BUTTON_PRESSED)
			{
				strncat(input_string, &currentNum, 1);
				currentDigit++;
				WriteDisplay(input_string);
			}
			else if(buttonData == BUTTON_HELD)
			{
				// Si holdeo el boton, debo procesar los datos
				ProcessingData();
			}
			else if(buttonData == BUTTON_LONG_HELD)
			{
				ChangeBrightnessLevel();
			}
		}
	}
	else if(cardStatus)
	{
		memset((void*)input_string, 0, MAX_STRING_LENGHT);
		bandaMag_getID(input_string);
		WriteDisplay(input_string);

		// Si se pasa la tarjeta, evaluo el ID
		ProcessingData();
	}
}

void ProcessingData(void)
{
	switch(stateMachine.state)
	{
	case IDLE: // checking ID entered
		if(IDSentinel(input_string))
		{
			WriteDisplay("valid id");
			stateMachine.validID = 1;
			strcpy(id_string, input_string);
		}
		else // si es invalido sigo en el mismo state idle
		{
			WriteDisplay("invalid id");
		}
		ClearInputVariables();

		break;

	case PIN: // checking PIN typed
		if(Alohomora(id_string, input_string))
		{
			WriteDisplay("valid pin");
			stateMachine.validPIN = 1;
			strcpy(pin_string, input_string);
		}
		else
		{
			WriteDisplay("invalid pin");
			ClearInputVariables();
		}
		break;
	}
}



void ClearInputVariables()
{
	bandaMag_reset();
	memset((void*)input_string, 0, MAX_STRING_LENGHT);
	currentDigit = 0;
	currentNum = '0';
	strcpy(input_string, "0");
	readButtonData(buttonEncoder);
	readEncoderData();
}


void ChangeBrightnessLevel()
{
	DisplaySetBrightnessLevel();
	return;
}

/*******************************************************************************
 ******************************************************************************/
