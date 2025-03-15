/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern uint8_t g_flag_switch;
extern uint8_t g_flag_message;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BATT_ADC_Pin GPIO_PIN_0
#define BATT_ADC_GPIO_Port GPIOA
#define BATT_EN_Pin GPIO_PIN_2
#define BATT_EN_GPIO_Port GPIOA
#define RFM69_CS_Pin GPIO_PIN_4
#define RFM69_CS_GPIO_Port GPIOA
#define RFM69_SCK_Pin GPIO_PIN_5
#define RFM69_SCK_GPIO_Port GPIOA
#define RFM69_MISO_Pin GPIO_PIN_6
#define RFM69_MISO_GPIO_Port GPIOA
#define RFM69_MOSI_Pin GPIO_PIN_7
#define RFM69_MOSI_GPIO_Port GPIOA
#define RFM69_DI0_IRQ_Pin GPIO_PIN_0
#define RFM69_DI0_IRQ_GPIO_Port GPIOB
#define RFM69_DI0_IRQ_EXTI_IRQn EXTI0_1_IRQn
#define RFM69_RST_Pin GPIO_PIN_1
#define RFM69_RST_GPIO_Port GPIOB
#define USART_TX_Pin GPIO_PIN_9
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_10
#define USART_RX_GPIO_Port GPIOA
#define LED_GRN_EN_Pin GPIO_PIN_3
#define LED_GRN_EN_GPIO_Port GPIOB
#define LED_RED_EN_Pin GPIO_PIN_4
#define LED_RED_EN_GPIO_Port GPIOB
#define SW_IN_Pin GPIO_PIN_6
#define SW_IN_GPIO_Port GPIOB
#define SW_IN_EXTI_IRQn EXTI4_15_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
