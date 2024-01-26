/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "gpio.h"
#include "i2c.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ssd1306.h"
#include <math.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define SSD1306_REFRESH_START_5 SSD1306_WIDTH * 5
#define SSD1306_REFRESH_START_6 SSD1306_WIDTH * 6
#define SSD1306_REFRESH_START_7 SSD1306_WIDTH * 7
#define TOTAL_START 100
#define TOTAL_END 999

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t total = TOTAL_START;
uint16_t times = 0;
uint16_t fps = 0;
// char get_length_temp[11] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM6) {
    fps = times;
    times = 0;
  }
}

/*
uint8_t get_length(uint32_t num)
{
  uint8_t length = 0;
  do
  {
    num /= 10;
    length++;
  } while (num != 0);
  return length;
}
*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); // 亮灯

  if (ssd1306_Init(&hi2c1) != 0) {
    Error_Handler();
  }

  HAL_Delay(200);

  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); // 灭灯

  ssd1306_SetCursor(7, 0);
  ssd1306_WriteString("Speed Test", Font_11x18, White);

  ssd1306_SetCursor(0, 30);
  ssd1306_WriteString("FPS", Font_7x10, White);

  ssd1306_UpdateScreen(&hi2c1);

  ssd1306_WriteCommand(&hi2c1, 0xB6);

  char buffer[17] = {0};
  // char formatter[11] = "%u     %lu\0";
  // char formatter[11] = "%u %lu\0\0\0";
  // uint8_t total_length = 0;
  // uint8_t formatter_filling, last_fps_length = 0, fps_length = 0;
  uint8_t commands5[] = {0xB5, 0x21, 0x00, 21};
  uint8_t commands6[] = {0xB6, 0x21, 0x00, 21};
  // uint8_t commands7[] = {0xB7, 0x21, 0x00, 28};

  HAL_TIM_Base_Start_IT(&htim6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /*
    fps_length = get_length(fps);
    if (fps_length != last_fps_length)
    {
      last_fps_length = fps_length;
      for (formatter_filling = 0; formatter_filling < 6 - fps_length;
    formatter_filling++)
      {
        formatter[formatter_filling + 2] = ' ';
      }
      formatter[(formatter_filling++) + 2] = '%';
      formatter[(formatter_filling++) + 2] = 'l';
      formatter[(formatter_filling++) + 2] = 'u';
      for (; formatter_filling < 9; formatter_filling++)
      {
        formatter[formatter_filling + 2] = 0;
      }
    }
    */

    ssd1306_SetCursor(0, 40);
    // sprintf(buffer, formatter, fps, ++total);
    sprintf(buffer, "%u", fps);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_SetCursor(0, 48);
    // sprintf(buffer, formatter, fps, ++total);
    sprintf(buffer, "%lu", ++total);
    ssd1306_WriteString(buffer, Font_7x10, White);

    // total_length = get_length(total) * 7;
    // total_length = 28;
    // commands[2] = total_length + 42;
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x00, 1, commands5, 4, 100);

    // HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x40, 1,
    // &SSD1306_Buffer[SSD1306_REFRESH_START_6], 42 + total_length, 100);
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x40, 1,
                      &SSD1306_Buffer[SSD1306_REFRESH_START_5], 21, 100);

    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x00, 1, commands6, 4, 100);
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x40, 1,
                      &SSD1306_Buffer[SSD1306_REFRESH_START_6], 21, 100);

    times++;

    if (total == TOTAL_END) {
      total = TOTAL_START;
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType =
      RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
