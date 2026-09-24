"""Send Ogg music files to the watch SD card over its USB serial port."""

import argparse
import re
import struct
import sys
import time
import zlib
from pathlib import Path

import serial


MAGIC = b"WSD1"
REPLY_MAGIC = b"\xa5\x5a"
CHUNK_SIZE = 512
MAX_FILE_SIZE = 20 * 1024 * 1024


def prepare(path: Path) -> tuple[bytes, int, int]:
    name = path.name.encode("ascii")
    if len(name) > 63 or not re.fullmatch(rb"[A-Za-z0-9_-][A-Za-z0-9_.-]*\.ogg", name):
        raise ValueError(f"文件名须为不超过 63 字节的英文 .ogg 文件: {path.name}")
    size = path.stat().st_size
    if not 0 < size <= MAX_FILE_SIZE:
        raise ValueError(f"文件大小超出范围: {path}")
    crc = 0
    with path.open("rb") as source:
        if source.read(4) != b"OggS":
            raise ValueError(f"文件不是 Ogg 容器: {path}")
        source.seek(0)
        for chunk in iter(lambda: source.read(64 * 1024), b""):
            crc = zlib.crc32(chunk, crc)
    return name, size, crc


def await_reply(port: serial.Serial, timeout: float, label: str) -> None:
    deadline = time.monotonic() + timeout
    matched = 0
    while time.monotonic() < deadline:
        byte = port.read(1)
        if not byte:
            continue
        value = byte[0]
        if matched < len(REPLY_MAGIC):
            matched = matched + 1 if value == REPLY_MAGIC[matched] else int(value == REPLY_MAGIC[0])
            continue
        if value == 0x06:
            return
        if value == 0x15:
            raise RuntimeError(f"手表拒绝{label}；检查是否已有同名文件或 SD 卡写入失败")
        matched = int(value == REPLY_MAGIC[0])
    raise TimeoutError(f"等待手表确认{label}超时；检查 COM 端口、固件和 SD 卡")


def send_file(port: serial.Serial, path: Path) -> None:
    name, size, crc = prepare(path)
    print(f"开始传输 {path.name}，{size:,} 字节，CRC32 {crc:08x}", flush=True)
    port.write(MAGIC + struct.pack("<BII", len(name), size, crc) + name)
    await_reply(port, 15, "文件头")
    sent = 0
    next_report = 10
    with path.open("rb") as source:
        while chunk := source.read(CHUNK_SIZE):
            written = port.write(chunk)
            if written != len(chunk):
                raise OSError(f"串口只写入 {written}/{len(chunk)} 字节")
            await_reply(port, 15, "数据块")
            sent += len(chunk)
            percent = sent * 100 // size
            if percent >= next_report:
                print(f"  {percent}%", flush=True)
                next_report = percent + 10
    await_reply(port, 30, "最终校验")
    print(f"完成：{path.name}", flush=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("port", help="手表的串口，例如 COM7")
    parser.add_argument("files", nargs="+", type=Path, help="要传入 SD 根目录的 Ogg 文件")
    args = parser.parse_args()
    for path in args.files:
        prepare(path)
    with serial.Serial(port=args.port, baudrate=115200, timeout=0.2, write_timeout=10) as port:
        port.dtr = False
        port.rts = False
        time.sleep(2)
        port.reset_input_buffer()
        for path in args.files:
            send_file(port, path)


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, RuntimeError, TimeoutError, serial.SerialException) as exc:
        print(f"传输失败：{exc}", file=sys.stderr)
        sys.exit(1)
