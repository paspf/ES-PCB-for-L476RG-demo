/*******************************************************************************
  * @file    isr.c
  * @author  paspf
  * @version V1.0
  * @date    2022-11-13
  * @copyright	paspf, GNU Public License 3
  * @brief   Default Interrupt Service Routines.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_tim2_ch2_ch7;
extern TIM_HandleTypeDef TIM_HandleBTN;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @fn DMA1_Channel7_IRQHandler
 * @brief Handles DMA1 channel7 global interrupt.
 */
void DMA1_Channel7_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_tim2_ch2_ch7);
}

/**
 * @fn TIM4_IRQHandler
 * @brief Handles TIM3 interrupt requests.
 */
void TIM4_IRQHandler() {
    HAL_TIM_IRQHandler(&TIM_HandleBTN);
}
