# STM32 Study: WLUART_Receiver

这是一个简单的 STM32 学习项目，用于 UART 相关代码练习。

## 硬件要求

- ZHKU 奇点战队 STM32F446RCT6 开发板
- SSD1306 驱动的 0.96 寸 I2C OLED 屏幕
- 不知道具体参数的 2.4G 无线串口透传模块
- CH341A USB - TTL 模块

## 功能说明

接收 UART1 发来的时间信息并通过屏幕显示，再转发给 UART2。