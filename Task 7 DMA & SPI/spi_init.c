#include "stm32f0xx.h"
#include "spi_init.h"
void spi_init(void){
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; //make tacktirovanie ON spi2
	//spi init
	SPI2->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;
	
	// Interrupt Handler bits are in CR2 register --> Set'em up!
	SPI2->CR2 = SPI_CR2_DS | /*SPI_CR2_TXEIE |*/ SPI_CR2_RXNEIE; //1111 = 16 BITS = word wide
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
