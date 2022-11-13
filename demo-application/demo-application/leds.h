/********************************************************************************
  * @file    	leds.h
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-18
  * @copyright	GNU Public License
  * @brief   	LED functions for L476RG ES-EP v1.0.
********************************************************************************/

#ifndef _leds_h
#define _leds_h

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/* Exported functions ------------------------------------------------------- */
void leds_periphery_board_init();
void leds_periphery_board_light_up();
void leds_periphery_board_off();
void leds_periphery_board_blink(GPIO_TypeDef* port, uint16_t pin);
void leds_onboard_init();
void leds_onboard_light_up();
void leds_onboard_off();

#endif /* _leds_h */
