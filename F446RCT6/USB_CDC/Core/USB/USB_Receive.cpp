#include "USB_Receive.h"

#include "crc.h"
#include "usbd_cdc_if.h"

#include "SSD1306_Shared.h"

USBReceivedDataState usbDataState = USBReceivedDataState::Idle;

std::array<uint8_t, 64> usbReceivedDataBuffer{};
uint16_t usbReceivedDataLength = 0;

void ProcessUSBPacket() {
	if (usbDataState != USBReceivedDataState::Received) {
		return;
	}
	usbDataState = USBReceivedDataState::Processed;

	// Fixed "C": 0x43
	// "W": 修改屏幕显存；"R"：读取屏幕显存；"U": 更新屏幕；"F": 填充屏幕为全显；"C": 清空屏幕；"P": 修改单个像素
	// "W" 和 "R"：起使索引 (2Bytes) + 数据长度(1Byte, <= 57)；"P"：X坐标 (1Byte) + Y坐标 (1Byte) + 颜色 (1Byte)
	// "W"：数据；"R"：无；"U"：无；"F"：无；"C"：无；"P"：无
	// Fixed "ED": 0x45 0x44

	if (usbReceivedDataLength < 4) {
		usbReceivedDataBuffer.fill(0);
		return;
	}

	if (usbReceivedDataBuffer[0] != 'C') {
		usbReceivedDataBuffer.fill(0);
		return;
	}

	if (usbReceivedDataBuffer[usbReceivedDataLength - 2] != 'E' || usbReceivedDataBuffer[usbReceivedDataLength - 1] != 'D') {
		usbReceivedDataBuffer.fill(0);
		return;
	}

	uint16_t startIndex = 0;
	uint8_t dataLength = 0;

	switch (usbReceivedDataBuffer[1]) {
	case 'W':
		if (usbReceivedDataLength < 7) {
			return;
		}
		// 小字节序
		startIndex = usbReceivedDataBuffer[2] + (usbReceivedDataBuffer[3] << 8);
		dataLength = usbReceivedDataBuffer[4];

		// 丢弃超出长度的数据
		if (dataLength > 57) {
			return;
		}

		// 丢弃超出范围的数据
		if (startIndex + dataLength > 1024) {
			return;
		}

		// 丢弃不完整的数据
		if (usbReceivedDataLength != 7 + dataLength) {
			return;
		}

		std::copy(usbReceivedDataBuffer.begin() + 5, usbReceivedDataBuffer.begin() + 5 + dataLength, ssd1306.GetBuffer().begin() + startIndex);

		break;
	case 'R':
		if (usbReceivedDataLength != 7) {
			return;
		}
		// 小字节序
		startIndex = usbReceivedDataBuffer[2] + (usbReceivedDataBuffer[3] << 8);
		dataLength = usbReceivedDataBuffer[4];

		// 丢弃超出长度的数据
		if (dataLength > 57) {
			return;
		}

		// 丢弃超出范围的数据
		if (startIndex + dataLength > 1024) {
			return;
		}

		CDC_Transmit_FS(ssd1306.GetBuffer().data() + startIndex, dataLength);

		break;
	case 'U':
		if (usbReceivedDataLength == 4) {
			ssd1306.UpdateScreen();
		}
		break;
	case 'F':
		if (usbReceivedDataLength == 4) {
			ssd1306.Fill();
		}
		break;
	case 'C':
		if (usbReceivedDataLength == 4) {
			ssd1306.Clear();
		}
		break;
	case 'P':
		if (usbReceivedDataLength == 7) {
			uint16_t x = usbReceivedDataBuffer[2];
			uint16_t y = usbReceivedDataBuffer[3];
			ssd1306.DrawPixel(x, y, usbReceivedDataBuffer[4] == 0 ? SSD1306::Color::Black : SSD1306::Color::White);
		}
	}

	usbReceivedDataBuffer.fill(0);
}