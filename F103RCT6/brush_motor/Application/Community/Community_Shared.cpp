#include "Callbacks_Shared.h"
#include "Community_Shared.h"

// 电机实际速度
float leftFrontSpeed = 0;   // 左前轮速度
float rightFrontSpeed = 0;  // 右前轮速度
float leftBackSpeed = 0;    // 左后轮速度
float rightBackSpeed = 0;   // 右后轮速度

// 电机目标速度
float leftFrontTarget = 0;  // 左前轮目标速度
float rightFrontTarget = 0; // 右前轮目标速度
float leftBackTarget = 0;   // 左后轮目标速度
float rightBackTarget = 0;  // 右后轮目标速度

inline bool MotorSpeed_ReceivedCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData) {
	if (rxHeader->DLC == 8) {
		if (rxHeader->StdId == 0x002) {
			// 如果接收到的是 ID 为 0x002 的 8 字节数据

			systemWatchDog.Feed();

			// 从 rxData 中解析出数据
			leftFrontTarget = *reinterpret_cast<float *>(rxData);
			rightFrontTarget = *reinterpret_cast<float *>(rxData + 4);
			leftBackTarget = leftFrontTarget;  // 暂定左后轮目标速度与左前轮相同
			rightBackTarget = rightFrontTarget; // 暂定右后轮目标速度与右前轮相同

			return true;
		}
		if (rxHeader->StdId == 0x004) {
			// 如果接收到的是 ID 为 0x004 的 8 字节数据

			systemWatchDog.Feed();

			// 从 rxData 中解析出数据
			leftFrontSpeed = *reinterpret_cast<float *>(rxData);
			rightFrontSpeed = *reinterpret_cast<float *>(rxData + 4);
			leftBackSpeed = leftFrontSpeed;  // 暂定左后轮实际速度与左前轮相同
			rightBackSpeed = rightFrontSpeed; // 暂定右后轮实际速度与右前轮相同

			return true;
		}
	}
	return false;
}

// 电机速度接收
Community community(&hcan, MotorSpeed_ReceivedCallback);