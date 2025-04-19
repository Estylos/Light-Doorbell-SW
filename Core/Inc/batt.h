/**
 * @file        batt.h
 * @brief       Battery driver
 * @author      Esteban CADIC
 * @version     1.0
 * @date        2025
 * @copyright   MIT License
 *
 */

#ifndef INC_BATT_H_
#define INC_BATT_H_

#include "main.h"


/*------------------------------------------------------------------------------
	CONSTANTS
------------------------------------------------------------------------------*/

/** Battery low voltage threshold */
#define BATT_LOW_VOLTAGE		3.4
/** Battery critical voltage threshold (system must be stopped) */
#define BATT_CRITICAL_VOLTAGE	2.9


/*------------------------------------------------------------------------------
	DECLARATIONS
------------------------------------------------------------------------------*/

/**
 * @brief Measure the battery voltage.
 * This function will also measure the VREF voltage to correctly calculate the battery voltage from the ADC value.
 * This is needed because VCC is not constant due to the discharge of the battery (if < 3V).
 * 
 * @return Battery voltage in volts 
 */
extern float BATT_MeasureVoltage(void);

#endif /* INC_BATT_H_ */
