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
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 7 DMA & SPI\spi_init.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 3 keypad and LEDs\GROUP_set_PDR.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 7 DMA & SPI\utils.h"
#include "C:\Users\79137\Pasha\6 semester\Microcontrollers\Tasks\Task 7 DMA & SPI\configure_SPI_DMA.h"


//this val is applied in SysTickInit()
const uint32_t SysTickFreq = 350;



uint32_t DEFAULT_ADC_VALUE = 0;
uint16_t pts_to_draw_size = 8;
Point pts_to_draw[] = {
{0, 0, 0, 0}, 
{1, 0, 0, 0}, 
{2, 0, 0, 0}, 
{3, 0, 0, 0}, 
{4, 0, 0, 0}, 
{5, 0, 0, 0},
{6, 0, 0, 0},
{7, 0, 0, 0}}; // Init dot position




uint16_t columns[] = {0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100};
uint16_t columns_size = 8;
uint16_t cur_column = 0x8000;



uint32_t timer_counter = 0;
uint32_t timer_counter_limit = 50;
uint32_t timer_counter_flag = 0;

void SysTick_Handler(void){

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
	if (ADC_value >lim7){// && ADC_value <lim8){
		return 0x40;
	}
	if (ADC_value >lim8){
		return 0x80;
	}
	return 0x0;
}

void change_column(uint16_t* columns, uint16_t columns_size){
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

uint16_t get_from_buffer(){
	//clear flag
	change_column(columns, columns_size);
	
	uint16_t res_leds_pattern = 0;
	uint16_t x_coord = get_x_coord(cur_column);
	
	for (uint16_t i = 0; i<pts_to_draw_size ; i++){
		if(pts_to_draw[i].x == x_coord){
			//uint16_t bit_pattern = get_led_bit_pattern(pts_to_draw[i].y);
			uint16_t bit_pattern = get_led_pattern(pts_to_draw[i].ADC_value);
			res_leds_pattern |= bit_pattern;
		}
	}
	res_leds_pattern |= cur_column;
	return res_leds_pattern;
}




uint16_t res_pattern = 0;
void SPI2_IRQHandler(void){
	uint16_t spi_2_sr = SPI2->SR;
	if((spi_2_sr  & SPI_SR_RXNE))
	{
		
		volatile uint16_t temp = SPI2->DR;
		volatile uint16_t res_pattern = get_from_buffer(); //find in pts_to_draw any leds
			
		GPIOA->BSRR = GPIO_BSRR_BS_8;//LE = 1
		SPI2->DR = res_pattern;
		GPIOA->BSRR = GPIO_BSRR_BR_8;//LE = 0
				
	}
}





uint32_t data_arr_size = 16;
uint32_t data_arr[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

uint32_t mean_value = 0;
uint32_t if__buffer_filled = 0;
uint32_t left_half_filled = 0;
uint32_t right_half_filled = 0;

void DMA1_Channel1_IRQHandler(void){
	
	
	if(DMA1->ISR & DMA_ISR_HTIF1){
		//This flag is cleared by software
		DMA1->IFCR |= DMA_IFCR_CHTIF1;
		if__buffer_filled = 1;
		
		mean_value = calc_mean_value(data_arr, 0, data_arr_size/2);
		if(left_half_filled == 0 && right_half_filled == 0){
			left_half_filled = 1;
			right_half_filled = 0;
		}
		else if(left_half_filled == 1 && right_half_filled == 0){
			left_half_filled = 0;
			right_half_filled = 1;
		}
		else if(right_half_filled == 1 && left_half_filled == 0){
			left_half_filled = 1;
			right_half_filled = 0;
		}
		
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


void configure_led(void){
	
	/*GPIOB PIN 0 */ 
	RCC->AHBENR  |= RCC_AHBENR_GPIOCEN; 	
	GPIOC->MODER |= GPIO_MODER_MODER8_1;	//Alternative function mode
	// We do not need to set the AF, because AF0 is set by default - from data sheet we know, that AF0 is a TIM3_CH1, 2, 3, 4 function
	//GPIOC->AFR[0] |= 0x01 << GPIO_AFRL_AFRL0_Pos; // PC0 - AFR1 (TIM3_CH3) AFR[0] == AFRL
}

void configure_TIM3(void){
	RCC->APB1ENR |=RCC_APB1ENR_TIM3EN;
	TIM3->ARR = 100; 																			// 
	
	TIM3->CCR3 = 0;  // Capture-Compare register
	TIM3->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE; // PWM mode 1, page 416
	TIM3->CCER |= TIM_CCER_CC3E; // OC output enable
 	
	TIM3->EGR |= TIM_EGR_UG; // initialize all the registers
	TIM3->CR1 |= TIM_CR1_CEN; // Enable counter
}

int main()
{
	// Command to the interrupt controller - turn on the interrupt handler!	
	NVIC_EnableIRQ(SPI2_IRQn);
	SysTickInit();
	spi_init();
	configure_SPI_DMA(data_arr, data_arr_size);
	configure_led();
	configure_TIM3();
	

	
	SPI2->DR = res_pattern;
	while(1){
		while(ADC1->CR & ADC_CR_ADSTART) 
		{
			// still ongoing conversion
		};
		
		ADC1->CR |= ADC_CR_ADSTART;
		
		
		if(if__buffer_filled && timer_counter_flag){
			uint32_t from = 0;
			uint32_t to = 0;
			
			if(left_half_filled){
				from = 0;
				to = data_arr_size / 2;
			}
			if(right_half_filled){
				from = data_arr_size / 2;
				to = data_arr_size;
			}
			
			if__buffer_filled = 0; // if mean value is calculated --> we update the points values
			timer_counter_flag = 0;
			//mean_value = calc_mean_value(data_arr, from, to);
			update_points();
			TIM3->CCR3 = mean_value / 2;  
		}
		
	}
	
}



