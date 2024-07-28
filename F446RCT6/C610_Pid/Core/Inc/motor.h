#pragma once

#include "can.h"

// 定义反馈信息的ID基础值
#define FEEDBACK_ID_BASE 0x201u
// 定义CAN控制ID的基础值
#define CAN_CONTROL_ID_BASE 0x200u
// 定义CAN控制ID的扩展值
#define CAN_CONTROL_ID_EXTEND 0x1ffu
// 定义电机的最大数量
#define MOTOR_MAX_NUM 7

// 电机信息结构体
typedef struct {
	// CAN ID
	uint16_t can_id;
	// 电机的转子角度
	uint16_t rotor_angle;
	// 电机的转子速度
	int16_t rotor_speed;
	// 电机的转矩电流
	int16_t current;
	// 电机的温度
	uint8_t temperature;
} motor_info_t;

/**
 * @brief  通过 CAN 总线发送电机控制数据
 * @param  id_range 设置 CAN ID 范围：0 对应 1~4 电机，1 对应 5~7 电机
 * @param  v1 电机 1 或 5 的电压
 * @param  v2 电机 2 或 6 的电压
 * @param  v3 电机 3 或 7 的电压
 * @param  v4 电机 4 的电压
 * @retval None
 */
void set_motor_current(uint8_t id_range, int16_t v1, int16_t v2, int16_t v3, int16_t v4);

/**
 * @brief  解析电机反馈数据
 * @param  rx_header 接收到的 CAN 消息头
 * @param  rx_data 接收到的 CAN 数据
 * @param  motor_info 电机信息结构体
 * @retval 解析的电机 ID，-1 为不是电机反馈数据
 */
int8_t parse_motor_feedback(CAN_RxHeaderTypeDef rx_header, uint8_t *rx_data, motor_info_t *motor_info);

/**
 * @brief  自定义 CAN 1 配置函数
 * @retval None
 */
void LC_CAN1_Config(void);