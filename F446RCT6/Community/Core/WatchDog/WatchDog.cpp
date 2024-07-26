#include "main.h"
#include "WatchDog.h"

WatchDog::WatchDog(uint32_t timeout_ms, std::function<void()> callback/* = nullptr*/)
	: _timeout(timeout_ms), _lastTime(0), _isEnabled(false) {
	if (callback) {
		_timeoutCallback = callback;
	}

	_lastTime = _getCurrentTime();
}

void WatchDog::Feed(bool isFeed/* = true*/) {
	if (_isEnabled && isFeed) {
		_lastTime = _getCurrentTime();
	}
}

void WatchDog::Tick(bool isTick/* = true*/) {
	if (_isEnabled && isTick) {
		uint32_t currentTime = _getCurrentTime();
		if (currentTime - _lastTime >= _timeout) {
			if (_timeoutCallback) {
				_timeoutCallback();
			}
			_lastTime = currentTime;  // 重置上次喂狗时间
		}
	}
}

void WatchDog::Enable() {
	_isEnabled = true;
	_lastTime = _getCurrentTime();
}

void WatchDog::Disable() {
	_isEnabled = false;
}

void WatchDog::SetTimeout(uint32_t new_timeout_ms) {
	_timeout = new_timeout_ms;
	_lastTime = _getCurrentTime();
}

void WatchDog::RegisterTimeoutCallback(std::function<void()> new_callback) {
	assert_param(new_callback != nullptr);

	_timeoutCallback = new_callback;
}

uint32_t WatchDog::_getCurrentTime() {
	return HAL_GetTick();  // 使用 HAL 库的 GetTick 函数
}