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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ssd1306.h"
#include "fonts.h"
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

#define FEEDBACK_ID_BASE      0x205
#define CAN_CONTROL_ID_BASE   0x1ff
#define CAN_CONTROL_ID_EXTEND 0x2ff
#define MOTOR_MAX_NUM         7

// 电机信息结构�???
typedef struct {
	// CAN ID
	uint16_t can_id;
	// 设置的电压�??
	int16_t  set_voltage;
	// 电机的转子角�???
	uint16_t rotor_angle;
	// 电机的转子�?�度
	int16_t  rotor_speed;
	// 电机的扭矩电�???
	int16_t  torque_current;
	// 电机的温�???
	uint8_t  temperature;
} motor_info_t;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// 电机信息
motor_info_t motor_info[MOTOR_MAX_NUM] = { 0 };

uint8_t show_count = 0;

int16_t voltage = 0;
int8_t voltage_step = 2;
int16_t voltage_count = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CAN_RxHeaderTypeDef rx_header = { 0 };
	uint8_t rx_data[8];

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data); // 接收数据

	// 判断是否为电机反馈数�???
	if (rx_header.StdId >= FEEDBACK_ID_BASE && rx_header.StdId < FEEDBACK_ID_BASE + MOTOR_MAX_NUM) {
		uint8_t index = rx_header.StdId - FEEDBACK_ID_BASE; // 获取电机索引
		motor_info[index].rotor_angle = ((rx_data[0] << 8) | rx_data[1]);
		motor_info[index].rotor_speed = ((rx_data[2] << 8) | rx_data[3]);
		motor_info[index].torque_current = ((rx_data[4] << 8) | rx_data[5]);
		motor_info[index].temperature = rx_data[6];
	}
}

/**
  * @brief  通过 CAN 总线发�?�电机控制数�???
  * @param  id_range 设置 CAN ID 范围�???0 对应 0x1ff�???1 对应 0x2ff
  * @param  v1 电机 1 �??? 5 的电�???
  * @param  v2 电机 2 �??? 6 的电�???
  * @param  v3 电机 3 �??? 7 的电�???
  * @param  v4 电机 4 的电�???
  * @retval None
  */
void set_motor_voltage(uint8_t id_range, int16_t v1, int16_t v2, int16_t v3, int16_t v4) {
	CAN_TxHeaderTypeDef tx_header;
	uint8_t             tx_data[8];

	tx_header.StdId = id_range == 0 ? 0x1ff : 0x2ff;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = 8;

	tx_data[0] = v1 >> 8;
	tx_data[1] = v1;
	tx_data[2] = v2 >> 8;
	tx_data[3] = v2;
	tx_data[4] = v3 >> 8;
	tx_data[5] = v3;
	tx_data[6] = v4 >> 8;
	tx_data[7] = v4;

	HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, (uint32_t *)CAN_TX_MAILBOX0);
	//HAL_CAN_AddTxMessage(&hcan2, &tx_header, tx_data, (uint32_t *)CAN_TX_MAILBOX0);
}

// 定时器中断回调函�???
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		//uint8_t id = ++show_count / 4;
		uint8_t id = 1;

		if (show_count == 31) {
			show_count = 0;
		}

		char buffer[64] = { 0 };

		// 写电�??? ID
		ssd1306_SetCursor(58, 0);
		ssd1306_WriteChar('1' + id, Font_11x18, White);

		ssd1306_SetCursor(50, 20);
		uint16ToString(motor_info[id].rotor_angle, buffer, 5);
		ssd1306_WriteString(buffer, Font_7x10, White);

		ssd1306_SetCursor(50, 30);
		int16ToString(motor_info[id].rotor_speed, buffer, 5);
		ssd1306_WriteString(buffer, Font_7x10, White);

		ssd1306_SetCursor(57, 40);
		int16ToString(motor_info[id].torque_current, buffer, 5);
		ssd1306_WriteString(buffer, Font_7x10, White);

		ssd1306_SetCursor(43, 50);
		uint8ToString(motor_info[id].temperature, buffer, 3);
		ssd1306_WriteString(buffer, Font_7x10, White);

		ssd1306_UpdateScreen(&hi2c2);
	}

	if (htim->Instance == TIM7) {
		voltage += voltage_step;
		if (voltage == 25000) {
			voltage_step = 0;
			voltage_count++;
		} else if (voltage == -25000) {
			voltage_step = 0;
			voltage_count--;
		}

		if (voltage_step == 0) {
			if (voltage_count == 10000) {
				voltage_step = -2;
				voltage_count = 0;
			} else if (voltage_count == -10000) {
				voltage_step = 2;
				voltage_count = 0;
			}
		}
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
	MX_CAN1_Init();
	MX_I2C2_Init();
	MX_TIM6_Init();
	MX_TIM7_Init();
	/* USER CODE BEGIN 2 */

	if (ssd1306_Init(&hi2c2) != 0) {
		Error_Handler();
	}


	CAN_FilterTypeDef  can_filter;

	can_filter.FilterBank = 0; // filter 0
	can_filter.FilterMode = CAN_FILTERMODE_IDMASK; // mask mode
	can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
	can_filter.FilterIdHigh = 0;
	can_filter.FilterIdLow = 0;
	can_filter.FilterMaskIdHigh = 0;
	can_filter.FilterMaskIdLow = 0; // set mask 0 to receive all can id
	can_filter.FilterFIFOAssignment = CAN_RX_FIFO0; // assign to fifo0
	can_filter.FilterActivation = ENABLE; // enable can filter
	can_filter.SlaveStartFilterBank = 14; // only meaningful in dual can mode

	HAL_CAN_ConfigFilter(&hcan1, &can_filter); // init can1 filter
	HAL_CAN_Start(&hcan1); // start can1
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); // enable can1 rx interrupt

	//HAL_CAN_ConfigFilter(&hcan2, &can_filter); // init can1 filter
	//HAL_CAN_Start(&hcan2); // start can2
	//HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING); // enable can2 rx interrupt

	// 展示电机信息
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("Motor  Info", Font_11x18, White);

	ssd1306_SetCursor(0, 20);
	ssd1306_WriteString("Angle:", Font_7x10, White);
	ssd1306_SetCursor(0, 30);
	ssd1306_WriteString("Speed:", Font_7x10, White);
	ssd1306_SetCursor(0, 40);
	ssd1306_WriteString("Torque:", Font_7x10, White);
	ssd1306_SetCursor(0, 50);
	ssd1306_WriteString("Temp:", Font_7x10, White);
	ssd1306_UpdateScreen(&hi2c2);

	HAL_TIM_Base_Start_IT(&htim6); // 启动定时�???6
	HAL_TIM_Base_Start_IT(&htim7); // 启动定时�???7
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */



	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		set_motor_voltage(0, 1000, voltage, 1000, 1000); // 设置电机 1~4 电压

	}
	/* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

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
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
	/* USER CODE BEGIN 6 */
			/* User can add his own implementation to report the file name and line number,
				 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
				 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
