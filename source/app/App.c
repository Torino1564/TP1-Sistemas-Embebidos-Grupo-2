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

enum UserMenuStates {
	CANCEL,
	BRIGHTNESS
};

/*******************************************************************************
 *                                MACROS
 ******************************************************************************/

#define MAX_STRING_LENGHT 16

/*******************************************************************************
 *                                VARIABLES
 ******************************************************************************/
static StateMachine stateMachine;
static char* id_string;
static char* pin_string;
static char* input_string;
static int currentDigit;
static char currentNum;
static uint16_t buttonEncoder;

/*******************************************************************************
 *                           FUNCIONES GLOBALES
 ******************************************************************************/

bool EnteringData(void);		// data input read
void ProcessingData(void);		// data input process
void ClearInputVariables(void); // clean input variables
void ChangeBrightnessLevel(void);
void State_Idle();
void State_Open();
void State_EnteringPin();
void State_UserMenu();
void State_BrightnessMenu();
void State_Cooldown();

/* Función de inicialización */
void App_Init (void)
{
	hw_DisableInterrupts();

	// Arranco los perifericos
	TimerInit();
	gpioInitInterrupts();
	DisplayInit();
	MagBandInit(MAG_DATA, MAG_CLOCK, MAG_ENABLE);
	buttonEncoder = NewButton(ENCODER_C, false);
	EncoderInit(ENCODER_A, ENCODER_B);

	// Estado inicial de la FSM
	stateMachine.pStateFunc = &State_Idle;
	stateMachine.menuState = CANCEL;
	stateMachine.brightnessLevel = DEFAULT_BRIGHTNESS_LEVEL;

	hw_EnableInterrupts();

	id_string = (char*)malloc(MAX_STRING_LENGHT);
	pin_string = (char*)malloc(MAX_STRING_LENGHT);
	input_string = (char*)malloc(MAX_STRING_LENGHT);

	memset((void*)id_string, 0, MAX_STRING_LENGHT);
	memset((void*)pin_string, 0, MAX_STRING_LENGHT);
	memset((void*)input_string, 0, MAX_STRING_LENGHT);

	gpioMode(PIN_LED_GREEN, OUTPUT);
	gpioMode(PIN_LED_RED, OUTPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);

	gpioWrite(PIN_LED_RED, HIGH);
	gpioWrite(PIN_LED_GREEN, HIGH);
	gpioWrite(PIN_LED_BLUE, HIGH);

	// pines de salida testpoint para las interrupciones
	gpioMode(SYSTICK_ISR, OUTPUT);
	gpioMode(PORT_ISR, OUTPUT);

	gpioWrite(PORT_ISR, LOW);
	gpioWrite(SYSTICK_ISR, LOW);

	ClearInputVariables();

	WriteDisplay("Ingrese ID");
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	stateMachine.pStateFunc();
}

void State_Idle()
{
	stateMachine.pMenuCalledFrom = &State_Idle;
	if (EnteringData())
	{
		if(IDSentinel(input_string))
		{
			WriteDisplay("valid id");
			stateMachine.validID = 1;
			strcpy(id_string, input_string);
			gpioWrite(PIN_LED_BLUE, LOW);
		}
		else // si es invalido sigo en el mismo state idle
		{
			WriteDisplay("invalid id");
		}
		ClearInputVariables();
	}
	if (stateMachine.validID)
	{
		stateMachine.pStateFunc = &State_EnteringPin;
		MagBandEnableInt(false);
	}
}

void State_EnteringPin()
{
	gpioWrite(PIN_LED_RED, HIGH);
	stateMachine.pMenuCalledFrom = &State_EnteringPin;
	if (EnteringData())
	{
		if(Alohomora(id_string, input_string))
		{
			WriteDisplay("open");
			stateMachine.validPIN = 1;
			gpioWrite(PIN_LED_BLUE, HIGH);
			strcpy(pin_string, input_string);
		}
		else
		{
			WriteDisplay("invalid pin");
			ClearInputVariables();
			stateMachine.pStateAfterCooldownFunc = &State_EnteringPin;
			stateMachine.pStateFunc = &State_Cooldown;
			stateMachine.cooldownTicks = MS_TO_TICKS(3000);
			gpioWrite(PIN_LED_RED, LOW);
			gpioWrite(PIN_LED_BLUE, HIGH);
			stateMachine.cooldownStartTime = Now();
		}
	}
	if (stateMachine.validPIN)
	{
		stateMachine.pStateAfterCooldownFunc = &State_Open;
		stateMachine.pStateFunc = &State_Cooldown;
		stateMachine.cooldownTicks = MS_TO_TICKS(5000);
		gpioWrite(PIN_LED_GREEN, LOW);
		stateMachine.cooldownStartTime = Now();
	}

}

void State_Open()
{
	gpioWrite(PIN_LED_GREEN, HIGH);
	MagBandEnableInt(true);
	WriteDisplay("Ingrese ID");
	stateMachine.validID = false;
	stateMachine.validPIN = false;
	stateMachine.pStateFunc = &State_Idle;
}

void State_Cooldown()
{
	const ticks now = Now();
	if (now - stateMachine.cooldownStartTime >= stateMachine.cooldownTicks)
	{
		if (stateMachine.pStateAfterCooldownFunc != NULL)
			stateMachine.pStateFunc = stateMachine.pStateAfterCooldownFunc;
		else
		{
			stateMachine.pStateFunc = &State_Idle;
		}
		ClearInputVariables();
	}
}

void State_UserMenu()
{
	uint8_t turnDir = 0;
	bool encoderStatus = readEncoderStatus();
	bool buttonStatus = readButtonStatus(buttonEncoder);
	MagBandEnableInt(false);

	if(encoderStatus)
	{
		// encoder
		turnDir = readEncoderData();
		if(turnDir && stateMachine.menuState < BRIGHTNESS)
		{
			// derecha
			WriteDisplay("Brillo");
			stateMachine.menuState++;
		}
		else if(!turnDir && stateMachine.menuState > CANCEL)
		{
			// izquierda
			WriteDisplay("Cancel");
			stateMachine.menuState--;
		}
	}

	if (buttonStatus)
	{
		uint8_t buttonData = readButtonData(buttonEncoder);
		// boton
		if(buttonData == BUTTON_PRESSED)
		{
			switch(stateMachine.menuState)
			{
			case CANCEL: 		// cancelar
				//WriteDisplay("Cancelar");
				stateMachine.pStateFunc = &State_Idle;
				ClearInputVariables();
				WriteDisplay("Ingrese Id");
				stateMachine.validID = false;
				stateMachine.validPIN = false;
				gpioWrite(PIN_LED_BLUE, HIGH);
				MagBandEnableInt(true);
				break;
			case BRIGHTNESS: 	// brillo
				WriteDisplay("Brillo");
				stateMachine.pStateFunc = &State_BrightnessMenu;
				stateMachine.brightnessLevel = DisplayGetBrightnessLevel();
			default:
				break;
			}
		}
		else if(buttonData == BUTTON_HELD || buttonData == BUTTON_LONG_HELD)
		{
			stateMachine.pStateFunc = stateMachine.pMenuCalledFrom;
			MagBandEnableInt(true);
			stateMachine.pMenuCalledFrom = NULL;
		}
		stateMachine.menuState = CANCEL;
	}


}

void State_BrightnessMenu()
{
	bool encoderStatus = readEncoderStatus();
	bool buttonStatus = readButtonStatus(buttonEncoder);
	if (encoderStatus)
	{
		uint8_t encoderData = readEncoderData();
		if (encoderData == RIGHT && stateMachine.brightnessLevel < 3)
		{
			stateMachine.brightnessLevel++;
		}
		else if (encoderData == LEFT && stateMachine.brightnessLevel > 1)
		{
			stateMachine.brightnessLevel--;
		}
		DisplaySetBrightnessLevel(stateMachine.brightnessLevel);
	}
	if (buttonStatus)
	{
		stateMachine.pStateFunc = stateMachine.pMenuCalledFrom;
		MagBandEnableInt(true);
	}
}

bool EnteringData(void)
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
			if(stateMachine.pStateFunc == &State_EnteringPin)
			{

				if(currentDigit >= 4)
				{
					char tempString[5] = {1,1,1,input_string[currentDigit],0};
					WriteDisplay(tempString);
				}
				else
				{
					char tempString[5] = {0,0,0,0,0};
					for(int i = 0 ; i < 4 ; i++)
					{
						if(i < currentDigit)
						{
							tempString[i] = 1; // nan
						}
						else
						{
							tempString[i] = input_string[currentDigit];
							break;
						}
					}
					WriteDisplay(tempString);
				}
			}
			else
			{
				if(currentDigit >= 4)
				{
					WriteDisplay(input_string+currentDigit-3);
				}
				else
				{
					WriteDisplay(input_string);
				}
			}
		}
		else if (buttonStatus)// BUTTON
		{
			uint8_t buttonData = readButtonData(buttonEncoder);
			if(buttonData == BUTTON_PRESSED)
			{
				strncat(input_string, &currentNum, 1);
				currentDigit++;
				if(stateMachine.pStateFunc == &State_EnteringPin)
				{

					if(currentDigit >= 4)
					{
						char tempString[5] = {1,1,1,input_string[currentDigit],0};
						WriteDisplay(tempString);
					}
					else
					{
						char tempString[5] = {0,0,0,0,0};
						for(int i = 0 ; i < 4 ; i++)
						{
							if(i < currentDigit)
							{
								tempString[i] = 1; // nan
							}
							else
							{
								tempString[i] = input_string[currentDigit];
								break;
							}
						}
						WriteDisplay(tempString);
					}
				}
				else
				{
					if(currentDigit >= 4)
					{
						WriteDisplay(input_string+currentDigit-3);
					}
					else
					{
						WriteDisplay(input_string);
					}
				}
			}
			else if(buttonData == BUTTON_HELD)
			{
				// Si holdeo el boton, debo procesar los datos
				return true;
			}
			else if(buttonData == BUTTON_LONG_HELD)
			{
				stateMachine.pStateFunc = &State_UserMenu;
				WriteDisplay("Menu");
			}
		}

	}
	else if(cardStatus)
	{
		memset((void*)input_string, 0, MAX_STRING_LENGHT);
		bandaMag_getID(input_string);
		WriteDisplay(input_string);

		// Si se pasa la tarjeta, evaluo el ID
		return true;
	}

	return false;
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

/*******************************************************************************
 ******************************************************************************/
