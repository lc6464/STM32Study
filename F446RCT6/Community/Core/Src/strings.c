#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "strings.h"

// 辅助函数：反转字符串
static void reverse(char *str, int length) {
  int i = 0, j = length - 1;
  while (i < j) {
    char temp = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
}

// 将 uint8_t 转换为字符串
int uint8ToString(uint8_t x, char str[], uint8_t minLength) {
  int i = 0;

  minLength = minLength == 0 ? 1 : minLength;

  while (x) // 转换字符
  {
    str[i++] = (x % 10) + '0';
    x = x / 10;
  }

  while (i < minLength) { // 补前导零
    str[i++] = '0';
  }

  reverse(str, i); // 反转字符串
  str[i] = '\0';
  return i;
}

// 将 int8_t 转换为字符串
int int8ToString(int8_t x, char str[], uint8_t minLength) {
  int i = 0, isNegative = 0;

  minLength = minLength == 0 ? 1 : minLength;

  if (x < 0) {
    isNegative = 1;
    x = -x;
  }

  while (x) // 转换字符
  {
    str[i++] = (x % 10) + '0';
    x = x / 10;
  }

  while (i < minLength) { // 补前导零
    str[i++] = '0';
  }

  if (isNegative) { // 负数处理
    str[i++] = '-';
  }

  reverse(str, i); // 反转字符串
  str[i] = '\0';
  return i;
}

// 将 uint16_t 转换为字符串
int uint16ToString(uint16_t x, char str[], uint8_t minLength) {
  int i = 0;

  minLength = minLength == 0 ? 1 : minLength;

  while (x) // 转换字符
  {
    str[i++] = (x % 10) + '0';
    x = x / 10;
  }

  while (i < minLength) { // 补前导零
    str[i++] = '0';
  }

  reverse(str, i); // 反转字符串
  str[i] = '\0';
  return i;
}

// 将 int16_t 转换为字符串
int int16ToString(int16_t x, char str[], uint8_t minLength) {
  int i = 0, isNegative = 0;

  minLength = minLength == 0 ? 1 : minLength;

  if (x < 0) {
    isNegative = 1;
    x = -x;
  }

  while (x) // 转换字符
  {
    str[i++] = (x % 10) + '0';
    x = x / 10;
  }

  while (i < minLength) { // 补前导零
    str[i++] = '0';
  }

  if (isNegative) { // 负数处理
    str[i++] = '-';
  }

  reverse(str, i); // 反转字符串
  str[i] = '\0';
  return i;
}

// 将 uint32_t 转换为字符串
int uint32ToString(uint32_t x, char str[], uint8_t minLength) {
  int i = 0;

  minLength = minLength == 0 ? 1 : minLength;

  while (x) // 转换字符
  {
    str[i++] = (x % 10) + '0';
    x = x / 10;
  }

  while (i < minLength) { // 补前导零
    str[i++] = '0';
  }

  reverse(str, i); // 反转字符串
  str[i] = '\0';
  return i;
}

// 将 int32_t 转换为字符串
int int32ToString(int32_t x, char str[], uint8_t minLength) {
  int i = 0, isNegative = 0;

  minLength = minLength == 0 ? 1 : minLength;

  if (x < 0) {
    isNegative = 1;
    x = -x;
  }

  while (x) // 转换字符
  {
    str[i++] = (x % 10) + '0';
    x = x / 10;
  }

  while (i < minLength) { // 补前导零
    str[i++] = '0';
  }

  if (isNegative) { // 负数处理
    str[i++] = '-';
  }

  reverse(str, i); // 反转字符串
  str[i] = '\0';
  return i;
}

// 将 int64_t 转换为字符串
int int64ToString(int64_t x, char *output) {
  uint8_t isNegative = 0;
  if (x < 0) // 负数处理
  {
    *output++ = '-';
    if (x == (-0x7fffffffffffffffLL - 1)) // 最小值懒得处理，直接写死
    {
      char temp[] = "9223372036854775808";
      memcpy(output, temp, 20);
      return 20;
    }
    x = -x;
    isNegative = 1;
  }

  int32_t high = (int32_t)(x / 1000000000), low = (int32_t)(x % 1000000000);

  char highString[11];
  char lowString[11];

  int highStringLength = int32ToString(high, highString, 0), lowStringLength;

  if (high) {
    lowStringLength = int32ToString(low, lowString, 9); // 低位需要前导零
    memcpy(output, highString, highStringLength);
    memcpy(output + highStringLength, lowString, lowStringLength + 1);
  } else {
    lowStringLength = int32ToString(low, output, 0); // 没有高位，低位无需前导零
  }

  return highStringLength + lowStringLength +
         isNegative; // 千万别把 isNegative 漏掉了
}

#define MAX_DECIMAL_PLACES 6 // 可调整以确定小数点后的位数

// 将 float 转换为字符串
int floatToString(float value, char *output, uint8_t precision) {
  int isNegative = 0;

  // 检查特殊值
  if (isnan(value)) {
    char temp[] = "NaN";
    memcpy(output, temp, 4);
    return 3;
  }
  if (isinf(value)) {
    if (value > 0) {
      char temp[] = "Infinity";
      memcpy(output, temp, 9);
      return 8;
    } else {
      char temp[] = "-Infinity";
      memcpy(output, temp, 10);
      return 9;
    }
  }

  // 处理符号位
  if (value < 0) {
    *output++ = '-';
    value = -value;
    isNegative = 1;
  }

  int32_t intPart = (int32_t)value;         // 获取整数部分
  float floatPart = value - (float)intPart; // 获取小数部分

  char intString[12];
  int intStringLength = int32ToString(intPart, intString, 0);

  memcpy(output, intString, intStringLength);
  output += intStringLength;

  // 如果没有小数部分，则直接返回整数字符串
  if (floatPart == 0) {
    *output = '\0';
    return intStringLength + isNegative;
  }

  // 添加小数点
  *output++ = '.';

  if (precision > MAX_DECIMAL_PLACES || precision == 0) {
    precision = MAX_DECIMAL_PLACES;
  }

  // 计算小数部分
  char decimalString[precision + 1];
  for (int i = 0; i < precision; ++i) {
    floatPart *= 10;
    int digit = (int)floatPart;
    floatPart -= digit;
    decimalString[i] = '0' + digit;
  }

  decimalString[precision] = '\0';
  memcpy(output, decimalString, precision);
  output += precision;

  *output = '\0'; // 字符串终止符

  return intStringLength + isNegative + precision + 1;
}