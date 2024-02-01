#ifndef __STRINGS_H__
#define __STRINGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

int int32ToString(int32_t x, char str[], uint16_t d);
int int64ToString(int64_t x, char *output);
int floatToString(float x, char *output);

#ifdef __cplusplus
}
#endif

#endif /* __STRINGS_H__ */
