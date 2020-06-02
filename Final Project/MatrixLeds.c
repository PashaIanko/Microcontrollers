#include "stm32f0xx.h"
#include "MatrixLeds.h"

#define MATRIX_SIZE 8

uint8_t get_matrix_size(){
	return MATRIX_SIZE;
}

void spi_init(){
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;//SysTick init on spi2 interface (located on APB port1)
	
	//spi init
	SPI2->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA; 
	SPI2->CR2 = SPI_CR2_DS;
	SPI2->CR1 |= SPI_CR1_SPE;
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
	
	//PA8 - SS
	GPIOA->MODER |= GPIO_MODER_MODER8_0;//slave select alternative function port
	
	//PB13 - SOI2_SCK
	//PB15 - SPI12_MOSI
	GPIOB->AFR[1] |= ( 0 << ( 4 * ( 13 - 8 ) ) ) | ( 0 << ( 4 * ( 15 - 8 ) ) );//0 == number of alternative function 4 == max bit amount for alternative function 8 == shift caused by 4*16=2*32
	GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1;//set ports manipulating by the outer device; turn off from odr?1057
	
	SPI2->CR2 = SPI_CR2_DS| SPI_CR2_RXNEIE;// | SPI_CR2_TXEIE 
	NVIC_EnableIRQ(SPI2_IRQn);// in initialization agree for handling//wasSPI1
}

uint8_t dec_to_line(uint8_t x){
	return 1 << x;
}
uint16_t position_code(uint8_t x, uint8_t y){
	return (dec_to_line(y) << 8) | dec_to_line(x);
}

uint16_t led_cross_on(uint8_t x, uint8_t y){
	static uint8_t mode = -1;
	mode = (mode + 1) % MATRIX_SIZE;
	
	if (mode == y - 1){
		return position_code(x, y - 1);
	}
	if (mode == y){
		return position_code(x - 1, y) | position_code(x + 1, y);
	}
	if (mode == y + 1){
		return position_code(x, y + 1);
	}
	
	return 0x0000;//position_code(0, mode+1);
}
