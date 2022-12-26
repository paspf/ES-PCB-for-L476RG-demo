/********************************************************************************
  * @file    	pushbutton.c
  * @author  	paspf
  * @version 	V1.4
  * @date    	2022-03-22
  * @copyright	paspf, GNU Public License 3
  * @brief   	Pushbutton functions.
  *
  * 			UM1725 page 876		TIM IRQ handler management, function summary
  * 			UM1725 page 58		NVIC functions
  * 			UM1725 page 149		NVIC functions
  * 			UM1725 page 893		TIM functions: HAL_TIM_IRQHandler
  * 			UM1725 page 899		TIM functions: HAL_TIM_PeriodElapsedCallback
  * 			UM1725 page 1281	LL GPIO Generic Driver: LL_GPIO_ReadOutputPort
  * 			UM1860 page 15		STM32CubeL4 MCU Package structure (shows where the low level drivers are stored)
********************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "pushbutton.h"
#include "stm32l4xx.h"

/* External variables ---------------------------------------------------------*/
/* Private typedefs -----------------------------------------------------------*/
/* Private defines ------------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
// Current state of push buttons.
static BTN_State BTN_1_State, BTN_2_State, BTN_NUCLEO_State;
static BTN_HoldState BTN_States;

// Initialize TIM low level resources.
TIM_HandleTypeDef TIM_HandleBTN = {0};

/* Private function prototypes ------------------------------------------------*/
/* Public functions -----------------------------------------------------------*/

/**
 * @fn BTN_Board_Init
 * @brief Prepares required STM32 peripherals for the on board button.
 */
void BTN_Board_Init() {
	// Enable GPIOC clock.
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = BTN_NUCLEO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BTN_NUCLEO_PORT, &GPIO_InitStruct);

	BTN_NUCLEO_State = BTN_State_Released;
}

/**
 * @fn BTN_Init
 * @brief Prepares required STM32 peripherals for S1 and S2.
 */
void BTN_Init(void) {
	// Enable GPIOC clock.
	__HAL_RCC_GPIOB_CLK_ENABLE();

	// Set output mode for pin 5 of port A.
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructure.Pin = BTN_1_PIN | BTN_2_PIN;

	HAL_GPIO_Init(BTN_1_PORT, &GPIO_InitStructure);

	// Set default button states.
	BTN_1_State = BTN_State_Released;
	BTN_2_State = BTN_State_Released;
	BTN_States.BTN_1 = 0x0;
	BTN_States.BTN_2 = 0x0;

	// Specify channel of TIMx.
	TIM_HandleBTN.Channel = HAL_TIM_ACTIVE_CHANNEL_1;

	// Specify clock devision.
	TIM_HandleBTN.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	// Specify counting direction
	TIM_HandleBTN.Init.CounterMode = TIM_COUNTERMODE_UP;

	/* Specify TIMx frequency.
	 * Clock / Prescaler / Period = Frequency
	 * 80 000 000 / 40 000 / 20 = 100Hz
	 * */
	TIM_HandleBTN.Init.Period = 20;
	TIM_HandleBTN.Init.Prescaler = 40000;

	// Countdown restarts with the RepetitionCounter value.
	TIM_HandleBTN.Init.RepetitionCounter = 0;

	// Specify the Timer TIMx.
	TIM_HandleBTN.Instance = TIM4;

	// Enable TIMx clock.
	__TIM4_CLK_ENABLE();

	// Initializes the TIM Time base Unit.
	HAL_TIM_Base_Init(&TIM_HandleBTN);

	// Starts the TIM Base generation in interrupt mode.
	HAL_TIM_Base_Start_IT(&TIM_HandleBTN);

	/*
	 * NVIC is described in RM0351 on page 395
	 */
	HAL_NVIC_SetPriority(TIM4_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

/**
 * @fn BTN_UpdateButtonStates
 * @brief Reads the buttons' GPIO inputs and applies a debouncing algorithm.
 * Should be called periodically, e.g. in 10ms (100Hz) intervals.
 */
void BTN_UpdateButtonStates(void) {
	// Left shift.
	BTN_States.BTN_1 = BTN_States.BTN_1 << 1;
	BTN_States.BTN_2 = BTN_States.BTN_2 << 1;

	// Read nucleo button. Button is low active.
	if (HAL_GPIO_ReadPin(BTN_NUCLEO_PORT, BTN_NUCLEO_PIN) == GPIO_PIN_RESET) {
		BTN_NUCLEO_State = BTN_State_Pressed;
	}

	// Read Button 1. Button is high active.
	if (HAL_GPIO_ReadPin(BTN_1_PORT, BTN_1_PIN) == GPIO_PIN_SET) {
		BTN_States.BTN_1 |= 0x1;
	}

	// Read Button 2. Button is high active.
	if (HAL_GPIO_ReadPin(BTN_2_PORT, BTN_2_PIN) == GPIO_PIN_SET) {
		BTN_States.BTN_2 |= 0x1;
	}

	// Check Button 1.
	if (BTN_States.BTN_1 == 0xFFFF) {
		BTN_1_State = BTN_State_Pressed;
	}
	else if (BTN_States.BTN_1 == 0x00) {
		BTN_1_State = BTN_State_Released;
	}

	// Check Button 2.
	if (BTN_States.BTN_2 == 0xFFFF) {
		BTN_2_State = BTN_State_Pressed;
	}
	else if (BTN_States.BTN_2 == 0x00) {
		BTN_2_State = BTN_State_Released;
	}
}

/**
 * @fn BTN_GetButtonState
 * @brief Returns the debounced state of a button.
 * @param ButtonID ID of the button.
 * @return State of the selected button.
 */
BTN_State BTN_GetButtonState(BTN_Identifier ButtonID) {
    switch (ButtonID){
		case BTN_NUCLEO:
			return BTN_NUCLEO_State;

        case BTN_ID_1:
        	return BTN_1_State;

        case BTN_ID_2:
        	return BTN_2_State;
    }
    return 0;
}

/**
 * @fn BTN_ResetButtonState
 * @brief Sets a button state to BTN_STATE_Released.
 * @param ButtonID ID of the selected button.
 */
void BTN_ResetButtonState(BTN_Identifier ButtonID) {
    switch (ButtonID) {
		case BTN_NUCLEO:
			BTN_NUCLEO_State = BTN_State_Released;
			break;

        case BTN_ID_1:
			BTN_1_State = BTN_State_Released;
			BTN_States.BTN_1 = 0x00;
			break;

        case BTN_ID_2:
			BTN_2_State = BTN_State_Released;
			BTN_States.BTN_2 = 0x00;
			break;
    }
}
