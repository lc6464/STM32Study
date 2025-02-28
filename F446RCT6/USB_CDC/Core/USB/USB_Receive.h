#pragma once

#include <array>
#include <cstdint>

enum class USBReceivedDataState {
	Idle,
	Received,
	Processed
};

extern USBReceivedDataState usbDataState;

extern std::array<uint8_t, 64> usbReceivedDataBuffer;
extern uint16_t usbReceivedDataLength;

void ProcessUSBPacket();