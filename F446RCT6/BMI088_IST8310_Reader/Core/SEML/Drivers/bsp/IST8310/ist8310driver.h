
/**
 * @file       IST8310driver.c/h
 * @brief      ist8310 is a 3-axis digital magnetometer, the file includes
 * initialization function, read magnetic field strength data function.
 * @note       IST8310 only support I2C
 * @history
 *  Version    Date            Author          Modification
 *  V1.0.0     Dec-26-2018     RM              1. 完成
 *  V1.1.0     Aug-17-2023     SY7_yellow      添加了对i2c_if的支持
 */

#ifndef IST8310DRIVER_H
#define IST8310DRIVER_H
#include "../../../Middlewares/Solution/AHRS/AHRS.h"
#include "main.h"
#ifdef USE_SEML_LIB
#include "../../hal/Interface/gpio_if.h"
#include "../../hal/Interface/i2c_if.h"
#endif
#define IST8310_IIC_ADDRESS 0x0E // the I2C address of IST8310

#define IST8310_DATA_READY_BIT 2

#define IST8310_NO_ERROR 0x00
#define IST8310_NO_SENSOR 0x40

typedef struct {
  uint8_t status;
#ifdef USE_SEML_LIB
  I2C_Handle_t I2C_Handle;
  GPIO_Handle_t RSTN_Pin;
#endif
} ist8310_Handle_t;

/**
 * @brief          初始化IST8310
 * @param[in]      ist8310_handle ist8310句柄
 * @retval         error value
 */
uint8_t ist8310_init(ist8310_Handle_t *ist8310_handle);

/**
 * @brief
 * 如果已经通过I2C的DMA方式读取到了从STAT1到DATAZL的数据，可以使用这个函数进行处理
 * @param[in] status_buf 数据指针,从STAT1(0x02) 寄存器到 DATAZL(0x08)寄存器
 * @param[out] ist8310_handle ist8310的句柄
 * @param[out] mag 磁场数据
 * @retval none
 */
void ist8310_read_over(ist8310_Handle_t *ist8310_handle, uint8_t *status_buf,
                       Mag_Data_t *mag);

/**
 * @brief 通过读取磁场数据
 * @param[in] ist8310_handle ist8310的句柄
 * @param[out] mag 磁场数据
 * @retval none
 */
void ist8310_read_mag(ist8310_Handle_t *ist8310_handle, Mag_Data_t *mag);
#endif
