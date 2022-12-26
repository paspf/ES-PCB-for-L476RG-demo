/**
  ******************************************************************************
  * @file    	main.c
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-12
  * @copyright	paspf, GNU Public License 3
  * @brief   	Default main function for Nucleo L476RG. Demo.
  ******************************************************************************
*/

#include "stm32l4xx.h"
#include "stm32l4xx_nucleo.h"
#include "stddef.h"
#include "ws2812b_devices.h"
#include "ws2812b.h"
#include "leds.h"
#include "spi.h"
#include "lcd.h"
#include "lcd_drawing.h"
#include "pushbutton.h"
#include "tests.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MS_TST_PAUSE 500
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_ch2_ch7;
char ok_text[3] = "OK";

/* Private function prototypes -----------------------------------------------*/
int swap(int* a, int* b);
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

int main(void) {
  /* STM32L4xx HAL library initialization:
	   - Configure flash, instruction and Data caches
	   - Configure systick to generate an interrupt each 1 msec
	   - Set NVIC Group Priority to 4
	   - Global MSP (MCU Support Package) initialization
	 */
	HAL_Init();

	// Configure the system clock to 80 MHz.
	SystemClock_Config();

	// Run swap demo.
	int a = 3;
	int b = 200;
	swap(&a, &b);

	// Initialize on-board led.
	leds_onboard_init();

	// Initialize on-board (nucleo) button.
	BTN_Board_Init();

	// Initialize periphery leds.
	leds_periphery_board_init();

	// Initialize display.
	LCD_Init();
	LCD_ClearFast();

	// Plot demo text to display.
	LCD_Print(52, 1, "demo", LCD_Font_6x7lcd);
	LCD_DrawLine(0, 18, 127, 18);
	LCD_Print(0, 3, "B1:", LCD_Font_6x7lcd);
	LCD_Print(0, 4, "S1:", LCD_Font_6x7lcd);
	LCD_Print(0, 5, "S2:", LCD_Font_6x7lcd);
	LCD_Print(50, 3, "D1-8:", LCD_Font_6x7lcd);
	LCD_Print(50, 4, "BRD:", LCD_Font_6x7lcd);
	LCD_Print(50, 5, "WSx:", LCD_Font_6x7lcd);

	// Initialize WS2812b leds.
	TIM2_Init();
	TIM2_GPIO_Init();
	DMA_Init();
	TIM2_DMA_Init();

	// Send color code to LEDs.
	HAL_Delay(50);
	WS2812b_set_color(0, 0, 0, 0);
	WS2812b_set_color(1, 0, 0, 0);
	WS2812b_set_color(2, 0, 0, 0);
	WS2812b_set_color(3, 0, 0, 0);
	WS2812b_send();
	HAL_Delay(50);

	// Initialize periphery buttons.
	BTN_Init();

	// Test buttons.
	uint8_t test_result = test_buttons();

	// Test leds.
	HAL_Delay(MS_TST_PAUSE);
	test_result |= test_leds();
	HAL_Delay(MS_TST_PAUSE);
	leds_periphery_board_light_up();
	HAL_Delay(MS_TST_PAUSE);
	leds_periphery_board_off();

	// Test on-board LED.
	HAL_Delay(MS_TST_PAUSE);
	test_result |= test_board_led();

	// Test on WS2812B.
	HAL_Delay(MS_TST_PAUSE);
	test_result |= test_ws2812();

	// Tests done.
	LCD_Print(0, 7, "   ", LCD_Font_6x7lcd);
	LCD_Print(20, 7, "Test done: ", LCD_Font_6x7lcd);
	LCD_Print(115, 7, "  ", LCD_Font_6x7lcd);
	if (test_result == 0) {
		LCD_Print(82, 7, "PASS ", LCD_Font_6x7lcd);
		bool abort = false;
		// Endless loop.
		WS2812b_fade_leds(&abort);
	}
	else {
		LCD_Print(82, 7, "FAIL ", LCD_Font_6x7lcd);
		WS2812b_set_color(0, 255, 0, 0);
		WS2812b_set_color(1, 255, 0, 0);
		WS2812b_set_color(2, 255, 0, 0);
		WS2812b_set_color(3, 255, 0, 0);
		WS2812b_send();
	}

	for (;;) {
		// Endless loop.
		HAL_Delay(50);
	}
}

/**
 * @fn swap
 * Swap values a and b.
 * @param a Will be replaced by b.
 * @param b Will be replaced by a.
 * @return 0 on success.
 */
int swap(int* a, int* b) {
	if (a == NULL || b == NULL) {
		return -1;
	}
	int tmp = *a;
	*a = *b;
	*b = tmp;
	return 0;
}

/**
 * @fn HAL_TIM_PeriodElapsedCallback
 * @brief Called when the period of any TIM is elapsed.
 * @param htim Reference to the triggering timer.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance == TIM4) {
		BTN_UpdateButtonStates();
	}
}

/**
 * @fn SystemClock_Config
 * @brief Configuration of the system clock.
 * The system clock is configured as follows:
 * 	System Clock source = PLL (HSI)
 * 	SYSCLK              = 80MHz
 *  HCLK                = 80MHz
 *  AHB Prescaler       = 1
 *  APB1 Prescaler      = 1
 *  APB2 Prescaler      = 1
 *  HSI Frequency       = 16MHz
 *  VDD(V)              = 3.3
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // Configure the main internal regulator output voltage.
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
    Error_Handler();
  }

  // Initializes the RCC Oscillators.
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  // Initializes the CPU, AHB and APB buses clocks.
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @fn Error_Handler
 * @brief Executed in case of error occurrence.
 */
static void Error_Handler(void) {
  while(1) {}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  while (1) {}
}
#endif
