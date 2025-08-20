/***************************************************************************//**
  @file     StateMachine.h
  @brief    Main state machine definition. All time definitions are in milliseconds
  @author   Group 2
 ******************************************************************************/

#ifndef _STATE_MACHINE_
#define _STATE_MACHINE_

enum States
{
	IDLE,
	PIN,
	OPEN,
	COOLDOWN,
	CHANGE_PIN,
	ADMIN
};

typedef struct {
	// Estado actual
	uint8_t state = IDLE;
	uint8_t stateAfterCooldown = IDLE;

	// Cooldown
	uint8_t cooldownTime = 1000;
	uint32_t cooldownTicks = 0;

	// Timeout
	uint8_t maxTimeout = 10000;
	uint32_t timeoutTicks = 0;

	// Variables
	bool validID = 0;
	bool validPIN = 0;
	uint8_t remainingAttemps = 3;
}
StateMachine;

#endif
