#pragma once

#include <stdint.h>

typedef struct IST8310_RawData
{
    float X;
    float Y;
    float Z;
} IST8310_RawData;

typedef enum IST8310_Status_e
{
    IST8310_OK = 0x00,
    IST8310_I2C_Error = 0x01,
    IST8310_ID_Error = 0x02,
} IST8310_Status_e;

typedef struct IST8310_t
{
    IST8310_Status_e Status;
    IST8310_RawData Data;
} IST8310_t;

extern IST8310_t ist8310_data;

// LSB -> uT 系数
#define MAG_RESO 0.3f

// I2C 定义
#define IST8310_I2C_ADDR 0x0E
#define IST8310_I2C hi2c3

IST8310_Status_e IST8310_Init(IST8310_t *ist8310_data);

// 基础读取函数
#define IST8310_ReadSingleData(addr, data) HAL_I2C_Mem_Read(&IST8310_I2C, (IST8310_I2C_ADDR << 1), addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 10)
#define IST8310_WriteSingleData(addr, data) HAL_I2C_Mem_Write(&IST8310_I2C, (IST8310_I2C_ADDR << 1), addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 10)
#define IST8310_ReadMultiData(addr, data, length) HAL_I2C_Mem_Read(&IST8310_I2C, (IST8310_I2C_ADDR << 1), addr, I2C_MEMADD_SIZE_8BIT, data, length, 10)
#define IST8310_WriteMultiData(addr, data, length) HAL_I2C_Mem_Write(&IST8310_I2C, (IST8310_I2C_ADDR << 1), addr, I2C_MEMADD_SIZE_8BIT, data, length, 10)

// 功能函数
HAL_StatusTypeDef IST8310_ReadMegData(IST8310_t *ist8310_data);

// 寄存器地址
#define IST8310_WMI_ADDR 0x00
#define IST8310_WMI_VALUE 0x10

#define IST8310_STAT1_ADDR 0x02

#define IST8310_DATA_XL_ADDR 0x03
#define IST8310_DATA_XH_ADDR 0x04
#define IST8310_DATA_YL_ADDR 0x05
#define IST8310_DATA_YH_ADDR 0x06
#define IST8310_DATA_ZL_ADDR 0x07
#define IST8310_DATA_ZH_ADDR 0x08

#define IST8310_STAT2_ADDR 0x09

#define IST8310_CNTL1_ADDR 0x0A
#define IST8310_CNTL1_SLEEP 0x00
#define IST8310_CNTL1_SINGLE 0x01
#define IST8310_CNTL1_CONTINUE 0x03

#define IST8310_CNTL2_ADDR 0x0B
#define IST8310_CNTL2_DRDY_DISABLE 0x00
#define IST8310_CNTL2_DRDY_LOW 0x08
#define IST8310_CNTL2_DRDY_HIGH 0x0C
#define IST8310_CNTL2_DRDY_DISABLE_SOFT_RESET 0x01
#define IST8310_CNTL2_DRDY_LOW_SOFT_RESET 0x09
#define IST8310_CNTL2_DRDY_HIGH_SOFT_RESET 0x0D

#define IST8310_SELF_CHECK_ADDR 0x0C

#define IST8310_TEMP_L_ADDR 0x1C
#define IST8310_TEMP_H_ADDR 0x1D

#define IST8310_AVGCNTL_ADDR 0x41
#define IST8310_AVGCNTL_TWICE_TWICE 0x09
#define IST8310_AVGCNTL_FOURTH_FOURTH 0x12
#define IST8310_AVGCNTL_EIGHTH_EIGHTH 0x1B
#define IST8310_AVGCNTL_SIXTEENTH_SIXTEENTH 0x24

#define IST8310_PDCNTL_ADDR 0x42
#define IST8310_PDCNTL_PERFORMANCE 0xC0