/**
  ******************************************************************************
  * @copyright (c) 2023 - ~, Singularity
  * @file   : i2c_if.h
  * @author : SY7_yellow
  * @brief  : i2c接口文件
  * @date   : 2023-8-17
  * @par Change Log：
  * <table>
  * <tr><th>Date           <th>Version     <th>Author      <th>Description
  * <tr><td>2023-08-17     <td>0.1         <td>SY7_yellow  <td>实现了基础功能
  * </table>
  *@details :
  * ============================================================================
  *                       How to use this driver  
  * ============================================================================
  * 1. 在设备的句柄中加入这条语句:"I2C_Handle_t I2C_Handle;"即可使用本文件
  * 2. 使用前调用SEML_I2C_Register对I2C句柄进行注册
  * 3. 发送函数:SEML_I2C_Transmit 接收函数:SEML_I2C_Receive
  ******************************************************************************
  * @attention:
  * 
  * 文件编码：UTF-8,出现乱码请勿上传! \n
  * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
  * 勿合并到master. \n
  * 
  * 防御性编程,对输入参数做有效性检查,并返回错误号. \n
  * 不要包含太多不相关的头文件. \n
  * 若发现bug请提交issue,详细描述现象(必须)和复现条件(选填),以便对应的负责人能
  * 够准确定位修复. \n
  * 若是存在多线程同时编辑的情况，请使用互斥锁防止某进程编辑时候被其他进程访问。
  * File encoding:UTF-8,Do not upload garbled code!\n
  * Please remember to change the version number. If you have not been tested, 
  * please use the suffix alpha,beta... And do not merge to master. \n
  * Defensive programming, where input arguments are checked for validity and
  * an error number is returned. \n
  * Don't include too many irrelevant headers. \n
  * If you find a bug, file an issue with a detailed description of the 
  * phenomenon (required) and conditions for reoccurrence (optional) so that 
  * the appropriate owner can locate the correct fix. \n
  * In the case of simultaneous editing by multiple threads, use a mutex to 
  * prevent one process from being accessed by other processes while it is 
  * editing. \n
  ******************************************************************************
  */
#ifndef _I2C_IF_H_
#define _I2C_IF_H_
#include "SEML_common.h"
#include "gpio_if.h"

#ifndef I2C_MEMADD_SIZE_8BIT
#define I2C_MEMADD_SIZE_8BIT            0x00000001U
#define I2C_MEMADD_SIZE_16BIT           0x00000010U
#endif

typedef uint16_t (*I2C_Transmit_t)(void *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
typedef uint16_t (*I2C_Receive_t)(void *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
typedef uint16_t (*I2C_Mem_Read_t)(void *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
typedef uint16_t (*I2C_Mem_Write_t)(void *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

typedef struct 
{
    void *hi2c;                     /**< i2c接口句柄 */
    I2C_Transmit_t I2C_Transmit;    /**< i2c发送函数 */
    I2C_Receive_t I2C_Receive;      /**< i2c接收函数 */
    I2C_Mem_Read_t I2C_Mem_Read;    /**< i2c读寄存器函数 */
    I2C_Mem_Write_t I2C_Mem_Write;  /**< i2c写寄存器函数 */
} I2C_Handle_t;

typedef struct
{
  GPIO_Handle_t SDA;
  GPIO_Handle_t SCL;
  SEML_LockType_t Lock;
} Software_I2C_HandleTypeDef;

/**
 * @brief 硬件I2C注册函数
 * @param config 配置结构体指针
 * @param hi2c_ i2c接口句柄
 */
#ifdef USE_HAL_DRIVER
#define Hardware_I2C_Register(I2C_Handle,hi2c_) SEML_I2C_Register(I2C_Handle,hi2c_,\
                                        HAL_I2C_Master_Transmit,HAL_I2C_Master_Receive,\
                                        HAL_I2C_Mem_Write,HAL_I2C_Mem_Read)
#endif

/**
 * @brief 软件I2C注册函数
 * @param config 配置结构体指针
 * @param hi2c_ i2c接口句柄
 */
#define Software_I2C_Register(I2C_Handle,hi2c_) SEML_I2C_Register(I2C_Handle,hi2c_,\
                                        Software_I2C_Transmit,Software_I2C_Receive,\
                                        Software_I2C_Mem_Write,Software_I2C_Mem_Read)

/**
 * @brief i2c注册函数
 * @param I2C_Handle i2c句柄
 * @param hi2c i2c接口句柄
 * @param I2C_Transmit i2c发送函数
 * @param I2C_Receive i2c接收函数
 * @param I2C_Mem_Write i2c写寄存器函数
 * @param I2C_Mem_Read i2c读寄存器函数
 */
void SEML_I2C_Register(I2C_Handle_t *I2C_Handle, void *hi2c,
                        I2C_Transmit_t I2C_Transmit, I2C_Receive_t I2C_Receive,
                        I2C_Mem_Write_t I2C_Mem_Write, I2C_Mem_Read_t I2C_Mem_Read);

/**
 * @brief i2c发送函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_I2C_Transmit(I2C_Handle_t *I2C_Handle, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief i2c接收函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_I2C_Receive(I2C_Handle_t *I2C_Handle, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief i2c读寄存器函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] MemAddress 内存地址
 * @param[in] MemAddSize 内存地址大小
 *  - I2C_MEMADD_SIZE_8BIT 内存地址为1字节
 *  - I2C_MEMADD_SIZE_16BIT 内存地址为2字节
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_I2C_Mem_Read(I2C_Handle_t *I2C_Handle, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief i2c写寄存器函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] MemAddress 内存地址
 * @param[in] MemAddSize 内存地址大小
 *  - I2C_MEMADD_SIZE_8BIT 内存地址为1字节
 *  - I2C_MEMADD_SIZE_16BIT 内存地址为2字节
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_I2C_Mem_Write(I2C_Handle_t *I2C_Handle, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief 软件i2c初始化
 * @param[out] hi2c 软件i2c句柄
 * @param[in] GPIO_SDA_Part SDA的GPIO端口
 * @param[in] GPIO_SDA_Pin SDA的GPIO引脚
 * @param[in] GPIO_SCL_Part SCL的GPIO端口
 * @param[in] GPIO_SCL_Pin SCL的GPIO引脚
 */
void Software_I2C_Init(Software_I2C_HandleTypeDef *hi2c, GPIO_TypeDef* GPIO_SDA_Part, uint16_t GPIO_SDA_Pin, GPIO_TypeDef* GPIO_SCL_Part, uint16_t GPIO_SCL_Pin);

/**
 * @brief 软件i2c接收函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_TIMEOUT 总线应答超时
 * @retval SEML_OK 执行完成
 */
SEML_StatusTypeDef Software_I2C_Receive(Software_I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief 软件i2c发送函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_TIMEOUT 总线应答超时
 * @retval SEML_OK 执行完成
 */
SEML_StatusTypeDef Software_I2C_Transmit(Software_I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief 软件i2c读寄存器函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] MemAddress 内存地址
 * @param[in] MemAddSize 内存地址大小
 *  - I2C_MEMADD_SIZE_8BIT 内存地址为1字节
 *  - I2C_MEMADD_SIZE_16BIT 内存地址为2字节
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_TIMEOUT 总线应答超时
 * @retval SEML_OK 执行完成
 */
SEML_StatusTypeDef Software_I2C_Mem_Read(Software_I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief 软件i2c写寄存器函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] MemAddress 内存地址
 * @param[in] MemAddSize 内存地址大小
 *  - I2C_MEMADD_SIZE_8BIT 内存地址为1字节
 *  - I2C_MEMADD_SIZE_16BIT 内存地址为2字节
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_TIMEOUT 总线应答超时
 * @retval SEML_OK 执行完成
 */
SEML_StatusTypeDef Software_I2C_Mem_Write(Software_I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);




#endif
