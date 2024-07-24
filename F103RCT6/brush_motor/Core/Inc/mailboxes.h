#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "can.h"

class Mailboxes {
private:
	static std::vector<std::unique_ptr<uint32_t>> _mailboxes; // 使用静态 vector 存储邮箱指针
	static constexpr uint8_t MAX_MAILBOXES = 3; // 最大邮箱数量

	// 私有构造函数，防止实例化
	Mailboxes() = default;

public:
	// 新建邮箱方法
	static uint32_t *Create() {
		// 检查是否已达到最大邮箱数量
		if (_mailboxes.size() >= MAX_MAILBOXES) {
			// 如果达到最大数量，取消最早的邮箱发送并移除
			_cancelAndRemoveOldestMailbox();
		}

		// 创建新的邮箱
		std::unique_ptr<uint32_t> newMailbox = std::make_unique<uint32_t>(0);
		uint32_t *mailboxPtr = newMailbox.get();

		// 将新邮箱添加到 vector 中
		_mailboxes.push_back(std::move(newMailbox));

		return mailboxPtr;
	}

	// 获取当前邮箱数量
	static size_t GetCount() {
		return _mailboxes.size();
	}

	// 清空所有邮箱
	static void Clear() {
		// 取消所有邮箱的发送
		for (const auto &mailbox : _mailboxes) {
			_cancelMailboxTransmission(*mailbox);
		}
		_mailboxes.clear();
	}

private:
	// 取消并移除最早的邮箱
	static void _cancelAndRemoveOldestMailbox() {
		if (!_mailboxes.empty()) {
			// 取消最早邮箱的发送
			_cancelMailboxTransmission(*_mailboxes.front());
			// 移除最早的邮箱
			_mailboxes.erase(_mailboxes.begin());
		}
	}

	// 取消指定邮箱的发送
	static void _cancelMailboxTransmission(uint32_t mailboxValue) {
		HAL_CAN_AbortTxRequest(&hcan, mailboxValue);
	}
};

// 初始化静态成员
std::vector<std::unique_ptr<uint32_t>> Mailboxes::_mailboxes;