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
	uint8_t state;
	uint8_t stateAfterCooldown;

	// Cooldown
	uint8_t cooldownTime;
	uint32_t cooldownTicks;

	// Timeout
	uint8_t maxTimeout;
	uint32_t timeoutTicks;

	// Variables
	bool validID;
	bool validPIN;
	uint8_t remainingAttemps;
}
StateMachine;

#endif
