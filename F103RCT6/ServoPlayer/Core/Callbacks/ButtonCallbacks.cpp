#include "Button_Shared.h"
#include "Status.h"

static inline void ec11ButtonShortPressCallback() {

}

static inline void ec11ButtonLongPressCallback() {

}

void RegisterButtonCallbacks() {
	// 注册旋转编码器按键回调函数
	ec11Button.RegisterShortPressCallback(ec11ButtonShortPressCallback);
	ec11Button.RegisterLongPressCallback(ec11ButtonLongPressCallback);
}