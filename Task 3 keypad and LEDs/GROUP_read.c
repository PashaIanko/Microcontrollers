#include "GROUP_read.h"

const uint8_t A_GROUP_READ_WRONG_PARAM = 2;

uint8_t A_GROUP_read(const uint32_t pin_numb){
	switch(pin_numb){
		case 5:
			return (GPIOA->IDR & GPIO_IDR_5) >> 5;
		case 4:
			return (GPIOA->IDR & GPIO_IDR_4) >> 4;
		default:
			return A_GROUP_READ_WRONG_PARAM;
	}
}
