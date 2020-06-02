#include "stm32f0xx.h"
#include "stdint.h"

typedef struct Point{
	uint16_t x;
	uint16_t y;
	uint32_t ADC_value;
	uint32_t filled;
} Point;

uint16_t get_max_x(Point* pts_arr, uint16_t size);
uint16_t get_min_x(Point* pts_arr, uint16_t size);
uint16_t get_min_y(Point* pts_arr, uint16_t size);
uint16_t get_max_y(Point* pts_arr, uint16_t size);
uint16_t get_led_bit_pattern(const uint16_t y_coord);
uint16_t get_x_coord(const uint16_t column);
void init_btns(void);

