#include "Callbacks_Shared.h"
#include "Community_Shared.h"
#include "Status.h"

inline bool MotorSpeed_ReceivedCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData) {
	if (rxHeader->DLC == 8) {
		if (rxHeader->StdId == 0x002) {
			// 如果接收到的是 ID 为 0x002 的 8 字节数据

			systemWatchDog.Feed();

			// 从 rxData 中解析出数据
			Status::leftFrontTarget = *reinterpret_cast<float *>(rxData);
			Status::rightFrontTarget = *reinterpret_cast<float *>(rxData + 4);
			Status::leftBackTarget = Status::leftFrontTarget;  // 暂定左后轮目标速度与左前轮相同
			Status::rightBackTarget = Status::rightFrontTarget; // 暂定右后轮目标速度与右前轮相同

			return true;
		}
		if (rxHeader->StdId == 0x004) {
			// 如果接收到的是 ID 为 0x004 的 8 字节数据

			systemWatchDog.Feed();

			// 从 rxData 中解析出数据
			Status::leftFrontSpeed = *reinterpret_cast<float *>(rxData);
			Status::rightFrontSpeed = *reinterpret_cast<float *>(rxData + 4);
			Status::leftBackSpeed = Status::leftFrontSpeed;  // 暂定左后轮实际速度与左前轮相同
			Status::rightBackSpeed = Status::rightFrontSpeed; // 暂定右后轮实际速度与右前轮相同

			return true;
		}
	}
	return false;
}

// 电机速度接收
Community community(&hcan, MotorSpeed_ReceivedCallback);