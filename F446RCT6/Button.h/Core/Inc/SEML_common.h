/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : SEML_common.h
 * @author : SY7_yellow
 * @brief  : SEML共用头文件
 * @date   : 2023-8-16
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-07-15     <td>0.1-alpha   <td>SY7_yellow
 *<td>添加了互斥锁,运行状态,参数断言
 * </table>
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
 ******************************************************************************
 */
#ifndef _SEML_COMMON_H_
#define _SEML_COMMON_H_
#include "main.h"
// 使用SEML库
#ifndef USE_SEML_LIB
#define USE_SEML_LIB
#endif
// 使用外部的SEML配置
#ifndef EXTERN_SEML_CONFIG
// SEML时间戳(默认ms)
#define SEML_GetTick() HAL_GetTick()
// SEML延时函数(默认ms)
#define SEML_Delay(ms) HAL_Delay(ms)
// 使用DEBUG模式
#ifndef DEBUG_MODE
#define DEBUG_MODE 1
#endif
// 使能软件看门狗
#define USE_SOFT_WITCHDOG
#endif
// #ifndef __GNUC__
// #define inline
// #endif

#define _FAST __attribute__((section("ccmram")))

/**
 * @brief SEML函数执行状态
 */
typedef enum {
  SEML_OK = 0x00U,     /**< 正常运行 */
  SEML_ERROR = 0x01U,  /**< 出现错误 */
  SEML_BUSY = 0x02U,   /**< 有其他进程正在编辑该句柄 */
  SEML_TIMEOUT = 0x03U /**< 超时 */
} SEML_StatusTypeDef;

/**
 * @brief 互斥锁状态
 */
typedef enum {
  SEML_UNLOCKED = 0, /**< 互斥锁解锁 */
  SEML_LOCKED        /**< 互斥锁锁定 */
} SEML_LockType_t;

/**
 * @brief 互斥锁是否锁定
 */
#define __SEML_IS_LOCK(__HANDLE__)                                             \
  do {                                                                         \
    if ((__HANDLE__)->Lock == SEML_LOCKED) {                                   \
      return HAL_BUSY;                                                         \
    }                                                                          \
  } while (0U)

/**
 * @brief 互斥锁锁定
 */
#define __SEML_LOCK(__HANDLE__) (__HANDLE__)->Lock = SEML_LOCKED
/**
 * @brief 互斥锁解锁
 */
#define __SEML_UNLOCK(__HANDLE__)                                              \
  do {                                                                         \
    (__HANDLE__)->Lock = SEML_UNLOCKED;                                        \
  } while (0U)
/**
 * @brief 临界区信号量
 */
extern uint8_t critical_section_count;
/**
 * @brief 进入临界区
 */
#define __SEML_ENTER_CRITICAL_SECTION()                                        \
  do {                                                                         \
    __disable_irq();                                                           \
    critical_section_count++;                                                  \
  } while (0U)
/**
 * @brief 退出临界区
 */
#define __SEML_LEAVE_CRITICAL_SECTION(__HANDLE__)                              \
  do {                                                                         \
    if (--critical_section_count == 0)                                         \
      __enable_irq();                                                          \
  } while (0U)

#ifndef assert_param
#ifdef USE_FULL_ASSERT
/**
 * @brief  assert_param宏用于检查函数的参数。
 * @param  expr
 * 如果expr为false，它调用assert_failed函数，该函数报告源文件的名称和失败调用的源行号。
 *         如果expr为true，则返回空值。
 * @retval None
 */
#define assert_param(expr)                                                     \
  ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#endif /* assert_param */

#endif
