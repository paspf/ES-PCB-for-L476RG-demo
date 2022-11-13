/********************************************************************************
  * @file    	ws2812b.c
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-12
  * @copyright	GNU Public License
  * @brief   	Initialize all devices required for the WS2812b leds.
********************************************************************************/

#include "stm32l4xx.h"
#include "stm32l4xx_nucleo.h"
#include "ws2812b_devices.h"

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch7;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @fn Error_Handler
 * @brief Called when an error occurs.
 */
static void Error_Handler() {
	while(1);
}

/**
 * @fn TIM2_Init
 * @brief Initialize TIM2.
 */
void TIM2_Init() {
  /* Peripheral clock enable */
  __HAL_RCC_TIM2_CLK_ENABLE();

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
    Error_Handler();
  }

  /* TIM clock source configuration */
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
    Error_Handler();
  }

  /* TIM master configuration */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }

  /* TIM output compare register configuration */
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @fn TIM2_GPIO_Init
 * @brief Connect TIM2 channel 2 to GPIO PB3.
 */
void TIM2_GPIO_Init() {
	/* Peripheral clock enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
 * @fn DMA_Init.
 * @brief Enable DMA controller clock, enable NVIC.
 */
void DMA_Init() {
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
 * @fn TIM2_DMA_Init
 * @brief Initialize DMA for TIM2.
 */
void TIM2_DMA_Init() {
    /* Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* TIM2 DMA Init */
    /* TIM2_CH2_CH4 Init */
    hdma_tim2_ch2_ch7.Instance = DMA1_Channel7;
    hdma_tim2_ch2_ch7.Init.Request = DMA_REQUEST_4;
    hdma_tim2_ch2_ch7.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim2_ch2_ch7.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim2_ch2_ch7.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim2_ch2_ch7.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim2_ch2_ch7.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim2_ch2_ch7.Init.Mode = DMA_NORMAL;
    hdma_tim2_ch2_ch7.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_tim2_ch2_ch7) != HAL_OK) {
      Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(&htim2, hdma[TIM_DMA_ID_CC2],hdma_tim2_ch2_ch7);
}

/**
 * @fn TIM2_DMA_DeInit
 * @brief Deinit DMA.
 */
void TIM2_DMA_DeInit() {
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* TIM2 DMA DeInit */
    HAL_DMA_DeInit(htim2.hdma[TIM_DMA_ID_CC2]);
}
