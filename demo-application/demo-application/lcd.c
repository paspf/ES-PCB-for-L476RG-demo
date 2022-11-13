/********************************************************************************
  * @file    	lcd.c
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-22
  * @copyright	GNU Public License
  * @brief   	Functions to display characters and strings on the LCD.
  *
********************************************************************************/

/* Includes -------------------------------------------------------------------*/
#include "lcd.h"
#include "spi.h"

/* External variables ---------------------------------------------------------*/
/* Private typedefs -----------------------------------------------------------*/
/* Private defines ------------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------*/
/* Functions ------------------------------------------------------------------*/


/**
 * @fn LCD_Init
 * @brief Initialize display and �C HW.
 */
void LCD_Init() {
	/* Enable Clock for required peripherals */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/* Display command/data pin GPIO configuration -> PB2 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = LCD_CD_PIN;
	HAL_GPIO_Init(LCD_CD_PORT, &GPIO_InitStructure);

	/* Display reset pin GPIO configuration -> PC4 */
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = LCD_NRST_PIN;
	HAL_GPIO_Init(LCD_NRST_PORT, &GPIO_InitStructure);

	/* Display backlight pin GPIO configuration -> PB5 */
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = LCD_BACKLIGHT_PIN;
	HAL_GPIO_Init(LCD_BACKLIGHT_PORT, &GPIO_InitStructure);

	/* Reset LCD */
	LCD_Reset();

	/* Enable backlight */
	LCD_SetBacklightState(1);

	/* Init SPI2 */
	SPI2_Init();

	/* Unreset LCD */
	LCD_UnReset();

	/* Select display as SPI device */
	SPI2_SelectDevice(SPI2_Device_LCD);

	/* Switch LCD to command mode */
	LCD_SetLCDMode(LCD_COMMAND_MODE);

	/* Setup display */
	LCD_ConfigDisplay();
}

/**
 * @fn LCD_Reset
 * @brief Reset LCD using the reset pin.
 */
void LCD_Reset() {
	HAL_GPIO_WritePin(LCD_NRST_PORT, LCD_NRST_PIN, GPIO_PIN_RESET);
}

/**
 * @fn LCD_UnReset
 * @brief Unreset the LCD using the reset pin.
 */
void LCD_UnReset() {
	HAL_GPIO_WritePin(LCD_NRST_PORT, LCD_NRST_PIN, GPIO_PIN_SET);
}

/**
 * @fn LCD_ConfigDisplay
 * @brief Configure display for internal voltage generation
 */
void LCD_ConfigDisplay() {
	/* Display start line: Display start line 0 */
	LCD_SendConfigData(0x40);
	/* ADC set: ADC reverse */
	LCD_SendConfigData(0xA1);
	/* Common output mode select: Normal COM0~COM63  */
	LCD_SendConfigData(0xC0);
	/* Display normal: Display normal */
	LCD_SendConfigData(0xA6);
	/* LCD bias set: Set bias 1/9 (Duty 1/65) */
	LCD_SendConfigData(0xA2);
	/* Power control set: Booster, Regulator and Follower on*/
	LCD_SendConfigData(0x2F);
	/* Booster ratio set: Set internal booster to 4x*/
	LCD_SendConfigData(0xF8);
	LCD_SendConfigData(0x00);
	/* V0 voltage regulator set: Contrast set */
	LCD_SendConfigData(0x27);
	/*  Electronic volume mode set: Contrast set */
	LCD_SendConfigData(0x81);
	LCD_SendConfigData(0x16);
	/* Static indicator set: No indicator */
	LCD_SendConfigData(0xAC);
	LCD_SendConfigData(0x00);
	/* Display ON/OFF: Display ON */
	LCD_SendConfigData(0xAF);
}

/**
 * @fn LCD_SendConfigData
 * @brief Send configuration data to display.
 * @param data Configuration data to send.
 */
void LCD_SendConfigData(uint8_t data) {
	/* Ensure command mode is enabled */
	LCD_SetLCDMode(LCD_COMMAND_MODE);
	SPI2_SendData(data);
}

/**
 * @fn LCD_SetBacklightState
 * @brief Enables or disables LCD backlight.
 * @param state State of the backlight. state >=1 => enabled, 0 = disabled.
 */
void LCD_SetBacklightState(uint8_t state) {
	if (state >= 1) {
		/* Enable backlight */
		HAL_GPIO_WritePin(LCD_BACKLIGHT_PORT, LCD_BACKLIGHT_PIN, GPIO_PIN_SET);
	}
	else {
		/* Disable backlight */
		HAL_GPIO_WritePin(LCD_BACKLIGHT_PORT, LCD_BACKLIGHT_PIN, GPIO_PIN_RESET);
	}
}

/**
 * @fn LCD_SetLCDMode
 * @brief Switch the mode of the LCD between
 * LCD_DATA_MODE and LCD_COMMAND_MODE. In command mode
 * the LCD is setup. Data mode is used to plot on the screen.
 * @param mode
 */
void LCD_SetLCDMode(LCD_MODE mode) {
	if (mode == LCD_COMMAND_MODE) {
		/* Set LCD to command mode */
		HAL_GPIO_WritePin(LCD_CD_PORT, LCD_CD_PIN, GPIO_PIN_RESET);
	}
	else {
		/* Set LCD to data mode */
		HAL_GPIO_WritePin(LCD_CD_PORT, LCD_CD_PIN, GPIO_PIN_SET);
	}
}

/**
 * @fn LCD_SetPageData
 * @brief Write data on a page of the display.
 * @param x Selects the column.
 * @param page Selects the page.
 * @param data Data to print.
 */
void LCD_SetPageData(uint8_t x, uint8_t page, uint8_t data) {
	/* Page boundaries check */
	if (page > 7) {
		return;
	}

	/* Set page and column */
	LCD_SetLCDMode(LCD_COMMAND_MODE);

	/* set page
	 * C/D D7 D6 D5 D4 D3 D2 D1 D0
	 * 0    1  0  1  1  0 A2 A1 A0
	 *          0xB
	 */

	uint8_t cmd = 0xB0 | page;
	SPI2_SendData(cmd);

	/* Send column upper nibble */
	cmd = 0x10 | (x >> 4);
	SPI2_SendData(cmd);

	/* Send column lower nibble */
	cmd = 0x00 | (0xF & x);
	SPI2_SendData(cmd);

	/* Send print data */
	LCD_SetLCDMode(LCD_DATA_MODE);
	SPI2_SendData(data);

	/* Switch LCD back to command mode */
	LCD_SetLCDMode(LCD_COMMAND_MODE);
}

/**
 * @fn LCD_DirectClear
 * @brief Clear the display by writing 0x0 in all column-page combinations.
 */
void LCD_Clear() {
	int page = 0;
	int column = 0;

	for (page = 0; page < LCD_Pages; page++) {
		for (column = 0; column < LCD_Width; column++) {
			LCD_SetPageData(column, page, 0x0);
		}
	}
}

/**
 * @fn LCD_DirectClearFast
 * @brief Clear the display by writing 0x0 in all column-page combinations.
 * This implementation is much faster since the auto-foreward function of the
 * LCD is used. After a page and a column have been selected, the display automatically
 * selects the next column in the selected page. This reduces the overhead because less
 * control data must be transmitted.
 */
void LCD_ClearFast() {
	int page = 0;
	int column = 0;

	for(page = 0; page < LCD_Pages; page++) {
		for(column = 0; column < LCD_Width; column++) {
			if(column == 0) {
				LCD_SetPageData(column, page, 0x0);
			}
			else {
				LCD_SetLCDMode(LCD_DATA_MODE);
				SPI2_SendData(0x0);
				LCD_SetLCDMode(LCD_COMMAND_MODE);
			}
		}
	}
}

/**
 * @fn LCD_Flicker
 * @brief Nostalgic lcd flickering.
 */
void LCD_Flicker() {
	LCD_OnOff(1);
	HAL_Delay(25);
	LCD_OnOff(0);
	HAL_Delay(25);
}

/**
 * @fn LCD_OnOff
 * @brief Turn LCD on or off.
 * @param state State >= 1 turns the LCD on. State == 0 turns the LCD off.
 */
void LCD_OnOff(uint8_t state) {
	if (state >= 1) {
		SPI2_SendData(0xAF);
	}
	else {
		SPI2_SendData(0xAE);
	}
}

/**
 * @fn LCD_PutPixel
 * @brief Put a single pixel on the LCD.
 * @param x Pixel x-coordinate.
 * @param y Pixel y-coordinate.
 */
void LCD_PutPixel(uint8_t x, uint8_t y) {
	static uint8_t prev_x, prev_page, prevdata;

	/* Page = Y/8 */
	uint8_t page = y >> 3;

	/* Pixel = Y%8 */
	uint8_t pixel = y & 7;

	uint8_t data = 1 << pixel;

	if ((page == prev_page) && (x == prev_x)) {
		prevdata |= data;
	}
	else {
		prev_x = x;
		prev_page = page;
		prevdata = data;
	}
	LCD_SetPageData(prev_x, prev_page, prevdata);
}
