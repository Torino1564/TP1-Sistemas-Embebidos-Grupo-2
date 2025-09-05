/*
 * AsciiToSeg7.h
 *
 *  Created on: Aug 25, 2025
 *  Author: jtori & jpla
 */

#ifndef APP_ASCII2SEG7_H_
#define APP_ASCII2SEG7_H_

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
} seg7_t;

seg7_t binary_to_seg7(uint8_t binary_data);

seg7_t ascii_to_seg7(char caracter);

#define CARACTER_OFFSET 10

#endif /* APP_ASCII2SEG7_H_ */
