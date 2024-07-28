#include "main.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

#include "strings.h"
#include "motor.h"

#include "PID_Shared.h"
#include "SSD1306_Shared.h"


// 电机信息
motor_info_t motor_info[MOTOR_MAX_NUM] = { 0 };

int16_t motor0_speed = -50;
int16_t motor1_speed = 50;
int8_t motor0_step = -10;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


// CAN 接收回调函数
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CAN_RxHeaderTypeDef rx_header = { 0 };
	uint8_t rx_data[8];

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data); // 接收数据

	int8_t result = parse_motor_feedback(rx_header, rx_data, motor_info); // 解析电机反馈数据

	if (result != -1) {
		// 获取电机的转速
		float current_speed = (float)motor_info[result].rotor_speed;

		if (result == 0) {
			pid0.Update(motor0_speed, current_speed);
		} else {
			pid1.Update(motor1_speed, current_speed);
		}

		set_motor_current(0, (int16_t)pid0.GetOutput(), (int16_t)pid1.GetOutput(), 0, 0); // 设置电机电压
	}
}


// 定时器中断回调函�?
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		char buffer[10] = { 0 };

		ssd1306.Clear();

		// 数据标签
		ssd1306.SetCursor(0, 5);
		ssd1306.WriteString("PID 0:", SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(0, 15);
		ssd1306.WriteString("Speed 0:", SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(0, 30);
		ssd1306.WriteString("PID 1:", SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(0, 40);
		ssd1306.WriteString("Speed 1:", SSD1306Fonts::Font_7x10);

		// 电机 1 的 PID 输出和转速
		ssd1306.SetCursor(60, 5);
		int16ToString((int16_t)pid0.GetOutput(), buffer, 0);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(60, 15);
		int16ToString(motor_info[0].rotor_speed, buffer, 0);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		// 电机 2 的 PID 输出和转速
		ssd1306.SetCursor(60, 30);
		int16ToString((int16_t)pid1.GetOutput(), buffer, 0);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(60, 40);
		int16ToString(motor_info[1].rotor_speed, buffer, 0);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.UpdateScreen();
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
	MX_DMA_Init();
	MX_CAN1_Init();
	MX_I2C2_Init();
	MX_TIM6_Init();
	/* USER CODE BEGIN 2 */

	  // 启动 OLED 显示屏
	if (ssd1306.Start() != HAL_OK) {
		Error_Handler();
	}

	// 展示 Hello 字样
	ssd1306.SetCursor(36, 22);
	ssd1306.WriteString("Hello", SSD1306Fonts::Font_11x18);
	ssd1306.UpdateScreen();

	LC_CAN1_Config(); // 配置 CAN1

	HAL_TIM_Base_Start_IT(&htim6); // 启动定时器 6 中断

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

			// 每 20ms 给电机 0 speed - 10，电机 1 speed + 10，到 -3000/3000 后保持 5s，随后反向加减，直到 3000/-3000
		if (motor0_speed == 3000 || motor0_speed == -3000) {
			motor0_step = -motor0_step;
			HAL_Delay(5000);
		}
		if (motor0_speed == 0) {
			HAL_Delay(3000);
		}
		motor0_speed += motor0_step;
		motor1_speed = -motor0_speed;
		HAL_Delay(20);
	}
	/* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers" // 这段代码由 CubeMX 自动生成，不做出修改

	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

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
		/* User can add his own implementation to report the file name and line
			 number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
			 line) */
			 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
