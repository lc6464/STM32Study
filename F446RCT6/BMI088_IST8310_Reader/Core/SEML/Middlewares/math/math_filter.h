/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : filter.h
 * @author : SY7_yellow
 * @brief  : 滤波器库
 * @date   : 2023-7-17
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-07-23     <td>0.1         <td>SY7_yellow  <td>添加了rms
 * <tr><td>2022-07-24     <td>0.2         <td>SY7_yellow  <td>添加了lowpass
 * </table>
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 *
 ******************************************************************************
 * @attention:
 *
 * 文件编码：UTF-8,出现乱码请勿上传! \n
 * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
 * 勿合并到master. \n
 * 防御性编程,对输入参数做有效性检查,并返回错误号. \n
 * 不要包含太多不相关的头文件. \n
 * 若发现bug请提交issue,详细描述现象(必须)和复现条件(选填),以便对应的负责人能
 * 够准确定位修复. \n
 * File encoding:UTF-8,Do not upload garbled code!\n
 * Please remember to change the version number. If you have not been tested,
 * please use the suffix alpha,beta... And do not merge to master. \n
 * Defensive programming, where input arguments are checked for validity and
 * an error number is returned. \n
 * Don't include too many irrelevant headers. \n
 * If you find a bug, file an issue with a detailed description of the
 * phenomenon (required) and conditions for reoccurrence (optional) so that
 * the appropriate owner can locate the correct fix. \n
 *
 ******************************************************************************
 */
#ifndef _FILTER_H_
#define _FILTER_H_
#include "../DataStructure/queue.h"
#include "SEML_common.h"
#include "math_common.h"
/**
 * @addtogroup 数学库
 * @{
 */
/**
 * @addtogroup 滤波器
 * @{
 */
/**
 * @brief 一阶低通滤波器
 * @param now 输入值
 * @param ave 上一次滤波的平均值
 * @param a 滤波系数
 * @return 滤波后结果
 */
#define Lowpass(now, ave, a) ((now) * (a) + (ave) * (1 - a))
/**
 * @addtogroup 均方根滤波器
 * @{
 */

/**
 * @brief 窗口均方根滤波器配置结构体
 */
typedef struct {
  uint16_t window_width; /**< 窗口大小 */
  float gain;
  uint16_t count;     /**< 计数值 */
  float RMS_value;    /**< rms值 */
  float square_sum;   /**< 平方和 */
  s_queue data_queue; /**< 数据队列*/
} RMS_filter_t;

/**
 * @brief 均方根滤波器初始化
 * @param[out] RMS_filter  均方根配置结构体指针
 * @param[in] window_width 滑动窗口大小
 * @param[in] init_value 初始值
 * @param[in,out] buffer 队列缓存数组
 */
void RMS_filter_Init(RMS_filter_t *RMS_filter, const uint16_t window_width,
                     const float init_value, float *buffer);

/**
 * @brief 均方根滤波器
 * @param[in,out] RMS_filter 滤波器指针
 * @param[in] data 输入数据
 * @return 滤波后结果
 */
_FAST float RMS_filter(RMS_filter_t *RMS_filter, const float data);

/**
 * @brief 删除均方根滤波器
 * @param[out] RMS_filter 均方根配置结构体指针
 */
void RMS_filter_DeInit(RMS_filter_t *RMS_filter);

///@} 均方根滤波器结束

/**
 * @addtogroup 均值滤波器
 * @{
 */

/**
 * @brief 窗口均值滤波器配置结构体
 */
typedef struct {
  uint16_t window_width; /**< 窗口大小 */
  float gain;
  uint16_t count;      /**< 计数值 */
  float average_value; /**< 均值 */
  s_queue data_queue;  /**< 数据队列*/
} average_filter_t;

/**
 * @brief 滑动均值滤波器初始化
 * @param[out] ave_filter  滑动均值配置结构体指针
 * @param[in] window_width 滑动窗口大小
 * @param[in] init_value 初始值
 * @param[in,out] buffer 队列缓存数组
 */
void average_filter_Init(average_filter_t *ave_filter,
                         const uint16_t window_width, const float init_value,
                         float *buffer);

/**
 * @brief 滑动均值滤波器
 * @param[in,out] ave_filter 滤波器指针
 * @param[in] data 输入数据
 * @return 滤波后结果
 */
float average_filter(average_filter_t *ave_filter, const float data);

/**
 * @brief 删除均值滤波器
 * @param[out] ave_filter 滤波器指针
 */
void average_filter_DeInit(average_filter_t *ave_filter);

///@} 均方根滤波器结束

///@} 滤波器结束
///@} 数学库结束
#endif
