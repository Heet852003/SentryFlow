from __future__ import annotations

import struct
import zlib
from dataclasses import dataclass


MAGIC = 0x53464C57  # 'SFLW'
VERSION = 1
HEADER_FMT = "!IBBHIII"  # (magic,u8,u8,u16,u32,u32,u32)
HEADER_SIZE = 20


@dataclass(frozen=True)
class Frame:
    version: int
    msg_type: int
    flags: int
    seq: int
    payload: bytes


def crc32(data: bytes) -> int:
    return zlib.crc32(data) & 0xFFFFFFFF


def encode_frame(msg_type: int, payload: bytes = b"", *, seq: int = 0, flags: int = 0) -> bytes:
    if not isinstance(payload, (bytes, bytearray)):
        raise TypeError("payload must be bytes")
    payload = bytes(payload)
    header = struct.pack(HEADER_FMT, MAGIC, VERSION, msg_type & 0xFF, flags & 0xFFFF, seq & 0xFFFFFFFF, len(payload), crc32(payload))
    return header + payload


def decode_frame(data: bytes) -> Frame:
    if len(data) < HEADER_SIZE:
        raise ValueError("insufficient data for header")
    magic, version, msg_type, flags, seq, payload_len, payload_crc = struct.unpack(HEADER_FMT, data[:HEADER_SIZE])
    if magic != MAGIC:
        raise ValueError("bad magic")
    if version != VERSION:
        raise ValueError("bad version")
    if len(data) != HEADER_SIZE + payload_len:
        raise ValueError("length mismatch")
    payload = data[HEADER_SIZE:]
    if crc32(payload) != payload_crc:
        raise ValueError("crc mismatch")
    return Frame(version=version, msg_type=msg_type, flags=flags, seq=seq, payload=payload)

