## Routing Model

SentryFlow includes a lightweight routing table to demonstrate routing protocol concepts in an embedded-friendly way.

### Routing table

- **IPv4 longest-prefix match** (LPM)
- Tie-breaker: **lower metric wins** when prefix length matches
- Capacity: `SF_ROUTE_TABLE_MAX` (default 256)

### Installing routes

Routes can be installed:

- At startup via firmware CLI `--route <prefix> <maskBits> <nextHop> <metric>`
- At runtime via protocol message `ROUTE_UPDATE`

### Lookup

Route lookup can be performed:

- Internally by firmware when deciding how to handle a peer
- Externally via `ROUTE_LOOKUP` protocol message (used by Python tooling)

### What this demonstrates

- A routing table data structure that is simple enough for embedded targets
- A protocol-based mechanism for distributing updates (simulating a routing protocol)
- Deterministic behavior (LPM + metric) suitable for CI regression tests

