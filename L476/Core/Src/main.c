/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>

#include "../../Sensors/bmp280/bmp280.h"
#include "../../Sensors/bmp280/bmp280_application.h"

#include "../../Sensors/vl6180x/vl6180x_application.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void bmp280_loop()
{
//	bmp280_initialize();
	bmp280_application_initialize();

	double altitude;
	uint8_t msg[64];
	uint16_t msg_len;

	while(true)
	{
		bmp280_application_get_altitude_delta(&altitude);

		msg_len = (uint16_t)sprintf((char*)msg, "BMP280: Altitude: %7.3f cm\r\n", altitude*100);
		HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);
		HAL_Delay(2000);
	}

}


void vl6180x_loop()
{
	bool result;
	uint8_t msg[92];
	uint16_t msg_len;
	uint8_t distance_mm;
	HAL_StatusTypeDef retval;

	uint8_t data_buffer[2] = {0};
	uint8_t memory_address = 0x000;
	uint16_t data_length = 2; //sizeof(data_buffer)/ sizeof(data_buffer[0]);
	uint16_t device_address = (0x29 << 1);


	retval = HAL_I2C_Mem_Read(&hi2c3, device_address, 0x016, I2C_MEMADD_SIZE_16BIT, data_buffer, data_length, HAL_MAX_DELAY);
	if (retval == HAL_OK)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X - read registers: addr: 0x%x | Startup flag: %d (0x%x) [OK]\r\n", 0x016,  data_buffer[0], data_buffer[1]);
	}
	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);


	retval = HAL_I2C_Mem_Read(&hi2c3, device_address, memory_address, I2C_MEMADD_SIZE_16BIT, data_buffer, data_length, HAL_MAX_DELAY);
	if (retval == HAL_OK)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X - read registers: addr: 0x%x | Device ID: %d (0x%x) [OK]\r\n", memory_address,  data_buffer[0], data_buffer[1]);
	}
	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);


	retval = HAL_I2C_Mem_Read(&hi2c3, device_address, memory_address, I2C_MEMADD_SIZE_8BIT, data_buffer, data_length, HAL_MAX_DELAY);
	if (retval == HAL_OK)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X - read registers: addr: 0x%x | Device ID: %d (0x%x) [OK]\r\n", memory_address, data_buffer[0], data_buffer[1]);
	}
	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);


	result = vl6180x_application_initialize_device();
	if (result == true)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X: device initialization OK: %d\r\n", result);

	}
	else
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X: device initialization FAILED: %d\r\n", result);
	}
	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);

	while(true && (result == true))
	{
		vl6180x_application_poll_measurement(&distance_mm);
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X: Distance: %dmm\r\n", distance_mm);
		HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);

		HAL_Delay(1000);
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */

  uint8_t msg[64];
  sprintf((char *)msg, "Hello World\r\n");
  HAL_UART_Transmit(&huart2, msg, (uint16_t)strlen((char*)msg), HAL_MAX_DELAY);

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 20e3);

  uint16_t msg_len;

  for (uint8_t i2c_addr = 0; i2c_addr < 127; i2c_addr++)
  {
	  HAL_StatusTypeDef result = HAL_I2C_IsDeviceReady(&hi2c3, (i2c_addr << 1), 3, 1000);
	  if (result == HAL_OK)
	  {
		  msg_len = (uint16_t)sprintf((char *)msg, "I2C device found at: %d (0x%x)\r\n", i2c_addr, i2c_addr);
		  HAL_UART_Transmit(&huart2, msg, msg_len, 1000);
	  }
  }

//  bmp280_loop();
  vl6180x_loop();

  msg_len = (uint16_t)sprintf((char *)msg, "Uh, we're not supposed to come here :/\r\n");
  HAL_UART_Transmit(&huart2, msg, msg_len, 1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
  while (1)
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
