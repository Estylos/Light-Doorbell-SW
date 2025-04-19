/**
 * @file        leds.c
 * @brief       LEDs driver
 * @author      Esteban CADIC
 * @version     1.0
 * @date        2025
 * @copyright   MIT License
 *
 */

#include "leds.h"

#include "batt.h"

void LEDs_SetColorBatteryVoltage(void)
{
	float batt_voltage = BATT_MeasureVoltage();

	if(batt_voltage < BATT_LOW_VOLTAGE) {
		HAL_GPIO_WritePin(LED_RED_EN_GPIO_Port, LED_RED_EN_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LED_GRN_EN_GPIO_Port, LED_GRN_EN_Pin, GPIO_PIN_SET);
	}
}

void LEDs_Reset(void)
{
	HAL_GPIO_WritePin(LED_GRN_EN_GPIO_Port, LED_GRN_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_RED_EN_GPIO_Port, LED_RED_EN_Pin, GPIO_PIN_RESET);
}

void LEDs_RXMessage(void)
{
	for(uint8_t i = 0; i < 3; i++)
	{
		for(uint8_t j = 0; j < 3; j++)
		{
			LEDs_SetColorBatteryVoltage();
			HAL_Delay(300);
			LEDs_Reset();
			HAL_Delay(100);
		}
		HAL_Delay(500);
	}

}
