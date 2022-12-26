/********************************************************************************
  * @file    	ws2812b.h
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-12
  * @copyright	paspf, GNU Public License 3
  * @brief   	Contains the header functions to use the ws2812b leds.
  *             Initialization of hardware must be done before calling any
  *             functions in ws2812b.h
********************************************************************************/

#ifndef __INIT_WS2812B_H
#define __INIT_WS2812B_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/
typedef enum {
	FADE_DIRECTION_UPWARDS,
	FADE_DIRECTION_DOWNWARDS
}FADE_DIRECTION;


/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define NUM_LEDS 4
#define MAX_LED_VALUE 255

/* Exported functions ------------------------------------------------------- */
int WS2812b_set_color(int led_id, uint8_t red, uint8_t green, uint8_t blue);
void WS2812b_set_brightness(uint8_t brightness);
void WS2812b_send();
void WS2812b_fade_leds(bool *abort);

#ifdef __cplusplus
}
#endif

#endif /* __INIT_WS2812B_H */
