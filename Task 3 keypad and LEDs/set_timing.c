#include "set_timing.h"


void C_GROUP_set_timing(){
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 
}

void A_GROUP_set_timing(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
}
