
/**
****************************(C) COPYRIGHT 2019 DJI****************************
* @file       IST8310driver.c/h
* @brief      ist8310 is a 3-axis digital magnetometer, the file includes initialization function,
*             read magnetic field strength data function.
*             IST8310是一款三轴数字磁力计，本文件包括初始化函数，读取磁场数据函数。
* @note       IST8310 only support I2C. IST8310只支持I2C。
* @history
*  Version    Date            Author          Modification
*  V1.0.0     Dec-26-2018     RM              1. done
*  V1.1.0     Aug-17-2023     SY7_yellow      添加了对i2c_if的支持
@verbatim
==============================================================================

==============================================================================
@endverbatim
****************************(C) COPYRIGHT 2019 DJI****************************
*/

#include "ist8310driver.h"
#include "ist8310driver_middleware.h"

#define MAG_SEN 0.3f // raw int16 data change to uT unit. 原始整型数据变成 单位ut

#define IST8310_WHO_AM_I 0x00				// ist8310 "who am I "
#define IST8310_WHO_AM_I_VALUE 0x10 // device ID

#define IST8310_WRITE_REG_NUM 4

// the first column:the registers of IST8310. 第一列:IST8310的寄存器
// the second column: the value to be writed to the registers.第二列:需要写入的寄存器值
// the third column: return error value.第三列:返回的错误码
static const uint8_t ist8310_write_reg_data_error[IST8310_WRITE_REG_NUM][3] = {
		{0x0B, 0x08, 0x01},	 // enalbe interrupt  and low pin polarity.开启中断，并且设置低电平
		{0x41, 0x12, 0x02},	 // average 4 times.平均采样四次
		{0x42, 0xC0, 0x03},	 // must be 0xC0. 必须是0xC0
		{0x0A, 0x0B, 0x04}}; // 200Hz output rate.200Hz输出频率

/**
 * @brief          初始化IST8310
 * @param[in]      ist8310_handle ist8310句柄
 * @retval         error value
 */
uint8_t ist8310_init(ist8310_Handle_t *ist8310_handle)
{
	static const uint8_t sleepTime = 50;
	uint8_t res = 0;
	uint8_t writeNum = 0;
	uint8_t temp;

#ifdef USE_SEML_LIB
	SEML_GPIO_Pin_Reset(&ist8310_handle->RSTN_Pin);
	SEML_Delay(sleepTime);
	SEML_GPIO_Pin_Set(&ist8310_handle->RSTN_Pin);
	SEML_Delay(sleepTime);
	SEML_I2C_Mem_Read(&ist8310_handle->I2C_Handle, IST8310_IIC_ADDRESS << 1, IST8310_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, &res, 1, 10);
#else
	ist8310_RST_L();
	ist8310_delay_ms(sleepTime);
	ist8310_RST_H();
	ist8310_delay_ms(sleepTime);
	res = ist8310_IIC_read_single_reg(IST8310_WHO_AM_I);
#endif
	if (res != IST8310_WHO_AM_I_VALUE)
	{
		return IST8310_NO_SENSOR;
	}

	// set mpu6500 sonsor config and check
	for (writeNum = 0; writeNum < IST8310_WRITE_REG_NUM; writeNum++)
	{
#ifdef USE_SEML_LIB
		temp = ist8310_write_reg_data_error[writeNum][1];
		SEML_I2C_Mem_Write(&ist8310_handle->I2C_Handle, IST8310_IIC_ADDRESS << 1,
											 ist8310_write_reg_data_error[writeNum][0], I2C_MEMADD_SIZE_8BIT, &temp, 1, 10);
#else
		ist8310_IIC_write_single_reg(ist8310_write_reg_data_error[writeNum][0], ist8310_write_reg_data_error[writeNum][1]);
#endif
#ifdef USE_SEML_LIB
		SEML_I2C_Mem_Read(&ist8310_handle->I2C_Handle, IST8310_IIC_ADDRESS << 1,
											ist8310_write_reg_data_error[writeNum][0], I2C_MEMADD_SIZE_8BIT, &res, 1, 10);
#else
		res = ist8310_IIC_read_single_reg(ist8310_write_reg_data_error[writeNum][0]);
#endif
		if (res != ist8310_write_reg_data_error[writeNum][1])
		{
			return ist8310_write_reg_data_error[writeNum][2];
		}
	}
	return IST8310_NO_ERROR;
}

/**
 * @brief 如果已经通过I2C的DMA方式读取到了从STAT1到DATAZL的数据，可以使用这个函数进行处理
 * @param[in] status_buf 数据指针,从STAT1(0x02) 寄存器到 DATAZL(0x08)寄存器
 * @param[out] ist8310_handle ist8310的句柄
 * @retval none
 */
void ist8310_read_over(ist8310_Handle_t *ist8310_handle,uint8_t *status_buf, Mag_Data_t *mag)
{

	if (status_buf[0] & 0x01)
	{
		int16_t temp_ist8310_data = 0;
		ist8310_handle->status |= 1 << IST8310_DATA_READY_BIT;

		temp_ist8310_data = (int16_t)((status_buf[2] << 8) | status_buf[1]);
		mag->x = MAG_SEN * temp_ist8310_data;
		temp_ist8310_data = (int16_t)((status_buf[4] << 8) | status_buf[3]);
		mag->y = MAG_SEN * temp_ist8310_data;
		temp_ist8310_data = (int16_t)((status_buf[6] << 8) | status_buf[5]);
		mag->z = MAG_SEN * temp_ist8310_data;
	}
	else
	{
		ist8310_handle->status &= ~(1 << IST8310_DATA_READY_BIT);
	}
}

/**
 * @brief 通过读取磁场数据
 * @param[in] ist8310_handle ist8310的句柄
 * @param[out] mag 磁场数据
 * @retval none
 */
void ist8310_read_mag(ist8310_Handle_t *ist8310_handle, Mag_Data_t *mag)
{
	uint8_t buf[6];
	int16_t temp_ist8310_data = 0;
	// read the "DATAXL" register (0x03)
#ifdef USE_SEML_LIB
	SEML_I2C_Mem_Read(&ist8310_handle->I2C_Handle, IST8310_IIC_ADDRESS << 1, 0x03, I2C_MEMADD_SIZE_8BIT, buf, 6, 10);
#else
	ist8310_IIC_read_muli_reg(0x03, buf, 6);
#endif
	temp_ist8310_data = (int16_t)((buf[1] << 8) | buf[0]);
	mag->x = MAG_SEN * temp_ist8310_data;
	temp_ist8310_data = (int16_t)((buf[3] << 8) | buf[2]);
	mag->y = MAG_SEN * temp_ist8310_data;
	temp_ist8310_data = (int16_t)((buf[5] << 8) | buf[4]);
	mag->z = MAG_SEN * temp_ist8310_data;
}
