import argparse
import asyncio
import time

from sentryflow_client import Msg, request_once


async def send_request(host: str, port: int, payload: bytes, seq: int) -> float:
    start = time.perf_counter()
    msg_type, _ = await request_once(host, port, Msg.ECHO, payload, seq=seq)
    end = time.perf_counter()
    if msg_type not in (Msg.ECHO_REPLY, Msg.ERROR):
        raise RuntimeError(f"unexpected response type: {msg_type}")
    return (end - start) * 1000.0


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate synthetic framed-protocol TCP traffic against SentryFlow firmware."
    )
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9000)
    parser.add_argument("--requests", type=int, default=1500)
    parser.add_argument("--concurrency", type=int, default=50)
    args = parser.parse_args()

    async def run() -> list[float]:
        sem = asyncio.Semaphore(max(1, args.concurrency))
        latencies: list[float] = []

        async def one(i: int) -> None:
            payload = f"req-{i}".encode("utf-8")
            async with sem:
                try:
                    lat = await send_request(args.host, args.port, payload, seq=i + 1)
                    latencies.append(lat)
                except Exception as exc:
                    print(f"request {i} failed: {exc}")

        await asyncio.gather(*(one(i) for i in range(args.requests)))
        return latencies

    latencies = asyncio.run(run())

    if not latencies:
        print("no successful requests")
        return

    latencies.sort()
    total = len(latencies)
    median = latencies[total // 2]
    p95 = latencies[int(total * 0.95) - 1]
    print(
        f"requests={total} median_ms={median:.2f} p95_ms={p95:.2f} "
        f"min_ms={latencies[0]:.2f} max_ms={latencies[-1]:.2f}"
    )


if __name__ == "__main__":
    main()

