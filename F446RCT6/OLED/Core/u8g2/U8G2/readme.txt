������HAL��U8G2����ֲ˵����Ӳ��IIC)
��ֲ˵����
��ֲ���Բ鿴CSDN�ϵ�����http://t.csdn.cn/Q3wVS
��U8G2_INIT.c�ϵĺ���u8x8_byte_i2c��....���ϸ���IIC�Ĵ���
����HAL_I2C_Master_Transmit(&hi2c1,u8x8_GetI2CAddress(u8x8), buffer, buf_idx,1000);

ʹ�ý̳̿��Կ��ұ����ӵ�����https://blog.csdn.net/qq_51096702/article/details/130257230

����������#include ��u8g2.h���ļ�
��main�����г�ʼ����Ӧ����
	u8g2_t u8g2;
u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0,u8x8_byte_i2c,u8x8_gpio_and_delay_template);