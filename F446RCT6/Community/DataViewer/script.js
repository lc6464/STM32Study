// script.js

/**
 * RPMDataResolver模块用于解析包含四个小字节序浮点数的16字节数据。
 */
const RPMDataResolver = {
	/**
	 * 解析提供的 ArrayBuffer 数据。
	 * @param {ArrayBuffer} buffer - 包含16字节数据的ArrayBuffer。
	 * @returns {object|null} - 包含 leftTargetRPM, rightTargetRPM, leftSpeedRPM, rightSpeedRPM 的对象，或在数据无效时返回 null。
	 */
	parse(buffer) {
		if (!(buffer instanceof ArrayBuffer) || buffer.byteLength !== 16) {
			// console.error('RPMDataResolver: 无效的数据，需要一个16字节的 ArrayBuffer。');
			return null;
		}

		const view = new DataView(buffer);
		try {
			const leftTarget = view.getFloat32(0, true);  // true 表示小字节序
			const rightTarget = view.getFloat32(4, true);
			const leftSpeed = view.getFloat32(8, true);
			const rightSpeed = view.getFloat32(12, true);

			return {
				leftTargetRPM: leftTarget,
				rightTargetRPM: rightTarget,
				leftSpeedRPM: leftSpeed,
				rightSpeedRPM: rightSpeed
			};
		} catch (error) {
			console.error('RPMDataResolver: 解析数据时出错:', error);
			return null;
		}
	}
};

class UIManager {
	constructor() {
		this.domElements = {
			leftTargetRPM: document.getElementById('leftTargetRPM'),
			rightTargetRPM: document.getElementById('rightTargetRPM'),
			leftSpeedRPM: document.getElementById('leftSpeedRPM'),
			rightSpeedRPM: document.getElementById('rightSpeedRPM'),
			statusMessage: document.getElementById('statusMessage'),
			connectSerialBtn: document.getElementById('connectSerialBtn'),
			validPacketsDisplay: document.getElementById('validPackets'),
			invalidPacketsDisplay: document.getElementById('invalidPackets'),
			sendTestDataBtn: document.getElementById('sendTestDataBtn'),
		};
		this.validPackets = 0;
		this.invalidPackets = 0;
		this.watchdogTimer = null;
	}

	updateRPM(data) {
		if (!data) return;
		this.domElements.leftTargetRPM.textContent = data.leftTargetRPM.toFixed(2);
		this.domElements.rightTargetRPM.textContent = data.rightTargetRPM.toFixed(2);
		this.domElements.leftSpeedRPM.textContent = data.leftSpeedRPM.toFixed(2);
		this.domElements.rightSpeedRPM.textContent = data.rightSpeedRPM.toFixed(2);
	}

	updateStatus(message, isConnected = false, isError = false) {
		this.domElements.statusMessage.textContent = `状态：${message}`;
		this.domElements.statusMessage.style.color = isError ? '#c0392b' : '#7f8c8d';
		this.domElements.connectSerialBtn.disabled = isConnected;
		if (isConnected) {
			this.domElements.connectSerialBtn.textContent = '断开连接';
		} else {
			this.domElements.connectSerialBtn.textContent = '连接串口';
		}
	}

	incrementValidPackets() {
		this.validPackets++;
		this.domElements.validPacketsDisplay.textContent = this.validPackets;
		this.resetWatchdog();
	}

	incrementInvalidPackets() {
		this.invalidPackets++;
		this.domElements.invalidPacketsDisplay.textContent = this.invalidPackets;
	}

	resetWatchdog() {
		clearTimeout(this.watchdogTimer);
		this.watchdogTimer = setTimeout(() => {
			this.updateStatus("数据超时，请检查连接或设备端。", this.domElements.connectSerialBtn.disabled, true);
		}, 3000); // 3秒无数据则超时
	}

	clearWatchdog() {
		clearTimeout(this.watchdogTimer);
	}
}

class SerialHandler {
	constructor(uiManager) {
		this.port = null;
		this.reader = null;
		this.keepReading = false;
		this.ui = uiManager;
		this.incomingBuffer = new Uint8Array();
		this.expectedPacketLength = 16; // 4 floats * 4 bytes/float
	}

	async connect() {
		if (!('serial' in navigator)) {
			this.ui.updateStatus('Web Serial API 不受此浏览器支持。', false, true);
			alert('Web Serial API 不受此浏览器支持。请使用最新版的 Chrome, Edge, 或 Opera 浏览器。');
			return;
		}

		try {
			this.port = await navigator.serial.requestPort();
			await this.port.open({ baudRate: 115200 }); // 您可以根据需要修改波特率
			this.ui.updateStatus('串口已连接，等待数据...', true);
			this.keepReading = true;
			this.readLoop();
			this.ui.resetWatchdog();
		} catch (err) {
			// 用户可能取消了选择串口的对话框
			if (err.name === 'NotFoundError' || err.name === "NotAllowedError") {
				this.ui.updateStatus('未选择串口或操作被取消。');
			} else {
				this.ui.updateStatus(`连接错误: ${err.message}`, false, true);
			}
			console.error('串口连接错误:', err);
		}
	}

	async disconnect() {
		this.keepReading = false; // 信号，让 readLoop 停止
		if (this.reader) {
			try {
				// 等待 reader.cancel() 完成，这将使 reader.read() 的 promise resolve/reject
				await this.reader.cancel();
			} catch (err) {
				console.warn("取消读取时出错:", err);
			}
			// releaseLock 会在 readLoop 的 finally 中处理
		}

		// 等待 readLoop 结束
		// 实际项目中可能需要更复杂的同步机制，但通常 cancel() 后，循环会很快退出

		if (this.port && this.port.readable) { // 检查 port 是否仍然存在且可读
			// 由于 reader.cancel() 会导致 readLoop 退出并释放锁
			// 此处不需要再次 releaseLock
		}

		if (this.port) {
			try {
				await this.port.close();
			} catch (err) {
				console.warn("关闭串口时出错:", err);
			}
		}

		this.port = null;
		this.reader = null; // 确保 reader 被清除
		this.incomingBuffer = new Uint8Array(); // 清空缓冲区
		this.ui.updateStatus('串口已断开。');
		this.ui.clearWatchdog();
		console.log("串口已断开");
	}

	async readLoop() {
		while (this.port && this.port.readable && this.keepReading) {
			this.reader = this.port.readable.getReader();
			try {
				while (this.keepReading) { // 再次检查 this.keepReading
					const { value, done } = await this.reader.read();
					if (done) {
						// 当 reader.cancel() 被调用时，done 会为 true
						break;
					}
					if (value && this.keepReading) { // 确保在处理数据前仍希望读取
						this.processIncomingData(value);
					}
				}
			} catch (error) {
				if (this.keepReading) { // 仅当不是主动断开时才报告错误
					this.ui.updateStatus(`读取错误: ${error.message}`, false, true);
					console.error('读取数据时出错:', error);
					await this.disconnect(); // 发生错误时断开连接
				}
			} finally {
				if (this.reader) { // 确保 reader 存在再释放
					this.reader.releaseLock();
					this.reader = null;
				}
			}
			if (!this.keepReading) { // 如果外部请求停止读取，则退出最外层循环
				break;
			}
		}
	}

	processIncomingData(newDataChunk) {
		const newBuffer = new Uint8Array(this.incomingBuffer.length + newDataChunk.length);
		newBuffer.set(this.incomingBuffer);
		newBuffer.set(newDataChunk, this.incomingBuffer.length);
		this.incomingBuffer = newBuffer;

		while (this.incomingBuffer.length >= this.expectedPacketLength) {
			const packetBytes = this.incomingBuffer.slice(0, this.expectedPacketLength);
			// 创建一个新的ArrayBuffer副本用于解析，避免直接操作共享的incomingBuffer的底层buffer
			const packetBuffer = packetBytes.buffer.slice(packetBytes.byteOffset, packetBytes.byteOffset + packetBytes.byteLength);

			this.incomingBuffer = this.incomingBuffer.slice(this.expectedPacketLength);

			const parsedData = RPMDataResolver.parse(packetBuffer);

			if (parsedData) {
				this.ui.updateRPM(parsedData);
				this.ui.incrementValidPackets();
			} else {
				this.ui.incrementInvalidPackets();
				console.warn('接收到无效数据包或解析失败（长度可能不为16字节）。');
			}
		}
	}

	simulateDataReceive() {
		const buffer = new ArrayBuffer(this.expectedPacketLength);
		const view = new DataView(buffer);

		const leftT = Math.random() * 1800 + 200;
		const rightT = Math.random() * 1800 + 200;
		const leftS = leftT * (0.95 + Math.random() * 0.1); // 目标RPM的95%-105%
		const rightS = rightT * (0.95 + Math.random() * 0.1);

		view.setFloat32(0, leftT, true);
		view.setFloat32(4, rightT, true);
		view.setFloat32(8, leftS, true);
		view.setFloat32(12, rightS, true);

		console.log(`模拟发送: LT=${leftT.toFixed(2)}, RT=${rightT.toFixed(2)}, LS=${leftS.toFixed(2)}, RS=${rightS.toFixed(2)}`);
		this.processIncomingData(new Uint8Array(buffer));
	}
}

// --- Main Application Logic ---
document.addEventListener('DOMContentLoaded', () => {
	const uiManager = new UIManager();
	const serialHandler = new SerialHandler(uiManager);

	uiManager.domElements.connectSerialBtn.addEventListener('click', async () => {
		if (serialHandler.port) { // 如果已连接，则断开
			await serialHandler.disconnect();
		} else { // 否则，尝试连接
			await serialHandler.connect();
		}
	});

	uiManager.domElements.sendTestDataBtn.addEventListener('click', () => {
		serialHandler.simulateDataReceive();
		if (serialHandler.port && serialHandler.keepReading) { // 如果已连接并正在读取，重置看门狗
			uiManager.resetWatchdog();
		} else if (!serialHandler.port) {
			uiManager.updateStatus("模拟数据已显示 (串口未连接)");
		}
	});

	uiManager.updateStatus('等待连接...');
	if (!('serial' in navigator)) {
		uiManager.updateStatus('Web Serial API 在此浏览器中不可用。', false, true);
		uiManager.domElements.connectSerialBtn.disabled = true;
	}
});

export { RPMDataResolver, SerialHandler, UIManager }; // 导出模块