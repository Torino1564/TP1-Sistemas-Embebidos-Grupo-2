/*
 * AsciiToSeg7.c
 *
 *  Created on: Aug 25, 2025
 *  Author: jtori & jpla
 */

#include "AsciiToSeg7.h"
#include <assert.h>

static const seg7_t seg7_table[38] = {
    // Dígitos 0–9
    {1,1,1,1,1,1,0,0}, // 0
    {0,1,1,0,0,0,0,0}, // 1
    {1,1,0,1,1,0,1,0}, // 2
    {1,1,1,1,0,0,1,0}, // 3
    {0,1,1,0,0,1,1,0}, // 4
    {1,0,1,1,0,1,1,0}, // 5
    {1,0,1,1,1,1,1,0}, // 6
    {1,1,1,0,0,0,0,0}, // 7
    {1,1,1,1,1,1,1,0}, // 8
    {1,1,1,1,0,1,1,0}, // 9

    // Letras A–Z
    {1,1,1,0,1,1,1,0}, // A
    {0,0,1,1,1,1,1,0}, // b
    {1,0,0,1,1,1,0,0}, // C
    {0,1,1,1,1,0,1,0}, // d
    {1,0,0,1,1,1,1,0}, // E
    {1,0,0,0,1,1,1,0}, // F
    {1,0,1,1,1,1,0,0}, // G
    {0,1,1,0,1,1,1,0}, // H
    {0,0,0,0,1,1,0,0}, // I (≈ l)
    {0,1,1,1,1,0,0,0}, // J
    {0,0,1,0,1,1,1,0}, // K (≈ k, se aproxima a H)
    {0,0,0,1,1,1,0,0}, // L
    {1,1,1,0,1,1,0,0}, // M (≈ n, se aproxima)
    {0,0,1,0,1,0,1,0}, // n
    {1,1,1,1,1,1,0,0}, // O
    {1,1,0,0,1,1,1,0}, // P
    {1,1,1,0,0,1,1,0}, // Q (≈ muestra como 9)
    {0,0,0,0,1,0,1,0}, // r
    {1,0,1,1,0,1,1,0}, // S
    {0,0,0,1,1,1,1,0}, // T
    {0,1,1,1,1,1,0,0}, // U
    {0,1,1,1,1,1,0,0}, // V (≈ U)
    {0,1,1,1,1,1,0,0}, // W (≈ U)
    {0,1,1,0,1,1,1,0}, // X (≈ H)
    {0,1,1,1,0,1,1,0}, // Y
    {1,1,0,1,1,0,1,0}, // Z


	// Caracteres Especiales
    {0,0,0,0,0,0,0,0}, // ' '
    {0,0,0,0,0,0,1,0},  //  -
};

seg7_t binary_to_seg7(uint8_t binary_data)
{
	return seg7_table[binary_data];
}

seg7_t ascii_to_seg7(char caracter)
{
	if(caracter >= 'a' && caracter <= 'z')
	{
		return seg7_table[caracter - 'a' + CARACTER_OFFSET];
	}
	else if (caracter >= 'A' && caracter <= 'Z')
	{
		return seg7_table[caracter - 'A' + CARACTER_OFFSET];
	}
	else if (caracter == ' ')
	{
		return seg7_table[36]; // numero magico
	}
	else
	{
		return seg7_table[37]; // guion
	}
}
