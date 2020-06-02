#include "stm32f0xx.h"
#include "MatrixButtons.h"

void user_button_init(void){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODER0;
}
void matrix_button_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
	//analyzing ports
	GPIOA->MODER &= ~GPIO_MODER_MODER4;//setting intput mode
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4;	//setting
	GPIOA->PUPDR |=  GPIO_PUPDR_PUPDR4_1;//pull down
	
	GPIOA->MODER &= ~GPIO_MODER_MODER5;//setting intput mode
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5;	//setting
	GPIOA->PUPDR |=  GPIO_PUPDR_PUPDR5_1;//pull down
	
	//scanning ports
	GPIOA->MODER &= ~GPIO_MODER_MODER15;	//setting
	GPIOA->MODER |=  GPIO_MODER_MODER15_0;//output mode
	
	GPIOC->MODER &= ~GPIO_MODER_MODER12;	//setting
	GPIOC->MODER |=  GPIO_MODER_MODER12_0;//output mode
}

uint8_t read_user_button(void){
	return GPIOA->IDR & GPIO_IDR_0;
}
Buttons read_outer_buttons(){
	static uint8_t mode = 0;
	Buttons buttons = { 0, 0, 0, 0 };
	
	switch (mode){
		case 0:
				GPIOC->ODR |= GPIO_ODR_12;
				buttons.right = (GPIOA->IDR & GPIO_IDR_4) >> 4;
				buttons.left = (GPIOA->IDR & GPIO_IDR_5) >> 5;
				GPIOC->ODR &= ~GPIO_ODR_12;
				mode++;
				break;
		case 1:
				GPIOA->ODR |= GPIO_ODR_15;
				buttons.up 		= (GPIOA->IDR & GPIO_IDR_4) >> 4;
				buttons.down 	= (GPIOA->IDR & GPIO_IDR_5) >> 5;
				GPIOA->ODR &= ~GPIO_ODR_15;
				mode = 0;
				break;
	}
	
	return buttons;
}
