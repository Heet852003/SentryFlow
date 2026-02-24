import argparse
import socket
import time


def send_request(host: str, port: int, payload: str) -> float:
    start = time.time()
    with socket.create_connection((host, port), timeout=2.0) as s:
        s.sendall(payload.encode("utf-8"))
        _ = s.recv(1024)
    end = time.time()
    return (end - start) * 1000.0


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate synthetic TCP traffic against SentryFlow firmware."
    )
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9000)
    parser.add_argument("--requests", type=int, default=1500)
    args = parser.parse_args()

    latencies = []
    for i in range(args.requests):
        msg = f"req-{i}"
        try:
            lat = send_request(args.host, args.port, msg)
            latencies.append(lat)
        except OSError as exc:
            print(f"request {i} failed: {exc}")

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

