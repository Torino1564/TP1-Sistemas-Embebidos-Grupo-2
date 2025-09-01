/*
 * Binary2BDC.c
 *
 *  Created on: Aug 25, 2025
 *      Author: jtori
 */

#include "Binary2BCD.h"
#include <assert.h>

static const bcd_data_t bcd_table[10] = {
    { .A = 1, .B = 1, .C = 1, .D = 1, .E = 1, .F = 1, .G = 0, .unused = 0 }, // 0
    { .A = 0, .B = 1, .C = 1, .D = 0, .E = 0, .F = 0, .G = 0, .unused = 0 }, // 1
    { .A = 1, .B = 1, .C = 0, .D = 1, .E = 1, .F = 0, .G = 1, .unused = 0 }, // 2
    { .A = 1, .B = 1, .C = 1, .D = 1, .E = 0, .F = 0, .G = 1, .unused = 0 }, // 3
    { .A = 0, .B = 1, .C = 1, .D = 0, .E = 0, .F = 1, .G = 1, .unused = 0 }, // 4
    { .A = 1, .B = 0, .C = 1, .D = 1, .E = 0, .F = 1, .G = 1, .unused = 0 }, // 5
    { .A = 1, .B = 0, .C = 1, .D = 1, .E = 1, .F = 1, .G = 1, .unused = 0 }, // 6
    { .A = 1, .B = 1, .C = 1, .D = 0, .E = 0, .F = 0, .G = 0, .unused = 0 }, // 7
    { .A = 1, .B = 1, .C = 1, .D = 1, .E = 1, .F = 1, .G = 1, .unused = 0 }, // 8
    { .A = 1, .B = 1, .C = 1, .D = 1, .E = 0, .F = 1, .G = 1, .unused = 0 }, // 9
};

bcd_data_t binary_to_bcd(uint8_t binary_data)
{
	return bcd_table[binary_data];
}
