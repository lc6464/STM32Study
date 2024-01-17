/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : button.h
 * @author : SY7_yellow
 * @brief  : 按键事件组件
 * @date   : 2023-8-26
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-08-26     <td>1.0         <td>SY7_yellow  <td>实现了基础功能
 * </table>
 *@details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 1. 调用Button_Init传入按键句柄,gpio输入函数和其配置(需要对其重封装让其只有一
 * 个参数,使用SEML库可以调用"gpio_if.h")
 * 2. 调用Button_Register注册按键事件回调函数
 * 3. 将Button_Scan放入毫秒级定时器中断中
 * 4. 测试配置,若按键手感欠佳可以适当微调参数,使用SEML库请在"main.h"定义
 * USE_EXTERN_BUTTON_CONFIG后将文件内配置信息复制到库外进行配置.
 * 5. 若没有时间戳可不定义Button_GetTick,但是需要配置Button_Sample_Time
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
#ifndef _BUTTON_H_
#define _BUTTON_H_
#include "main.h"
#include "SEML_common.h"
#include "gpio_if.h"
#ifndef USE_EXTERN_BUTTON_CONFIG

/// @brief 按键采样滤波窗口大小
#define Button_Sampling_Window 6
/// @brief 按键长按阈值
#define Button_Long_Press_Threshold 500
/// @brief 按键点击超时时间
#define Button_Clink_Timeout 150

/**
 * @brief 按键获取事件戳函数
 * 若没有系统时间戳可不定义,但是得在Button_Sample_Time填入采样周期
 */
#define Button_GetTick() SEML_GetTick()
/**
 * @brief 按键采样频率
 * 若定义了按键获取事件戳函数不用管他,单位(ms)
 */
#define Button_Sample_Time 5

#else
// 配置错误检查
#ifndef Button_Sampling_Window
#error Using external configuration but undefined Button_Sampling_Window.(button.h)
#endif

#ifndef Button_Long_Press_Threshold
#error Using external configuration but undefined Button_Long_Press_Threshold.(button.h)
#endif

#ifndef Button_Clink_Timeout
#error Using external configuration but undefined Button_Clink_Timeout.(button.h)
#endif

#if !(defined Button_Sample_Time) && !(defined Button_GetTick)
#error Using external configuration but undefined Button_Sample_Time.(button.h)
#endif

#endif // USE_EXTERN_BUTTON_CONFIG
/**
 * @brief 按键按下事件
 */
typedef enum
{
	Press_None = 0,		 /**< 无按键事件 */
	Press_Up,					 /**< 按键松开 */
	Press_Down,				 /**< 按键按下 */
	Single_Clink,			 /**< 单击按键 */
	Double_Clink,			 /**< 双击按键 */
	Multiple_clicks,	 /**< 多次点击(>2次) */
	Long_Press_Start,	 /**< 按键长按开始 */
	Long_Press_Hold,	 /**< 按键长按 */
	Long_press_Release /**< 按键长按释放 */
} Button_Event_t;
/// @brief 是否为按键事件
#define IS_BUTTON_EVENT(a) (a == Press_None || a == Press_Up || a == Press_Down ||            \
														a == Single_Clink || a == Double_Clink || a == Multiple_clicks || \
														a == Long_Press_Start || a == Long_Press_Hold || a == Long_press_Release)
/// @brief 按键事件数量
#define Button_Event_Num 8
/// @brief 按键句柄结构体
typedef struct
{
	uint8_t event : 4;															/**< 按键事件 */
	uint8_t status : 3;															/**< 按键状态 */
	uint8_t long_press_flag : 1;										/**< 是否为长按 */
	uint8_t active_level : 1;												/**< 按键真电平 */
	uint8_t clicks_count : 7;												/**< 点击次数 */
	uint8_t press_count;														/**< 按键滤波计数 */
	uint32_t press_down_time;												/**< 按下时间 */
	uint32_t press_up_time;													/**< 松开时间 */
	#ifdef USE_SEML_LIB
	GPIO_Handle_t GPIO_Handle;
	#else
	uint8_t (*get_level_fun)(void *);								/**< 获取按键电平函数 */
	void *get_level_conf;														/**< 获取按键电平配置 */
	#endif
	void (*callback_fun[Button_Event_Num])(void *); /**< 按键回调函数 */
	void *next;																			/**< 链表指针 */
} Button_t;

/**
 * @brief 获取按键按下次数
 * @param button 按键句柄
 */
#define Get_Button_PressCount(button) ((button)->clicks_count)

/**
 * @brief 获取当前按键按下状态
 * @param button 按键句柄
 */
#define Get_Button_Press(button) (((button)->event == Press_Down || (button)->event == Long_Press_Hold) ? Press_Down : Press_Up)
#ifdef USE_SEML_LIB
/**
 * @brief 按键初始化
 * @param button 按键句柄
 * @param active_level 按键按下电平
 */
void Button_Init(Button_t *button, FunctionalState active_level);

#else
/**
 * @brief 按键初始化
 * @param button 按键句柄
 * @param get_level_fun 获取按键函数
 * @param get_level_config 获取按键配置
 * @param active_level 按键按下电平
 */
void Button_Init(Button_t *button, uint8_t (*get_level_fun)(void *), void *get_level_config, FunctionalState active_level);
#endif

/**
 * @brief 按键事件回调注册
 * @param button 按键句柄
 * @param event 按键事件
 * @param callback_fun 事件回调函数
 */
void Button_Register(Button_t *button, Button_Event_t event, void (*callback_fun)(Button_t *));

/**
 * @brief 按键状态更新函数
 * @param button 按键句柄
 */
void Button_Handle(Button_t *button);

/**
 * @brief 按键扫描函数
 * 对所有按键进行扫描
 */
void Button_Scan(void *config);

#endif
