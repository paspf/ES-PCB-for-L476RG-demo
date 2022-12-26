/********************************************************************************
  * @file    spi.c
  * @author  paspf
  * @version V1.0
  * @date    2022-03-22
  * @copyright	paspf, GNU Public License 3
  * @brief   Contains functions to display characters and strings on the LCD.
********************************************************************************/

#ifndef _spi_h
#define _spi_h

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/* Public typedefs -----------------------------------------------------------*/
typedef enum {
  SPI2_Device_LCD = 0,
  SPI2_Device_Plug
} SPI2_Device;

/* Public defines ------------------------------------------------------------*/
#define SPI2_CS_LCD_PIN   	GPIO_PIN_1
#define SPI2_CS_LCD_PORT   	GPIOB

#define SPI2_CS_PLUG_PIN 	GPIO_PIN_10
#define SPI2_CS_PLUG_PORT 	GPIOA

/* Public macros -------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void SPI2_Init();
void SPI2_SendData(uint8_t data);
uint8_t SPI2_SelectDevice(SPI2_Device device);
void SPI2_LockCS();
void SPI2_UnlockCS();

#endif /* _spi_h */
