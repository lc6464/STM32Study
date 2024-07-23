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

#include "PID.h"
#include "encoder.h"
#include "motor.h"

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

// 四个电机
Motor motor0(&htim1, TIM_CHANNEL_3, TIM_CHANNEL_4);
Motor motor1(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2);
Motor motor2(&htim8, TIM_CHANNEL_3, TIM_CHANNEL_4);
Motor motor3(&htim8, TIM_CHANNEL_2, TIM_CHANNEL_1);

// 四个编码器
Encoder encoder0(&htim3);
Encoder encoder1(&htim4);
Encoder encoder2(&htim2);
Encoder encoder3(&htim5);

// 四个 PID
PIDController pid0(-1.0f, -2.0f, -0.1f, 0.1f, -1000, 1000, -800, 800, 0.1f);
PIDController pid1(-1.0f, -2.0f, -0.1f, 0.1f, -1000, 1000, -800, 800, 0.1f);
PIDController pid2(-1.0f, -2.0f, -0.1f, 0.1f, -1000, 1000, -800, 800, 0.1f);
PIDController pid3(-1.0f, -2.0f, -0.1f, 0.1f, -1000, 1000, -800, 800, 0.1f);

int16_t target_speed = 0;
int16_t round_speed = 0;
int8_t speed_step = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // 编码器定时器溢出更新
  encoder0.OverflowCallback(htim);
  encoder1.OverflowCallback(htim);
  encoder2.OverflowCallback(htim);
  encoder3.OverflowCallback(htim);

  if (htim->Instance == TIM6) {
    // 100ms TIM6

    encoder0.Update();
    float result0 =
        pid0.Update((float)(round_speed + target_speed), encoder0.GetSpeed());
    motor0.SetSpeed(static_cast<int16_t>(result0));

    encoder1.Update();
    float result1 =
        pid1.Update((float)(round_speed + target_speed), encoder1.GetSpeed());
    motor1.SetSpeed(static_cast<int16_t>(result1));

    encoder2.Update();
    float result2 =
        pid2.Update((float)(round_speed - target_speed), encoder2.GetSpeed());
    motor2.SetSpeed(static_cast<int16_t>(result2));

    encoder3.Update();
    float result3 =
        pid3.Update((float)(-round_speed + target_speed), encoder3.GetSpeed());
    motor3.SetSpeed(static_cast<int16_t>(result3));

    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
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
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_I2C2_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */

  encoder0.Start();
  encoder1.Start();
  encoder2.Start();
  encoder3.Start();

  motor0.Start();
  motor1.Start();
  motor2.Start();
  motor3.Start();

  HAL_TIM_Base_Start_IT(&htim6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    // 电机速度：300 -> -300 | 3s | -300 -> 300 | 3s
    if (target_speed >= 250 || target_speed <= -250) {
      speed_step = -speed_step;
      HAL_Delay(3000);
    }

    target_speed += speed_step << 1;
    round_speed += speed_step;
    HAL_Delay(50);

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored                                                 \
    "-Wmissing-field-initializers" // 这段代码由 CubeMX 自动生成，不做出修改

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

#pragma GCC diagnostic pop

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
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
           number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
     file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
