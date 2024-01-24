#include "main.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

    char uart_idle_callback_buffer[12] = {0};

    void HDMA_USART1_TX_TransferComplete(DMA_HandleTypeDef *hdma)
    {
        __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_HT_FLAG_INDEX(hdma));

        hdma->State = HAL_DMA_STATE_READY;

        // 清除
        __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);
    }

    void USART1_IDLECallback(UART_HandleTypeDef *huart)
    {
        if (huart->Instance == USART1)
        {
            if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)
            {
                __HAL_UART_CLEAR_IDLEFLAG(huart);

                HAL_UART_DMAStop(huart);

                int bytes = sprintf(uart_idle_callback_buffer, "%ld\n", UART_RECEIVE_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx));
                HAL_UART_Transmit_DMA(huart, (uint8_t *)uart_idle_callback_buffer, bytes);
                memset(uart_receive_buffer, 0, UART_RECEIVE_BUFFER_SIZE);

                HAL_UART_Receive_DMA(huart, uart_receive_buffer, UART_RECEIVE_BUFFER_SIZE);
            }
        }
    }

#ifdef __cplusplus
}
#endif