from __future__ import annotations

import asyncio
import contextlib
import struct
from dataclasses import dataclass
from typing import Optional

from sentryflow_protocol import HEADER_FMT, HEADER_SIZE, decode_frame, encode_frame


class Msg:
    PING = 1
    PONG = 2
    ECHO = 3
    ECHO_REPLY = 4
    GET_STATS = 5
    STATS_REPLY = 6
    ROUTE_UPDATE = 7
    ROUTE_ACK = 8
    ROUTE_LOOKUP = 9
    ROUTE_REPLY = 10
    ERROR = 255


@dataclass(frozen=True)
class Stats:
    total_requests: int
    bad_frames: int
    routes_installed: int
    uptime_ms: int
    last_latency_us: int
    avg_latency_us: int


async def read_exactly(reader: asyncio.StreamReader, n: int) -> bytes:
    data = await reader.readexactly(n)
    if len(data) != n:
        raise ConnectionError("unexpected EOF")
    return data


async def request_once(
    host: str,
    port: int,
    msg_type: int,
    payload: bytes = b"",
    *,
    seq: int = 1,
    timeout_s: float = 2.0,
) -> tuple[int, bytes]:
    reader, writer = await asyncio.wait_for(asyncio.open_connection(host, port), timeout=timeout_s)
    try:
        writer.write(encode_frame(msg_type, payload, seq=seq))
        await writer.drain()

        header = await asyncio.wait_for(read_exactly(reader, HEADER_SIZE), timeout=timeout_s)
        # Peek payload_len from header: magic,u8,u8,u16,u32, payload_len(u32), crc(u32)
        payload_len = struct.unpack(HEADER_FMT, header)[5]
        payload_bytes = await asyncio.wait_for(read_exactly(reader, payload_len), timeout=timeout_s)
        frame = decode_frame(header + payload_bytes)
        return frame.msg_type, frame.payload
    finally:
        writer.close()
        with contextlib.suppress(Exception):
            await writer.wait_closed()


def parse_stats(payload: bytes) -> Stats:
    if len(payload) != 40:
        raise ValueError("bad stats payload length")
    total, bad, routes, uptime = struct.unpack("!QQQQ", payload[:32])
    last_us, avg_us = struct.unpack("!II", payload[32:])
    return Stats(
        total_requests=total,
        bad_frames=bad,
        routes_installed=routes,
        uptime_ms=uptime,
        last_latency_us=last_us,
        avg_latency_us=avg_us,
    )


def encode_route_entries(entries: list[tuple[str, int, str, int]]) -> bytes:
    """
    entries: list of (prefix_ip, mask_bits, next_hop_ip, metric)
    Layout per entry (16 bytes):
      prefix(u32_be), mask(u8), reserved(u8=0), metric(u16_be), next_hop(u32_be), reserved(u32=0)
    """
    import ipaddress

    out = bytearray()
    for prefix, mask_bits, next_hop, metric in entries:
        p = int(ipaddress.IPv4Address(prefix))
        nh = int(ipaddress.IPv4Address(next_hop))
        out += struct.pack("!IBBHII", p, mask_bits & 0xFF, 0, metric & 0xFFFF, nh, 0)
    return bytes(out)


def encode_route_lookup(ip: str) -> bytes:
    import ipaddress

    return struct.pack("!I", int(ipaddress.IPv4Address(ip)))


def parse_route_reply(payload: bytes) -> Optional[tuple[int, int, str]]:
    if len(payload) != 8:
        raise ValueError("bad route reply length")
    mask_bits = payload[0]
    metric = struct.unpack("!H", payload[2:4])[0]
    next_hop_int = struct.unpack("!I", payload[4:8])[0]
    if metric == 0xFFFF or next_hop_int == 0:
        return None
    import ipaddress

    return mask_bits, metric, str(ipaddress.IPv4Address(next_hop_int))

