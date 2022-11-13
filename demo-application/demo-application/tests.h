/********************************************************************************
  * @file    	tests.h
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-11-13
  * @copyright	GNU Public License
  * @brief   	Header file for tests.c.
********************************************************************************/


#ifndef _TEST_H
#define _TEST_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/
#define TST_OK_POS_0 20

/* Exported functions ------------------------------------------------------- */
uint8_t test_buttons();
uint8_t test_leds();
uint8_t test_board_led();
uint8_t test_ws2812();

#endif /* _TEST_H */
