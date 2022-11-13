/********************************************************************************
  * @file    	tests.c
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-22
  * @copyright	GNU Public License
  * @brief   	Runs all tests for L476RG ES-EP v1.0
********************************************************************************/

#include "tests.h"
#include "leds.h"
#include "lcd_drawing.h"
#include "pushbutton.h"
#include "ws2812b.h"
#include "stm32l4xx.h"

/**
 * @fn test_buttons
 * @brief Test all three buttons. On button press 'ok'
 * is printed for the corresponding button.
 */
uint8_t test_buttons() {
	uint8_t pressed = 0x0;
	BTN_ResetButtonState(BTN_ID_1);
	BTN_ResetButtonState(BTN_ID_2);

	LCD_Print(30, 7, "Press Buttons", LCD_Font_6x7lcd);
	while (pressed != 0x7) {
		if(BTN_GetButtonState(BTN_NUCLEO) == BTN_State_Pressed) {
			BTN_ResetButtonState(BTN_NUCLEO);
			LCD_Print(TST_OK_POS_0, 3, "OK", LCD_Font_6x7lcd);
			pressed |= 0x1;
		}
		if(BTN_GetButtonState(BTN_ID_1) == BTN_State_Pressed) {
			BTN_ResetButtonState(BTN_ID_1);
			LCD_Print(TST_OK_POS_0, 4, "OK", LCD_Font_6x7lcd);
			pressed |= 0x2;
		}
		if(BTN_GetButtonState(BTN_ID_2) == BTN_State_Pressed) {
			BTN_ResetButtonState(BTN_ID_2);
			LCD_Print(TST_OK_POS_0, 5, "OK", LCD_Font_6x7lcd);
			pressed |= 0x4;
		}
		HAL_Delay(100);
	}
	return 0x0;
}

/**
 * @fn wait_for_user_input
 * @brief Waits unitl the user presses a button.
 * Returns a uint8_t with the following button assignments:
 * bit '1': BTN_NUCLEO.
 * bit '2': BTN_ID_1.
 * bit '3': BTN_ID_2.
 * @return Value with bitwise encoding of the buttons.
 */
uint8_t wait_for_user_input() {
	uint8_t pressed = 0x0;
	while (pressed == 0x0) {
		if(BTN_GetButtonState(BTN_ID_1) == BTN_State_Pressed) {
			BTN_ResetButtonState(BTN_ID_1);
			// Ok button.
			pressed |= 0x2;
		}
		if(BTN_GetButtonState(BTN_ID_2) == BTN_State_Pressed) {
			BTN_ResetButtonState(BTN_ID_2);
			// NOk button.
			pressed |= 0x4;
		}
		HAL_Delay(100);
	}
	return pressed;
}

uint8_t determine_test_result(uint8_t pressed, uint8_t display_page) {
	uint8_t ok = 0x0;
	if (pressed == 0x4) {
		// Not Okay button pressed.
		LCD_Print(80, display_page, "nOK", LCD_Font_6x7lcd);
		ok = 0x1;
	}
	else if (pressed == 0x2)  {
		// Okay button pressed.
		LCD_Print(80, display_page, "OK", LCD_Font_6x7lcd);
	}
	else {
		// Print error if both buttons are pressed.
		LCD_Print(80, display_page, "e", LCD_Font_6x7lcd);
		ok = 0x2;
	}
	return ok;
}


/**
 * @fn test_leds
 * @brief Light up all LEDs after each other.
 * The user has to press a button to signal the
 * program, that all leds have lit up.
 */
uint8_t test_leds() {
	BTN_ResetButtonState(BTN_ID_1);
	BTN_ResetButtonState(BTN_ID_2);

	LCD_Print(0, 7, "nOK", LCD_Font_6x7lcd);
	LCD_Print(30, 7, " Check LEDs  ", LCD_Font_6x7lcd);
	LCD_Print(115, 7, "OK", LCD_Font_6x7lcd);
	HAL_Delay(500);

	// Light up each led one time.
	leds_periphery_board_blink(GPIOC, GPIO_PIN_5);
	leds_periphery_board_blink(GPIOC, GPIO_PIN_6);
	leds_periphery_board_blink(GPIOC, GPIO_PIN_7);
	leds_periphery_board_blink(GPIOC, GPIO_PIN_8);
	leds_periphery_board_blink(GPIOC, GPIO_PIN_9);
	leds_periphery_board_blink(GPIOC, GPIO_PIN_10);
	leds_periphery_board_blink(GPIOC, GPIO_PIN_11);
	leds_periphery_board_blink(GPIOC, GPIO_PIN_12);

	uint8_t pressed = wait_for_user_input();
	return determine_test_result(pressed, 3);
}

/**
 * @fn test_board_led
 * Test the on-board led. Light up the led and wait until the
 * user signals the program if the led has light up.
 */
uint8_t test_board_led() {
	BTN_ResetButtonState(BTN_ID_1);
	BTN_ResetButtonState(BTN_ID_2);
	LCD_Print(0, 7, "nOK", LCD_Font_6x7lcd);
	LCD_Print(30, 7, "Check BRD LED", LCD_Font_6x7lcd);
	LCD_Print(115, 7, "OK", LCD_Font_6x7lcd);

	leds_onboard_light_up();

	uint8_t pressed = wait_for_user_input();

	leds_onboard_off();
	return determine_test_result(pressed, 4);
}

/**
 * @fn test_ws2812
 * @brief Light up ws2812 leds in different colors.
 * The pattern is:
 * 0: Red
 * 1: Green
 * 2: Blue
 * 4: While
 */
uint8_t test_ws2812() {
	BTN_ResetButtonState(BTN_ID_1);
	BTN_ResetButtonState(BTN_ID_2);
	LCD_Print(0, 7, "nOK", LCD_Font_6x7lcd);
	LCD_Print(30, 7, "Check WS2812b", LCD_Font_6x7lcd);
	LCD_Print(115, 7, "OK", LCD_Font_6x7lcd);

	HAL_Delay(50);
	WS2812b_set_brightness(30);
	WS2812b_set_color(0, 255, 0, 0);
	WS2812b_set_color(1, 255, 0, 0);
	WS2812b_set_color(2, 255, 0, 0);
	WS2812b_set_color(3, 255, 0, 0);
	WS2812b_send();
	HAL_Delay(500);

	WS2812b_set_color(0, 0, 255, 0);
	WS2812b_set_color(1, 0, 255, 0);
	WS2812b_set_color(2, 0, 255, 0);
	WS2812b_set_color(3, 0, 255, 0);
	WS2812b_send();
	HAL_Delay(500);

	WS2812b_set_color(0, 0, 0, 255);
	WS2812b_set_color(1, 0, 0, 255);
	WS2812b_set_color(2, 0, 0, 255);
	WS2812b_set_color(3, 0, 0, 255);
	WS2812b_send();
	HAL_Delay(500);

	WS2812b_set_color(0, 100, 100, 100);
	WS2812b_set_color(1, 100, 100, 100);
	WS2812b_set_color(2, 100, 100, 100);
	WS2812b_set_color(3, 100, 100, 100);
	WS2812b_send();
	HAL_Delay(500);

	WS2812b_set_color(0, 0, 0, 0);
	WS2812b_set_color(1, 0, 0, 0);
	WS2812b_set_color(2, 0, 0, 0);
	WS2812b_set_color(3, 0, 0, 0);
	WS2812b_send();

	uint8_t pressed = wait_for_user_input();
	return determine_test_result(pressed, 5);
}
