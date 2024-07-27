#pragma once

#include <cstring>

enum class SystemStatus { Stopped, Running, EmergencyBrake, Error };

enum class SpeedMode { Off, Low, High, EmergencyBrake };

extern SystemStatus status;
extern SpeedMode mode;

inline void SystemStatusToString(SystemStatus status, char *buffer) {
  switch (status) {
  case SystemStatus::Stopped:
    strcpy(buffer, "Stop");
    break;
  case SystemStatus::Running:
    strcpy(buffer, "Run");
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