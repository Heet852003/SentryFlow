import argparse
import asyncio
import statistics
import time

from sentryflow_client import Msg, request_once

async def measure_latency(host: str, port: int, count: int, concurrency: int) -> list[float]:
    sem = asyncio.Semaphore(max(1, concurrency))
    samples: list[float] = []

    async def one(i: int) -> None:
        payload = f"ping-{i}".encode("utf-8")
        async with sem:
            start = time.perf_counter()
            try:
                msg_type, _ = await request_once(host, port, Msg.PING, payload, seq=i + 1)
                if msg_type != Msg.PONG:
                    raise RuntimeError(f"expected PONG, got {msg_type}")
            except Exception as exc:
                print(f"request {i} failed: {exc}")
                return
            end = time.perf_counter()
            samples.append((end - start) * 1000.0)

    await asyncio.gather(*(one(i) for i in range(count)))
    return samples

def summarize(samples: list[float]) -> None:
    if not samples:
        print("no successful samples")
        return

    samples.sort()
    median = statistics.median(samples)
    p95 = samples[int(len(samples) * 0.95) - 1]
    print(
        f"count={len(samples)} "
        f"min_ms={min(samples):.2f} median_ms={median:.2f} p95_ms={p95:.2f} max_ms={max(samples):.2f}"
    )
    if median < 100.0:
        print("Target achieved: median latency < 100ms")
    else:
        print("Target NOT achieved: median latency >= 100ms")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Measure end-to-end latency (framed protocol) against SentryFlow firmware."
    )
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9000)
    parser.add_argument("--requests", type=int, default=500)
    parser.add_argument("--concurrency", type=int, default=10)
    args = parser.parse_args()

    samples = asyncio.run(measure_latency(args.host, args.port, args.requests, args.concurrency))
    summarize(samples)


if __name__ == "__main__":
    main()

