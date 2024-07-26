#pragma once

#include <cstdint>

int uint8ToString(uint8_t x, char str[], uint8_t minLength = 0);
int int8ToString(int8_t x, char str[], uint8_t minLength = 0);
int uint16ToString(uint16_t x, char str[], uint8_t minLength = 0);
int int16ToString(int16_t x, char str[], uint8_t minLength = 0);
int uint32ToString(uint32_t x, char str[], uint8_t minLength = 0);
int int32ToString(int32_t x, char str[], uint8_t minLength = 0);
int int64ToString(int64_t x, char *output);
int floatToString(float x, char *output, uint8_t precision);