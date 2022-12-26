/********************************************************************************
  * @file    	lcd.c
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-22
  * @copyright	paspf, GNU Public License 3
  * @brief   	Functions to display characters and strings on the LCD.
  *
********************************************************************************/

#ifndef _lcd_h
#define _lcd_h

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/* Public typedefs -----------------------------------------------------------*/
typedef enum {
  LCD_COMMAND_MODE=0,
  LCD_DATA_MODE
} LCD_MODE;


/* Public defines ------------------------------------------------------------*/
#define LCD_BACKLIGHT_PIN 	GPIO_PIN_5
#define LCD_BACKLIGHT_PORT 	GPIOB

#define LCD_CD_PIN        	GPIO_PIN_2
#define LCD_CD_PORT      	GPIOB

#define LCD_NRST_PIN      	GPIO_PIN_4
#define LCD_NRST_PORT     	GPIOC

#define LCD_Width 			128
#define LCD_Height 			64
#define LCD_Pages 			8

/* Public macros -------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void LCD_Init();
void LCD_Reset();
void LCD_UnReset();
void LCD_SetBacklightState(uint8_t state);
void LCD_SetLCDMode(LCD_MODE mode);
void LCD_SetPageData(uint8_t X, uint8_t Page, uint8_t Data);
void LCD_Clear();
void LCD_PutPixel(uint8_t X, uint8_t Y);
void LCD_ConfigDisplay();
void LCD_SendConfigData(uint8_t Data);
void LCD_OnOff(uint8_t state);
void LCD_ClearFast();
void LCD_flicker();
#endif
