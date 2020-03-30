#include "stm32f0xx.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\GROUP_on.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\GROUP_off.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\GROUP_read.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\GROUP_set_PDR.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\set_GPIM.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\set_GPOM.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\set_timing.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\leds.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\GROUP_set_PDR.h"


uint32_t active_keypad_pin = 15; // initial pin to put current in


//this val is applied in SysTickInit()
const uint32_t SysTickFreq = 350;

//buttons' macroses
const uint32_t PA15_PA4 = 0;
const uint32_t PA15_PA5 = 1;
const uint32_t PC12_PA4 = 2;
const uint32_t PC12_PA5 = 3;

uint32_t caught_btn_macros = 0;
uint32_t if_caught_btn = 0;
uint32_t INVALID_LED = 100;


void spi_init(){
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; //make tacktirovanie ON spi2
	//spi init
	SPI2->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;
	SPI2->CR2 = SPI_CR2_DS; //1111 = 16 BITS = word wide
	SPI2->CR1 |= SPI_CR1_SPE; //spi enable

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN; // make tactirovanie on ports a and b
	// now we must translate general-purpose ports in alternative functions
	//PA8 - SS
	GPIOA->MODER |= GPIO_MODER_MODER8_0; // here can be error
	//PB13 - SPI2_SCK
	//PB15 - SPI2_MOSI
	GPIOB->AFR[1] |= (0 << (4 * (13 - 8))) | (0 << (4 * (15 - 8))); // 0 - nuber of column of alternative func
	GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1; // we told that ports pb13 and pb15 now are managed using special system


}



typedef struct Point{
	uint16_t x;
	uint16_t y;
} Point;




uint16_t PA15_PA4_counter = 0;
uint16_t PA15_PA5_counter = 0;
uint16_t PC12_PA4_counter = 0;
uint16_t PC12_PA5_counter = 0;
uint16_t threshold_counter_val = 23;

void check_counter(uint16_t * const counter, const uint32_t macros){
	if(*counter >= threshold_counter_val){
		caught_btn_macros = macros;
		if_caught_btn = 1;
		*counter = 0;
	}
}

void check_btns_legs(const uint32_t active_keypad_pin){
	
	if (active_keypad_pin == 15){
		if(A_GROUP_read(4)){
			PA15_PA4_counter ++;
			check_counter(&PA15_PA4_counter, PA15_PA4);
			return;
		}
		else{
			PA15_PA4_counter = 0;
			if_caught_btn = 0;
		}
		if (A_GROUP_read(5)){ // upper btm = A5, active pin = 15
			PA15_PA5_counter ++;
			check_counter(&PA15_PA5_counter, PA15_PA5);
			return;
		}
		else{
			PA15_PA5_counter = 0;
			if_caught_btn = 0;
		}
	
	}
	else if (active_keypad_pin == 12){
		if(A_GROUP_read(4)){
			PC12_PA4_counter ++;
			check_counter(&PC12_PA4_counter, PC12_PA4);
			return;
		}
		else {
			PC12_PA4_counter = 0;
			if_caught_btn = 0;
		}
		if (A_GROUP_read(5)){
			PC12_PA5_counter ++;
			check_counter(&PC12_PA5_counter, PC12_PA5);
			return;
		}
		else{
			PC12_PA5_counter = 0;
			if_caught_btn = 0;
		}
	}
	
	//If we aint got no catch anything
	caught_btn_macros = INVALID_LED;
	if_caught_btn = 0;
	return;

}

void init_btns(){
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

Point pts_to_draw[] = {{1, 1}, {1, 2}, {1, 0}, {0,1}, {2, 1}}; // Init dot position
//Point pts_to_draw[] = {{0, 0}, {1, 1}, {2, 0}, {1,2}, {1, 3}}; // Init dot position

uint16_t pts_to_draw_size = 5;
uint16_t matrix_x_limit_left = 0;
uint16_t matrix_x_limit_right = 7;
uint16_t matrix_y_limit_up = 7;
uint16_t matrix_y_limit_down = 0;

uint16_t cur_led = 0x4;
uint16_t columns[] = {0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100};
uint16_t columns_size = 8;
uint16_t cur_column = 0x8000;

void change_column(){
	for (uint16_t i = 0; i< columns_size; ++i){
		if(columns[i] == cur_column && i < columns_size -1){
			cur_column = columns[i+1];
			return;
		}
		else if (columns[i] == cur_column && i == columns_size-1){
			cur_column = columns[0];
			return;
		}
	}
}

uint32_t timer_counter = 0;
uint32_t timer_counter_limit = 1;
uint32_t flag = 0;
void SysTick_Handler(void){
		
	if(active_keypad_pin == 15){
		//turn off the PA15 leg
		A_GROUP_off(active_keypad_pin);
		
		//turn on the PC12 leg
		C_GROUP_on(12);
		active_keypad_pin = 12;
				
	}
	else if (active_keypad_pin == 12){
		//turn off
		C_GROUP_off(active_keypad_pin);
		
		A_GROUP_on(15);
		active_keypad_pin = 15;
	}
	check_btns_legs(active_keypad_pin); // write in global variable if caught any signal from buttons outputs PA4, PA5
	
	//change current column
	timer_counter ++;
	if(timer_counter == timer_counter_limit){
			flag = 1;
			timer_counter = 0;
	}
}


void SysTickInit(){
	SystemCoreClockUpdate();
	
	SysTick->LOAD = SystemCoreClock / SysTickFreq - 1;
	
	SysTick->VAL = 0;
	
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk; 
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

uint16_t find_leds_on_column(){
	uint16_t res_leds_pattern = 0;
	uint16_t x_coord = get_x_coord(cur_column);
	
	for (uint16_t i = 0; i<pts_to_draw_size ; i++){
		if(pts_to_draw[i].x == x_coord){
			uint16_t bit_pattern = get_led_bit_pattern(pts_to_draw[i].y);
			res_leds_pattern |= bit_pattern;
		}
	}
	return res_leds_pattern;
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

void update_leds_coordinates(const uint32_t caught_btn_macros){
	
	if(caught_btn_macros == PA15_PA5){ //PA15_PA4 - down
		uint16_t max_y = get_max_y(&pts_to_draw[0], pts_to_draw_size); //max y coordinate in pts_to_draw configuration
			if(max_y < matrix_y_limit_up){
				for (uint16_t i = 0; i< pts_to_draw_size; i++){
					pts_to_draw[i].y +=1;
				}
			}
	}
	
	if(caught_btn_macros == PA15_PA4){ // PA15_PA5 - up
		uint16_t min_y = get_min_y(&pts_to_draw[0], pts_to_draw_size); //max y coordinate in pts_to_draw configuration
			if(min_y > matrix_y_limit_down){
				for (uint16_t i = 0; i< pts_to_draw_size; i++){
					pts_to_draw[i].y -=1;
				}
			}
	}
	
	if(caught_btn_macros == PC12_PA4){ // PC12_PA4 - left
		uint16_t min_x = get_min_x(&pts_to_draw[0], pts_to_draw_size); //max y coordinate in pts_to_draw configuration
			if(min_x > matrix_x_limit_left){
				for (uint16_t i = 0; i< pts_to_draw_size; i++){
					pts_to_draw[i].x -=1;
				}
			}
	}
	
	if(caught_btn_macros == PC12_PA5){ //PC12_PA5 RIGHT
		uint16_t max_x = get_max_x(&pts_to_draw[0], pts_to_draw_size); //max y coordinate in pts_to_draw configuration
			if(max_x < matrix_x_limit_right){
				for (uint16_t i = 0; i< pts_to_draw_size; i++){
					pts_to_draw[i].x +=1;
				}
			}
	}
	
}

int main()
{
	spi_init();
	SysTickInit();
	init_btns();
	C_GROUP_set_timing();
	A_GROUP_set_timing();
	
	A_GROUP_set_GPIM(4);
	A_GROUP_set_GPIM(5);
	
	C_GROUP_set_GPOM(12);
	A_GROUP_set_GPOM(15);
	
	A_GROUP_SET_PDR(4);
	A_GROUP_SET_PDR(5);
	
	
	A_GROUP_on(active_keypad_pin); // Give current on PA15
	
	while(1){
		
		if(flag == 1){
			flag = 0;
			change_column();
			uint16_t leds_pattern = find_leds_on_column(); //find in pts_to_draw any leds
			uint16_t res_pattern = 0;
			res_pattern |= cur_column;
			res_pattern |= leds_pattern;
			
			
			GPIOA->BSRR = GPIO_BSRR_BR_8; //LETCH ENABLE = 0

			if(! (SPI2->SR & SPI_SR_BSY))
			{
				SPI2->DR = res_pattern;//cur_column | cur_led;//0x8001;
			}
			while(SPI2->SR & SPI_SR_BSY);
			GPIOA->BSRR = GPIO_BSRR_BS_8; //LETCH ENABLE = 1
		}
		
		if (if_caught_btn){
			if_caught_btn = 0;
			//Here - UP, DOWN --> change pts_to_draw coordinates
			update_leds_coordinates(caught_btn_macros);
		}
		
	}
	
}



