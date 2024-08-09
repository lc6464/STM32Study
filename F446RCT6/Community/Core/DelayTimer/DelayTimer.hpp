#pragma once

#include "main.h"

class DelayTimer {
private:
	// 防止实例化
	explicit DelayTimer() = default;

	/**
	 * @brief 获取当前 SysTick 定时器的值
	 * @return uint32_t 当前 SysTick 定时器的值
	 */
	static uint32_t GetCurrentTick() {
		return SysTick->VAL;
	}

	/**
	 * @brief 获取系统时钟频率
	 * @return uint32_t 系统时钟频率（Hz）
	 */
	static uint32_t GetSystemClockFreq() {
		return HAL_RCC_GetSysClockFreq();
	}

public:
	/**
	 * @brief 阻塞延迟指定的 SysTick 时钟周期数
	 * @param ticks 要延迟的 SysTick 时钟周期数
	 */
	static void DelayTicks(uint64_t ticks) {
		if (ticks == 0) {
			return;
		}

		uint32_t start = GetCurrentTick();
		uint32_t last = start;
		uint64_t elapsed = 0;

		do {
			uint32_t current = GetCurrentTick();

			if (current > last) {
				// 发生了溢出
				elapsed += last + (SysTick->LOAD - current);
			} else {
				elapsed += last - current;
			}

			last = current;
		} while (elapsed < ticks);
	}

	/**
	 * @brief 阻塞延迟指定的微秒数
	 * @param us 要延迟的微秒数
	 */
	static void DelayMicroseconds(uint64_t us) {
		uint32_t clockFreq = GetSystemClockFreq();
		uint64_t ticksPerUs = clockFreq / 1000000;
		DelayTicks(us * ticksPerUs);
	}

	/**
	 * @brief 阻塞延迟指定的毫秒数
	 * @param ms 要延迟的毫秒数
	 */
	static void DelayMilliseconds(uint64_t ms) {
		uint32_t clockFreq = GetSystemClockFreq();
		uint64_t ticksPerMs = clockFreq / 1000;
		DelayTicks(ms * ticksPerMs);
	}
};