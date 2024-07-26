#pragma once

#include <cstdint>

/**
 * @brief SSD1306 字体类
 */
class SSD1306Font {
public:
  /**
   * @brief 构造函数
   * @param width 字体宽度（像素）
   * @param height 字体高度（像素）
   * @param fontData 指向字体数据数组的指针
   */
  SSD1306Font(uint8_t width, uint8_t height, const uint16_t *fontData)
      : FontWidth(width), FontHeight(height), data(fontData) {}

  const uint8_t FontWidth;  // 字体宽度
  const uint8_t FontHeight; // 字体高度
  const uint16_t *data;     // 字体数据
};

/**
 * @brief SSD1306 字体管理类
 */
class SSD1306Fonts {
public:
  /**
   * @brief 字体对象
   */
  static const SSD1306Font Font_7x10;
  static const SSD1306Font Font_11x18;
  static const SSD1306Font Font_16x26;

private:
  /**
   * @brief 字体数据
   */
  static const uint16_t Font7x10[];
  static const uint16_t Font11x18[];
  static const uint16_t Font16x26[];
};