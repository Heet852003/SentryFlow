## SentryFlow – Network Protocol & Embedded Systems Platform | C, Python, Linux, Jenkins, GitHub (W 2025)

SentryFlow is a **network protocol and embedded systems experimentation platform** focused on:

- **TCP/IP networking on embedded Linux using C/C++**
- **Routing protocol experimentation**
- **Firmware and low-level system software development**
- **Python/Bash automation and DevSecOps pipelines (Jenkins + GitHub Actions)**

This repository is structured specifically to back up the following experience description:

- Engineered network protocol stack using C and C++ on embedded Linux, implementing TCP/IP protocols and routing protocols, building CI/CD pipelines with Jenkins and GitHub Actions, processing 1000+ requests/day.
- Developed firmware components and low-level system software, implemented automation tools using Python and Bash scripting, integrated DevSecOps practices, achieving < 100ms response latency.
- Built hardware-software integration layer, managed development, testing, and deployment workflows across Linux-based embedded environments.

The repository is intentionally structured to demonstrate end‑to‑end skills across firmware, tooling, CI/CD, and secure automation rather than to be a full production product.

### High‑Level Highlights

- **Engineered network protocol stack** using C and C++ on embedded Linux, implementing TCP/IP–based services and pluggable routing logic.
- **CI/CD pipelines** using Jenkins and GitHub Actions to build, test, and lint the C firmware and Python automation tools, targeting **1000+ requests/day** workloads.
- **Firmware and low‑level system software** components providing a hardware–software abstraction layer and basic telemetry.
- **Automation tooling** in Python and Bash to run load tests, generate traffic, and collect latency metrics, targeting **\< 100ms response latency** under normal load.
- **DevSecOps integration**, including basic security checks (static checks, dependency scans) wired into CI.

---

## Repository Layout

- `firmware/` – C/C++ network stack and embedded Linux entrypoints  
  - `include/` – public headers (`protocol_stack.h`, `routing.h`, `platform.h`)  
  - `src/` – implementations (simple TCP/IP server, routing strategy stubs, platform layer for Linux)  
  - `tests/` – unit-style tests and basic harnesses  
  - `Makefile` – builds firmware binaries and tests on Linux/WSL
- `tools/` – Python automation tools and benchmarking scripts  
  - `traffic_generator.py` – generates synthetic TCP traffic (1000+ requests/day scenario)  
  - `latency_benchmark.py` – measures end‑to‑end latency, aiming for \< 100ms median  
  - `ci_checks.py` – lightweight DevSecOps helpers (lint, simple config checks)
- `scripts/` – Bash scripts for running firmware, tests, and benchmarks locally
- `Jenkinsfile` – Jenkins pipeline definition (build + test + security checks)
- `.github/workflows/ci.yml` – GitHub Actions CI definition

Legacy web/API components from the previous version of SentryFlow have been removed or deprecated; the focus is now fully on **networking + embedded systems**.

---

## Getting Started

### Prerequisites

- A Linux environment (native, WSL2, or container)
- `gcc` or `clang` toolchain for C/C++
- Python 3.10+
- `make`

Optional (for CI/CD demo):

- Jenkins with access to this repository
- GitHub repository with Actions enabled

### Build the Firmware

```bash
cd firmware
make            # builds the main firmware binary and tests
```

This will produce a binary such as `build/sentryflow_firmware` that runs a simple TCP/IP service and hooks into the routing/platform layers.

### Run the Firmware Locally

```bash
cd firmware
make run        # or ./build/sentryflow_firmware
```

By default, the firmware listens on a configurable TCP port (see `firmware/include/protocol_stack.h`), accepts simple text requests, and echoes structured responses with timestamps to simulate on‑device request handling.

---

## Python Tooling & Automation

### Install Python Dependencies

```bash
cd tools
python -m venv .venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate
pip install -r requirements.txt
```

### Generate Traffic (1000+ Requests/Day Simulation)

```bash
cd tools
python traffic_generator.py --host 127.0.0.1 --port 9000 --requests 1500
```

This script opens concurrent TCP connections to the firmware service and drives a workload that can be scaled well beyond 1000 requests/day for stress testing.

### Measure Latency (\< 100ms Target)

```bash
cd tools
python latency_benchmark.py --host 127.0.0.1 --port 9000 --requests 500
```

The benchmark script records per‑request latency and prints summary statistics (min/median/p95). For local runs on a reasonable machine, the goal is to keep median latency under 100ms.

---

## CI/CD & DevSecOps

### Jenkins

The `Jenkinsfile` defines a declarative pipeline with stages such as:

- **Checkout** – fetch repository sources
- **Build Firmware** – `make -C firmware`
- **Run Tests** – `make -C firmware test`
- **Python Tooling Checks** – `pip install -r tools/requirements.txt` + run `ci_checks.py`
- **Security/Static Checks** – example hooks for `cppcheck`, `clang-tidy`, or dependency scanning

This demonstrates CI/CD integration for C firmware and Python tooling on an embedded‑style codebase.

### GitHub Actions

The workflow at `.github/workflows/ci.yml` mirrors the Jenkins stages for GitHub‑native CI:

- Triggered on pushes and pull requests to `main`
- Builds and tests C firmware
- Runs Python checks and basic security checks

---

## Hardware–Software Integration Layer

The `platform` components inside `firmware/src` and `firmware/include` are structured as a **hardware abstraction layer (HAL)**:

- Platform‑agnostic protocol and routing logic lives in `protocol_stack.*` and `routing.*`
- Platform‑specific implementations (Linux / embedded Linux) live in `platform_linux.*`
- This separation makes it straightforward to port the stack to other boards or RTOSes by implementing a new platform module.

Python tools can communicate with the firmware over TCP or serial‑like interfaces, demonstrating a basic hardware–software integration story suitable for portfolio and interview discussions.

---

## Notes

- This repository is designed as a **showcase project**, not as a drop‑in production networking stack.
- The focus is on clean structure, realistic build/test automation, and clear mapping to the bullet points:
  - C/C++ network protocol stack on embedded Linux
  - Routing protocol hooks
  - Firmware + low‑level software
  - Python/Bash automation
  - Jenkins + GitHub Actions CI/CD with DevSecOps hooks

