/***************************************************************************//**
  @file     gpio.c Rev2.0
  @brief    gpio perisferics functions
  @author   Group 2
 ******************************************************************************/

#include "gpio.h"
#include "hardware.h"

static void (*callbackMatrix[160])(void) = {0};

/*******************************************************************************
 *                                FUNCTIONS
 ******************************************************************************/

void gpioSetupISR(pin_t pin, uint8_t interrupt_mode, void (*pCallback)())
{
	uint32_t portValue = (interrupt_mode<<PORT_PCR_IRQC_SHIFT);
	uint32_t portMask = PORT_PCR_IRQC_MASK;

	static PORT_Type * const portBase[] = PORT_BASE_PTRS;

	portBase[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] = (portBase[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] & ~portMask) | portValue;

	callbackMatrix[pin] = pCallback;
}

void gpioMode (pin_t pin, uint8_t mode)
{
	uint32_t portValue = mode | (1<<PORT_PCR_MUX_SHIFT); // el valor que debo ingresar al port
	uint32_t portMask = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_MUX_MASK; // mascara para modificar el pull enable, el pull set y el mux

	static PORT_Type * const portBase[] = PORT_BASE_PTRS;

	portBase[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] = ((portBase[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] & ~portMask) | portValue);

	uint32_t gpioValue = 0;
	if(mode == OUTPUT)
	{
		gpioValue = (1<<PIN2NUM(pin));
	}
	uint32_t gpioMask = (1<<PIN2NUM(pin)); // mascara para modificar el bit numero pin del PDDR. Este sirve para setear si es input u output

	static GPIO_Type * const gpioBase[] = GPIO_BASE_PTRS;

	gpioBase[PIN2PORT(pin)]->PDDR = ((gpioBase[PIN2PORT(pin)]->PDDR & ~gpioMask) | gpioValue); // Si es entrada o salida en el GPIO

}


void gpioWrite (pin_t pin, bool value)
{

	static PORT_Type * const portBase[] = PORT_BASE_PTRS;

	uint32_t perisferic = portBase[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] & PORT_PCR_MUX_MASK;
	perisferic = perisferic >> PORT_PCR_MUX_SHIFT;
	if(!(perisferic == 0b001))
	{
		return; // si el puerto no esta configurado como gpio saltea el resto
	}

	static GPIO_Type * const gpioBase[] = GPIO_BASE_PTRS; //arreglo de punteros a struct tipo GPIO_Type

	uint32_t gpioMask = (1<<PIN2NUM(pin)); // mascara para modificar el bit numero pin del PDOR
	uint32_t gpioValue = (((uint32_t)value)<<PIN2NUM(pin)); //ubico el valor en la pos pin

	gpioBase[PIN2PORT(pin)]->PDOR = ((gpioBase[PIN2PORT(pin)]->PDOR & ~gpioMask) | gpioValue);
}


void gpioToggle (pin_t pin)
{
	static PORT_Type * const portBase[] = PORT_BASE_PTRS;

	uint32_t perisferic = portBase[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] & PORT_PCR_MUX_MASK;
	perisferic = perisferic >> PORT_PCR_MUX_SHIFT;
	if(!(perisferic == 0b001))
	{
		return; // si el puerto no esta configurado como gpio saltea el resto
	}

	static GPIO_Type * const gpioBase[] = GPIO_BASE_PTRS;

	uint32_t gpioMask = (1<<PIN2NUM(pin));

	gpioBase[PIN2PORT(pin)]->PDOR = gpioBase[PIN2PORT(pin)]->PDOR ^ gpioMask;

}


bool gpioRead (pin_t pin)
{
	static GPIO_Type * const gpioBase[] = GPIO_BASE_PTRS;

	uint32_t gpioMask = (1<<PIN2NUM(pin));

	bool result = (gpioBase[PIN2PORT(pin)]->PDIR & gpioMask)>>PIN2NUM(pin);

	return result;
}


__ISR__ PORTA_IRQHandler(void)
{
	static PORT_Type * const portBase[] = PORT_BASE_PTRS;
	uint32_t ISFR = portBase[PA]->ISFR;
	uint8_t contador = 0;
	while(!(ISFR & 0x01))
	{
		ISFR = ISFR>>1;
		contador++;
	}
	//que debo hacer? borro el flag o no?

	portBase[PA]->ISFR = 1<<contador;

	callbackMatrix[PA*32 + contador]();

}

__ISR__ PORTB_IRQHandler(void)
{
	static PORT_Type * const portBase[] = PORT_BASE_PTRS;
	uint32_t ISFR = portBase[PB]->ISFR;
	uint8_t contador = 0;
	while(!(ISFR & 0x01))
	{
		ISFR = ISFR>>1;
		contador++;
	}
	//que debo hacer? borro el flag o no?

	portBase[PB]->ISFR = 1<<contador;

	callbackMatrix[PB*32 + contador]();
}

__ISR__ PORTC_IRQHandler(void)
{

}

__ISR__ PORTD_IRQHandler(void)
{

}

__ISR__ PORTE_IRQHandler(void)
{

}
