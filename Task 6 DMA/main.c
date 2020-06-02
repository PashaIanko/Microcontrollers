#include "stm32f0xx.h"



/* 
FLAGS:

ADDIS: // Set ADDIS=1 to disable the ADC and put the ADC in power down mode.
ADSTART = 0 - means no conversion ongoing
CHSEL SCANDIR - channel selection


*/

void disable_adc(){
	/* (1) Stop any ongoing conversion */
	/* (2) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
	/* (3) Disable the ADC */
	/* (4) Wait until the ADC is fully disabled */
	ADC1->CR |= ADC_CR_ADSTP; /* (1) */
	while ((ADC1->CR & ADC_CR_ADSTP) != 0) /* (2) */
	{
		/* For robust implementation, add here time-out management */
	}
	ADC1->CR |= ADC_CR_ADDIS; /* (3) */
	while ((ADC1->CR & ADC_CR_ADEN) != 0) /* (4) */
	{
		/* For robust implementation, add here time-out management */
	}
}

void btn_init() {
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //Enable A group on bus AHB
	GPIOA->MODER &= ~GPIO_MODER_MODER0; //GPIO_MODER_MODER0 - vse bity vystavlennye v 1
	
}

void set_LED_GPOM(uint32_t pin_numb){
	
	//set general purpose output mode for LED diode
	switch(pin_numb){
		case 6:
			GPIOC->MODER |= GPIO_MODER_MODER6_0; 
			break;
		case 7:
			GPIOC->MODER |= GPIO_MODER_MODER7_0; 
			break;
		case 8:
			GPIOC->MODER |= GPIO_MODER_MODER8_0; 
			break;
		case 9:
			GPIOC->MODER |= GPIO_MODER_MODER9_0;
			break;
		default:
			break;
	}
}

void led_init(){
	//rybilnili dernut (RCC -reset clock control, registr vy)
	
	/*Enable clock for C group periphery (LED diodes)*/
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; /*|= - setting proper bit. 
																		Which bit? taktirovanie zaveli*/
	
	
	set_LED_GPOM(6);
	set_LED_GPOM(7);
	set_LED_GPOM(8);
	set_LED_GPOM(9);
	
}

void led_off(const uint32_t pin_numb){
	
	switch(pin_numb){
		case 6:
			GPIOC->ODR &= ~GPIO_ODR_6;
			break;
		case 7:
			GPIOC->ODR &= ~GPIO_ODR_7;
			break;
		case 8:
			GPIOC->ODR &= ~GPIO_ODR_8;
			break;
		case 9:
			GPIOC->ODR &= ~GPIO_ODR_9;
			break;
		default:
			break;
	}
	return;
	
}

void led_on(const uint32_t pin_numb){
	
	switch(pin_numb){
		case 6:
			GPIOC->ODR |= GPIO_ODR_6;
			break;
		case 7:
			GPIOC->ODR |= GPIO_ODR_7;
			break;
		case 8:
			GPIOC->ODR |= GPIO_ODR_8;
			break;
		case 9:
			GPIOC->ODR |= GPIO_ODR_9;
			break;
		default:
			break;
		
	}
	return;

}

void all_leds_off(){
	led_off(6);
	led_off(7);
	led_off(8);
	led_off(9);
}

void all_leds_on(){
	led_on(6);
	led_on(7);
	led_on(8);
	led_on(9);
}



uint32_t data_arr_size = 16;
uint32_t data_arr[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t MAX_UINT32_T = 4294967290;
uint32_t mean_value = 0;
uint32_t if_calculated = 0;

void mask_data(uint32_t * data_arr, uint32_t arr_size, uint32_t mask){
	for(uint32_t i = 0; i<arr_size; i++){
		data_arr[i] &= mask;
	}
}

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
		if_calculated = 1;
		
		
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


int main(){
	btn_init();
	led_init();
	
	
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
	
	
							
	uint32_t lim1 = 0;
	uint32_t lim2 = 200;
	uint32_t lim3 = 600;
	uint32_t lim4 = 1000;
	
	
	while(1){
		while(ADC1->CR & ADC_CR_ADSTART) 
		{
			// still ongoing conversion
		};
		
		ADC1->CR |= ADC_CR_ADSTART;
		
		if(if_calculated){
			if_calculated = 0;
			
				
				
			if((mean_value < lim2 && mean_value > lim1) && mean_value != 0)
			{
				//all_leds_off();
				led_on(6);
			}
			if(mean_value < lim3 && mean_value > lim2)
			{
				//all_leds_off();
				led_on(9);
			}
			if(mean_value > lim3)
			{
				//all_leds_off();
				led_on(8);
			}
			
				
			
		}
		
		
	}
	
		
	// Disable ADC
	disable_adc();
	
	return 0;
}

