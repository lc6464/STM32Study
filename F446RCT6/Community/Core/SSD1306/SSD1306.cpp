#include "SSD1306.h"

HAL_StatusTypeDef SSD1306::Start() {
	uint8_t status = 0; // 初始化状态变量为0

	// 初始化LCD显示屏
	status += WriteCommand(0xAE); // 发送命令：关闭显示
	status += WriteCommand(0x20); // 发送命令：设置内存寻址模式
	status += WriteCommand(0x10); // 发送命令：设置为页面寻址模式 (00=水平寻址模式, 01=垂直寻址模式, 10=页面寻址模式, 11=无效)
	status += WriteCommand(0xB0); // 发送命令：设置页面起始地址为第0到第7页
	status += WriteCommand(0xC8); // 发送命令：设置COM输出扫描方向
	status += WriteCommand(0x00); // 发送命令：设置低列地址
	status += WriteCommand(0x10); // 发送命令：设置高列地址
	status += WriteCommand(0x40); // 发送命令：设置起始行地址
	status += WriteCommand(0x81); // 发送命令：设置对比度控制寄存器
	status += WriteCommand(0xFF); // 发送命令：设置对比度为最大值
	status += WriteCommand(0xA1); // 发送命令：设置段重映射 (0到127)
	status += WriteCommand(0xA6); // 发送命令：设置为正常显示模式（不反转显示）

	status += WriteCommand(0xA8); // 发送命令：设置多路复用率 (1到64)
	status += WriteCommand(HEIGHT - 1); // 设置多路复用率值（屏幕高度减1）

	status += WriteCommand(0xA4); // 发送命令：关闭整个显示 (0xA4=显示跟随 RAM 内容, 0xA5=忽略 RAM 内容，全部点亮)
	status += WriteCommand(0xD3); // 发送命令：设置显示偏移
	status += WriteCommand(0x00); // 发送命令：无偏移
	status += WriteCommand(0xD5); // 发送命令：设置显示时钟分频比/振荡频率
	status += WriteCommand(0xF0); // 发送命令：设置分频比
	status += WriteCommand(0xD9); // 发送命令：设置预充电周期
	status += WriteCommand(0x22); // 设置预充电周期的值

	status += WriteCommand(0xDA); // 发送命令：设置COM引脚硬件配置
	status += WriteCommand(COM_LR_REMAP << 5 | COM_ALTERNATIVE_PIN_CONFIG << 4 | 0x02); // 设置 COM 引脚硬件配置的具体值

	status += WriteCommand(0xDB); // 发送命令：设置 VComH 电平
	status += WriteCommand(0x20); // 设置 VComH 电平的值为 0.77xVcc
	status += WriteCommand(0x8D); // 发送命令：设置DC-DC控制
	status += WriteCommand(0x14); // 启用 DC-DC 转换器
	status += WriteCommand(0xAF); // 发送命令：打开显示屏

	if (status != 0) {
		return HAL_ERROR;
	}

	// 清屏
	Fill(Color::Black);

	// 刷新缓冲区到屏幕
	UpdateScreen();

	// 设置默认值
	_currentX = 0;
	_currentY = 0;
	_started = 1;

	return HAL_OK;
}

HAL_StatusTypeDef SSD1306::WriteCommand(uint8_t command) {
	return HAL_I2C_Mem_Write(_hi2c, I2C_ADDR, 0x00, 1, &command, 1, 10);
}

void SSD1306::Fill(Color color) {
	// 填充屏幕缓冲区
	for (auto &byte : _buffer) {
		byte = (color == Color::Black) ? 0x00 : 0xFF;
	}
}

void SSD1306::UpdateScreen() {
	for (uint8_t i = 0; i < 8; i++) {
		WriteCommand(0xB0 + i);
		WriteCommand(0x00);
		WriteCommand(0x10);

		HAL_I2C_Mem_Write(_hi2c, I2C_ADDR, 0x40, 1, _buffer.data() + (WIDTH * i), WIDTH, 100);
	}
}

void SSD1306::DrawPixel(uint8_t x, uint8_t y, Color color/* = Color::White*/) {
	if (x >= WIDTH || y >= HEIGHT) {
		// 不要在缓冲区外写入
		return;
	}

	// 检查是否需要反转颜色
	if (_inverted) {
		color = static_cast<Color>(static_cast<uint8_t>(color) ^ 1);
	}

	// 绘制正确颜色的像素
	if (color == Color::White) {
		_buffer[x + (y / 8) * WIDTH] |= 1 << (y % 8);
	} else {
		_buffer[x + (y / 8) * WIDTH] &= ~(1 << (y % 8));
	}
}

char SSD1306::WriteChar(char ch, SSD1306_Fonts Font, Color color/* = Color::White*/) {
	// 检查当前行剩余空间
	if (WIDTH <= (_currentX + Font.FontWidth) || HEIGHT <= (_currentY + Font.FontHeight)) {
		// 当前行空间不足
		return 0;
	}

	// 将字体翻译到屏幕缓冲区
	for (uint32_t i = 0; i < Font.FontHeight; i++) {
		uint32_t b = Font.data[(ch - 32) * Font.FontHeight + i];
		for (uint32_t j = 0; j < Font.FontWidth; j++) {
			if ((b << j) & 0x8000) {
				DrawPixel(_currentX + j, (_currentY + i), color);
			} else {
				DrawPixel(_currentX + j, (_currentY + i), static_cast<Color>(static_cast<uint8_t>(color) ^ 1));
			}
		}
	}

	// 当前空间已被占用
	_currentX += Font.FontWidth;

	// 返回写入的字符以进行验证
	return ch;
}

char SSD1306::WriteString(const char *str, SSD1306_Fonts Font, Color color/* = Color::White*/) {
	// 写入直到空字节
	while (*str) {
		if (WriteChar(*str, Font, color) != *str) {
			// 字符无法写入
			return *str;
		}

		// 下一个字符
		str++;
	}

	// 一切正常
	return *str;
}

void SSD1306::SetCursor(uint8_t x, uint8_t y) {
	_currentX = x;
	_currentY = y;
}

void SSD1306::InvertColors() {
	_inverted = !_inverted;
}