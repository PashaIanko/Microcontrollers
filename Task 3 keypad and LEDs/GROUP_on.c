#include "GROUP_on.h"

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
