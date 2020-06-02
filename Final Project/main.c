#include "stm32f0xx.h"
#include "MatrixButtons.h"
#include "MatrixLeds.h"
#include "stdlib.h"

void SysTick_init(){
	SystemCoreClockUpdate();//recalculates working frequency and updates SystemCoreClock variable --- must be called before using this variable
	SysTick->LOAD = SystemCoreClock / 1000 - 1;
	//MACROSES HSI_VALUE, HSE_VALUE defines if quantum frequency generator is connected
	SysTick->VAL = 0;
	
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}



uint8_t buttons_scan_counter = 0;
uint8_t buttons_scan_timer_flag = 0;

uint16_t obstacles_spawn_counter = 0;
uint8_t obstacles_spawn_flag = 0;

uint8_t bullet_fly_counter = 0;
uint8_t bullet_fly_flag = 0;


void SysTick_Handler(void){//handler of system interruptions must have the same name as  defaults
	buttons_scan_counter++;
	obstacles_spawn_counter++;
	bullet_fly_counter++;
	
	if (buttons_scan_counter >= 80){
		buttons_scan_counter = 0;
		buttons_scan_timer_flag = 1;
	}
	if (obstacles_spawn_counter >= 1000){
		obstacles_spawn_counter = 0;
		obstacles_spawn_flag = 1;
	}
	if (bullet_fly_counter >= 5){
		bullet_fly_counter = 0;
		bullet_fly_flag = 1;
	}
}


typedef struct {
	uint8_t x;
	uint8_t y;
}Point;

uint8_t is_shooting_flag = 0;

uint16_t matrix_data_buf[8] = {};
uint16_t obstacle_data_buf[8] = {};
	
void SPI2_IRQHandler(void){
	static uint8_t line = 0;
	line = (line + 1) % 8;
	
	uint8_t status = SPI2->SR;
	uint8_t irq_rxne_enabled = SPI2->CR2 & SPI_CR2_RXNEIE;

	status = SPI2->SR;
	if ( (status & SPI_SR_RXNE) && irq_rxne_enabled ){
		volatile uint16_t data = SPI2->DR;

		GPIOA->BSRR = GPIO_BSRR_BS_8;
		SPI2->DR = matrix_data_buf[line];
		GPIOA->BSRR = GPIO_BSRR_BR_8;
	}
}

uint16_t decrease_y(uint16_t position){
	return ((position & 0xFF00) >> 1) | (position & 0x00FF);
}
uint16_t remove_x(uint16_t position, uint8_t x){
	return (0xFF00 & position) | (0x00FF & position & ~position_code(x, 0));
}
void update_matrix_data_buf(const Point player, const Point bullet){
	for (int i = 0; i < 8; i++){
		matrix_data_buf[i] = led_cross_on(player.x, player.y) | obstacle_data_buf[i];//
	}
	matrix_data_buf[bullet.y] |= position_code(bullet.x, bullet.y);
}
uint16_t obstacle_code(void){
	return position_code(0, 7) | (uint8_t)(rand());
}
void add_obstacles(void){
	for (int i = 0; i < 7; i++){
		obstacle_data_buf[i] = decrease_y(obstacle_data_buf[i+1]);
	}
	obstacle_data_buf[7] = obstacle_code();
}

void sensor_configure(){
	
	/* Configure TCS */
/* With a charge transfer around 2.5 µs */
/* (1) Select fPGCLK = fHCLK/32,
 Set pulse high = 2xtPGCLK,Master
          Set pulse low = 2xtPGCLK 
          Set Max count value = 16383 pulses
          Enable TSC */
/* (2) Disable hysteresis */
/* (3) Enable end of acquisition IT */
/* (4) Sampling enabled, G2IO4 */
/* (5) Channel enabled, G2IO3 */
/* (6) Enable group, G2 */
	
	/*A group set clock (G2 group)*/
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	/*G2 Group = PA4 PA5 PA6 PA7. We need to enable analog mode*/
	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5
	| GPIO_MODER_MODER6 | GPIO_MODER_MODER7)) | GPIO_MODER_MODER4_1
	| GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1; 
	
	/*Ports PA4 PA5 PA6 PA7 - Time sensor is alternative func #3*/
	/*Select AF3 on PA 4 5 6 7*/
	GPIOA->AFR[0] |= 
	(0x03 << GPIO_AFRL_AFRL4_Pos) |
	(0x03 << GPIO_AFRL_AFRL5_Pos) |
	(0x03 << GPIO_AFRL_AFRL6_Pos) |
	(0x03 << GPIO_AFRL_AFRL7_Pos) ; 
	
	/*clock*/
	RCC->AHBENR |= RCC_AHBENR_TSCEN;
	
	/*Enable before register settings*/
	TSC->CR |= TSC_CR_TSCE;
	TSC->CR |= 
	TSC_CR_PGPSC_2 |
	TSC_CR_PGPSC_0 | 
	
	TSC_CR_CTPH_0 |  // pulse high state duration
	TSC_CR_CTPL_0 | // pulse low state duration (charge transfer)
	
	TSC_CR_MCV_2 |
	TSC_CR_MCV_1; /* (1) */
	
	TSC->IOHCR &= (uint32_t)(~(TSC_IOHCR_G2_IO4 | TSC_IOHCR_G2_IO3)); /* (2) */
	//TSC->IER = TSC_IER_EOAIE; /* (3) */
	TSC->IOSCR = TSC_IOSCR_G2_IO4; /* (4) Sampling control register -> sampling capacitor*/
	TSC->IOCCR = TSC_IOCCR_G2_IO3; /* (5) Channel control register -> Channel I/O*/
	TSC->IOGCSR |= TSC_IOGCSR_G2E; /* (6) */
	
}



int main() {
	SysTick_init();
	spi_init();
	sensor_configure();
	matrix_button_init();
	Buttons buttons = {};
	Point cross_center = {};
	Point bullet = {};
		
	SPI2->DR = 0x0000;
		
	TSC->CR|=TSC_CR_START;
	
	while(1){
		//check buttons and change location
		{
			if (buttons_scan_timer_flag){
				buttons = read_outer_buttons();
				if (read_user_button()){
					for (int i = 0; i < 8; i++){
						obstacle_data_buf[i] = 0;
					}
				}
				buttons_scan_timer_flag = 0;
			}
			if (obstacles_spawn_flag){
				add_obstacles();
				obstacles_spawn_flag = 0;
			}
			if (bullet_fly_flag){
				if (is_shooting_flag){
					if ((0x00FF & obstacle_data_buf[bullet.y] & position_code(bullet.x, bullet.y)) != 0){
						obstacle_data_buf[bullet.y] = remove_x(obstacle_data_buf[bullet.y], bullet.x);
						is_shooting_flag = 0;
					}
					
					bullet.y++;
					if (bullet.y >= 8){
						is_shooting_flag = 0;
					}
				}
				bullet_fly_flag = 0;
			}
			if (!is_shooting_flag){
				bullet = cross_center;
			}
		}
			
		//on button pressed
		{
			if (buttons.up){
				cross_center.x -= 1;
				cross_center.x %= get_matrix_size();
				buttons.up = 0;
			}
			if (buttons.down){
				cross_center.x += 1;
				cross_center.x %= get_matrix_size();
				buttons.down = 0;
			}
			if (buttons.right){
				is_shooting_flag = 1;
				bullet = cross_center;
				buttons.right = 0;
			}
		}
		//update draw matrix
		update_matrix_data_buf(cross_center, bullet);
		
		//on sensor
		
		if ((TSC->ISR & TSC_ISR_MCEF) == (TSC_ISR_MCEF)){
			const volatile uint32_t flag = 1;
			TSC->ICR |= TSC_ICR_MCEIC;
		}
	
		if ((TSC->ISR & TSC_ISR_EOAF) == TSC_ISR_EOAF)
		{
			const uint32_t sensor_max = 0x581;
			TSC->ICR |= TSC_ICR_EOAIC; /* Clear flag */
			uint32_t enabled_io_group = 1;
			uint32_t AcquisitionValue = TSC->IOGXCR[enabled_io_group]; /* Get G2 counter value */
			if(AcquisitionValue <= sensor_max){
				const volatile uint32_t flag = 1;
			}
			TSC->CR|=TSC_CR_START;
		}
	}
}

