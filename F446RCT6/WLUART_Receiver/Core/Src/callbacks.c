#include "i2c.h"
#include "main.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

#include "fonts.h"
#include "ssd1306.h"

#ifdef __cplusplus
extern "C" {
#endif

void USART1_ReceiveProcessor(uint32_t data_length) {
  if (data_length != 8) // 目标数据是类似于 00:00:00 的样式，长度为 8
  {
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"Invalid",
                          7); // 向 USART1 报告数据无效
    return;
  }

  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"OK", 2); // 向 USART1 报告数据有效
  HAL_UART_Transmit_DMA(&huart2, uart_receive_buffer, 8); // 向 USART2 转发数据

  ssd1306_SetCursor(19, 24);
  ssd1306_WriteString((char *)uart_receive_buffer, Font_11x18, White);

  uint8_t commands[3] = {0x22, 0x03, 0x07};
  HAL_I2C_Mem_Write(&hi2c2, SSD1306_I2C_ADDR, 0x00, 1, commands, 3,
                    15); // 不能连续用 DMA，否则下一个会报 Busy，懒得写队列了
  HAL_I2C_Mem_Write_DMA(&hi2c2, SSD1306_I2C_ADDR, 0x40, 1,
                        &SSD1306_Buffer[SSD1306_WIDTH * 3], SSD1306_WIDTH * 2);
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
    if (data_length < 8) {
      return;
    }

    HAL_UART_DMAStop(huart);

    uart_receive_buffer[data_length] = 0;
    USART1_ReceiveProcessor(data_length);

    memset(uart_receive_buffer, 0, UART_RECEIVE_BUFFER_SIZE);

    HAL_UART_Receive_DMA(huart, uart_receive_buffer, UART_RECEIVE_BUFFER_SIZE);
  }
}

#ifdef __cplusplus
}
#endif