import argparse
import statistics
import socket
import time


def measure_latency(host: str, port: int, count: int) -> None:
    samples = []
    for i in range(count):
        start = time.time()
        try:
            with socket.create_connection((host, port), timeout=2.0) as s:
                s.sendall(f"ping-{i}".encode("utf-8"))
                _ = s.recv(1024)
        except OSError as exc:
            print(f"request {i} failed: {exc}")
            continue
        end = time.time()
        samples.append((end - start) * 1000.0)

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
        description="Measure end-to-end latency against SentryFlow firmware."
    )
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9000)
    parser.add_argument("--requests", type=int, default=500)
    args = parser.parse_args()

    measure_latency(args.host, args.port, args.requests)


if __name__ == "__main__":
    main()

