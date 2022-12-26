/********************************************************************************
  * @file    	ws2812b.c
  * @author  	paspf
  * @version 	V1.0
  * @date    	2022-03-12
  * @copyright	paspf, GNU Public License 3
  * @brief   	Send color codes to an array of ws2812b leds using DMA.
********************************************************************************/

#include "ws2812b.h"
#include "stm32l4xx.h"
#include "stm32l4xx_nucleo.h"

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t led_brightness = 255;
static uint8_t led_data[NUM_LEDS][3];
static uint8_t data_is_sent_flag = 0;
static uint32_t pwm_duty[(24*NUM_LEDS)+50];


/* Private function prototypes -----------------------------------------------*/
static void apply_brightness(uint8_t *r, uint8_t *g, uint8_t *b);
static long map(long x, long input_max, long output_max);

/* Functions -----------------------------------------------------------------*/

/**
 * @fn WS2812b_set_color
 * @brief Set the color of a single led.
 * @param led_id Id of the led.
 * @param red Red color code (8 bit).
 * @param green Green color code (8 bit).
 * @param blue Blue color code (8 bit).
 * @return 0 on success. -1 if led_id is not within NUM_LEDS.
 */
int WS2812b_set_color(int led_id, uint8_t red, uint8_t green, uint8_t blue) {
	if (led_id > NUM_LEDS) {
		return -1;
	}
	led_data[led_id][0] = red;
	led_data[led_id][1] = green;
	led_data[led_id][2] = blue;
	return 0;
}

/**
 * @fn WS2812b_set_brightness.
 * @brief Set the brighness of all leds.
 * @param brightness rightness between 0 and 255.
 */
void WS2812b_set_brightness(uint8_t brightness) {
	led_brightness = brightness;
}

/**
 * @fn WS2812b_send
 * @brief Send led color array to WS2812 leds using a DMA and TIM2 channel 2.
 */
void WS2812b_send() {
	uint32_t led_id = 0;
	uint32_t color;

	for (int i = 0; i < NUM_LEDS; i++) {
		uint8_t r = led_data[i][0];
		uint8_t g = led_data[i][1];
		uint8_t b = led_data[i][2];

		// Apply brightness.
		apply_brightness(&r,&g,&b);

		// Write all RGB values into color.
		color = ((g<<16) | (r<<8) | (b));

		// Loop through the 24 color bits of a LED.
		for (int i = 23; i >= 0; i--) {
			if (color & (1<<i)) {
				// Transmit '1' bit: 850ns high, 400ns low -> 68% PWM duty @800KHz PWM.
				pwm_duty[led_id] = 68;
			}
			else {
				// Transmit '0' bit: 400ns high, 850ns low -> 32% PWM duty @800KHz PWM.
				pwm_duty[led_id] = 32;
			}
			led_id++;
		}
	}

	// Reset code: just send 50 us '0'.
	for (int i = 0; i < 50; i++) {
		pwm_duty[led_id] = 0;
		led_id++;
	}

	// Start PWM with DMA.
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, pwm_duty, led_id);

	// Wait until data is transmitted to WS2812b leds.
	while (!data_is_sent_flag){};
	data_is_sent_flag = 0;
}

/**
 * @fn fade
 * @brief Fade a single value. Private function.
 * @param brightness Current brightness / value to fade.
 * @param fade_amount amount to fade / change / fade speed.
 * @param direction direction of fade / change.
 * @return Faded brightness value.
 */
static int fade(int brightness, int fade_amount, FADE_DIRECTION direction) {
	// Reverse the direction of the fading if downward is set.
	if (direction == FADE_DIRECTION_DOWNWARDS) {
		fade_amount = -fade_amount;
	}

	// Add amount to brightness level.
	brightness = brightness + fade_amount;

	// Limit brightness to 8-bit (0-255).
	if (brightness < 0) brightness = 0;
	if (brightness > MAX_LED_VALUE) brightness = MAX_LED_VALUE;

	// Returns the new brightness level.
	return brightness;
}

/**
 * @fn color_fade
 * @brief Fade a single led. Private function.
 * @param arr_pixel Array containing 8 bit elements for each color. Passed by
 * reference and updated by this function.
 * @param level Current fade level (0-2).
 * @param amount Amount to fade.
 */
static void color_fade(uint8_t *arr_pixel, uint8_t *level, uint8_t amount) {
	uint8_t* brightnessRed = &arr_pixel[0];
	uint8_t* brightnessGreen = &arr_pixel[1];
	uint8_t* brightnessBlue = &arr_pixel[2];

	// Level 0: red 100% - 0%, green 0% - 100%.
	if (*level == 0) {
		*brightnessRed = fade(*brightnessRed, amount, FADE_DIRECTION_DOWNWARDS);
		*brightnessGreen = fade(*brightnessGreen, amount, FADE_DIRECTION_UPWARDS);
		if(*brightnessRed == 0 && *brightnessGreen == MAX_LED_VALUE) (*level)++;
	}
	// Level 1: green 100% - 0%, blue 0% - 100%.
	else if (*level == 1) {
		*brightnessGreen = fade(*brightnessGreen, amount, FADE_DIRECTION_DOWNWARDS);
		*brightnessBlue = fade(*brightnessBlue, amount, FADE_DIRECTION_UPWARDS);
		if(*brightnessGreen == 0 && *brightnessBlue == MAX_LED_VALUE) (*level)++;
	}
	// Level 3: blue 100% - 0%, red 0% - 100%.
	else if (*level == 2) {
		*brightnessBlue = fade(*brightnessBlue, amount, FADE_DIRECTION_DOWNWARDS);
		*brightnessRed = fade(*brightnessRed, amount, FADE_DIRECTION_UPWARDS);
		if(*brightnessBlue == 0 && *brightnessRed == MAX_LED_VALUE) *level = 0;
	}
	else {
		// Error.
		while(1);
	}
}

/**
 * @fn WS2812b_fade_leds
 * @brief Fade all leds. Blocking function. Use abort to stop fading.
 * @param abort Passed by reference, set to true, to stop fading.
 */
void WS2812b_fade_leds(bool *abort) {
	static uint8_t level[NUM_LEDS] = {0};

	led_data[0][0] = 255;
	led_data[0][1] = 0;
	led_data[0][2] = 0;

	led_data[1][0] = 191;
	led_data[1][1] = 64;
	led_data[1][2] = 0;

	led_data[2][0] = 127;
	led_data[2][1] = 128;
	led_data[2][2] = 0;

	led_data[3][0] = 63;
	led_data[3][1] = 192;
	led_data[3][2] = 0;

	WS2812b_set_brightness(50);
	WS2812b_send();

	while (!*abort) {
		for (int i = 0; i < NUM_LEDS; i++) {
			color_fade(&led_data[i][0], &level[i], 5);
		}
		WS2812b_send();
		HAL_Delay(50);
	}
}

/**
 * @fn apply_brightness
 * @brief Apply the configured brightness to the given colors.
 * @param r Addresses of the color red to map within the global brightness.
 * @param g Addresses of the color green to map within the global brightness.
 * @param b Addresses of the color blue to map within the global brightness.
 */
static void apply_brightness(uint8_t *r, uint8_t *g, uint8_t *b) {
	*r = map(*r, MAX_LED_VALUE, led_brightness);
	*g = map(*g, MAX_LED_VALUE, led_brightness);
	*b = map(*b, MAX_LED_VALUE, led_brightness);
}


/**
 * @fn map
 * @brief Maps an input value from 0 to input_max to an output value within the
 * range 0 to output max.
 * @param x Value to map.
 * @param input_max Maximum expected input
 * @param output_max Maximum allowed output.
 * @return Mapped input x.
 */
static long map(long x, long input_max, long output_max) {
    return (x * output_max) / input_max;
}

/**
 * @fn HAL_TIM_PWM_PulseFinishedCallback
 * @brief Overwrite _weak HAL function. Register if DMA is finished.
 * @param htim Timer handle.
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_2);
		data_is_sent_flag = 1;
	}
}
