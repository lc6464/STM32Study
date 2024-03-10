#ifndef __U8G2_INIT_H
#define __U8G2_INIT_H

#include "main.h"
#include "u8g2.h"
#include "i2c.h"
uint8_t u8g2_gpio_and_delay_stm32_hal(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_stm32_hal_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void testDrawProcess(u8g2_t *u8g2);//进度条显示
void testShowFont(u8g2_t *u8g2);//字体测试 数字英文可选用 u8g2_font_ncenB..(粗) 系列字体
void testDrawFrame(u8g2_t *u8g2);//画空心矩形
void testDrawRBox(u8g2_t *u8g2);//画实心圆角矩形
void testDrawCircle(u8g2_t *u8g2);//画空心圆
void testDrawFilledEllipse(u8g2_t *u8g2);//画实心椭圆
void testDrawMulti(u8g2_t *u8g2);//环形测试
void U8G2_Init(u8g2_t *u8g2);//u8g2库初始化

#endif
