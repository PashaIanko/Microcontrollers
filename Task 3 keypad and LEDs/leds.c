#include "leds.h"

void led_on(const uint32_t pin_numb){
	
	switch(pin_numb){
		case 6:
			GPIOC->ODR |= GPIO_ODR_6;
			break;
		case 7:
			GPIOC->ODR |= GPIO_ODR_7;
			break;
		case 8:
			GPIOC->ODR |= GPIO_ODR_8;
			break;
		case 9:
			GPIOC->ODR |= GPIO_ODR_9;
			break;
		default:
			break;
		
	}
	return;

}

void led_off(const uint32_t pin_numb){
	
	switch(pin_numb){
		case 6:
			GPIOC->ODR &= ~GPIO_ODR_6;
			break;
		case 7:
			GPIOC->ODR &= ~GPIO_ODR_7;
			break;
		case 8:
			GPIOC->ODR &= ~GPIO_ODR_8;
			break;
		case 9:
			GPIOC->ODR &= ~GPIO_ODR_9;
			break;
		default:
			break;
	}
	return;
	
}
