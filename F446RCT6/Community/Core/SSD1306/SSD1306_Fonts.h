#pragma once

#include <cstdint>

// 结构体用于定义字体
typedef struct {
  const uint8_t FontWidth;  // 字体宽度（以像素为单位）
  const uint8_t FontHeight; // 字体高度（以像素为单位）
  const uint16_t *data;     // 指向字体数据数组的指针
} SSD1306_Fonts;

// 导出 3 种可用的字体
extern SSD1306_Fonts SSD1306Font_7x10;  // 导出 7x10 大小的字体定义
extern SSD1306_Fonts SSD1306Font_11x18; // 导出 11x18 大小的字体定义
extern SSD1306_Fonts SSD1306Font_16x26; // 导出 16x26 大小的字体定义