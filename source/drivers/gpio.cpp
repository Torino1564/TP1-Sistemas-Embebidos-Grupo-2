#include "gpio.h"
#include "hardware.h"

static const GPIO_Type* gpioArray[5] = GPIO_BASE_PTRS;
static const PORT_Type* portArray[5] = PORT_BASE_PTRS;

PORT_Type* getPort(pin_t pin)
{
	const uint8_t portNum = PIN2PORT(pin);
	return portArray[portNum];
}

static void Empty() {}

static void(*pCallbacks[5*32])() = {&Empty};
#define CALLBACK(portNum, pinNum) pCallbacks[portNum*32 + pinNum]


void gpioMode (pin_t pin, uint8_t mode)
{
	// Electrical setting. Use PCR
	const uint8_t pinNum = PIN2NUM(pin);
	const uint8_t portNum = PIN2PORT(pin);

	PORT_Type* pPortBase = portArray[portNum];
	uint32_t* pcr = &pPortBase->PCR[pinNum];
	uint32_t mask = 0;


	GPIO_Type* pGpio = gpioArray[portNum];

	// MUX set to Alt1
	mask |= PORT_PCR_MUX(1);

	if (mode == INPUT_PULLUP || mode == INPUT_PULLDOWN)
	{
		mask |= PORT_PCR_PE(1u);
		if (mode == INPUT_PULLUP)
		{
			mask |= PORT_PCR_PS(1u);
		}
	}

	*pcr = mask;

	// Configure GPIO
	uint32_t* pPDDR = &pGpio->PDDR;
	const uint32_t value = (mode == INPUT) << pinNum;
	uint32_t pddr = (~*pPDDR & value) | (*pPDDR & value);
	*pPDDR = pddr;
}

void gpioWrite(pin_t pin, bool value)
{
	const uint8_t pinNum = PIN2NUM(pin);
	const uint8_t portNum = PIN2PORT(pin);
	GPIO_Type* pGpio = gpioArray[portNum];
	if (value)
	{
		pGpio->PSOR = (uint32_t)(1u << pinNum);
	}
	else
	{
		pGpio->PCOR = (uint32_t)(1u << pinNum);
	}
}

void gpioToggle (pin_t pin)
{
	const uint8_t pinNum = PIN2NUM(pin);
	const uint8_t portNum = PIN2PORT(pin);
	GPIO_Type* pGpio = gpioArray[portNum];
	pGpio->PTOR = (uint32_t)(1u << pinNum);
}

bool gpioRead (pin_t pin)
{
	const uint8_t pinNum = PIN2NUM(pin);
	const uint8_t portNum = PIN2PORT(pin);
	const GPIO_Type* pGpio = gpioArray[portNum];
	return pGpio->PTOR;
}


void gpioSetupISR(pin_t pin, uint8_t interrupt_mode, void (*pCallback)())
{
	const uint8_t pinNum = PIN2NUM(pin);
	const uint8_t portNum = PIN2PORT(pin);

	const PORT_Type* pPortBase = portArray[portNum];
	uint32_t* pcr = &pPortBase->PCR[pinNum];
	*pcr |= PORT_PCR_IRQC(interrupt_mode);
}
