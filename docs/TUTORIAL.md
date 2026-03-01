# SentryFlow Tutorial – Digital Twin Simulation & Validation

This tutorial walks you through running the full stack and using the UI and API.

## Prerequisites

- **Docker & Docker Compose** (recommended), or:
  - Node 18+ (frontend)
  - Python 3.10+ (API and tools)
  - C compiler (gcc/clang) and make (firmware)
  - Java 17+ (validation service, optional)
  - Maven (for building Java service)

---

## Option A: Run with Docker (recommended)

1. **Clone and start all services**

   ```bash
   cd SentryFlow-2
   docker-compose up -d
   ```

   This starts:

   - **engine** (C++) – port 9000  
   - **validation** (Java) – port 8080  
   - **api** (Python FastAPI) – port 8000  
   - **frontend** (React, served by nginx) – port 3000  

2. **Open the app**

   - Browser: [http://localhost:3000](http://localhost:3000)  
   - The dashboard shows system health and engine metrics.  
   - Use **Digital Twin Models** to see models, **Validation** to run checks, **Simulation** to run load tests.

3. **Stop**

   ```bash
   docker-compose down
   ```

---

## Option B: Run locally (development)

1. **Build and run the C++ engine**

   ```bash
   cd firmware
   make && ./build/bin/sentryflow_firmware --bind 0.0.0.0 --port 9000
   ```

   Leave this terminal open.

2. **Install and run the API**

   ```bash
   pip install -r api/requirements.txt
   export PYTHONPATH=$PWD:$PWD/tools
   python -m uvicorn api.main:app --host 0.0.0.0 --port 8000
   ```

3. **Install and run the frontend**

   ```bash
   cd frontend
   npm install && npm run dev
   ```

   Open [http://localhost:3000](http://localhost:3000). The Vite dev server proxies `/api` to the API.

4. **(Optional) Java validation service**

   ```bash
   cd simulation-engine
   mvn package -DskipTests
   java -jar target/simulation-engine-2025.1.0.jar
   ```

   Uses port 8080 by default.

---

## Using the UI

- **Dashboard:** Health and engine metrics refresh periodically.  
- **Digital Twin Models:** List models; use **Validate** or **Simulate** per model.  
- **Validation:** Select a model and click **Run validation** to execute checks.  
- **Simulation:** Select a model, set request count (e.g. 1000), and run. View median/P95 latency.  
- **Documentation:** In-app overview and links to API/QA.

---

## Using the API (curl)

```bash
# Health
curl http://localhost:8000/api/health

# List twins
curl http://localhost:8000/api/twins

# Run validation (replace MODEL_ID)
curl -X POST http://localhost:8000/api/validate/MODEL_ID

# Run simulation (1000 requests)
curl -X POST "http://localhost:8000/api/simulate/MODEL_ID?requests=1000"
```

---

## Python automation

From the repo root:

```bash
# Validation script (engine must be running)
python tools/validate_simulation.py --host 127.0.0.1 --port 9000

# Traffic generator (1000+ requests)
python tools/traffic_generator.py --host 127.0.0.1 --port 9000 --requests 1500

# Latency benchmark (< 100 ms target)
python tools/latency_benchmark.py --host 127.0.0.1 --port 9000 --requests 500
```

---

## Scaling (1000+ users)

- The frontend is static (React build) and can be served by any CDN or nginx.  
- The API is stateless; run multiple replicas behind a load balancer.  
- The C++ engine is a single process; for higher throughput, run multiple engine instances and load-balance by port or use multiple API replicas each pointing to different engines.  
- Use Docker Compose scaling: `docker-compose up -d --scale api=3` (and configure a reverse proxy to api).
