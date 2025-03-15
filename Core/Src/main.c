/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "retarget.h"
#include "rfm69.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DOORBELL_CODE 0x42
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t g_flag_switch = 0;
uint8_t g_flag_message = 0;

uint8_t flag_sleep = 0;

// 2nd byte required otherwise FIFO will not empty...
uint8_t tx_message[] = { DOORBELL_CODE };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void MCU_Sleep(void);
static void MCU_Wakeup(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */
	uint8_t rx_buffer[1];
	RFM69_t tx;

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	HAL_Delay(2000); // Delay for the STLINK before sleep
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ADC_Init();
	MX_SPI1_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	RetargetInit(&huart1); // printf()

	// RFM69
	tx.cs.pin = RFM69_CS_Pin;
	tx.cs.port = RFM69_CS_GPIO_Port;
	tx.reset.pin = RFM69_RST_Pin;
	tx.reset.port = RFM69_RST_GPIO_Port;
	tx.spi = &hspi1;
	tx.high_power_en = 1;

	RFM69_Init(&tx);

	if(RFM69_SetPowerDBm(&tx, 20))
		printf("RFM69_SetPowerDBm() to 20 dBm failure!\n");

	RFM69_SetMode(&tx, RFM69_MODE_RX);
	printf("RFM69 initialized!\n");

	// Go to stop mode
	MCU_Sleep();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while(1)
	{
		if(flag_sleep == 1)
			MCU_Wakeup();

		if(g_flag_switch) // Switch pressed
		{
			HAL_GPIO_WritePin(LED_GRN_EN_GPIO_Port, LED_GRN_EN_Pin, GPIO_PIN_SET);

			// Changing DI0 mapping to nothing
			RFM69_ChangeDI0Mapping(&tx, RFM69_DI0_TX_NONE);

			// Sending the code
			printf("Switch pressed! Sending the code...\n");
			RFM69_SendMessage(&tx, tx_message, sizeof(tx_message) / sizeof(tx_message[0]));
			RFM69_SetMode(&tx, RFM69_MODE_RX);

			HAL_GPIO_WritePin(LED_GRN_EN_GPIO_Port, LED_GRN_EN_Pin, GPIO_PIN_RESET);
			g_flag_switch = 0;

			// Changing DI0 mapping to RX PayloadReady
			RFM69_ChangeDI0Mapping(&tx, RFM69_DI0_RX_PAYLOAD_READY);
		}

		if(g_flag_message) // Message in the RFM69 FIFO (DI0 IRQ)
		{
			// Disable RFM69 DI0 IRQ
			HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);

			printf("Reading the message in the RFM69 FIFO\n");
			size_t bytes_received = RFM69_ReceiveMessage(&tx, rx_buffer, sizeof(rx_buffer) / sizeof(rx_buffer[0]));

			if(bytes_received > 0)
			{
				printf("%d bytes received : ", bytes_received);
				for(size_t i = 0; i < bytes_received; i++)
					printf("%02X ", rx_buffer[i]);
				printf("\n");

				if(rx_buffer[0] == DOORBELL_CODE) // Doorbell code received
					BlinkLEDS();
			}

			g_flag_message = 0;

			// Enable RFM69 DI0 IRQ
			HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
		}

		if(g_flag_message == 0 && g_flag_switch == 0)
			MCU_Sleep();

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
static void MCU_Sleep(void)
{
	flag_sleep = 1;

	printf("Going to STM32 stop mode...\n");

	HAL_SuspendTick();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

static void MCU_Wakeup(void)
{
	printf("Waking up!\n");

	SystemClock_Config();
	HAL_ResumeTick();

	flag_sleep = 0;
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while(1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
