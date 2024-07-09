#include "motor.h"

/**
 * @brief  通过 CAN 总线发送电机控制数据
 * @param  id_range 设置 CAN ID 范围：0 对应 1~4 电机，1 对应 5~7 电机
 * @param  v1 电机 1 或 5 的电压
 * @param  v2 电机 2 或 6 的电压
 * @param  v3 电机 3 或 7 的电压
 * @param  v4 电机 4 的电压
 * @retval None
 */
void set_motor_voltage(uint8_t id_range, int16_t v1, int16_t v2, int16_t v3, int16_t v4) {
	CAN_TxHeaderTypeDef tx_header;
	uint8_t tx_data[8];

	tx_header.StdId = id_range == 0 ? 0x1ff : 0x2ff;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = 8;

	tx_data[0] = v1 >> 8;
	tx_data[1] = v1;
	tx_data[2] = v2 >> 8;
	tx_data[3] = v2;
	tx_data[4] = v3 >> 8;
	tx_data[5] = v3;
	tx_data[6] = v4 >> 8;
	tx_data[7] = v4;

	HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, (uint32_t *)CAN_TX_MAILBOX0);
}


/**
 * @brief  解析电机反馈数据
 * @param  rx_header 接收到的 CAN 消息头
 * @param  rx_data 接收到的 CAN 数据
 * @param  motor_info 电机信息结构体
 * @retval 解析的电机 ID，-1 为不是电机反馈数据
 */
int8_t parse_motor_feedback(CAN_RxHeaderTypeDef rx_header, uint8_t *rx_data, motor_info_t *motor_info) {
	// 判断是否为电机反馈数据
	if (rx_header.StdId >= FEEDBACK_ID_BASE && rx_header.StdId < FEEDBACK_ID_BASE + MOTOR_MAX_NUM) {
		uint8_t index = rx_header.StdId - FEEDBACK_ID_BASE; // 获取电机索引
		motor_info[index].rotor_angle = ((rx_data[0] << 8) | rx_data[1]);
		motor_info[index].rotor_speed = ((rx_data[2] << 8) | rx_data[3]);
		motor_info[index].current = ((rx_data[4] << 8) | rx_data[5]);
		motor_info[index].temperature = rx_data[6];

		return index;
	}
	return -1;
}


/**
 * @brief  自定义 CAN 1 配置函数
 * @retval None
 */
void LC_CAN1_Config(void) {
	CAN_FilterTypeDef can_filter; // 定义一个 CAN 过滤器结构体变量

	can_filter.FilterBank = 0;                     // 设置过滤器编号为 0
	can_filter.FilterMode = CAN_FILTERMODE_IDMASK; // 设置过滤器模式为标识符掩码模式
	can_filter.FilterScale = CAN_FILTERSCALE_32BIT; // 设置过滤器尺度为 32 位

	// 接收所有 CAN 标识符
	can_filter.FilterIdHigh = 0; // 设置过滤器高 16 位标识符为 0
	can_filter.FilterIdLow = 0;  // 设置过滤器低 16 位标识符为 0
	can_filter.FilterMaskIdHigh = 0; // 设置过滤器高 16 位屏蔽标识符为 0
	can_filter.FilterMaskIdLow = 0; // 设置过滤器低 16 位屏蔽标识符为 0

	can_filter.FilterFIFOAssignment = CAN_RX_FIFO0; // 将过滤器关联到 FIFO0
	can_filter.FilterActivation = ENABLE;           // 启用该过滤器
	can_filter.SlaveStartFilterBank = 14; // 在双 CAN 模式下有意义，设置从过滤器起始编号为 14

	HAL_CAN_ConfigFilter(&hcan1, &can_filter); // 配置 CAN1 的过滤器
	HAL_CAN_Start(&hcan1);                     // 启动 CAN1
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); // 使能 CAN1 接收 FIFO0 消息挂起中断
}