#include "can.h"
#include "Motor_Shared.hpp"

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CAN_RxHeaderTypeDef rx_header;
	uint8_t rx_data[8];

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data); // 接收数据

	if (leftMotor.HandleRxData(rx_header, rx_data)) {
		return;
	}

	if (rightMotor.HandleRxData(rx_header, rx_data)) {
		return;
	}
}
