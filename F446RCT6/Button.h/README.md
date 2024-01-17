# STM32 Study: Button.h

这是一个简单的 STM32 学习项目，用于演示通过 Button.h 读取按钮控制 LED。

## 硬件要求

- ZHKU 奇点战队 STM32F446RCT6 开发板

## 功能说明
- Press SW1: `SW_State ^= 0b01;`
- Press SW2: `SW_State ^= 0b10;`

| SW_State | R | G | B |
| :------: |:-:|:-:|:-:|
|   0b00   | 0 | 0 | 0 |
|   0b01   | 0 | 1 | 0 |
|   0b10   | 1 | 0 | 0 |
|   0b11   | 0 | 0 | 1 |