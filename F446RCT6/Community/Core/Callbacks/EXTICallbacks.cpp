#include <cstdint>

#include "gpio.h"
#include "usart.h"

#include "DelayTimer.hpp"

// 指示无线串口模块是否正在配置
bool isWirelessUARTModuleBeingConfigured = false;

constexpr uint8_t ConfigurationData[] = {
	0xAA, 0x5A,
	0x22, 0x44, // 网络 0x2244
	0x11, 0x33, // 节点 0x1133
	0x00, 0x06, // 固定 0, 功率 6 (-8dBm)
	0x00, 0x08, // 固定 0, 波特率 8 (115200bps)
	0x00, 0x00, // 固定 0, 信道 0
	0x00, 0x00, // 固定 0x0000
	0x02, 0x12, // 空中速率 2 (2Mbps), 似乎是固定 0x12
	0x00, 0xD0  // 固定 0, 和校验 0xD0
};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == Key1_Pin) {
		// 按下 Key1，配置无线串口模块
		isWirelessUARTModuleBeingConfigured = true;

		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET); // 灭红灯
		HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET); // 亮蓝灯
		HAL_GPIO_WritePin(Wireless_SET_GPIO_Port, Wireless_SET_Pin, GPIO_PIN_RESET); // 进入配置模式

		// 等待无线串口模块进入配置模式
		DelayTimer::DelayMilliseconds(10);

		uint8_t waitTimes = 0;

		// 直接通过 TxState 判断串口是否忙
		while (huart1.gState == HAL_UART_STATE_BUSY_TX) {
			DelayTimer::DelayMilliseconds(10); // 等待
			if (++waitTimes >= 10) { // 最多等待 100ms
				break;
			}
		}

		if (huart1.gState != HAL_UART_STATE_READY) {
			HAL_GPIO_WritePin(Wireless_SET_GPIO_Port, Wireless_SET_Pin, GPIO_PIN_SET); // 退出配置模式
			HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET); // 灭蓝灯
			isWirelessUARTModuleBeingConfigured = false;

			// 如果串口忙，亮红灯
			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET); // 亮红灯
			return;
		}

		// 保存原波特率并修改波特率至 9600bps
		uint32_t originalBaudRate = huart1.Init.BaudRate;
		huart1.Init.BaudRate = 9600;
		HAL_StatusTypeDef status = HAL_HalfDuplex_Init(&huart1);

		if (status != HAL_OK) {
			HAL_GPIO_WritePin(Wireless_SET_GPIO_Port, Wireless_SET_Pin, GPIO_PIN_SET); // 退出配置模式
			HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET); // 灭蓝灯
			isWirelessUARTModuleBeingConfigured = false;

			// 如果设置波特率失败，亮红灯
			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET); // 亮红灯
			return;
		}

		status = HAL_UART_Transmit(&huart1, ConfigurationData, sizeof(ConfigurationData), 100);

		// 恢复原波特率
		huart1.Init.BaudRate = originalBaudRate;
		HAL_StatusTypeDef restoreStatus = HAL_HalfDuplex_Init(&huart1);

		HAL_GPIO_WritePin(Wireless_SET_GPIO_Port, Wireless_SET_Pin, GPIO_PIN_SET); // 退出配置模式
		HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET); // 灭蓝灯
		isWirelessUARTModuleBeingConfigured = false;

		if (status != HAL_OK || restoreStatus != HAL_OK) {
			// 如果发送失败或恢复失败，亮红灯
			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET); // 亮红灯
		}
	}
}