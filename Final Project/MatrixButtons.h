#ifndef _MATRIX_BUTTONS_H
#define _MATRIX_BUTTONS_H

#include "stdint.h"

void user_button_init(void);
void matrix_button_init(void);

typedef struct{
	uint8_t up;
	uint8_t down;
	uint8_t left;
	uint8_t right;
} Buttons;

uint8_t read_user_button(void);
Buttons read_outer_buttons(void);

#endif
