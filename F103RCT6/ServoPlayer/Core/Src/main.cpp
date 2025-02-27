#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "BMI088_Shared.h"
#include "data.h"
#include "EC11_Shared.h"
#include "SSD1306_Shared.h"

void RegisterButtonCallbacks();

/**
	* @brief  The application entry point.
	* @retval int
	*/
int main(void) {
	HAL_Init();
	SystemClock_Config();

	MX_GPIO_Init();

	// 开蓝灯
	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);

	// 启用电池电压采样
	HAL_GPIO_WritePin(VBat_Sample_Enable_GPIO_Port, VBat_Sample_Enable_Pin, GPIO_PIN_SET);

	MX_DMA_Init();
	MX_ADC3_Init(); // 独立工作，采样电池电压和输入电压

	// 校准 ADC 并启动采样
	HAL_ADCEx_Calibration_Start(&hadc3);

	// ADC3 独立工作，DMA 传输
	HAL_ADC_Start_DMA(&hadc3, (uint32_t *)voltage_adc_buffer.data(), voltage_adc_buffer.size());

	MX_TIM1_Init(); // 用于生成 PWM 信号便于调试，其中 Channel1 可以控制舵机
	MX_TIM2_Init(); // 仅用于计数（定时），频率 2.5kHz
	MX_TIM3_Init(); // 用于读取 EC11 旋转编码器
	MX_TIM6_Init(); // 仅用于计数（定时），频率 1kHz
	MX_I2C1_Init(); // 连接两块屏幕，交替刷新
	MX_SPI2_Init(); // 连接 BMI088 IMU

	ec11.Start();
	RegisterButtonCallbacks();

	// 等待外设上电初始化
	HAL_Delay(50);

	uint8_t error_count = 0;

	// 启动 BMI088 IMU
	auto [accel_status, gyro_status] = bmi088.Start();
	error_count += static_cast<uint8_t>(accel_status) + static_cast<uint8_t>(gyro_status);

	// 启动 OLED 屏幕
	error_count += ssd1306_0.Start();
	error_count += ssd1306_1.Start();

	HAL_TIM_Base_Start_IT(&htim2); // 2.5kHz
	HAL_TIM_Base_Start_IT(&htim6); // 1kHz

	HAL_TIM_Base_Start_IT(&htim1); // 50Hz
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 启动 PWM 输出

	// 关蓝灯，开红/绿灯，如果初始化失败则红灯常亮，否则绿灯常亮
	HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin | (error_count ? LED_R_Pin : LED_G_Pin));

	while (1) {

	}
}

/**
	* @brief System Clock Configuration
	* @retval None
	*/
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct{ };
	RCC_ClkInitTypeDef RCC_ClkInitStruct{ };
	RCC_PeriphCLKInitTypeDef PeriphClkInit{ };

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
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
