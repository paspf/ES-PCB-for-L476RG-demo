/*******************************************************************************
  * @file    	pushbutton.h
  * @author  	paspf
  * @version 	V1.3
  * @date    	2022-03-22
  * @copyright	GNU Public License
  * @brief   	Pushbutton functions.
  *
*******************************************************************************/

#ifndef _pushbuttons_h
#define _pushbuttons_h

#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/* BTN debouncer */
typedef struct {
  uint16_t BTN_1;
  uint16_t BTN_2;
} BTN_HoldState;

/* Public typedefs -----------------------------------------------------------*/
typedef enum {
  BTN_ID_1 = 0,
  BTN_ID_2,
  BTN_NUCLEO
} BTN_Identifier;

typedef enum {
  BTN_State_Released=0,
  BTN_State_Pressed
} BTN_State;

/* Public defines ------------------------------------------------------------*/
#define BTN_1_PIN        	GPIO_PIN_9
#define BTN_1_PORT      	GPIOB

#define BTN_2_PIN        	GPIO_PIN_8
#define BTN_2_PORT      	GPIOB

#define BTN_NUCLEO_PIN      GPIO_PIN_13
#define BTN_NUCLEO_PORT     GPIOC

/* Public macros -------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void BTN_Board_Init();
void BTN_Init(void);
void BTN_UpdateButtonStates(void);
BTN_State BTN_GetButtonState(BTN_Identifier ButtonID);
void BTN_ResetButtonState(BTN_Identifier ButtonID);

#endif /* _pushbuttons_h */
