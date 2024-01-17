/**
  ******************************************************************************
  * @copyright (c) 2023 - ~, Singularity
  * @file   : gpio_if.h
  * @author : SY7_yellow
  * @brief  : gpio接口文件
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
  * 使用SEML_GPIO_Pin_Register初始化gpio,随后即可愉快使用了
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
#ifndef _GPIO_IF_H_
#define _GPIO_IF_H_
#include "SEML_common.h"

/**
 * @brief GPIO句柄
 */
typedef struct
{
	GPIO_TypeDef *GPIO_Part; /**< GPIO端口 */
	uint16_t GPIO_Pin;			 /**< GPIO引脚 */
} GPIO_Handle_t;

/**
 * @brief 注册GPIO引脚
 * @param GPIO_PART GPIO端口
 * @param GPIO_PIN  GPIO引脚
 */
#define SEML_GPIO_Pin_Register(GPIO_Handle, GPIO_PART, GPIO_PIN) \
	do                                                           \
	{                                                            \
		assert_param(GPIO_Handle != NULL);                         \
		assert_param(GPIO_PART != NULL);                           \
		assert_param(IS_GPIO_PIN(GPIO_PIN));                       \
		(GPIO_Handle)->GPIO_Part = GPIO_PART;                      \
		(GPIO_Handle)->GPIO_Pin = GPIO_PIN;                        \
	} while (0)

/**
 * @brief GPIO引脚输出置1
 * @param GPIO_Handle GPIO句柄
 */
void SEML_GPIO_Pin_Set(GPIO_Handle_t *GPIO_Handle);
/**
 * @brief GPIO引脚输出置0
 * @param GPIO_Handle GPIO句柄
 */
void SEML_GPIO_Pin_Reset(GPIO_Handle_t *GPIO_Handle);
/**
 * @brief GPIO引脚输出
 * @param GPIO_Handle GPIO句柄
 * @param status 输出状态
 */
void SEML_GPIO_Pin_Output(GPIO_Handle_t *GPIO_Handle, uint16_t status);
/**
 * @brief GPIO引脚输出反转
 * @param GPIO_Handle GPIO句柄
 */
void SEML_GPIO_Pin_Toggle(GPIO_Handle_t *GPIO_Handle);
/**
 * @brief GPIO引脚输入
 * @param GPIO_Handle GPIO句柄
 */
uint16_t SEML_GPIO_Pin_Input(GPIO_Handle_t *GPIO_Handle);

#endif
