#include "set_GPOM.h"

void C_GROUP_set_GPOM(const uint32_t pin_numb){
	
	//set general purpose output mode for LED diode
	switch(pin_numb){
		case 6:
			GPIOC->MODER |= GPIO_MODER_MODER6_0; 
			break;
		case 7:
			GPIOC->MODER |= GPIO_MODER_MODER7_0; 
			break;
		case 8:
			GPIOC->MODER |= GPIO_MODER_MODER8_0; 
			break;
		case 9:
			GPIOC->MODER |= GPIO_MODER_MODER9_0;
			break;
		case 12:
			GPIOC->MODER |= GPIO_MODER_MODER12_0;
			break;
		default:
			break;
	}
}

void A_GROUP_set_GPOM(const uint32_t pin_numb){
	
	//set general purpose output mode for LED diode
	switch(pin_numb){
		case 6:
			GPIOA->MODER |= GPIO_MODER_MODER6_0; 
			break;
		case 7:
			GPIOA->MODER |= GPIO_MODER_MODER7_0; 
			break;
		case 8:
			GPIOA->MODER |= GPIO_MODER_MODER8_0; 
			break;
		case 9:
			GPIOA->MODER |= GPIO_MODER_MODER9_0;
			break;
		case 12:
			GPIOA->MODER |= GPIO_MODER_MODER12_0;
			break;
		case 15:
			GPIOA->MODER |= GPIO_MODER_MODER15_0;
			break;
		default:
			break;
	}
}
