# STM32 Study: UART_Homework

这是一个简单的 STM32 学习项目，用于完成 UART 相关作业要求。

## 硬件要求

- ZHKU 奇点战队 STM32F446RCT6 开发板

## 功能说明
- USART1 接收到数据翻转绿灯电平
- USART1 接收到不区分大小写的 `hello world` 后翻转红灯电平并输出 `Hello!\n`
- USART1 接收到简单的四则预算后转换为 `int32_t` ，再转换为 `int64_t` (+-*) 或 `float` (/) 运算，并输出算式及结果
- USART1 接收到其他数据输出 `ERROR!\n`