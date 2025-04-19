/**
 * @file        batt.h
 * @brief       Battery driver header
 * @author      Esteban CADIC
 * @version     1.0
 * @date        2025
 * @copyright   MIT License
 *
 */

#ifndef INC_BATT_H_
#define INC_BATT_H_

#include "main.h"

#define BATT_LOW_VOLTAGE		3.4
#define BATT_CRITICAL_VOLTAGE	2.9

extern float BATT_MeasureVoltage(void);

#endif /* INC_BATT_H_ */
