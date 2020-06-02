#include "stm32f0xx.h"
void A_GROUP_on(const uint32_t pin_numb){
	switch(pin_numb){
		case 15:
			GPIOA->ODR |= GPIO_ODR_15;
			break;
		case 12:
			GPIOA->ODR |= GPIO_ODR_12;
			break;
		default:
			break;
	}
}

void C_GROUP_on(const uint32_t pin_numb){
	switch(pin_numb){
		case 15:
			GPIOC->ODR |= GPIO_ODR_15;
			break;
		case 12:
			GPIOC->ODR |= GPIO_ODR_12;
			break;
		default:
			break;
	}
}
void A_GROUP_off(const uint32_t pin_numb){
	switch(pin_numb){
		case 15:
			GPIOA->ODR &= ~GPIO_ODR_15;
			break;
		case 12:
			GPIOA->ODR &= ~GPIO_ODR_12;
			break;
		default:
			break;
	}
}



void C_GROUP_off(const uint32_t pin_numb){
	switch(pin_numb){
		case 15:
			GPIOC->ODR &= ~GPIO_ODR_15;
			break;
		case 12:
			GPIOC->ODR &= ~GPIO_ODR_12;
			break;
		default:
			break;
	}
}
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
