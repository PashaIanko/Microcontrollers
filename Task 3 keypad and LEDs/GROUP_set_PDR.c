#include "GROUP_set_PDR.h"


void A_GROUP_SET_PDR(const uint32_t pin_numb){
	switch(pin_numb){
		case 4:
			//10 - pull down
			GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_1;
			GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4_0;
			break;
		case 5:
			GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_1;
			GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5_0;
			break;
		default:
			break;
	}
}

