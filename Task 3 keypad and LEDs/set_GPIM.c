#include "set_GPIM.h"

void A_GROUP_set_GPIM(const uint32_t pin_numb){
	
	//set general purpose input mode for A group - 00 bits
	switch(pin_numb){
		
		case 4:
			GPIOA->MODER &= ~GPIO_MODER_MODER4_0;
			break;
		
		case 5:
			GPIOA->MODER &= ~GPIO_MODER_MODER5_0;
			break;
		
		default:
			break;
	}
}
