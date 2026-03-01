# SentryFlow REST API

Digital Twin Simulation & Validation – API reference.

**Base URL:** `http://localhost:8000` (or your deployment host).

All responses are JSON. Timestamps are ISO 8601.

---

## Health

### `GET /api/health`

System health: API and C++ simulation engine connectivity.

**Response:**

```json
{
  "status": "ok",
  "api": true,
  "simulationEngine": true,
  "timestamp": "2025-02-28T12:00:00Z"
}
```

- `status`: `"ok"` | `"degraded"` | `"error"`
- `simulationEngine`: `true` if the C++ engine (TCP port 9000) is reachable.

---

## Engine metrics

### `GET /api/stats`

Live metrics from the C++ simulation engine.

**Response:** (503 if engine unavailable)

```json
{
  "totalRequests": 1500,
  "badFrames": 0,
  "routesInstalled": 4,
  "uptimeMs": 120000,
  "lastLatencyUs": 450,
  "avgLatencyUs": 380
}
```

---

## Digital Twin models

### `GET /api/twins`

List all digital twin models.

**Response:** Array of:

```json
{
  "id": "twin-network-1",
  "name": "Network Routing Twin",
  "description": "Digital twin for routing protocol simulation.",
  "version": "1.0.0",
  "status": "draft",
  "createdAt": "2025-01-15T10:00:00Z",
  "updatedAt": "2025-02-01T14:30:00Z"
}
```

### `GET /api/twins/{id}`

Get a single model. Returns 404 if not found.

---

## Validation

### `POST /api/validate/{model_id}`

Run validation checks on a digital twin model (engine reachability, stats, echo roundtrip).

**Response:**

```json
{
  "id": "uuid",
  "modelId": "twin-network-1",
  "passed": true,
  "checks": [
    { "name": "Simulation engine reachable", "passed": true, "message": null },
    { "name": "Engine stats readable", "passed": true, "message": null },
    { "name": "Echo roundtrip", "passed": true, "message": null }
  ],
  "durationMs": 45,
  "timestamp": "2025-02-28T12:00:00Z"
}
```

---

## Simulation

### `POST /api/simulate/{model_id}?requests=1000`

Run a simulation against the C++ engine (traffic burst). Default `requests=1000`.

**Response:**

```json
{
  "id": "run-uuid",
  "modelId": "twin-network-1",
  "status": "completed",
  "requestsTotal": 1000,
  "latencyMedianMs": 0.42,
  "latencyP95Ms": 1.2,
  "startedAt": "2025-02-28T12:00:00Z",
  "completedAt": "2025-02-28T12:00:05Z"
}
```

### `GET /api/simulate/run/{run_id}`

Get a simulation run by ID. 404 if not found.
