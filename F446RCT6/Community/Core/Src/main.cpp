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
#include "can.h"
#include "gpio.h"
#include "i2c.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "community.h"
#include "fonts.h"
#include "ssd1306.h"
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

int16_t speed = 0;
int16_t target = 0;
int16_t pidOut = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM6) {
    constexpr uint8_t id = 0;

    char buffer[64] = {0};

    ssd1306_Fill(Black);

    // 展示电机信息
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Motor  Info", Font_11x18, White);

    ssd1306_SetCursor(0, 20);
    ssd1306_WriteString("Target:", Font_7x10, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString("Speed:", Font_7x10, White);
    ssd1306_SetCursor(0, 40);
    ssd1306_WriteString("PID Out:", Font_7x10, White);

    // 写电机 ID
    ssd1306_SetCursor(58, 0);
    ssd1306_WriteChar('1' + id, Font_11x18, White);

    ssd1306_SetCursor(50, 20);
    int16ToString(target, buffer, 0);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_SetCursor(50, 30);
    int16ToString(speed, buffer, 0);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_SetCursor(57, 40);
    int16ToString(pidOut, buffer, 0);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_UpdateScreen(&hi2c2);
  }
}

inline bool MotorSpeed_ReceivedCallback(CAN_RxHeaderTypeDef *rxHeader,
                                        uint8_t *rxData) {
  if (rxHeader->StdId == 0x1f0 && rxHeader->DLC == 6) {
    // 如果接收到的是 ID 为 0x1f0 的 6 字节数据
    /*
            txData[0] = static_cast<uint8_t>(speed);
            txData[1] = static_cast<uint8_t>(speed >> 8);
            txData[2] = static_cast<uint8_t>(target);
            txData[3] = static_cast<uint8_t>(target >> 8);
            txData[4] = static_cast<uint8_t>(pidOut);
            txData[5] = static_cast<uint8_t>(pidOut >> 8);
    */

    // 从 rxData 中解析出数据
    speed = rxData[0] | (rxData[1] << 8);
    target = rxData[2] | (rxData[3] << 8);
    pidOut = rxData[4] | (rxData[5] << 8);

    return true;
  }
  return false;
}

// 电机速度接收
Community community0(&hcan1, MotorSpeed_ReceivedCallback);
Community community1(&hcan2, MotorSpeed_ReceivedCallback);

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8];

  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data); // 接收数据

  if (community1.ExecuteRxCallback(&rx_header, rx_data)) {
    // 对 0x200 发送 0x00
    CAN_TxHeaderTypeDef tx_header;
    uint8_t tx_data[1] = {0};

    tx_header.StdId = 0x200;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 1;
    tx_header.TransmitGlobalTime = DISABLE;

    uint32_t mailbox;

    community1.Transmit(&tx_header, tx_data, &mailbox);

    return;
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
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_I2C2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  community0.Start();

  CAN_FilterTypeDef filterConfig;

  filterConfig.FilterBank = 14;
  filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  filterConfig.FilterIdHigh = 0;
  filterConfig.FilterIdLow = 0;
  filterConfig.FilterMaskIdHigh = 0;
  filterConfig.FilterMaskIdLow = 0;
  filterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  filterConfig.FilterActivation = ENABLE;
  filterConfig.SlaveStartFilterBank = 14;

  community1.Start(&filterConfig);

  ssd1306_Init(&hi2c2);

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored                                                 \
    "-Wmissing-field-initializers" // 这段代码由 CubeMX 自动生成，不做出修改

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

#pragma GCC diagnostic pop

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
