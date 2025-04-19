/**
 * @file        batt.c
 * @brief       Battery driver
 * @author      Esteban CADIC
 * @version     1.0
 * @date        2025
 * @copyright   MIT License
 *
 */

#include <stdio.h>

#include "batt.h"
#include "adc.h"


/*------------------------------------------------------------------------------
	CONSTANTS
------------------------------------------------------------------------------*/

#define VREFINT									1.2
#define ADC_RAW_TO_VOLT(value_adc, vref_adc)	( (value_adc * VREFINT) / vref_adc)

// Voltage divider was calculated to give 3.0V (VCC) when VBAT = 4.5V
#define BATT_VOLTAGE_FORMULA(x)					(x * 1.56)


/*------------------------------------------------------------------------------
	FONCTIONS
------------------------------------------------------------------------------*/

float BATT_MeasureVoltage(void) {
	uint32_t vref_adc_raw = 0;
	uint32_t batt_adc_raw = 0;

	HAL_GPIO_WritePin(BATT_EN_GPIO_Port, BATT_EN_Pin, GPIO_PIN_SET);

	HAL_Delay(10); // Wait 10 ms before measuring (avoid wrong measurements)

	HAL_ADC_Start(&hadc);

	// Measure BATT voltage
	HAL_ADC_PollForConversion(&hadc, 100); // 100 ms timeout
	batt_adc_raw = HAL_ADC_GetValue(&hadc);

	// Measure VREF (1.2V reference)
	HAL_ADC_PollForConversion(&hadc, 100);
	vref_adc_raw = HAL_ADC_GetValue(&hadc);

	HAL_GPIO_WritePin(BATT_EN_GPIO_Port, BATT_EN_Pin, GPIO_PIN_RESET);
	HAL_ADC_Stop(&hadc);

	return BATT_VOLTAGE_FORMULA(ADC_RAW_TO_VOLT(batt_adc_raw, vref_adc_raw));
}
