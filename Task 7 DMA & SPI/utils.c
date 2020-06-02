#include "stm32f0xx.h"
#include "utils.h"


void init_btns(void){
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER &= ~GPIO_MODER_MODER4_0;
	GPIOA->MODER &= ~GPIO_MODER_MODER5_0;
	
	GPIOC->MODER |= GPIO_MODER_MODER12_0;
	GPIOA->MODER |= GPIO_MODER_MODER15_0;
	
	//Pull up resistors
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_1;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4_0;
	
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_1;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5_0;
	
}

uint16_t get_x_coord(const uint16_t column){
	switch(column){
		case 0x8000:
			return 0;
		case 0x4000:
			return 1;
		case 0x2000:
			return 2;
		case 0x1000:
			return 3;
		case 0x800:
			return 4;
		case 0x400:
			return 5;
		case 0x200:
			return 6;
		case 0x100:
			return 7;
		default:
			return 100;
			
	}
}

uint16_t get_led_bit_pattern(const uint16_t y_coord){
	switch(y_coord){
		case 0:
			return 0x1;
		case 1:
			return 0x2;
		case 2:
			return 0x4;
		case 3:
			return 0x8;
		case 4:
			return 0x10;
		case 5:
			return 0x20;
		case 6:
			return 0x40;
		case 7:
			return 0x80;
		default:
			return 0x0;
	}
	
}

uint16_t get_max_y(Point* pts_arr, uint16_t size){
	uint16_t max_y = pts_arr[0].y;
	for(uint16_t i = 0; i<size; i++){
		if((pts_arr+i)->y >= max_y){
			max_y = (pts_arr+i)->y;
		}
	}
	return max_y;
}

uint16_t get_min_y(Point* pts_arr, uint16_t size){
	uint16_t min_y = pts_arr[0].y;
	for(uint16_t i = 0; i<size; i++){
		if((pts_arr+i)->y <= min_y){
			min_y = (pts_arr+i)->y;
		}
	}
	return min_y;
}

uint16_t get_min_x(Point* pts_arr, uint16_t size){
	uint16_t min_x = pts_arr[0].x;
	for(uint16_t i = 0; i<size; i++){
		if((pts_arr+i)->x <= min_x){
			min_x = (pts_arr+i)->x;
		}
	}
	return min_x;
}

uint16_t get_max_x(Point* pts_arr, uint16_t size){
	uint16_t max_x = pts_arr[0].x;
	for(uint16_t i = 0; i<size; i++){
		if((pts_arr+i)->x >= max_x){
			max_x = (pts_arr+i)->x;
		}
	}
	return max_x;
}
