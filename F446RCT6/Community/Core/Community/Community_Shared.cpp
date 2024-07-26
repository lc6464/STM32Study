#include "Community_Shared.h"

// 电机控制板状态
MotorsStatus motorsStatus = MotorsStatus::Error;

// 电机实际速度
float leftSpeed = 0;   // 左轮速度
float rightSpeed = 0;  // 右轮速度

// 电机目标速度
float leftTarget = 0;  // 左轮目标速度
float rightTarget = 0; // 右轮目标速度

inline bool MotorSpeed_ReceivedCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData) {
	if (rxHeader->DLC == 8) {
		if (rxHeader->StdId == 0x002) {
			// 如果接收到的是 ID 为 0x002 的 8 字节数据

			motorsStatus = MotorsStatus::OK;
			communityWatchDog.Feed();

			// 从 rxData 中解析出数据
			leftTarget = *reinterpret_cast<float *>(rxData);
			rightTarget = *reinterpret_cast<float *>(rxData + 4);

			return true;
		}
		if (rxHeader->StdId == 0x004) {
			// 如果接收到的是 ID 为 0x004 的 8 字节数据

			motorsStatus = MotorsStatus::OK;
			communityWatchDog.Feed();

			// 从 rxData 中解析出数据
			leftSpeed = *reinterpret_cast<float *>(rxData);
			rightSpeed = *reinterpret_cast<float *>(rxData + 4);

			return true;
		}
	}
	return false;
}

// 电机速度接收
Community community0(&hcan1, MotorSpeed_ReceivedCallback);
Community community1(&hcan2, MotorSpeed_ReceivedCallback);

WatchDog communityWatchDog(300, []() { motorsStatus = MotorsStatus::Error; });