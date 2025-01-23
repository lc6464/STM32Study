#include "main.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

#include <array>

struct Color {
	uint8_t green;
	uint8_t red;
	uint8_t blue;
};

// std::array<Color, 8> colors = {
// 	{
// 		{ 255, 0, 0 }, { 0, 255, 0 }, { 0, 0, 255 },
// 		{ 255, 255, 0 }, { 0, 255, 255 }, { 255, 0, 255 },
// 		{ 0, 0, 0 }, { 255, 255, 255 }
// 	}
// };
std::array<Color, 2> colors = {
	{
		{ 255, 0, 0 }, { 0, 255, 0 }
	}
};

// std::array<uint16_t, 48 * 8 + 1> data{};
std::array<uint16_t, 48 * 2 + 1> data{};

template <size_t N>
static inline HAL_StatusTypeDef WS2812B_Send(std::array<Color, N> &colors) {
	// std::array<uint8_t, 48 * N + 1> data{};

	// 0b0 -> 1, 4; 0b1 -> 4, 1; last byte is 0
	// color order is GRB
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < 8; j++) {
			auto green = colors[i].green & (1 << (7 - j));
			data[48 * i + 2 * j] = green ? 5 : 1;
			data[48 * i + 2 * j + 1] = green ? 1 : 5;
		}

		for (size_t j = 0; j < 8; j++) {
			auto red = colors[i].red & (1 << (7 - j));
			data[48 * i + 16 + 2 * j] = red ? 5 : 1;
			data[48 * i + 16 + 2 * j + 1] = red ? 1 : 5;
		}

		for (size_t j = 0; j < 8; j++) {
			auto blue = colors[i].blue & (1 << (7 - j));
			data[48 * i + 32 + 2 * j] = blue ? 5 : 1;
			data[48 * i + 32 + 2 * j + 1] = blue ? 1 : 5;
		}
	}

	data[48 * N] = 0;
	// HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	return HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)(data.data()), data.size());
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
}


int main(void) {
	HAL_Init();
	SystemClock_Config();

	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM1_Init();

	// HAL_TIM_Base_Start(&htim1);

	// HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

	// bool last = false;
	// while (1) {
	// 	last = !last;
	// 	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, last ? 1 : 5);
	// 	HAL_Delay(50);
	// }



	HAL_Delay(100);

	while (1) {
		WS2812B_Send(colors);
		HAL_Delay(100);
	}

	while (1) {
	}
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct{};
	RCC_ClkInitTypeDef RCC_ClkInitStruct{};

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
