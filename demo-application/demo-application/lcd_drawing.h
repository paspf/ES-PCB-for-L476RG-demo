/********************************************************************
 * Project:
 * File:       lcd_drawing.c
 *
 * System:     STM32L476RG (Cortex M4)
 *
 * Date:       2008-11-10, 2020-03-08, 2022-03-22
 * Authors:    S. Apel
 * 			   modified for STM32L476RG by paspf
 *
 * Rights:     Technische Hochschule Mittelhessen
 * License:    GPL
 ********************************************************************
 * Description:
 * Contains functions to display characters and strings on the LCD
 ********************************************************************/

#ifndef _lcd_drawing_h
#define _lcd_drawing_h

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"
#include "lcd.h"

/* Public typedefs -----------------------------------------------------------*/
/* Public defines ------------------------------------------------------------*/
#define LCD_Font_Width(f)         (f[0])
#define LCD_Font_Height(f)        (f[1])
#define LCD_Font_PagesPerChar(f)  (f[2])
#define LCD_Font_CharsetOffset(f) (f[3])
#define LCD_Font_Data(f, x)       (f[4+x])

/* Public macros -------------------------------------------------------------*/
/* Public constants ----------------------------------------------------------*/
extern const uint8_t LCD_Font_6x7int[];
extern const uint8_t LCD_Font_6x7lcd[];
extern const uint8_t LCD_Font_11x14[];
extern const uint8_t LCD_Font_21x28[];

/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void LCD_Print(uint8_t X, uint8_t StartPage, char* String, const uint8_t Font[]);
void LCD_DrawLine(uint8_t XStart, uint8_t YStart, uint8_t XEnd, uint8_t YEnd);
void LCD_DrawCircle(uint8_t X, uint8_t Y, uint8_t Radius);

#endif
