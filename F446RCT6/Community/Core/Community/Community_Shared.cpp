#include "Community_Shared.h"

// 电机实际速度
int16_t leftSpeed = 0;   // 左轮速度
int16_t rightSpeed = 0;  // 右轮速度

// 电机目标速度
int16_t leftTarget = 0;  // 左轮目标速度
int16_t rightTarget = 0; // 右轮目标速度

inline bool MotorSpeed_ReceivedCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData) {
	if (rxHeader->StdId == 0x1f0 && rxHeader->DLC == 8) {
		// 如果接收到的是 ID 为 0x1f0 的 6 字节数据

		// 从 rxData 中解析出数据
		leftSpeed = rxData[0] | (rxData[1] << 8);
		leftTarget = rxData[2] | (rxData[3] << 8);
		rightSpeed = rxData[4] | (rxData[5] << 8);
		rightTarget = rxData[6] | (rxData[7] << 8);

		return true;
	}
	return false;
}

// 电机速度接收
Community community0(&hcan1, MotorSpeed_ReceivedCallback);
Community community1(&hcan2, MotorSpeed_ReceivedCallback);