/********************************************************************************
  * @file    	leds.c
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-22
  * @copyright	GNU Public License
  * @brief   	LED functions for L476RG ES-EP v1.0
********************************************************************************/

#include "leds.h"
#include "stm32l4xx.h"
#include "stm32l4xx_nucleo.h"

/**
 * @fn leds_periphery_board_init
 * @brief Initializes LEDs connected to GPIOC on the periphery board.
 */
void leds_periphery_board_init() {
	__HAL_RCC_GPIOC_CLK_ENABLE();
	/* GPIOC leds */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructure.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |  GPIO_PIN_12;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
 * @fn leds_periphery_board_light_up
 * @brief Light up all leds connected to GPIOC on the periphery board.
 */
void leds_periphery_board_light_up() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |  GPIO_PIN_12, GPIO_PIN_SET);
}

/**
 * @fn leds_periphery_board_off
 * @brief Turn off all leds connected to GPIOC on the periphery board.
 */
void leds_periphery_board_off() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |  GPIO_PIN_12, GPIO_PIN_RESET);
}

/**
 * @fn leds_periphery_board_blink
 * @brief Turn on an led, wait 300ms and turn it off again.
 * @param port GPIO port of the led.
 * @param pin Pin of the led.
 */
void leds_periphery_board_blink(GPIO_TypeDef* port, uint16_t pin) {
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
	HAL_Delay(300);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	HAL_Delay(10);
}

/**
 * @fn led_onboard_init.
 * @brief Initializes the on board led.
 */
void leds_onboard_init() {
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 * @fn led_onboard_light_up
 * @brief Light up the on board led.
 */
void leds_onboard_light_up() {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}

/**
 * @fn leds_onboard_off
 * @brief Turn off the on board led.
 */
void leds_onboard_off() {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}
