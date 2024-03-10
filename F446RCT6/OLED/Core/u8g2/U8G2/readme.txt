以下是HAL库U8G2的移植说明（硬件IIC)
移植说明：
移植可以查看CSDN上的文章http://t.csdn.cn/Q3wVS
在U8G2_INIT.c上的函数u8x8_byte_i2c（....）上更改IIC的传输
引脚HAL_I2C_Master_Transmit(&hi2c1,u8x8_GetI2CAddress(u8x8), buffer, buf_idx,1000);

使用教程可以看右边链接的文章https://blog.csdn.net/qq_51096702/article/details/130257230

在主函数中#include “u8g2.h”文件
在main函数中初始化相应代码
	u8g2_t u8g2;
u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0,u8x8_byte_i2c,u8x8_gpio_and_delay_template);