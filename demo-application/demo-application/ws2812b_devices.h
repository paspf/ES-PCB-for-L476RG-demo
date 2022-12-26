/********************************************************************************
  * @file    	ws2812b_devices.h
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-12
  * @copyright	paspf, GNU Public License 3
  * @brief   	Contains the header of all initialization functions required for
  * 		    the ws2812b leds.
********************************************************************************/

#ifndef __INIT_DEVICES_WS2812B_H
#define __INIT_DEVICES_WS2812B_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TIM2_Init();
void TIM2_DMA_Init();
void TIM2_GPIO_Init();
void DMA_Init();
void TIM2_DMA_DeInit();

#ifdef __cplusplus
}
#endif

#endif /* __INIT_DEVICES_WS2812B_H */
