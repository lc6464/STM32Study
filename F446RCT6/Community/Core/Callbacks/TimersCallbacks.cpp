#include <algorithm>
#include <cmath>
#include <cstring>

#include "Community_Shared.h"
#include "Mailboxes.h"
#include "RemoteControl_Shared.h"
#include "SSD1306_Shared.h"
#include "strings.h"
#include "tim.h"

enum class SystemStatus { Stopped, Running, EmergencyBrake, Error };

enum class SpeedMode { Off, Low, High, EmergencyBrake };

auto status = SystemStatus::Stopped;
auto mode = SpeedMode::Off;

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

// 将摇杆坐标映射到电机速度
void MapRemoteToMotorSpeed(int16_t remoteX, int16_t remoteY,
                           float &leftMotorSpeed, float &rightMotorSpeed) {
  // 紧急停车模式下直接停止
  if (mode == SpeedMode::EmergencyBrake) {
    leftMotorSpeed = 0.0f;
    rightMotorSpeed = 0.0f;
    return;
  }

  // Stop 模式下缓慢停止
  if (mode == SpeedMode::Off) {
    if (leftMotorSpeed > 0.0f) {
      leftMotorSpeed -= std::min(30.0f, leftMotorSpeed);
    } else if (leftMotorSpeed < 0.0f) {
      leftMotorSpeed += std::min(30.0f, -leftMotorSpeed);
    }

    if (rightMotorSpeed > 0.0f) {
      rightMotorSpeed -= std::min(30.0f, rightMotorSpeed);
    } else if (rightMotorSpeed < 0.0f) {
      rightMotorSpeed += std::min(30.0f, -rightMotorSpeed);
    }
    return;
  }

  if (remoteX == 0 && remoteY == 0) {
    leftMotorSpeed = 0.0f;
    rightMotorSpeed = 0.0f;
    return;
  }

  float maxTargetSpeed = mode == SpeedMode::Low ? 300.0f : 500.0f;

  // 限制遥控器输入范围
  remoteX = std::clamp(remoteX, static_cast<int16_t>(-660),
                       static_cast<int16_t>(660));
  remoteY = std::clamp(remoteY, static_cast<int16_t>(-660),
                       static_cast<int16_t>(660));

  // 计算遥控器输入的模长
  float magnitude = std::sqrt(remoteX * remoteX + remoteY * remoteY);

  // 如果输入太小，认为是静止状态
  if (magnitude < 20.0f) {
    leftMotorSpeed = 0.0f;
    rightMotorSpeed = 0.0f;
    return;
  }

  // 将模长映射到电机速度范围
  float speed = (magnitude / 660.0f) * maxTargetSpeed;

  // 判断是否需要执行原地掉头或小半径掉头
  const float turnThreshold = 0.95f; // 可以根据需要调整这个阈值
  if (std::abs(remoteX) > turnThreshold * magnitude) {
    if (std::abs(remoteY) <= 30) {
      // 执行等速原地掉头
      if (remoteX > 0) {
        // 向右转
        leftMotorSpeed = speed;
        rightMotorSpeed = -speed;
      } else {
        // 向左转
        leftMotorSpeed = -speed;
        rightMotorSpeed = speed;
      }
    } else {
      // 执行小半径掉头
      float turnRatio = std::abs(static_cast<float>(remoteY) / remoteX);
      float innerWheelSpeed = speed * turnRatio;
      float outerWheelSpeed = speed;

      if (remoteX > 0) {
        // 向右转
        leftMotorSpeed = outerWheelSpeed;
        rightMotorSpeed = -innerWheelSpeed;
      } else {
        // 向左转
        leftMotorSpeed = -innerWheelSpeed;
        rightMotorSpeed = outerWheelSpeed;
      }
    }
  } else {
    // 正常行驶模式
    // 计算方向角（弧度）
    float angle = std::atan2(remoteY, remoteX);

    // 根据角度计算左右轮速度差
    float turnFactor = std::cos(angle);

    // 计算左右轮速度
    leftMotorSpeed = speed * (1.0f + turnFactor);
    rightMotorSpeed = speed * (1.0f - turnFactor);
  }

  // 考虑到电机安装方向，左轮速度取反
  leftMotorSpeed = -leftMotorSpeed;

  // 限制电机速度范围
  leftMotorSpeed = std::clamp(leftMotorSpeed, -maxTargetSpeed, maxTargetSpeed);
  rightMotorSpeed =
      std::clamp(rightMotorSpeed, -maxTargetSpeed, maxTargetSpeed);
}

// 映射遥控状态到系统状态和速度模式
void MapRemoteToStatusAndMode() {
  auto rcStatus = remoteControl.GetStatus();
  if (rcStatus != RemoteControl::Status::Running) {
    status = SystemStatus::Error;
    mode = SpeedMode::EmergencyBrake;
    return;
  }

  // 右开关 上下 -> EB + EB  中 -> 由左开关决定
  // 左开关 上中下 -> Stop + Off / Run + Low / Run + High
  if (controllerData.RightSwitch != RemoteControl::SwitchPosition::Middle) {
    status = SystemStatus::EmergencyBrake;
    mode = SpeedMode::EmergencyBrake;
    return;
  }

  if (controllerData.LeftSwitch == RemoteControl::SwitchPosition::Up) {
    status = SystemStatus::Stopped;
    mode = SpeedMode::Off;
  } else {
    status = SystemStatus::Running;
    mode = controllerData.LeftSwitch == RemoteControl::SwitchPosition::Middle
               ? SpeedMode::Low
               : SpeedMode::High;
  }
}

// 定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM7) { // 10ms
    // Watchdogs
    remoteControl.Tick();

    return;
  }

  if (htim->Instance == TIM6) { // 100ms
    // Watchdogs
    communityWatchDog.Tick();

    // 更新当前系统状态和速度模式
    MapRemoteToStatusAndMode();

    // 更新电机速度
    MapRemoteToMotorSpeed(controllerData.RightStickX,
                          controllerData.RightStickY, leftTarget, rightTarget);

    // 发送电机速度
    auto mailbox = Mailboxes::Create();
    auto sent_result =
        community1.SendSpeed(0x002, mailbox, leftTarget, rightTarget);
    UNUSED(sent_result);

    // 更新屏幕
    char buffer[8] = {0};

    ssd1306.Clear();

    /*
            @todo 展示信息
            RC: n, Motor: OK/Err     // 遥控状态；电机状态                 高 11
       -> 0, 0 Status: Stop/Run/EB/Err  // 系统状态：停止/运行/紧急停止/错误 高
       12 -> 0, 11 Left: ttt.t,sss.s        // 左电机目标，左电机转速 高 10 ->
       0, 23 Right: ttt.t,sss.s      // 右电机目标，右电机转速             高 10
       -> 0, 33 Mode: Off/Low/High/EB    // 速度模式：关闭/低速/高速/紧急停止 高
       10 -> 0, 43 Stick: x, y              // 右摇杆坐标 高 10 -> 0, 53
    */

    ssd1306.SetCursor(0, 0);
    ssd1306.WriteString("RC: ", SSD1306Fonts::Font_7x10);
    uint8ToString(static_cast<uint8_t>(remoteControl.GetStatus()), buffer);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
    ssd1306.WriteString(", Motor: ", SSD1306Fonts::Font_7x10);
    MotorStatusToString(buffer);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

    ssd1306.SetCursor(0, 11);
    ssd1306.WriteString("Status: ", SSD1306Fonts::Font_7x10);
    SystemStatusToString(status, buffer);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

    ssd1306.SetCursor(0, 23);
    ssd1306.WriteString("Left: ", SSD1306Fonts::Font_7x10);
    floatToString(leftTarget, buffer, 1);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
    ssd1306.WriteString(",", SSD1306Fonts::Font_7x10);
    floatToString(leftSpeed, buffer, 1);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

    ssd1306.SetCursor(0, 33);
    ssd1306.WriteString("Right: ", SSD1306Fonts::Font_7x10);
    floatToString(rightTarget, buffer, 1);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
    ssd1306.WriteString(",", SSD1306Fonts::Font_7x10);
    floatToString(rightSpeed, buffer, 1);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

    ssd1306.SetCursor(0, 43);
    ssd1306.WriteString("Mode: ", SSD1306Fonts::Font_7x10);
    SpeedModeToString(mode, buffer);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

    ssd1306.SetCursor(0, 53);
    ssd1306.WriteString("Stick: ", SSD1306Fonts::Font_7x10);
    int16ToString(controllerData.RightStickX, buffer, 0);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
    ssd1306.WriteString(",", SSD1306Fonts::Font_7x10);
    int16ToString(controllerData.RightStickY, buffer, 0);
    ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

    ssd1306.UpdateScreen();

    return;
  }
}

// 备用代码
/*

// 将开关位置转换为字符串
inline char *SwitchToString(RemoteControl::SwitchPosition position, char
*buffer) { switch (position) { case RemoteControl::SwitchPosition::Up: return
strcpy(buffer, "U"); case RemoteControl::SwitchPosition::Down: return
strcpy(buffer, "D"); case RemoteControl::SwitchPosition::Middle: return
strcpy(buffer, "M"); default: return strcpy(buffer, "?");
        }
};

// 展示遥控器信息
ssd1306.SetCursor(25, 0);
ssd1306.WriteString("RC Info", SSD1306Fonts::Font_11x18);

if (remoteControl.GetStatus() != RemoteControl::Status::Running) {
        ssd1306.SetCursor(25, 30);
        ssd1306.WriteString("Error ", SSD1306Fonts::Font_11x18);
        uint8ToString(static_cast<uint8_t>(remoteControl.GetStatus()), buffer);
        ssd1306.WriteString(buffer, SSD1306Fonts::Font_11x18);
} else {
        ssd1306.SetCursor(0, 20);
        ssd1306.WriteString("S L:", SSD1306Fonts::Font_7x10);
        ssd1306.SetCursor(0, 30);
        ssd1306.WriteString("S R:", SSD1306Fonts::Font_7x10);
        ssd1306.SetCursor(0, 40);
        ssd1306.WriteString("3PS:", SSD1306Fonts::Font_7x10);
        ssd1306.SetCursor(0, 50);
        ssd1306.WriteString("Dial:", SSD1306Fonts::Font_7x10);

        // Stick L: X,Y
        ssd1306.SetCursor(40, 20);
        int16ToString(controllerData.LeftStickX, buffer, 0);
        ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
        ssd1306.WriteChar(',', SSD1306Fonts::Font_7x10);
        int16ToString(controllerData.LeftStickY, buffer, 0);
        ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

        // Stick R: X,Y
        ssd1306.SetCursor(40, 30);
        int16ToString(controllerData.RightStickX, buffer, 0);
        ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
        ssd1306.WriteChar(',', SSD1306Fonts::Font_7x10);
        int16ToString(controllerData.RightStickY, buffer, 0);
        ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

        // 3PS: L,R
        ssd1306.SetCursor(40, 40);
        SwitchToString(controllerData.LeftSwitch, buffer);
        ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
        ssd1306.WriteChar(',', SSD1306Fonts::Font_7x10);
        SwitchToString(controllerData.RightSwitch, buffer);
        ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

        // Dial: D
        ssd1306.SetCursor(40, 50);
        int16ToString(controllerData.Dial, buffer, 0);
        ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
}


*/