#ifndef _MATRIX_LEDS_H
#define _MATRIX_LEDS_H

#include "stdint.h"

void spi_init(void);

uint8_t get_matrix_size(void);

uint16_t position_code(uint8_t x, uint8_t y);

uint16_t led_cross_on(uint8_t x, uint8_t y);
uint16_t led_h_line_on(uint8_t x, uint8_t y);
uint16_t led_v_line_on(uint8_t x, uint8_t y);


#endif
