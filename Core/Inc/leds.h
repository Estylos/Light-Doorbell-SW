/**
 * @file        leds.h
 * @brief       LEDs driver header
 * @author      Esteban CADIC
 * @version     1.0
 * @date        2025
 * @copyright   MIT License
 *
 */

#ifndef INC_LEDS_H_
#define INC_LEDS_H_

#include "main.h"


/*------------------------------------------------------------------------------
	DECLARATIONS
------------------------------------------------------------------------------*/

/**
 * @brief Enable the green LED if the battery voltage is above the battery threshold, otherwise enable the red LED.
 * 
 */
extern void LEDs_SetColorBatteryVoltage(void);

/**
 * @brief Disable all LEDs.
 * 
 */
extern void LEDs_Reset(void);

/**
 * @brief Blink the LEDs to indicate a message reception.
 * Depending on the battery voltage, the green or red LED will blink.
 * 
 */
extern void LEDs_RXMessage(void);

#endif /* INC_LEDS_H_ */
