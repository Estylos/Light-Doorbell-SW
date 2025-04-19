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

extern void LEDs_SetColorBatteryVoltage(void);
extern void LEDs_Reset(void);
extern void LEDs_RXMessage(void);

#endif /* INC_LEDS_H_ */
