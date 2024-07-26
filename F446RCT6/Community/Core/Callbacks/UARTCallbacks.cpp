#include "RemoteControl_Shared.h"
#include "usart.h"

// 串口 DMA 接收完成回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (remoteControl.RxCallback(huart)) {
    // 如果接收成功
    remoteControl >> controllerData;
    return;
  }
}