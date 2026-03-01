"""
SentryFlow Digital Twin Simulation & Validation – REST API.
Gateway to C++ simulation engine; serves frontend and supports 1000+ users.
"""
from __future__ import annotations

import asyncio
import os
import sys
import time
import uuid
from pathlib import Path

# Allow importing from project tools (sentryflow_client, sentryflow_protocol)
REPO_ROOT = Path(__file__).resolve().parent.parent
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))
if str(REPO_ROOT / "tools") not in sys.path:
    sys.path.insert(0, str(REPO_ROOT / "tools"))

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel

# Lazy import after path setup
from sentryflow_client import (
    Msg,
    parse_stats,
    request_once,
)

# Configuration from environment
ENGINE_HOST = os.environ.get("SENTRYFLOW_ENGINE_HOST", "127.0.0.1")
ENGINE_PORT = int(os.environ.get("SENTRYFLOW_ENGINE_PORT", "9000"))
VALIDATION_SERVICE_URL = os.environ.get("SENTRYFLOW_VALIDATION_URL", "http://localhost:8080")

app = FastAPI(
    title="SentryFlow API",
    description="Digital Twin Simulation & Validation – REST API",
    version="2025.1.0",
)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


# In-memory store for demo (replace with DB in production)
TWINS: list[dict] = [
    {
        "id": "twin-network-1",
        "name": "Network Routing Twin",
        "description": "Digital twin for routing protocol simulation and validation.",
        "version": "1.0.0",
        "status": "validated",
        "createdAt": "2025-01-15T10:00:00Z",
        "updatedAt": "2025-02-01T14:30:00Z",
    },
    {
        "id": "twin-traffic-1",
        "name": "Traffic Flow Twin",
        "description": "High-load traffic simulation model (1000+ requests).",
        "version": "1.1.0",
        "status": "deployed",
        "createdAt": "2025-01-20T09:00:00Z",
        "updatedAt": "2025-02-10T11:00:00Z",
    },
    {
        "id": "twin-latency-1",
        "name": "Latency Validation Twin",
        "description": "Model for validating < 100ms response latency.",
        "version": "1.0.0",
        "status": "draft",
        "createdAt": "2025-02-01T08:00:00Z",
        "updatedAt": "2025-02-15T16:00:00Z",
    },
]

SIMULATION_RUNS: dict[str, dict] = {}


async def engine_reachable() -> bool:
    try:
        _, _ = await asyncio.wait_for(
            request_once(ENGINE_HOST, ENGINE_PORT, Msg.PING, b"", timeout_s=1.0),
            timeout=2.0,
        )
        return True
    except Exception:
        return False


async def get_engine_stats() -> dict | None:
    try:
        _, payload = await asyncio.wait_for(
            request_once(ENGINE_HOST, ENGINE_PORT, Msg.GET_STATS, b"", timeout_s=2.0),
            timeout=3.0,
        )
        s = parse_stats(payload)
        return {
            "totalRequests": s.total_requests,
            "badFrames": s.bad_frames,
            "routesInstalled": s.routes_installed,
            "uptimeMs": s.uptime_ms,
            "lastLatencyUs": s.last_latency_us,
            "avgLatencyUs": s.avg_latency_us,
        }
    except Exception:
        return None


# --- REST endpoints ---


@app.get("/api/health")
async def health():
    """System health: API and simulation engine (C++)."""
    engine_ok = await engine_reachable()
    status = "ok" if engine_ok else "degraded"
    return {
        "status": status,
        "api": True,
        "simulationEngine": engine_ok,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
    }


@app.get("/api/stats")
async def stats():
    """Engine metrics from C++ simulation engine."""
    data = await get_engine_stats()
    if data is None:
        raise HTTPException(status_code=503, detail="Simulation engine unavailable")
    return data


@app.get("/api/twins")
async def list_twins():
    """List digital twin models."""
    return TWINS


@app.get("/api/twins/{twin_id}")
async def get_twin(twin_id: str):
    """Get a single digital twin model."""
    for t in TWINS:
        if t["id"] == twin_id:
            return t
    raise HTTPException(status_code=404, detail="Twin not found")


class ValidationCheck(BaseModel):
    name: str
    passed: bool
    message: str | None = None


class ValidationResult(BaseModel):
    id: str
    modelId: str
    passed: bool
    checks: list[ValidationCheck]
    durationMs: int
    timestamp: str


@app.post("/api/validate/{model_id}", response_model=ValidationResult)
async def validate_model(model_id: str):
    """Run validation on a digital twin model (protocol + optional Java validation service)."""
    if not any(t["id"] == model_id for t in TWINS):
        raise HTTPException(status_code=404, detail="Model not found")
    start = time.perf_counter()
    checks: list[dict] = []
    # Check 1: Engine reachable
    engine_ok = await engine_reachable()
    checks.append({
        "name": "Simulation engine reachable",
        "passed": engine_ok,
        "message": None if engine_ok else "C++ engine not responding",
    })
    # Check 2: Stats readable
    stats_data = await get_engine_stats()
    checks.append({
        "name": "Engine stats readable",
        "passed": stats_data is not None,
        "message": None if stats_data else "Failed to read engine stats",
    })
    # Check 3: PING/PONG roundtrip
    try:
        _, pl = await request_once(ENGINE_HOST, ENGINE_PORT, Msg.ECHO, b"validate", timeout_s=2.0)
        echo_ok = pl == b"validate"
        checks.append({
            "name": "Echo roundtrip",
            "passed": echo_ok,
            "message": None if echo_ok else "Echo payload mismatch",
        })
    except Exception as e:
        checks.append({"name": "Echo roundtrip", "passed": False, "message": str(e)})
    duration_ms = int((time.perf_counter() - start) * 1000)
    passed = all(c["passed"] for c in checks)
    return ValidationResult(
        id=str(uuid.uuid4()),
        modelId=model_id,
        passed=passed,
        checks=[ValidationCheck(**c) for c in checks],
        durationMs=duration_ms,
        timestamp=time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
    )


class SimulationRun(BaseModel):
    id: str
    modelId: str
    status: str
    requestsTotal: int
    latencyMedianMs: float | None = None
    latencyP95Ms: float | None = None
    startedAt: str
    completedAt: str | None = None


@app.post("/api/simulate/{model_id}", response_model=SimulationRun)
async def run_simulation(model_id: str, requests: int = 1000):
    """Run a simulation against the C++ engine (traffic burst)."""
    if not any(t["id"] == model_id for t in TWINS):
        raise HTTPException(status_code=404, detail="Model not found")
    run_id = str(uuid.uuid4())
    started_at = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    SIMULATION_RUNS[run_id] = {
        "id": run_id,
        "modelId": model_id,
        "status": "running",
        "requestsTotal": requests,
        "latencyMedianMs": None,
        "latencyP95Ms": None,
        "startedAt": started_at,
        "completedAt": None,
    }
    # Run traffic in background and collect latencies
    latencies_us: list[float] = []

    async def do_requests(n: int):
        nonlocal latencies_us
        for i in range(min(n, 500)):  # Cap concurrent-style to avoid overload
            try:
                t0 = time.perf_counter()
                await request_once(ENGINE_HOST, ENGINE_PORT, Msg.PING, f"sim-{i}".encode(), timeout_s=5.0)
                latencies_us.append((time.perf_counter() - t0) * 1e6)
            except Exception:
                pass
        # If more requests, do in batches
        for _ in range(0, n - 500, 100):
            for j in range(100):
                try:
                    t0 = time.perf_counter()
                    await request_once(ENGINE_HOST, ENGINE_PORT, Msg.PING, b"x", timeout_s=2.0)
                    latencies_us.append((time.perf_counter() - t0) * 1e6)
                except Exception:
                    pass

    try:
        await do_requests(requests)
        latencies_ms = [x / 1000.0 for x in latencies_us]
        latencies_ms.sort()
        n = len(latencies_ms)
        median_ms = latencies_ms[n // 2] if n else None
        p95_ms = latencies_ms[int(n * 0.95)] if n else None
        SIMULATION_RUNS[run_id].update({
            "status": "completed",
            "latencyMedianMs": median_ms,
            "latencyP95Ms": p95_ms,
            "completedAt": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        })
    except Exception:
        SIMULATION_RUNS[run_id].update({
            "status": "failed",
            "completedAt": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        })
    return SimulationRun(**SIMULATION_RUNS[run_id])


@app.get("/api/simulate/run/{run_id}", response_model=SimulationRun)
async def get_simulation_run(run_id: str):
    if run_id not in SIMULATION_RUNS:
        raise HTTPException(status_code=404, detail="Run not found")
    return SimulationRun(**SIMULATION_RUNS[run_id])


# Mount static frontend if built (for production Docker)
FRONTEND_DIST = REPO_ROOT / "frontend" / "dist"
if FRONTEND_DIST.is_dir():
    app.mount("/", StaticFiles(directory=str(FRONTEND_DIST), html=True), name="frontend")
else:
    @app.get("/")
    def _root():
        return {"message": "SentryFlow API. Serve frontend from / or use proxy. Frontend not built."}


if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
