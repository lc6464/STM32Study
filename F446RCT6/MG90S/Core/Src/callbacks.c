#include "main.h"
#include "usart.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void USART1_ReceiveProcessor(uint32_t data_length) {

  char *buffer = (char *)uart_receive_buffer;

  if (sscanf(buffer, "%hu", &compare) == 1) {
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"OK!\n", 4);
    return;
  }

  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"ERROR!\n", 7);
}

void USART1_IDLECallback(UART_HandleTypeDef *huart) { // USART1 空闲中断回调函数

  if (huart->Instance == USART1 &&
      __HAL_UART_GET_FLAG(huart,
                          UART_FLAG_IDLE)) { // 判断是否是 USART1 的空闲中断

    __HAL_UART_CLEAR_IDLEFLAG(huart); // 清除串口空闲中断标志位
    uint32_t data_length =
        UART_RECEIVE_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);

    HAL_UART_DMAStop(huart);

    USART1_ReceiveProcessor(data_length);

    memset(uart_receive_buffer, 0, UART_RECEIVE_BUFFER_SIZE);
    HAL_UART_Receive_DMA(huart, uart_receive_buffer, UART_RECEIVE_BUFFER_SIZE);
  }
}

#ifdef __cplusplus
}
#endif