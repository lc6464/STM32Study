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
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <math.h>
#include <string.h>

#include "ist8310.h"
#include "strings.h"

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

char xyz[80] = {0};

/*
char mxyz[512] = { 0 };
float maxX = 0.0,
maxY = 0.0,
maxZ = 0.0,
minX = 0.0,
minY = 0.0,
minZ = 0.0;
*/

float Xoffest = (0.0 - 66.6) / 2.0, Yoffest = (34.5 - 30.0) / 2.0,
      Kx = 2.0 / (0.0 + 66.6), Ky = 2.0 / (34.5 + 30.0);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM6) {
    IST8310_ReadMegData(&ist8310_data);

    float x = ist8310_data.Data.X, y = ist8310_data.Data.Y,
          z = ist8310_data.Data.Z, magAngle = 0.0f;

    x = (x - Xoffest) * Kx;
    x = (y - Yoffest) * Ky;

    if ((x > 0) && (y > 0))
      magAngle = atan(y / x) * 57;
    else if ((x > 0) && (y < 0))
      magAngle = 360 + atan(y / x) * 57;
    else if ((x == 0) && (y > 0))
      magAngle = 90;
    else if ((x == 0) && (y < 0))
      magAngle = 270;
    else if (x < 0)
      magAngle = 180 + atan(y / x) * 57;

    strcpy(xyz, "X: ");
    uint8_t xLength = floatToString(x, xyz + 3);
    strcpy(xyz + 3 + xLength, "\nY: ");
    uint8_t yLength = floatToString(y, xyz + 3 + xLength + 4);
    strcpy(xyz + 3 + xLength + 4 + yLength, "\nZ: ");
    uint8_t zLength = floatToString(z, xyz + 3 + xLength + 4 + yLength + 4);
    strcpy(xyz + 3 + xLength + 4 + yLength + 4 + zLength, "\nAngle: ");
    uint8_t angleLength = floatToString(
        magAngle, xyz + 3 + xLength + 4 + yLength + 4 + zLength + 7);
    strcpy(xyz + 3 + xLength + 4 + yLength + 4 + zLength + 7 + angleLength,
           "\n");

    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)xyz,
                          3 + xLength + 4 + yLength + 4 + zLength + 7 +
                              angleLength + 1);
  }
}

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
  MX_DMA_Init();
  MX_I2C3_Init();
  MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  IST8310_Status_e status = IST8310_Init(&ist8310_data);
  if (status) {
    char error[24] = {0};

    strcpy(error, "IST8310 Init Error: ");

    *(error + 20) = status + 48;
    *(error + 21) = '\n';

    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)error, 22);

    Error_Handler();
  }

  /*

  uint8_t loading[] = { 230, 160, 161, 229, 135, 134, 228, 184, 173 };

  HAL_UART_Transmit_DMA(&huart1, loading, 9);

  for (uint16_t i = 0; i < 300; i++) {
    IST8310_ReadMegData(&ist8310_data);
    maxX = ist8310_data.Data.X > maxX ? ist8310_data.Data.X : maxX;
    maxY = ist8310_data.Data.Y > maxY ? ist8310_data.Data.Y : maxY;
    maxZ = ist8310_data.Data.Z > maxZ ? ist8310_data.Data.Z : maxZ;
    minX = ist8310_data.Data.X < minX ? ist8310_data.Data.X : minX;
    minY = ist8310_data.Data.Y < minY ? ist8310_data.Data.Y : minY;
    minZ = ist8310_data.Data.Z < minZ ? ist8310_data.Data.Z : minZ;

    HAL_Delay(60);
  }

  strcpy(mxyz, "Max X: ");
  uint8_t xLength = floatToString(maxX, mxyz + 6);
  strcpy(mxyz + 6 + xLength, "\nMax Y: ");
  uint8_t yLength = floatToString(maxY, mxyz + 6 + xLength + 8);
  strcpy(mxyz + 6 + xLength + 8 + yLength, "\nMax Z: ");
  uint8_t zLength = floatToString(maxZ, mxyz + 6 + xLength + 8 + yLength + 8);
  strcpy(mxyz + 6 + xLength + 8 + yLength + 8 + zLength, "\nMin X: ");
  uint8_t minXLength = floatToString(minX, mxyz + 6 + xLength + 8 + yLength + 8
  + zLength + 8); strcpy(mxyz + 6 + xLength + 8 + yLength + 8 + zLength + 8 +
  minXLength, "\nMin Y: "); uint8_t minYLength = floatToString(minY, mxyz + 6 +
  xLength + 8 + yLength + 8 + zLength + 8 + minXLength + 8); strcpy(mxyz + 6 +
  xLength + 8 + yLength + 8 + zLength + 8 + minXLength + 8 + minYLength, "\nMin
  Z: "); uint8_t minZLength = floatToString(minZ, mxyz + 6 + xLength + 8 +
  yLength + 8 + zLength + 8 + minXLength + 8 + minYLength + 8); strcpy(mxyz + 6
  + xLength + 8 + yLength + 8 + zLength + 8 + minXLength + 8 + minYLength + 8 +
  minZLength, "\n");

  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)mxyz,
    6 + xLength + 8 + yLength + 8 + zLength + 8 + minXLength + 8 + minYLength +
  8 + minZLength + 1);

  HAL_Delay(1000);

  */

  HAL_TIM_Base_Start_IT(&htim6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {

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

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
   */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
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
