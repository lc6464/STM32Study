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
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BEEPER_VOLUME 15

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

const float frequency_n1[] = {
    130.81278, 146.83238, 164.81378, 174.61412, 195.99772, 220.00000, 246.94165,
};

const float frequency_0[] = {
    261.62557, 293.66477, 329.62756, 349.22823, 391.99543, 440.00000, 493.88330,
};

const float frequency_1[] = {
    523.25113, 587.32953, 659.25512, 698.45646, 783.99087, 880.00000, 987.76660,
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void set_prescaler(float frequency) // 通过设置预分频器来得到需要的频率
{
  __HAL_TIM_SET_PRESCALER(&htim2, (uint32_t)(100000.0 / frequency) - 1);
}

void set_volume(uint8_t volume) // 0~100
{
  // mapping: 0~100 -> 0~900
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, volume * 9);
}

void play(float frequency, uint32_t duration) {
  HAL_GPIO_WritePin(GPIOC, LED_R_Pin, GPIO_PIN_RESET);
  if (frequency >= 0.0) // 小于零不改变频率
  {
    set_prescaler(frequency);
  }
  HAL_Delay(duration - 80);
  HAL_GPIO_WritePin(GPIOC, LED_R_Pin, GPIO_PIN_SET);
  HAL_Delay(80);
  set_volume(0);
  HAL_Delay(20);
  set_volume(BEEPER_VOLUME);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM6) {
    HAL_GPIO_TogglePin(GPIOC, LED_B_Pin);
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
  MX_TIM2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  set_volume(BEEPER_VOLUME);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    // 播放扬基歌
    play(frequency_0[0], 480);  // 1
    play(-1.0, 480);            // 1
    play(frequency_0[1], 480);  // 2
    play(frequency_0[2], 480);  // 3
    play(frequency_0[0], 480);  // 1
    play(frequency_0[2], 480);  // 3
    play(frequency_0[1], 480);  // 2
    play(frequency_n1[4], 480); //.5

    play(frequency_0[0], 480);  // 1
    play(-1.0, 480);            // 1
    play(frequency_0[1], 480);  // 2
    play(frequency_0[2], 480);  // 3
    play(frequency_0[0], 980);  // 1 -
    play(frequency_n1[6], 480); // .7
    play(frequency_n1[4], 480); // .5

    play(frequency_0[0], 480); // 1
    play(-1.0, 480);           // 1
    play(frequency_0[1], 480); // 2
    play(frequency_0[2], 480); // 3
    play(frequency_0[3], 480); // 4
    play(frequency_0[2], 480); // 3
    play(frequency_0[1], 480); // 2
    play(frequency_0[0], 480); // 1

    play(frequency_n1[6], 480); // .7
    play(frequency_n1[4], 480); // .5
    play(frequency_n1[5], 480); // .6
    play(frequency_n1[6], 480); // .7
    play(frequency_0[0], 980);  // 1 -
    play(-1.0, 980);            // 1 -

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // 歇一秒
    set_volume(0);
    HAL_Delay(1000);
    set_volume(BEEPER_VOLUME);
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
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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
