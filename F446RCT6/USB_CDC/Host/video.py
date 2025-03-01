import subprocess
import time

import serial
from PIL import Image
from serial.tools import list_ports


def select_serial_port():
    stm32_ports = [
        port for port in list_ports.comports() if "USB VID:PID=0483:5740" in port.hwid
    ]
    if len(stm32_ports) == 1:
        print(f"检测到STM32虚拟串口：{stm32_ports[0].device}")
        return stm32_ports[0].device

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


def process_frame(raw_frame):
    img = Image.frombytes("L", (128, 64), raw_frame)
    img = img.convert("1", dither=Image.Dither.FLOYDSTEINBERG)
    # img = img.convert("1")

    display_buffer = bytearray(1024)
    for page in range(8):
        for col in range(128):
            byte = 0
            for bit in range(8):
                y = page * 8 + bit
                if img.getpixel((col, y)):
                    byte |= 1 << bit
            display_buffer[page * 128 + col] = byte
    return display_buffer


def create_data_packets(buffer):
    packets = []
    index = 0
    while index < 1024:
        chunk_size = min(57, 1024 - index)
        packets.append({"start": index, "data": buffer[index : index + chunk_size]})
        index += chunk_size
    return packets


def send_command(ser, command, data=b"", start=0):
    packet = bytearray(b"C")
    packet.append(ord(command))
    if command == "W":
        packet.extend(start.to_bytes(2, "little"))
        packet.append(len(data))
        packet.extend(data)
    packet.extend(b"ED")
    ser.write(packet)


def main():
    port = select_serial_port()
    with serial.Serial(port, baudrate=3000000, timeout=0.1) as ser:
        print(f"已连接串口：{port}")

        while True:
            need_exit = False

            video_path = input("请输入视频路径：")
            cmd = [
                "ffmpeg",
                "-hwaccel",
                "cuda",
                "-i",
                video_path,
                "-vf",
                "fps=30,scale=128:64:flags=lanczos",
                "-pix_fmt",
                "gray",
                "-f",
                "rawvideo",
                "pipe:1",
            ]

            ffmpeg_process = subprocess.Popen(
                cmd, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL
            )
            frame_interval = 1 / 30
            next_frame_time = time.monotonic()

            # 初始化帧率计数器
            fps_start = time.monotonic()
            frame_count = 0

            try:
                while True:
                    raw_frame = ffmpeg_process.stdout.read(128 * 64)
                    if not raw_frame or len(raw_frame) != 128 * 64:
                        break

                    display_buffer = process_frame(raw_frame)

                    # 发送数据包
                    for packet in create_data_packets(display_buffer):
                        send_command(
                            ser, "W", data=packet["data"], start=packet["start"]
                        )

                    # 触发屏幕刷新
                    send_command(ser, "U")

                    # 帧率统计
                    current_time = time.monotonic()
                    frame_count += 1

                    # 每秒更新一次帧率显示
                    if current_time - fps_start >= 1.0:
                        fps = frame_count / (current_time - fps_start)
                        print(f"\r当前帧率: {fps:.2f}fps", end="", flush=True)
                        frame_count = 0
                        fps_start = current_time

                    # 精确帧率控制
                    sleep_time = next_frame_time - current_time
                    if sleep_time > 0:
                        time.sleep(sleep_time)
                    next_frame_time += frame_interval

            except KeyboardInterrupt:
                print("\n传输中断")  # 添加换行保证提示正常显示
                need_exit = True
            finally:
                # 结束前换行保证控制台显示正常

                ffmpeg_process.kill()
                ffmpeg_process.wait()

            print()
            time.sleep(0.1)
            send_command(ser, "C")
            time.sleep(0.01)
            send_command(ser, "U")

            if need_exit:
                return


if __name__ == "__main__":
    main()
