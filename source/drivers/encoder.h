/***************************************************************************//**
  @file     encoder.h
  @brief    Header for the driver of the encoder
  @author   Group 2
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	bool rotDir; // 0 counter clockwise / 1 clockwise
	uint32_t outA;
	uint32_t outB;
} encoder_t;
