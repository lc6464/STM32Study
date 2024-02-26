/**
  ******************************************************************************
  * @copyright (c) 2023 - ~, Singularity
  * @file   : spi_if.h
  * @author : SY7_yellow
  * @brief  : spi接口文件
  * @date   : 2023-8-18
  * @par Change Log：
  * <table>
  * <tr><th>Date           <th>Version     <th>Author      <th>Description
  * <tr><td>2023-08-18     <td>0.1-alpha   <td>SY7_yellow  <td>实现了基础功能
  * </table>
  *@details :
  * ============================================================================
  *                       How to use this driver
  * ============================================================================
  * 1. 在设备的句柄中加入这条语句:"SPI_Handle_t SPI_Handle;"即可使用本文件
  * 2. 使用前调用SEML_SPI_Register对SPI句柄进行注册
  * 3. 发送函数:SEML_SPI_Transmit 接收函数:SEML_SPI_Receive
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
#ifndef _SPI_IF_H_
#define _SPI_IF_H_
#include "SEML_common.h"


typedef uint16_t (*SPI_Transmit_t)(void *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
typedef uint16_t (*SPI_Receive_t)(void *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
typedef uint16_t (*SPI_TransmitReceive_t)(void *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size,uint32_t Timeout);

typedef struct
{
    void *hspi;                                     /**< spi接口句柄 */
    SPI_Transmit_t SPI_Transmit;                    /**< spi发送函数 */
    SPI_Receive_t SPI_Receive;                      /**< spi接收函数 */
    SPI_TransmitReceive_t SPI_TransmitReceive;      /**< spi发送接收函数 */
} SPI_Handle_t;

/**
 * @brief 硬件SPI注册函数
 * @param config 配置结构体指针
 * @param hi2c_ SPI接口句柄
 */
#ifdef USE_HAL_DRIVER
#define Hardware_SPI_Register(SPI_Handle,hspi_) SEML_SPI_Register(SPI_Handle,hspi_,\
                                        HAL_SPI_Transmit,HAL_SPI_Receive,\
                                        HAL_SPI_TransmitReceive)
#endif

/**
 * @brief SPI注册函数
 * @param[out] SPI_Handle SPI句柄
 * @param[in] hspi SPI接口句柄
 * @param[in] SPI_Transmit SPI发送函数
 * @param[in] SPI_Receive SPI接收函数
 * @param[in] SPI_TransmitReceive SPI发送接收函数
 */
void SEML_SPI_Register(SPI_Handle_t *SPI_Handle, void *hspi,
                        SPI_Transmit_t SPI_Transmit, SPI_Receive_t SPI_Receive, SPI_TransmitReceive_t SPI_TransmitReceive);

/**
 * @brief SPI接收函数
 * @param[in] SPI_Handle SPI句柄
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_SPI_Receive(SPI_Handle_t *SPI_Handle, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief SPI发送函数
 * @param[in] SPI_Handle SPI句柄
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_SPI_Transmit(SPI_Handle_t *SPI_Handle, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief SPI发送接收函数
 * @param[in] SPI_Handle SPI句柄
 * @param[in] pTxData 发送数据指针
 * @param[out] pRxData 接收数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_SPI_TransmitReceive(SPI_Handle_t *SPI_Handle, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size,uint32_t Timeout);

#endif
