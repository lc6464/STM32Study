#include "main.h"
#include "usart.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void USART1_ReceiveProcessor(uint32_t data_length) {
  ToggleLED(LED_G_Pin);

  while (data_length--) {
    uart_receive_buffer[data_length] =
        tolower(uart_receive_buffer[data_length]);
  }

  char *buffer = (char *)uart_receive_buffer;

  if (strstr(buffer, "hello world") != NULL) {
    ToggleLED(LED_R_Pin);
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"Hello!\n", 7);

    return;
  }

  if (sscanf(buffer, "%ld + %ld", &a, &b) == 2) {
    c = (int64_t)a + (int64_t)b;
    operation = '+';

    return;
  }

  if (sscanf(buffer, "%ld - %ld", &a, &b) == 2) {
    c = (int64_t)a - (int64_t)b;
    operation = '-';

    return;
  }

  if (sscanf(buffer, "%ld * %ld", &a, &b) == 2) {
    c = (int64_t)a * (int64_t)b;
    operation = '*';

    return;
  }

  if (sscanf(buffer, "%ld / %ld", &a, &b) == 2) {
    if (b == 0) {
      HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"Divisor cannot be zero!\n",
                            24);
    } else {
      operation = '/';
    }

    return;
  }

  if (sscanf(buffer, "%lu", &d) == 1) {
    operation = 'n';

    return;
  }

  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"ERROR!\n", 7);
}

void USART1_IDLECallback(UART_HandleTypeDef *huart) // USART1 空闲中断回调函数
{

  if (huart->Instance == USART1 &&
      __HAL_UART_GET_FLAG(huart,
                          UART_FLAG_IDLE)) // 判断是否是 USART1 的空闲中断
  {
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