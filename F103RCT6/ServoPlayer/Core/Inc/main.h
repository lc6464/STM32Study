/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file           : main.h
	* @brief          : Header for main.c file.
	*                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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

	void SystemClock_Config(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_R_Pin GPIO_PIN_13
#define LED_R_GPIO_Port GPIOC
#define LED_G_Pin GPIO_PIN_14
#define LED_G_GPIO_Port GPIOC
#define LED_B_Pin GPIO_PIN_15
#define LED_B_GPIO_Port GPIOC
#define Encoder_SW_Pin GPIO_PIN_2
#define Encoder_SW_GPIO_Port GPIOC
#define Encoder_SW_EXTI_IRQn EXTI2_IRQn
#define WLM_SET_Pin GPIO_PIN_4
#define WLM_SET_GPIO_Port GPIOC
#define WLM_FUNC_Pin GPIO_PIN_5
#define WLM_FUNC_GPIO_Port GPIOC
#define VBat_Sample_Enable_Pin GPIO_PIN_10
#define VBat_Sample_Enable_GPIO_Port GPIOA
#define Key_A_Pin GPIO_PIN_11
#define Key_A_GPIO_Port GPIOA
#define Key_A_EXTI_IRQn EXTI15_10_IRQn
#define Key_B_Pin GPIO_PIN_12
#define Key_B_GPIO_Port GPIOA
#define Key_B_EXTI_IRQn EXTI15_10_IRQn
#define CS_Accel_Pin GPIO_PIN_12
#define CS_Accel_GPIO_Port GPIOC
#define CS_GYRO_Pin GPIO_PIN_2
#define CS_GYRO_GPIO_Port GPIOD
#define Key_C_Pin GPIO_PIN_4
#define Key_C_GPIO_Port GPIOB
#define Key_C_EXTI_IRQn EXTI4_IRQn
#define Key_D_Pin GPIO_PIN_5
#define Key_D_GPIO_Port GPIOB
#define Key_D_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
