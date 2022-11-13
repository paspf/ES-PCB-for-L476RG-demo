/********************************************************************
 * Project:
 * File:       lcd_drawing.c
 *
 * System:     STM32L476RG (Cortex M4)
 *
 * Date:       2008-11-12, 2020-03-08, 2022-03-22
 * Authors:    S. Apel
 * 			   modified for STM32L476RG by paspf
 *
 * Rights:     Technische Hochschule Mittelhessen
 * License:    GPL
 ********************************************************************
 * Description:
 * Contains functions to display characters and strings on the LCD
 ********************************************************************/

/* Includes -------------------------------------------------------------------*/
#include "lcd.h"
#include "lcd_drawing.h"
#include "stm32l4xx.h"

/* External variables ---------------------------------------------------------*/
/* Private typedefs -----------------------------------------------------------*/
/* Private defines ------------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------*/
/* Private functions ----------------------------------------------------------*/

/**
 * @fn LCD_Print
 * Print a string.
 * @param X X-Position.
 * @param StartPage Page / Line to start.
 * @param String String to print.
 * @param Font Font.
 */
void LCD_Print(uint8_t X, uint8_t StartPage, char* String, const uint8_t Font[]) {
  char c, v;
  int Page, CharPos, CharXPos, CharOffs;

  for (CharPos = 0; String[CharPos]; CharPos++) {
    /* Draw character unless it's a space */
    if (String[CharPos] != 0x20) {
      /* Determine offset into the data table for this character */
      c = String[CharPos] - LCD_Font_CharsetOffset(Font);
      CharOffs = LCD_Font_Width(Font) * LCD_Font_PagesPerChar(Font)*c;

      for (CharXPos=0; (CharXPos < LCD_Font_Width(Font)) && (X < LCD_Width); CharXPos++, X++)
        for (Page=0; Page < LCD_Font_PagesPerChar(Font); Page++) {
          v = LCD_Font_Data(Font, CharOffs + CharXPos * LCD_Font_PagesPerChar(Font) + Page);
          LCD_SetPageData(X, StartPage + Page, v);
        }
    }
    else {
      /* It's a space, let's draw it a little faster */
      /* Also take note that some fonts don't have this char */
      for (CharXPos = 0; (CharXPos < LCD_Font_Width(Font)) && (X < LCD_Width); CharXPos++, X++)
        for (Page = 0; Page < LCD_Font_PagesPerChar(Font); Page++)
          LCD_SetPageData(X, StartPage+Page, 0);
    }

    /* Stop string output if end of display reached */
    if (X >= LCD_Width) return;
  }
}

/**
 * @fn LCD_DrawLine
 * Draw a line.
 * @param XStart X-Start position in pixels.
 * @param YStart Y-Start position in pixels.
 * @param XEnd X-End position in pixels.
 * @param YEnd Y-End position in pixels.
 */
void LCD_DrawLine(uint8_t XStart, uint8_t YStart, uint8_t XEnd, uint8_t YEnd) {
  // Non-C Standard: GCC nested function.
  // http://gcc.gnu.org/onlinedocs/gcc/Nested-Functions.html
  int sgn(int x){ return (x > 0) - (x < 0); }

  int x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;

  dx = XEnd - XStart;
  dy = YEnd - YStart;

  incx = sgn(dx);
  incy = sgn(dy);
  if (dx < 0) dx = -dx;
  if (dy < 0) dy = -dy;

  if (dx > dy) {
    pdx = incx; pdy = 0;
    ddx = incx; ddy = incy;
     es = dy;    el = dx;
  }
  else {
    pdx = 0;    pdy = incy;
    ddx = incx; ddy = incy;
     es = dx;    el = dy;
  }

  x = XStart;
  y = YStart;
  err = el/2;

  LCD_PutPixel(x, y);

  for(t = 0; t < el; ++t) {
    err -= es;

    if (err < 0) {
      err += el;
      x += ddx;
      y += ddy;
    }
    else {
      x += pdx;
      y += pdy;
    }

    LCD_PutPixel(x, y);
   }
}

/**
 * @fn LCD_DrawCircle
 * Draw a circle.
 * @param XStart X-Start position in pixels.
 * @param YStart Y-Start position in pixels.
 * @param Radius Radius in pixels.
 */
void LCD_DrawCircle(uint8_t X, uint8_t Y, uint8_t Radius) {
  int f = 1 - Radius;
  int ddF_x = 1;
  int ddF_y = -2 * Radius;
  int dx = 0;
  int dy = Radius;

  LCD_PutPixel(X, Y + Radius);
  LCD_PutPixel(X, Y - Radius);
  LCD_PutPixel(X + Radius, Y);
  LCD_PutPixel(X - Radius, Y);

  while(dx < dy) {
    if(f >= 0) {
      dy--;
      ddF_y += 2;
      f += ddF_y;
    }

    dx++;
    ddF_x += 2;
    f += ddF_x;

    LCD_PutPixel(X + dx, Y + dy);
    LCD_PutPixel(X - dx, Y + dy);
    LCD_PutPixel(X + dx, Y - dy);
    LCD_PutPixel(X - dx, Y - dy);
    LCD_PutPixel(X + dy, Y + dx);
    LCD_PutPixel(X - dy, Y + dx);
    LCD_PutPixel(X + dy, Y - dx);
    LCD_PutPixel(X - dy, Y - dx);
  }
}
