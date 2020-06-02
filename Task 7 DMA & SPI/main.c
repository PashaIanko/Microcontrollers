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
//#include "GROUP_ON_OFF_READ.c"
#include "spi_init.h"
#include "utils.h"
#include "configure_SPI_DMA.h"

uint32_t active_keypad_pin = 15; // initial pin to put current in


//this val is applied in SysTickInit()
const uint32_t SysTickFreq = 350;

//buttons' macroses
const uint32_t PA15_PA4 = 0;
const uint32_t PA15_PA5 = 1;
const uint32_t PC12_PA4 = 2;
const uint32_t PC12_PA5 = 3;

volatile uint32_t caught_btn_macros = 0;
volatile uint32_t if_caught_btn = 0;
uint32_t INVALID_LED = 100;



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
			PA15_PA4_counter ++; // DOWN
		
			check_counter(&PA15_PA4_counter, PA15_PA4);
		}
		else{
			PA15_PA4_counter = 0;
		}
		if (A_GROUP_read(5)){ // upper btm = A5, active pin = 15
			PA15_PA5_counter ++; // UP
			check_counter(&PA15_PA5_counter, PA15_PA5);
		}
		else{
			PA15_PA5_counter = 0;
		}
		return;
	
	}
	else if (active_keypad_pin == 12){
		if(A_GROUP_read(4)){
			PC12_PA4_counter ++; // LEFT
			check_counter(&PC12_PA4_counter, PC12_PA4);
		}
		else {
			PC12_PA4_counter = 0;
		}
		if (A_GROUP_read(5)){
			PC12_PA5_counter ++; // RIGHT
			check_counter(&PC12_PA5_counter, PC12_PA5);
		}
		else{
			PC12_PA5_counter = 0;
		}
		return;
	}
	
	//If we aint got no catch anything
	caught_btn_macros = INVALID_LED;
	if_caught_btn = 0;
	return;

}


//Point pts_to_draw[] = {{1, 1}, {1, 2}, {1, 0}, {0,1}, {2, 1}}; // Init dot position
uint32_t DEFAULT_ADC_VALUE = 0;
Point pts_to_draw[] = {
{0, 0, 0, 0}, 
{1, 0, 0, 0}, 
{2, 0, 0, 0}, 
{3, 0, 0, 0}, 
{4, 0, 0, 0}, 
{5, 0, 0, 0},
{6, 0, 0, 0},
{7, 0, 0, 0}}; // Init dot position

//Point pts_to_draw[] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};

uint16_t pts_to_draw_size = 8;
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
uint32_t timer_counter_limit = 25;
uint32_t timer_counter_flag = 0;

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
			
			timer_counter = 0;
			timer_counter_flag = 1;
	}
}


void SysTickInit(){
	SystemCoreClockUpdate();
	
	SysTick->LOAD = SystemCoreClock / SysTickFreq - 1;
	
	SysTick->VAL = 0;
	
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk; 
}

uint16_t get_led_pattern(uint32_t ADC_value){
	uint32_t lim1 = 0;
	uint32_t lim2 = 70;
	uint32_t lim3 = 140;
	uint32_t lim4 = 210;
	uint32_t lim5 = 280;
	uint32_t lim6 = 350;
	uint32_t lim7 = 420;
	uint32_t lim8 = 490;
	//uint32_t lim9 = 560;
	if (ADC_value >lim1 && ADC_value <lim2){
		return 0x1;
	}
	if (ADC_value >lim2 && ADC_value <lim3){
		return 0x2;
	}
	if (ADC_value >lim3 && ADC_value <lim4){
		return 0x4;
	}
	if (ADC_value >lim4 && ADC_value <lim5){
		return 0x8;
	}
	if (ADC_value >lim5 && ADC_value <lim6){
		return 0x10;
	}
	if (ADC_value >lim6 && ADC_value <lim7){
		return 0x20;
	}
	if (ADC_value >lim7 && ADC_value <lim8){
		return 0x40;
	}
	if (ADC_value >lim8){
		return 0x80;
	}
	return 0x0;
}


uint16_t find_leds_on_column(){
	uint16_t res_leds_pattern = 0;
	uint16_t x_coord = get_x_coord(cur_column);
	
	for (uint16_t i = 0; i<pts_to_draw_size ; i++){
		if(pts_to_draw[i].x == x_coord){
			//uint16_t bit_pattern = get_led_bit_pattern(pts_to_draw[i].y);
			uint16_t bit_pattern = get_led_pattern(pts_to_draw[i].ADC_value);
			res_leds_pattern |= bit_pattern;
		}
	}
	return res_leds_pattern;
}



void update_leds_coordinates(const uint32_t caught_btn_macros){
	
	if(caught_btn_macros == PA15_PA5){ //PA15_PA4 - UP
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


uint16_t res_pattern = 0;
void SPI2_IRQHandler(void){
	uint16_t spi_2_sr = SPI2->SR;
		

	if((spi_2_sr  & SPI_SR_RXNE))
	{
		
		//clear flag
		change_column();
		volatile uint16_t temp = SPI2->DR;
		volatile uint16_t leds_pattern = find_leds_on_column(); //find in pts_to_draw any leds
		res_pattern = 0;
		res_pattern |= cur_column;
		res_pattern |= leds_pattern;
		
		GPIOA->BSRR = GPIO_BSRR_BS_8;//LE = 1
		SPI2->DR = res_pattern;
		GPIOA->BSRR = GPIO_BSRR_BR_8;//LE = 0
				
	}
}

void mask_data(uint32_t * data_arr, uint32_t arr_size, uint32_t mask){
	for(uint32_t i = 0; i<arr_size; i++){
		data_arr[i] &= mask;
	}
}

uint32_t data_arr_size = 16;
uint32_t data_arr[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t MAX_UINT32_T = 4294967290;
uint32_t mean_value = 0;
uint32_t if__buffer_filled = 0;

uint32_t calc_mean_value(uint32_t* data_arr, const uint32_t max_size){
	uint32_t sum = 0;
	uint32_t counter = 0;
	for(uint32_t i = 0; i < max_size; ++i ){
		if(sum <= MAX_UINT32_T){
			sum += data_arr[i];
			counter++;
		}
		else{
			return 0;
		}
	}
	return (uint32_t)(sum / counter);
}


void DMA1_Channel1_IRQHandler(void){
	
	
	if(DMA1->ISR & DMA_ISR_HTIF1){
		//This flag is cleared by software
		DMA1->IFCR |= DMA_IFCR_CHTIF1;
		if__buffer_filled = 1;
		
		uint32_t mask = 0xFFFF;
		mask_data(data_arr, data_arr_size, mask);
		
		volatile uint32_t data = data_arr[0];
		data = data_arr[1];
		data = data_arr[2];
		data = data_arr[3];
		data = data_arr[4];
		data = data_arr[5];
		data = data_arr[6];
		data = data_arr[7];
		
		
		mean_value = calc_mean_value(data_arr, data_arr_size / 2);
		
		
		
		// Here we for some reason get 026B026B instead of 026B
	}
	if(DMA1->ISR & DMA_ISR_TCIF1){
		const volatile int flag1 = 1;
		DMA1->IFCR |= DMA_IFCR_CTCIF1;
	}
	if(DMA1->ISR & DMA_ISR_TEIF1){
		const volatile int flag1 = 1;
		DMA1->IFCR |= DMA_IFCR_CTEIF1;
	}
	if(DMA1->ISR & DMA_ISR_GIF1){
		const volatile int flag1 = 1;
		DMA1->IFCR |= DMA_IFCR_CGIF1;
	}
	
}	

uint8_t all_filled(void){
	for(uint16_t i = 0; i < pts_to_draw_size; ++i){
		if(pts_to_draw[i].filled == 0){
			return 0;
		}
	}
	return 1;
}

void update_points(void){
	if(all_filled()){
		for(uint16_t i = 0; i < pts_to_draw_size-1; ++i){
			pts_to_draw[i].ADC_value = pts_to_draw[i+1].ADC_value;
		}
		pts_to_draw[pts_to_draw_size-1].ADC_value = mean_value;
		return;
	}
	else{
		for(uint16_t i = 0; i < pts_to_draw_size; ++i){
		if(pts_to_draw[i].filled == 0){
			pts_to_draw[i].ADC_value = mean_value;
			pts_to_draw[i].filled=1;
			return;
		}
	}
	}

	
}

int main()
{
	
	
	// Command to the interrupt controller - turn on the interrupt handler!	
	NVIC_EnableIRQ(SPI2_IRQn);
	
	//Allow periphery to generate interrupt requests
	// control bits TXEIE RXNEIE
	
	SysTickInit();
	init_btns();
	spi_init();
	
	
	configure_SPI_DMA(data_arr, data_arr_size);

	
	A_GROUP_on(active_keypad_pin); // Give current on PA15
	SPI2->DR = res_pattern;
	while(1){
		while(ADC1->CR & ADC_CR_ADSTART) 
		{
			// still ongoing conversion
		};
		
		ADC1->CR |= ADC_CR_ADSTART;
		
		if (if_caught_btn){
			if_caught_btn = 0;
			update_leds_coordinates(caught_btn_macros);
		}
		if(if__buffer_filled && timer_counter_flag){
			if__buffer_filled = 0; // if mean value is calculated --> we update the points values
			timer_counter_flag = 0;
						
			/*uint32_t mask = 0xFFFF;
			mask_data(data_arr, data_arr_size, mask);
			mean_value = calc_mean_value(data_arr, data_arr_size / 2);*/
			update_points();
		}
		
	}
	
}



