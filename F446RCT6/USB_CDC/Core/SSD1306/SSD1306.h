#pragma once

#include <array>

#include "i2c.h"
#include "SSD1306_Fonts.h"

class SSD1306 {
private:
	static constexpr uint16_t WIDTH = 128;
	static constexpr uint16_t HEIGHT = 64;
	static constexpr uint8_t COM_LR_REMAP = 0;
	static constexpr uint8_t COM_ALTERNATIVE_PIN_CONFIG = 1;

	uint8_t _i2c_address;     // I2C 地址
	I2C_HandleTypeDef *_hi2c; // I2C 句柄
	std::array<uint8_t, WIDTH *HEIGHT / 8> _buffer{}; // 屏幕缓冲区
	uint16_t _currentX; // 当前 X 坐标
	uint16_t _currentY; // 当前 Y 坐标
	uint8_t _inverted;  // 是否反转
	uint8_t _started;   // 是否初始化

	/**
	 * @brief 发送命令到 OLED
	 * @param command 命令
	 * @return 操作状态
	 */
	HAL_StatusTypeDef WriteCommand(uint8_t command);

public:
	enum class Color {
		Black = 0, // 黑色，不显示
		White = 1  // 取决于显示屏
	};

	/**
	 * @brief 构造函数，初始化屏幕
	 * @param hi2c I2C 句柄
	 * @param address I2C 地址
	 */
	explicit SSD1306(I2C_HandleTypeDef *hi2c, uint8_t address = 0x78)
		: _i2c_address(address), _hi2c(hi2c), _currentX(0), _currentY(0), _inverted(0), _started(0) { }

	/**
	 * @brief 启动屏幕
	 * @return 操作状态
	 */
	HAL_StatusTypeDef Start();

	/**
	 * @brief 更新屏幕
	 */
	void UpdateScreen();

	/**
	 * @brief 填充屏幕
	 * @param color 填充颜色
	 */
	void Fill(Color color = Color::White);

	/**
	 * @brief 清屏
	 */
	void Clear() { Fill(Color::Black); }

	/**
	 * @brief 绘制像素
	 * @param x X 坐标
	 * @param y Y 坐标
	 * @param color 像素颜色
	 */
	void DrawPixel(uint8_t x, uint8_t y, Color color = Color::White);

	/**
	 * @brief 写字符
	 * @param ch 字符
	 * @param Font 字体
	 * @param color 字符颜色
	 * @return 写入的字符
	 */
	char WriteChar(char ch, SSD1306Font Font, Color color = Color::White);

	/**
	 * @brief 写字符串
	 * @param str 字符串
	 * @param Font 字体
	 * @param color 字符颜色
	 * @return 写入的字符
	 */
	char WriteString(const char *str, SSD1306Font Font, Color color = Color::White);

	/**
	 * @brief 设置光标位置
	 * @param x X 坐标
	 * @param y Y 坐标
	 */
	void SetCursor(uint8_t x, uint8_t y);

	/**
	 * @brief 反转颜色
	 */
	void InvertColors();

	/**
	 * @brief 获取屏幕缓冲区引用
	 * @return 屏幕缓冲区引用
	 */
	std::array<uint8_t, WIDTH *HEIGHT / 8> &GetBuffer() { return _buffer; }

};