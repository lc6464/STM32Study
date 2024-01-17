/**
	 \mainpage SEML 奇点机器人实验室嵌入式软件中间件层库
	 *
	 * ** 介绍 **
	 * ------------
	 * 本用户手册包含了SEML几乎所有的函数说明，可以方便查阅找到对应的函数。
	 * SEML主要结构如下：
	 * - 驱动层(Drivers)：提供硬件和软件之间接口
	 *   - 板级支持层(Board Support Pack，BSP)：提供硬件驱动及接口，通常BSP层都是由厂家或者网上开源。
	 *   - 硬件抽象层(Hardware Abstract Layer，HAL)：对BSP层进一步封装，形成统一的接口形式。
	 * - 中间件(Middlewares)：提供基础的软件层面支持
	 *   - 数学支持库(Math Library)：提供数学运算。
	 *   - 控制算法支持库(Controler Library)：提供各种各样的控制器。
	 *   - 数据结构支持库(Data Structure Library)：提供通用的数据结构类型。
	 *   - 系统库(System Library)：提供诸如内存管理、任务间通讯、外部通讯、调试等功能。
	 *   - 解算库(Solution Library)：提供解算算法，如陀螺仪解算、底盘运动模型解算、功率限制等。
	 *   - 通讯库(Communications Library)：提供电机通讯、裁判系统解算、多机通讯等。
	 * - 应用层(Application)
	 *   - 底盘功能库：实现基础的底盘功能，并且一些额外功能也在其中。
	 *   - 云台功能库：实现基础的云台功能，并且一些额外功能也在其中。
	 *   - 发射机构功能库：实现基础的发射机构功能，并且一些额外功能也在其中。
	 *   - 自瞄功能库：实现基础的自喵，自喵相关的函数也在其中。
	 *   - 机械臂功能库：工程的爪子。
	 * ----------
	 *
	 */
#ifndef _SEML_H_
// 硬件抽象层接口
#include "gpio_if.h" /**< GPIO抽象层接口 */
//#include "i2c_if.h"	 /**< I2C抽象层接口 */
//#include "spi_if.h"	 /**< SPI抽象层接口 */
//#include "can_if.h"	 /**< CAN抽象层接口 */
// 系统组件
#include "button.h"		 /**< 按键模块 */
// 数学运算
//#include "my_math.h"
// 控制器
//#include "PID.h" /**< PID控制器模块 */



#endif
