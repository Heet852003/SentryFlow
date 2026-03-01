#!/usr/bin/env python3
"""
SentryFlow – Simulation engine validation script.
Validates the C++ simulation engine performance and correctness for Digital Twin QA.
Run: python validate_simulation.py [--host HOST] [--port PORT]
"""
from __future__ import annotations

import argparse
import asyncio
import sys
import time
from pathlib import Path

# Ensure tools is on path when run from repo root
TOOLS = Path(__file__).resolve().parent
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

from sentryflow_client import Msg, parse_stats, request_once


async def check_ping(host: str, port: int) -> bool:
    try:
        _, _ = await asyncio.wait_for(
            request_once(host, port, Msg.PING, b"validate", timeout_s=2.0),
            timeout=3.0,
        )
        return True
    except Exception:
        return False


async def check_echo(host: str, port: int) -> bool:
    try:
        _, payload = await asyncio.wait_for(
            request_once(host, port, Msg.ECHO, b"echo-test", timeout_s=2.0),
            timeout=3.0,
        )
        return payload == b"echo-test"
    except Exception:
        return False


async def check_stats(host: str, port: int) -> tuple[bool, str]:
    try:
        _, payload = await asyncio.wait_for(
            request_once(host, port, Msg.GET_STATS, b"", timeout_s=2.0),
            timeout=3.0,
        )
        s = parse_stats(payload)
        return True, (
            f"requests={s.total_requests} uptime_ms={s.uptime_ms} avg_latency_us={s.avg_latency_us}"
        )
    except Exception as e:
        return False, str(e)


async def check_latency_target(host: str, port: int, samples: int = 20) -> tuple[bool, float]:
    latencies_ms = []
    for i in range(samples):
        try:
            t0 = time.perf_counter()
            await request_once(host, port, Msg.PING, f"lat-{i}".encode(), timeout_s=2.0)
            elapsed_ms = (time.perf_counter() - t0) * 1000
            latencies_ms.append(elapsed_ms)
        except Exception:
            pass
    if not latencies_ms:
        return False, 0.0
    avg = sum(latencies_ms) / len(latencies_ms)
    return avg < 100.0, avg


async def main() -> int:
    p = argparse.ArgumentParser(description="Validate SentryFlow simulation engine")
    p.add_argument("--host", default="127.0.0.1", help="Engine host")
    p.add_argument("--port", type=int, default=9000, help="Engine port")
    p.add_argument("--samples", type=int, default=20, help="Latency sample count")
    args = p.parse_args()

    host, port = args.host, args.port
    failed = 0

    print("SentryFlow simulation engine validation")
    print(f"Target: {host}:{port}")
    print()

    ok = await check_ping(host, port)
    print(f"  PING:    {'PASS' if ok else 'FAIL'}")
    if not ok:
        failed += 1

    ok = await check_echo(host, port)
    print(f"  ECHO:    {'PASS' if ok else 'FAIL'}")
    if not ok:
        failed += 1

    ok, msg = await check_stats(host, port)
    print(f"  STATS:   {'PASS' if ok else 'FAIL'} {msg}")
    if not ok:
        failed += 1

    ok, avg_ms = await check_latency_target(host, port, args.samples)
    print(f"  LATENCY: {'PASS' if ok else 'FAIL'} (avg={avg_ms:.2f} ms, target < 100 ms)")
    if not ok:
        failed += 1

    print()
    if failed:
        print(f"Validation failed ({failed} check(s) failed)")
        return 1
    print("All checks passed.")
    return 0


if __name__ == "__main__":
    sys.exit(asyncio.run(main()))
