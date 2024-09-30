#include "Motor.hpp"
#include <algorithm>

void Motor::Start() {
	// 配置 CAN 过滤器
	CAN_FilterTypeDef filterConfig{};

	filterConfig.FilterBank = 0;
	filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	filterConfig.FilterIdHigh = 0;
	filterConfig.FilterIdLow = 0;
	filterConfig.FilterMaskIdHigh = 0;
	filterConfig.FilterMaskIdLow = 0;
	filterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	filterConfig.FilterActivation = ENABLE;
	filterConfig.SlaveStartFilterBank = 14;

	Start(filterConfig);
}

bool Motor::HandleRxData(const CAN_RxHeaderTypeDef &rxHeader, const uint8_t *rxData) {
	// 检查是否为本电机的反馈数据
	if (rxHeader.StdId == static_cast<uint32_t>(0x200 + _motorId)) {
		ParseFeedback(rxData);
		return true;
	}
	return false;
}

HAL_StatusTypeDef Motor::SendControlCommand(Motor *motor1, Motor *motor2, Motor *motor3, Motor *motor4) {
	// 确保至少有一个电机需要控制
	if (!motor1 && !motor2 && !motor3 && !motor4) {
		return HAL_ERROR;
	}

	CAN_TxHeaderTypeDef txHeader{};
	uint8_t txData[8] = { 0 };

	// 设置 CAN 发送帧头
	txHeader.StdId = (motor1->_motorId <= 4) ? CAN_CONTROL_ID_BASE : CAN_CONTROL_ID_EXTEND;
	txHeader.ExtId = 0;
	txHeader.IDE = CAN_ID_STD;
	txHeader.RTR = CAN_RTR_DATA;
	txHeader.DLC = 8;
	txHeader.TransmitGlobalTime = DISABLE;

	// 填充发送数据
	auto fillMotorData = [&txData](Motor *motor, int index) {
		if (motor) {
			txData[index * 2] = (motor->_currentCurrent >> 8) & 0xFF;
			txData[index * 2 + 1] = motor->_currentCurrent & 0xFF;
		}
		};

	fillMotorData(motor1, 0);
	fillMotorData(motor2, 1);
	fillMotorData(motor3, 2);
	fillMotorData(motor4, 3);

	// 发送CAN消息
	uint32_t mailbox;
	return HAL_CAN_AddTxMessage(&motor1->_hcan, &txHeader, txData, &mailbox);
}

void Motor::ParseFeedback(const uint8_t *rxData) {
	// 解析反馈数据
	Feedback feedback{};
	feedback.angle = (static_cast<uint16_t>(rxData[0]) << 8) | rxData[1];
	feedback.speed = static_cast<int16_t>((static_cast<uint16_t>(rxData[2]) << 8) | rxData[3]);
	feedback.torque = static_cast<int16_t>((static_cast<uint16_t>(rxData[4]) << 8) | rxData[5]);

	// 更新当前速度
	_currentSpeed = feedback.speed;

	// 调用回调函数（如果已设置）
	if (_callback) {
		_callback(feedback);
	}
}
