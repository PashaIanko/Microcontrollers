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

int main(){
	btn_init();
	led_init();
	
	
	// CLOCK
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; /* (1) */
	RCC->CR2 |= RCC_CR2_HSI14ON; /* (2) */
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) /* (3) */
	{
		
	}
	
	
	// CALIBRATION
	if ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
		ADC1->CR |= ADC_CR_ADDIS; 
	}
	while ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
	
	}
	//ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN; // DMA clock enable
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
	
	
	
	//ADEN
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) 
	{
		ADC1->ISR |= ADC_ISR_ADRDY;
	}
	ADC1->CR |= ADC_CR_ADEN; 
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0)
	{
		
	}
	
	// CONTINUE TO WRITE ADEN???
	
	
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
	
	
	
	volatile uint16_t prev_res = 0;
	while(1){
		while(ADC1->CR & ADC_CR_ADSTART) 
		{
			// still ongoing conversion
		};
		ADC1->CR |= ADC_CR_ADSTART;
		while((ADC1->CFGR1 & ADC_ISR_EOC) == 0){
		 // waiting end of conversion
		}
		// converted data is stored in the 16bit ADC_DR register
		volatile uint32_t result = ADC1->DR; 
		if(result != prev_res)
		{
			
			uint16_t lim1 = 0;
			uint16_t lim2 = 200;
			uint16_t lim3 = 600;
			uint16_t lim4 = 1000;
			prev_res = result;
			
			if(prev_res < lim2 && prev_res > lim1)
			{
				all_leds_off();
				led_on(6);
			}
			if(prev_res < lim3 && prev_res > lim2)
			{
				all_leds_off();
				led_on(7);
			}
			if(prev_res < lim4 && prev_res > lim3)
			{
				all_leds_off();
				led_on(8);
			}
			
		}
	}
	
		
	// Disable ADC
	disable_adc();
	
	return 0;
}

