/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : queue.h
 * @author : SY7_yellow
 * @brief  : 队列模块
 * @date   : 2023-7-16
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-04-05     <td>1.0         <td>SY7_yellow  <td>实现最基础的队列
 * <tr><td>2023-07-16     <td>1.1-alpha   <td>SY7_yellow  <td>对其进行泛型化
 * <tr><td>2023-07-17     <td>1.1         <td>SY7_yellow  <td>使其能够代替栈的功能
 * <tr><td>2023-07-19     <td>1.2         <td>SY7_yellow  <td>使其支持静态数组
 * <tr><td>2023-8-7       <td>1.3-alpha   <td>SY7_yellow  <td>增加队列元素下标访问
 * <tr><td>2023-8-25      <td>1.4         <td>SY7_yellow  <td>使其线程安全
 * </table>
 * @details:
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 1. 此队列采用泛型编程,对任何数据都可以直接求其类型大小后使用.\n
 * 2. 由于采用泛型编程,对于诸如"[警告]用于算术的void *类型的指针"是正常现象.\n
 * 3. 初始化时候选择满队处理类型,会根据配置来执行对应操作：
 *    - 报错：返回满队错误码
 *    - 重新申请内存：自动扩大队列大小
 *    - 覆盖队头：丢弃队头数据进行覆盖
 * 4. 在不需要使用该队列时(包括函数内调用后)需要执行DeinitQueue来释放队列内存,
 *    防止内存溢出.\n
 * 5. 为了通用性,此队列支持后向出队(栈),可以直接替代栈的作用\n
 * 6. 队列运行出错通常会返回错误码(非0),但是若出现队列配置数据异常会直接进入
 *    Error_Handler().所以,只使用库里面提供的接口,不要直接对配置结构体进行修
 *    改.
 * 7. 对于单片机，由于其栈区很小，可分配容量很小，使用时候一般传入一个静态
 *    (全局)数组进去，并且满队处理不能使用重新申请内存。
 * 8. 该文件内函数线程安全,在多个线程同时访问的情况下失去竞争的会返回SEML_BUSY.
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
#ifndef _QUEUE_H
#define _QUEUE_H
#include "SEML_common.h"
/**
 * @addtogroup 数据结构库
 * @{
 */

/**
 * @addtogroup 队列
 * @{
 */

// @brief 重新扩展大小
#define QueueReapplySize 10

/**
 * @brief 满队处理方式
 */
typedef enum
{
	queue_full_hander_error = 0, /**< 报错*/
	queue_full_hander_reapply,	 /**< 重新申请内存*/
	queue_full_hander_cover,		 /**< 覆盖队头*/
} queue_full_hander_t;
// @brief 处理方式断言
#define IS_QUEUE_FULL_HANDER(x) ((x) == queue_full_hander_error || (x) == queue_full_hander_reapply || (x) == queue_full_hander_cover)

/**
 * @brief 满队处理方式
 */
typedef enum
{
	queue_read = 0, /**< 读取队列元素 */
	queue_write,		/**< 写入队列元素 */
} queue_read_while_t;
// @brief 处理方式断言
#define IS_QUEUE_READ_WHILE(x) ((x) == queue_read || (x) == queue_write)

/**
 * @brief 队列错误码
 */
typedef enum
{
	queue_no_error = 0,	 /**< 队列无错误						*/
	queue_full_error,		 /**< 队列满								*/
	queue_empty_error,	 /**< 队列空								*/
	queue_reapply_error, /**< 队列内存再申请出错		*/
	queue_config_error,	 /**< 队列配置出错					*/
	queue_visit_error		 /**< 访问队列元素出错			*/
} queue_error_code_t;

/**
 * @brief 队列结构体
 */
typedef struct
{
	void *address;												 /**< 队列初始地址			*/
	uint16_t front;												 /**< 队列头指针				*/
	uint16_t rear;												 /**< 队列尾指针				*/
	uint16_t size;												 /**< 队列大小					*/
	uint16_t elem_size;										 /**< 队列元素大小			*/
	queue_full_hander_t full_hander : 2;	 /**< 满队处理方式			*/
	uint8_t error_code : 4;								 /**< 队列错误码				*/
	FunctionalState use_extern_buffer : 1; /**< 使用外部缓存数组	*/
	SEML_LockType_t Lock : 1;							 /**< 互斥锁						*/
} s_queue;

/**
 * @brief 初始化队列
 * @param[in,out] queue 队列结构体指针
 * @param[in] elem_size 元素大小，通常是sizeof()
 * @param[in] size 队列大小
 * @param[in] buffer 缓存数组，使用栈区自动分配可传NULL,传入大小需比申请尺寸大1
 * @param[in] full_hander 满队处理方式
 * @attention 使用外部缓存数组时候满队处理方式不能使用queue_full_hander_reapply
 * @return 队列执行状态
 */
SEML_StatusTypeDef InitQueue(s_queue *queue, const uint16_t elem_size, const uint16_t size, void *buffer, const queue_full_hander_t full_hander);

/**
 * @brief 删除队列
 * @param[in,out] queue 队列结构体指针
 */
void DeleteQueue(s_queue *queue);

/**
 * @brief 队尾入队
 * @param[in,out] queue 队列结构体指针
 * @param[in] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef EnQueue(s_queue *queue, const void *data);

/**
 * @brief 队头出队
 * @param[in,out] queue 队列结构体指针
 * @param[out] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef DeQueue(s_queue *queue, void *data);

/**
 * @brief 队尾出队
 * @param[in,out] queue 队列结构体指针
 * @param[out] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef DeQueueRear(s_queue *queue, void *data);

/**
 * @brief 获取队头数据(不出队)
 * @param[in,out] queue 队列结构体指针
 * @param[out] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef GetQueueFront(s_queue *queue, void *data);

/**
 * @brief 获取队尾数据(不出队)
 * @param[in,out] queue 队列结构体指针
 * @param[out] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef GetQueueRear(s_queue *queue, void *data);

/**
 * @brief 获取队列长度
 * @param[in] queue 队列结构体指针
 * @return 队列长度
 */
uint16_t GetQueueLong(const s_queue *queue);

/**
 * @brief 访问队列内元素
 * @param[in,out] queue 队列结构体指针
 * @param[in] index 元素下标,小于队列长度
 * @param[in,out] data 数据指针
 * @param[in] status 读写状态
 * @return 队列执行状态
 */
SEML_StatusTypeDef VisitQueueElem(s_queue *queue, uint16_t index, void *data, queue_read_while_t status);

/**
 * @} 队列结束
 */

/**
 * @} 数据结构库结束
 */

#endif

/*******************End Of File*****************************/
