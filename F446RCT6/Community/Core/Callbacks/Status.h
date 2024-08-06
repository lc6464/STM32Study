#pragma once

#include <cstring>

enum class SystemStatus {
	Stopped,               // 停止
	RunningAsOneStick,     // 右杆 XY 控制
	RunningAsDualStick,    // 双杆 Y 控制
	EmergencyBrake,        // 紧急制动
	Error                  // 错误状态（如遥控器离线）
};

enum class SpeedMode {
	Off,
	Low,
	High,
	EmergencyBrake
};

extern SystemStatus status;
extern SpeedMode mode;


inline void SystemStatusToString(SystemStatus status, char *buffer) {
	switch (status) {
	case SystemStatus::Stopped:
		strcpy(buffer, "Stop");
		break;
	case SystemStatus::RunningAsOneStick:
		strcpy(buffer, "1 Stick");
		break;
	case SystemStatus::RunningAsDualStick:
		strcpy(buffer, "2 Stick");
		break;
	case SystemStatus::EmergencyBrake:
		strcpy(buffer, "EB");
		break;
	case SystemStatus::Error:
		strcpy(buffer, "Err");
		break;
	}
}

inline void SpeedModeToString(SpeedMode mode, char *buffer) {
	switch (mode) {
	case SpeedMode::Off:
		strcpy(buffer, "Off");
		break;
	case SpeedMode::Low:
		strcpy(buffer, "Low");
		break;
	case SpeedMode::High:
		strcpy(buffer, "High");
		break;
	case SpeedMode::EmergencyBrake:
		strcpy(buffer, "EB");
		break;
	}
}