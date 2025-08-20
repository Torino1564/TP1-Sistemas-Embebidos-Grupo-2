/***************************************************************************//**
  @file     encoder.h
  @brief    Header for the driver of the encoder
  @author   Group 2
 ******************************************************************************/

typedef struct {
	bool rotDir = 1; // 0 counter clockwise / 1 clockwise
	uint32_t outA = 0;
	uint32_t outB = 0;
} encoder_t;
