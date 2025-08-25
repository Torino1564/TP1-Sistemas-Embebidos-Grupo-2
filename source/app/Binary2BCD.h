/*
 * Binary2BCD.h
 *
 *  Created on: Aug 25, 2025
 *      Author: jtori
 */

#ifndef APP_BINARY2BCD_H_
#define APP_BINARY2BCD_H_

#include <stdint.h>

typedef struct
{
	uint8_t A			: 		1;
	uint8_t B			: 		1;
	uint8_t C			: 		1;
	uint8_t D			: 		1;
	uint8_t E			: 		1;
	uint8_t F			: 		1;
	uint8_t G			: 		1;
	uint8_t unused		: 		1;
} bdc_data_t;

bdc_data_t binary_to_bcd(uint8_t binary_data);

#endif /* APP_BINARY2BCD_H_ */
