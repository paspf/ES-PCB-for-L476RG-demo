/********************************************************************************
  * @file    	spi.c
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-22
  * @copyright	GNU Public License
  * @brief   	SPI configuration
********************************************************************************/

/* Includes -------------------------------------------------------------------*/
#include "spi.h"

/* External variables --------------------------------------------------------*/
/* Private typedefs -----------------------------------------------------------*/
/* Private defines ------------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
uint8_t SPI2_ChipSelectLocked = 1;
SPI_HandleTypeDef hspi;

/* Private function prototypes ------------------------------------------------*/
/* Private functions ----------------------------------------------------------*/

/**
 * @fn SPI2_Init()
 * @brief Initialize SPI2.
 */
void SPI2_Init() {
	/**** Enable Clock ****/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_SPI2_CLK_ENABLE();

	/**** SPI Chip Select Configuration ****/

	/* Setup Chip Select (CS) for display */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = SPI2_CS_LCD_PIN;
	HAL_GPIO_Init(SPI2_CS_LCD_PORT, &GPIO_InitStructure);

	/* Setup Chip Select (CS) for plug-wire */
	//GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	//GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	//GPIO_InitStructure.Pin = SPI2_CS_PLUG_PIN;
	//HAL_GPIO_Init(SPI2_CS_PLUG_PORT, &GPIO_InitStructure);

	/* Set Chip Select to negative LOW */
	HAL_GPIO_WritePin(SPI2_CS_LCD_PORT, SPI2_CS_LCD_PIN, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(SPI2_CS_PLUG_PORT, SPI2_CS_PLUG_PIN, GPIO_PIN_SET);


	/* SPI2 MOSI MISO and CLK */
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);


	/**** SPI Configuration ****/
	hspi.Instance = SPI2;

	/* Specifies the SPI operating mode */
	hspi.Init.Mode = SPI_MODE_MASTER;
	/* Specifies the SPI bidirectional mode state */
	hspi.Init.Direction = SPI_DIRECTION_2LINES;
	/* Specifies the SPI data size */
	hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	/* Specifies the serial clock steady state */
	hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
	/* Specifies the clock active edge for the bit capture */
	hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
	/* Specifies whether the NSS signal is managed by hardware (NSS pin) or by software using the SSI bit */
	hspi.Init.NSS = SPI_NSS_SOFT;
	/* Specifies the Baud Rate prescaler value which will be used to configure the transmit and receive SCK clock */
	hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // fPCLK / prescaler = 80MHz / 8 = 10MHz
	/* Specifies whether data transfers start from MSB or LSB bit */
	hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	/* Specifies if the TI mode is enabled or not.  */
	hspi.Init.TIMode = SPI_TIMODE_DISABLED;
	/* Specifies if the CRC calculation is enabled or not */
	hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	/* Specifies whether the NSSP signal is enabled or not */
	hspi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

	/* Initialize the SPI registers */
	HAL_SPI_Init(&hspi);

	/* Everything ready, we unlock the chip select */
	SPI2_UnlockCS();
}

/**
 * @fn SPI2_SendData
 * @brief Send a single byte using SPI2.
 * @param data Data to send.
 */
void SPI2_SendData(uint8_t data) {
	/* Wait until SPI is ready */
	while (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY) {
		// Do nothing....
	}
	/* Send SPI data */
	 HAL_SPI_Transmit(&hspi, &data, 1, HAL_MAX_DELAY);
	/* Wait for display */
	for (int i = 0; i < 10; i ++);
}

/**
 * @fn SPI2_SelectDevice
 * @brief Select a SPI device using chip select wires.
 * @param device Device to use.
 * @return 0 on success.
 */
uint8_t SPI2_SelectDevice(SPI2_Device device) {
	/* Abort if someone is already accessing the bus */
	if (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY) return 0;

	/* Disable all devices */
	HAL_GPIO_WritePin(SPI2_CS_LCD_PORT, SPI2_CS_LCD_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI2_CS_PLUG_PORT, SPI2_CS_PLUG_PIN, GPIO_PIN_SET);

	switch (device) {
		case SPI2_Device_LCD:
			HAL_GPIO_WritePin(SPI2_CS_LCD_PORT, SPI2_CS_LCD_PIN, GPIO_PIN_RESET);
			break;
		case SPI2_Device_Plug:
			HAL_GPIO_WritePin(SPI2_CS_PLUG_PORT, SPI2_CS_PLUG_PIN, GPIO_PIN_RESET);
			break;
		default:
			return 1;
		}
  SPI2_LockCS();

  return 0;
}

/**
 * @fn SPI2_LockCS
 * @brief Lock SPI2 chip select / selected device.
 */
void SPI2_LockCS() {
  SPI2_ChipSelectLocked = 1;
}

/**
 * @fn SPI2_UnlockCS
 * @brief Unlock SPI2 chip select / selected device.
 */
void SPI2_UnlockCS() {
  SPI2_ChipSelectLocked = 0;
}
