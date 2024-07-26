#include "RemoteControl.h"

RemoteControl::RemoteControl(UART_HandleTypeDef *huart) : _huart(huart) {
  Reset();
}

void RemoteControl::Start() {
  HAL_UART_Receive_DMA(_huart, _buffer.data(), BUFFER_SIZE);
  _status = Status::Running;
}

void RemoteControl::Stop() {
  HAL_UART_Abort(_huart);
  _status = Status::Stopped;
}

void RemoteControl::Reset() {
  _controllerData = {
      1024, 1024, 1024, 1024, SwitchPosition::Unknown, SwitchPosition::Unknown,
      1024};
  _status = Status::Unknown;
}

bool RemoteControl::RxCallback(const UART_HandleTypeDef *huart) {
  if (huart->Instance != _huart->Instance) {
    return false;
  }

  if (!ParseReceivedData()) {
    return RxCallbackErrorHandler();
  }

  _lastReceiveTime = HAL_GetTick();
  Start();
  return true;
}

void RemoteControl::WatchDog() {
  uint32_t currentTime = HAL_GetTick();
  if (_status != Status::Stopped && _status != Status::Unknown &&
      (currentTime - _lastReceiveTime) > TIMEOUT_DURATION) {
    RxTimeoutCallback();
  }
}

std::optional<RemoteControl::ControllerData>
RemoteControl::GetControllerData() const {
  if (_status == Status::Running) {
    return _controllerData;
  }
  return std::nullopt;
}

void RemoteControl::RxTimeoutCallback() {
  Reset();
  Stop();
  Start();
  _status = Status::Timeout;
}

bool RemoteControl::RxCallbackErrorHandler() {
  Reset();
  Stop();
  Start();
  _status = Status::Error;
  return false;
}

bool RemoteControl::ParseReceivedData() {
  // 解算接收到的数据
  _controllerData.RightStickX = (_buffer[0] | (_buffer[1] << 8)) & 0x07ff;
  _controllerData.RightStickX -= 1024;
  if (!IsValidChannelData(_controllerData.RightStickX)) {
    return false;
  }

  _controllerData.RightStickY =
      ((_buffer[1] >> 3) | (_buffer[2] << 5)) & 0x07ff;
  _controllerData.RightStickY -= 1024;
  if (!IsValidChannelData(_controllerData.RightStickY)) {
    return false;
  }

  _controllerData.LeftStickX =
      ((_buffer[2] >> 6) | (_buffer[3] << 2) | (_buffer[4] << 10)) & 0x07ff;
  _controllerData.LeftStickX -= 1024;
  if (!IsValidChannelData(_controllerData.LeftStickX)) {
    return false;
  }

  _controllerData.LeftStickY = ((_buffer[4] >> 1) | (_buffer[5] << 7)) & 0x07ff;
  _controllerData.LeftStickY -= 1024;
  if (!IsValidChannelData(_controllerData.LeftStickY)) {
    return false;
  }

  _controllerData.Dial = ((_buffer[16]) | (_buffer[17] << 8)) & 0x07ff;
  _controllerData.Dial -= 1024;
  if (!IsValidChannelData(_controllerData.Dial)) {
    return false;
  }

  _controllerData.LeftSwitch =
      static_cast<SwitchPosition>(((_buffer[5] >> 4) & 0x000C) >> 2);
  _controllerData.RightSwitch =
      static_cast<SwitchPosition>((_buffer[5] >> 4) & 0x0003);

  return true;
}

bool RemoteControl::operator>>(ControllerData &controllerData) {
  auto data = GetControllerData();
  if (data.has_value()) {
    controllerData = *data;
    return true;
  }
  return false;
}