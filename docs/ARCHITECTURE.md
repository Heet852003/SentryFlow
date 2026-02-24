## SentryFlow Architecture (Embedded Networking Focus)

SentryFlow is organized to resemble a **firmware-style networking component** running on embedded Linux:

- `firmware/`: C/C++ implementation of a small protocol stack + routing table + Linux platform layer
- `tools/`: Python tooling for traffic generation, latency benchmarking, and protocol-level interaction
- CI: `.github/workflows/ci.yml` and `Jenkinsfile` build, test, and statically analyze the firmware and tools

### Firmware layering

- **Protocol framing (`sf_protocol.*`)**
  - Binary frame header with magic/version/type/flags/seq/payload_len/payload_crc32
  - Streaming decode via `sf_rxbuf_t` to support partial TCP reads
- **Command handling (`platform_linux.c`)**
  - Parses frames and dispatches to message handlers (PING/ECHO/GET_STATS/ROUTE_UPDATE/ROUTE_LOOKUP)
- **Routing (`routing_table.*`, `routing.*`)**
  - Longest-prefix match for IPv4 routes
  - Route updates delivered via a dedicated message type
- **HAL (`hal_linux.c`)**
  - Provides platform telemetry (uptime/monotonic time/pid) via a stable interface
- **Platform (`platform_linux.c`)**
  - Non-blocking sockets + `epoll` event loop
  - Incremental read, frame parsing, response queueing, incremental write

### Why this structure

It mirrors typical embedded firmware constraints and design:

- Separation of platform-independent logic (protocol, routing) from platform-dependent I/O
- Testable modules (framing + routing table) with a self-test entrypoint
- Tooling that speaks the same protocol for repeatable load/latency runs and CI regression checks

