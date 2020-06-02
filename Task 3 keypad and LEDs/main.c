#include "stm32f0xx.h" //Root header
#include "set_GPOM.h"
#include "set_GPIM.h"
#include "set_timing.h"
#include "leds.h"
#include "GROUP_on.h"
#include "GROUP_off.h"
#include "GROUP_read.h"
#include "GROUP_set_PDR.h"


typedef struct led{
	uint32_t is_on;
	uint32_t pin_numb;
	uint32_t btn_macros;
} led;

uint32_t keypad_pins[] = {12, 15};
uint32_t active_keypad_pin = 15; // initial pin to put current in


//this val is applied in SysTickInit()
const uint32_t SysTickFreq = 60;

//buttons' macroses
const uint32_t PA15_PA4 = 0;
const uint32_t PA15_PA5 = 1;
const uint32_t PC12_PA4 = 2;
const uint32_t PC12_PA5 = 3;

const uint32_t LED_ON = 1;
const uint32_t LED_OFF = 0;
const uint32_t INVALID_LED = 100;

const uint32_t LEDS_SIZE = 4;
led leds[4]; // 4 == LEDS_SIZE, but compiler gives errors

uint32_t caught_btn_macros = 0;
uint32_t if_caught_btn = 0;

void check_btns_legs(const uint32_t active_keypad_pin){
	
	if (active_keypad_pin == 15){
		if(A_GROUP_read(4)){
			caught_btn_macros = PA15_PA4; // lower btn - active pin = 15, PA4
			if_caught_btn = 1;
			led_on(leds[1].pin_numb);
			return;
		}
		else{
			led_off(leds[1].pin_numb);
		}
		if (A_GROUP_read(5)){ // upper btm = A5, active pin = 15
			caught_btn_macros = PA15_PA5;
			if_caught_btn = 1;
			led_on(leds[0].pin_numb);
			return;
		}
		else{
			led_off(leds[0].pin_numb);
		}
	
	}
	else if (active_keypad_pin == 12){
		if(A_GROUP_read(4)){
			caught_btn_macros = PC12_PA4;
			if_caught_btn = 1;
			led_on(leds[2].pin_numb);
			return;
		}
		else {
			led_off(leds[2].pin_numb);
		}
		if (A_GROUP_read(5)){
			caught_btn_macros = PC12_PA5;
			if_caught_btn = 1;
			led_on(leds[3].pin_numb);
			return;
		}
		else{
			led_off(leds[3].pin_numb);
		}
	}
	
	//If we aint got no catch anything
	caught_btn_macros = INVALID_LED;
	if_caught_btn = 0;
	return;

}

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
	
}




void SysTickInit(){
	SystemCoreClockUpdate();
	
	SysTick->LOAD = SystemCoreClock / SysTickFreq - 1;
	
	SysTick->VAL = 0;
	
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk; 
}


led* get_led(const uint32_t button_macros){
	for (uint32_t i = 0; i < LEDS_SIZE; i++){
		if(leds[i].btn_macros == button_macros){
			return &leds[i];
		}
	}
	return 0;
}

void switch_led(const uint32_t button_macros){ // if a led is on - turn it off, and vice versa
	if(button_macros == INVALID_LED){
		return;
	}
	led* cur_led = get_led(button_macros); // on/off
	
	cur_led->is_on = LED_ON;
	led_on(cur_led->pin_numb);
	/*if (cur_led->is_on == LED_ON){
		led_off(cur_led->pin_numb);
		cur_led->is_on = LED_OFF;
	}
	else{
		led_on(cur_led->pin_numb);
		cur_led->is_on = LED_ON;
	}*/
}

int main (void)
{
	C_GROUP_set_timing(); //RCC, c6 c7 c8 c9 - leds, c12 - btns
	A_GROUP_set_timing(); //RCC, A4, A5, A12, A15 - buttons
	
	C_GROUP_set_GPOM(6); // PC6 - led - set Generaral purpose output mode
	C_GROUP_set_GPOM(7);
	C_GROUP_set_GPOM(8);
	C_GROUP_set_GPOM(9);
	
	A_GROUP_set_GPIM(4); // PA4, PA5 - we are going to read bytes from there
	A_GROUP_set_GPIM(5); // 
	
	C_GROUP_set_GPOM(12); // We are going to set '1' with frequency ~50 Hz on each of the PA12, PA15
	A_GROUP_set_GPOM(15);
	
	// pull_up resistors set for PA4, PA5
	A_GROUP_SET_PDR(4);
	A_GROUP_SET_PDR(5);
	
	//Init clock
	SysTickInit();
	
		
	//initialize led structs
	led led1 = {0, 7, PA15_PA5};
	led led2 = {0, 6, PA15_PA4};
	led led3 = {0, 9, PC12_PA4};
	led led4 = {0, 8, PC12_PA5};
	leds[0] = led1;
	leds[1] = led2;
	leds[2] = led3;
	leds[3] = led4;
	
	
	A_GROUP_on(active_keypad_pin); // Give current on PA15
	while(1){
		// In SysTick Handler all logics
		if (if_caught_btn){
			//uint32_t cur_btn_macros = caught_btn_macros;
			//switch_led(cur_btn_macros);
		}
	}
}
