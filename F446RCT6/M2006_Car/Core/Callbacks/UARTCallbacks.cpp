#include "RemoteControl_Shared.hpp"
#include "usart.h"
// #include "SEGGER_RTT.h"

// 串口 DMA 接收完成回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (remoteControl.RxCallback(huart)) {
		// 如果接收成功
		remoteControl >> controllerData;

		// Segger RTT 输出遥控器数据
		// SEGGER_RTT_printf(0, "LX: %d, LY: %d, RX: %d, RY: %d, LS: %d, RS: %d, LSW: %d, RSW: %d\n",
		// 	controllerData.LeftX, controllerData.LeftY, controllerData.RightX, controllerData.RightY,
		// 	controllerData.LeftSlider, controllerData.RightSlider, controllerData.LeftSwitch, controllerData.RightSwitch);

		return;
	}
}