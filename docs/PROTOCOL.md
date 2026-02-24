## SentryFlow Framed Protocol v1

SentryFlow uses a **binary framed protocol** over TCP. The goal is to demonstrate:

- Framing over a stream transport
- Versioning and forward compatibility
- Payload integrity via CRC32
- Message types representing embedded networking workflows

### Frame header (20 bytes)

All multi-byte values are **big-endian**.

| Field | Size | Description |
|---|---:|---|
| `magic` | 4 | `0x53464C57` (`'SFLW'`) |
| `version` | 1 | `1` |
| `type` | 1 | Message type (`SF_MSG_*`) |
| `flags` | 2 | Bit flags |
| `seq` | 4 | Sequence id (echoed back in responses) |
| `payload_len` | 4 | Payload length in bytes |
| `payload_crc32` | 4 | CRC32 of payload bytes |

### Payload

Payload interpretation depends on message type.

### Message types (selected)

- `PING` → `PONG`: payload is opaque bytes, echoed back
- `ECHO` → `ECHO_REPLY`: payload is opaque bytes, echoed back
- `GET_STATS` → `STATS_REPLY`: binary stats payload (see below)
- `ROUTE_UPDATE` → `ROUTE_ACK`: installs routes into the routing table
- `ROUTE_LOOKUP` → `ROUTE_REPLY`: returns best next hop for a destination IP

### `STATS_REPLY` payload (40 bytes)

| Field | Size |
|---|---:|
| `total_requests` | 8 |
| `bad_frames` | 8 |
| `routes_installed` | 8 |
| `uptime_ms` | 8 |
| `last_latency_us` | 4 |
| `avg_latency_us` | 4 |

### `ROUTE_UPDATE` payload

Payload is a concatenation of **16-byte route records**:

- `prefix_be` (4)
- `mask_bits` (1)
- `reserved` (1)
- `metric_be` (2)
- `next_hop_be` (4)
- `reserved` (4)

### `ROUTE_REPLY` payload (8 bytes)

- `mask_bits` (1)
- `reserved` (1)
- `metric_be` (2)
- `next_hop_be` (4)

