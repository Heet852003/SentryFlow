# SentryFlow – Quality Assurance

QA processes for the Digital Twin Simulation & Validation platform.

---

## Test pyramid

1. **Unit tests**
   - **C firmware:** `make -C firmware test` (protocol, routing, self-test)
   - **Python protocol:** `pytest tools/tests/test_protocol.py`
   - **Python API:** `pytest tools/tests/test_api.py` (from repo root: `PYTHONPATH=. pytest tools/tests/test_api.py` or `python -m pytest tools/tests/ -v`)

2. **Integration / validation**
   - **Simulation engine validation:** `python tools/validate_simulation.py --host HOST --port PORT`  
     Ensures PING, ECHO, STATS, and latency target (< 100 ms).
   - **Traffic load:** `python tools/traffic_generator.py --requests 1500`  
     Confirms 1000+ requests/day scenario.
   - **Latency benchmark:** `python tools/latency_benchmark.py --requests 500`  
     Validates median/P95 latency.

3. **CI**
   - GitHub Actions (`.github/workflows/ci.yml`): build firmware, run firmware tests, run Python tests.
   - Jenkins (`Jenkinsfile`): same plus optional security/static checks.

---

## Running all tests

From repo root:

```bash
# Firmware
cd firmware && make test && cd ..

# Python (with venv and deps)
cd tools
python -m venv .venv
.venv/bin/activate   # Windows: .venv\Scripts\activate
pip install -r requirements.txt
python -m pytest tests/ -v
python ci_checks.py
cd ..

# API tests (need api and tools on path)
PYTHONPATH=. python -m pytest tools/tests/test_api.py -v
```

---

## Validation checklist (release)

- [ ] Firmware builds and `make test` passes  
- [ ] All `pytest tools/tests/` pass  
- [ ] `validate_simulation.py` passes against a running engine  
- [ ] Latency benchmark shows median < 100 ms under normal load  
- [ ] Traffic generator completes 1500+ requests without errors  
- [ ] Docker Compose stack starts and health endpoint returns OK  
- [ ] Frontend builds: `cd frontend && npm run build`  
- [ ] Technical documentation (README, API.md, TUTORIAL.md, QA.md) reviewed  

---

## Performance targets

| Metric              | Target        |
|---------------------|---------------|
| Median latency      | < 100 ms      |
| Supported load      | 1000+ users/requests |
| Engine availability| Health check passing  |

These are validated by the Python automation scripts and the in-app Simulation and Validation flows.
