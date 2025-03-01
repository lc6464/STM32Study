import serial
from serial.tools import list_ports
from PIL import Image, ImageOps
import sys

def select_serial_port():
    # 尝试自动检测STM32虚拟串口
    stm32_ports = [port for port in list_ports.comports() if 'USB VID:PID=0483:5740' in port.hwid]
    if len(stm32_ports) == 1:
        print(f"检测到STM32虚拟串口：{stm32_ports[0].device}")
        return stm32_ports[0].device

    # 交互式选择串口
    ports = list_ports.comports()
    print("\n可用串口列表：")
    for i, port in enumerate(ports):
        print(f"{i+1}. {port.device} ({port.description})")
    while True:
        try:
            choice = int(input("请选择串口序号：")) - 1
            return ports[choice].device
        except (ValueError, IndexError):
            print("无效输入，请重新选择")

def process_image(image_path):
    # 图像处理流水线
    img = Image.open(image_path)
    img = ImageOps.fit(img, (128, 64), method=Image.Resampling.LANCZOS)
    img = img.convert('1', dither=Image.Dither.FLOYDSTEINBERG)  # 直接转换为1位模式

    # 生成显示缓冲区
    display_buffer = bytearray(1024)
    for page in range(8):
        for col in range(128):
            byte = 0
            for bit in range(8):
                y = page * 8 + bit
                if img.getpixel((col, y)):
                    byte |= (1 << bit)
            display_buffer[page * 128 + col] = byte
    return display_buffer

def create_data_packets(buffer):
    # 数据包分块（最大57字节/包）
    packets = []
    index = 0
    while index < 1024:
        chunk_size = min(57, 1024 - index)
        packet = {
            'start': index,
            'data': buffer[index:index+chunk_size],
            'retries': 0
        }
        packets.append(packet)
        index += chunk_size
    return packets

def send_command(ser, command, data=b'', start=0, read_length=0):
    # 构建命令包
    packet = bytearray(b'C')
    packet.append(ord(command))

    if command == "W":
        packet.extend(start.to_bytes(2, 'little'))
        packet.append(len(data))
        packet.extend(data)

    if command == "R":
        packet.extend(start.to_bytes(2, 'little'))
        packet.append(read_length)

    packet.extend(b'ED')
    ser.write(packet)
    return packet

def verify_data(ser, packet):
    # 发送读取请求
    send_command(ser, 'R', start=packet['start'], read_length=len(packet['data']))

    # 读取返回数据
    expected_len = len(packet['data'])
    received = ser.read(expected_len)

    # 验证数据
    if len(received) != expected_len:
        print(f"❌ 校验失败：索引{packet['start']}-{packet['start']+expected_len}，未收到响应")
        return False
    if received != packet['data']:
        print(f"❌ 校验失败：索引{packet['start']}-{packet['start']+expected_len}，数据不一致")
        return False
    return True

def main():
    # 初始化串口
    port = select_serial_port()
    with serial.Serial(port, baudrate=256000, timeout=1) as ser:
        print(f"已连接串口：{port}")

        while True:
            # 读取并处理图像
            image_path = input("请输入图片路径：")
            display_buffer = process_image(image_path)

            # 创建数据包
            packets = create_data_packets(display_buffer)
            print(f"生成{len(packets)}个数据包，总大小{len(display_buffer)}字节")

            # 数据传输阶段
            MAX_RETRIES = 3
            for packet in packets:
                success = False
                while not success and packet['retries'] <= MAX_RETRIES:
                    # 发送数据包
                    send_command(ser, 'W', data=packet['data'], start=packet['start'])
                    print(f"📤 发送：包起始{packet['start']}，长度{len(packet['data'])}")

                    # 验证数据
                    if verify_data(ser, packet):
                        success = True
                        print(f"✅ 验证通过：包起始{packet['start']}")
                    else:
                        packet['retries'] += 1
                        print(f"🔄 重试：包起始{packet['start']}，第{packet['retries']}次")

                if not success:
                    print(f"❌ 错误：包起始{packet['start']}超过最大重试次数")
                    sys.exit(1)

            # 发送刷新命令
            send_command(ser, 'U')
            print("🔄 已发送屏幕刷新命令")

if __name__ == "__main__":
    main()