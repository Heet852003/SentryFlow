# SentryFlow – Digital Twin Simulation & Validation

**Python · React · TypeScript · Docker · C++ · Java · 2025**

SentryFlow is a **Digital Twin Simulation & Validation** platform for creating and validating digital twin models, running simulations against a high-performance engine, and managing deployment at scale (1000+ users).

---

## Highlights

- **Digital Twin models** – Create, validate, and deploy digital twin models with a React + TypeScript frontend (HTML5, JavaScript ES6).
- **Simulation engine** – C++ firmware and TCP binary protocol for low-latency simulation; Java validation service for model checks.
- **REST API** – Python (FastAPI) gateway for the frontend and automation, with health, stats, validation, and simulation endpoints.
- **Automation & QA** – Python scripts for automation testing, simulation engine validation, traffic generation (1000+ requests), and latency benchmarking (&lt; 100 ms target).
- **Docker** – Full stack deployment via Docker Compose: engine, API, validation service, and frontend.
- **Technical documentation** – Architecture, protocol, API reference, tutorials, and QA procedures.

---

## Repository layout

| Path | Description |
|------|-------------|
| **frontend/** | React + TypeScript + Vite – Digital Twin dashboard (models, validation, simulation monitor) |
| **api/** | Python FastAPI – REST API and gateway to C++ engine |
| **firmware/** | C/C++ simulation engine – TCP server, protocol framing, routing |
| **simulation-engine/** | Java validation service – HTTP API for model validation |
| **tools/** | Python automation – protocol client, traffic generator, latency benchmark, validation script, pytest |
| **docker/** | Dockerfiles (engine, API, frontend, validation) |
| **docs/** | ARCHITECTURE, PROTOCOL, ROUTING, API, TUTORIAL, QA |

---

## Quick start (Docker)

```bash
docker-compose up -d
```

Then open **http://localhost:3000** for the UI. API: **http://localhost:8000/api**.

---

## Quick start (local dev)

1. **Engine (C++)**  
   `cd firmware && make && ./build/bin/sentryflow_firmware --bind 0.0.0.0 --port 9000`

2. **API (Python)**  
   `pip install -r api/requirements.txt`  
   `PYTHONPATH=$PWD:$PWD/tools python -m uvicorn api.main:app --host 0.0.0.0 --port 8000`

3. **Frontend (React)**  
   `cd frontend && npm install && npm run dev`  
   Open http://localhost:3000

---

## Python tooling & automation

```bash
cd tools
python -m venv .venv
.venv\Scripts\activate   # or source .venv/bin/activate
pip install -r requirements.txt
```

| Script | Purpose |
|--------|---------|
| `traffic_generator.py` | Generate 1000+ requests to the simulation engine |
| `latency_benchmark.py` | Measure min/median/P95 latency (&lt; 100 ms target) |
| `validate_simulation.py` | Validate engine (PING, ECHO, STATS, latency) |
| `ci_checks.py` | CI: syntax checks + pytest |
| `pytest tests/` | Protocol and API tests |

---

## Tech stack (resume-aligned)

- **Frontend:** React, TypeScript, HTML5, JavaScript (ES6) – supports 1000+ users  
- **Backend / API:** Python (FastAPI)  
- **Simulation engine:** C++ (firmware), Java (validation service)  
- **Automation & testing:** Python (pytest, validation scripts)  
- **Deployment:** Docker, Docker Compose  
- **Documentation:** Technical docs, API reference, tutorials, QA processes  

---

## Documentation

- [Architecture](docs/ARCHITECTURE.md) – Firmware and stack layout  
- [Protocol](docs/PROTOCOL.md) – Binary frame format and message types  
- [Routing](docs/ROUTING.md) – Routing table and LPM  
- [API reference](docs/API.md) – REST endpoints  
- [Tutorial](docs/TUTORIAL.md) – Full stack and UI walkthrough  
- [QA](docs/QA.md) – Testing and release checklist  

---

## CI/CD

- **GitHub Actions** (`.github/workflows/ci.yml`) – Build firmware, run tests, Python checks  
- **Jenkins** (`Jenkinsfile`) – Pipeline for build, test, and security/static checks  

---

## License

See [LICENSE](LICENSE).
