#include "stm32f0xx.h"
#include "configure_SPI_DMA.h"



void configure_SPI_DMA(uint32_t* data_arr, uint32_t data_arr_size){
	// CLOCK
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; /* (1) */
	RCC->CR2 |= RCC_CR2_HSI14ON; /* (2) */
	RCC->AHBENR |=RCC_AHBENR_DMA1EN; // DMA clock enable
	//RCC->AHBENR |= RCC_AHBENR_DMAEN;
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) /* (3) */
	{
		
	}
	
	
	// DMA CONFIGURATION /////////////////////////////////
	
	DMA1_Channel1->CPAR = (uint32_t)(&(ADC1->DR)); // peripheral data register
	
	DMA1_Channel1->CMAR = (uint32_t)(&data_arr[0]);
	DMA1_Channel1->CNDTR = data_arr_size;
	DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0
                    | DMA_CCR_TEIE | DMA_CCR_TCIE | DMA_CCR_HTIE | DMA_CCR_CIRC; /* increment, size, interrupts, circular mode */
	
	//DMA1_Channel1->CCR &= (~DMA_CCR_DIR); // Read from periphery
	DMA1_Channel1->CCR |= DMA_CCR_EN; /* Enable dma */

	NVIC_EnableIRQ(DMA1_Channel1_IRQn); /* enable interrupt */
	NVIC_SetPriority(DMA1_Channel1_IRQn,0); /* set priority */
	
	//////////////////////////////////////////////
	
	
	// CALIBRATION
	if ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
		ADC1->CR |= ADC_CR_ADDIS; 
	}
	while ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
	
	}
	
	ADC1->CR |= ADC_CR_ADCAL; 
	while ((ADC1->CR & ADC_CR_ADCAL) != 0)
	{
  
	}
	
	
	
	// RESOLUTION
	// We need to configure a resolution (12, 10, 6, 8 bits)
	// We can change RES bits only when ADEN bit is reset
	if ((ADC1->CR & ADC_CR_ADEN) == 0 )
	{
		ADC1->CFGR1 |= ADC_CFGR1_RES_0;
	}
	
	
	// DMAEN BIT - after calibration phase
	if((ADC1->CR & ADC_CR_ADSTART) == 0){
		ADC1->CFGR1 |= ADC_CFGR1_DMAEN;
	}
	//DMA Circular mode
	if((ADC1->CR & ADC_CR_ADSTART) == 0){
		ADC1->CFGR1 |= ADC_CFGR1_DMACFG;
	}
	
	//ADEN
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) 
	{
		ADC1->ISR |= ADC_ISR_ADRDY;
	}
	ADC1->CR |= ADC_CR_ADEN; 
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0)
	{
		
	}
	
	
	
	
	// CONFIGURING
	// RESISTOR - ANALOG MODE
	GPIOA->MODER |= GPIO_MODER_MODER1; 
	
	// CHANNEL SELECTION
	if((ADC1->CR & ADC_CR_ADEN) && !(ADC1->CR & ADC_CR_ADSTART)){
		
		//ADC1->CFGR1 |= ADC_CFGR1_CONT; //CONT = 1
		ADC1->CFGR1 &= (~ADC_CFGR1_CONT);
		
		ADC1->CFGR1 |= ADC_CFGR1_SCANDIR;
		
		//ADC1->CHSELR = ADC_CHSELR_CHSEL16; // Temperature sensor
		ADC1->CHSELR = ADC_CHSELR_CHSEL1; // Resistor

		ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; // Page 235 - Programmable sampling time
		ADC->CCR |= ADC_CCR_TSEN;

	}
}

