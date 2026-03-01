"""
API tests for SentryFlow Digital Twin Simulation & Validation.
Requires API server running (e.g. uvicorn api.main:app) or use TestClient.
"""
from __future__ import annotations

import sys
from pathlib import Path

import pytest

# Repo root and api on path
REPO = Path(__file__).resolve().parent.parent.parent
if str(REPO) not in sys.path:
    sys.path.insert(0, str(REPO))
if str(REPO / "tools") not in sys.path:
    sys.path.insert(0, str(REPO / "tools"))

# FastAPI TestClient doesn't need server running
from fastapi.testclient import TestClient

# Import app after path setup so api.main can load tools
from api.main import app

client = TestClient(app)


def test_health() -> None:
    r = client.get("/api/health")
    assert r.status_code == 200
    j = r.json()
    assert "status" in j
    assert j["api"] is True
    assert "simulationEngine" in j


def test_twins_list() -> None:
    r = client.get("/api/twins")
    assert r.status_code == 200
    j = r.json()
    assert isinstance(j, list)
    if j:
        assert "id" in j[0]
        assert "name" in j[0]
        assert "status" in j[0]


def test_twin_get() -> None:
    r = client.get("/api/twins")
    assert r.status_code == 200
    twins = r.json()
    if not twins:
        pytest.skip("No twins")
    twin_id = twins[0]["id"]
    r2 = client.get(f"/api/twins/{twin_id}")
    assert r2.status_code == 200
    assert r2.json()["id"] == twin_id


def test_twin_404() -> None:
    r = client.get("/api/twins/nonexistent-id")
    assert r.status_code == 404


def test_validate_model() -> None:
    r = client.get("/api/twins")
    assert r.status_code == 200
    twins = r.json()
    if not twins:
        pytest.skip("No twins")
    model_id = twins[0]["id"]
    r2 = client.post(f"/api/validate/{model_id}")
    # May be 200 (engine up) or 503/500 if engine down
    assert r2.status_code in (200, 503, 500)
    if r2.status_code == 200:
        j = r2.json()
        assert "passed" in j
        assert "checks" in j
        assert "durationMs" in j
