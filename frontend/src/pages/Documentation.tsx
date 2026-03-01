import './Documentation.css';

export default function Documentation(): JSX.Element {
  return (
    <div className="page docs-page">
      <h1>Documentation &amp; Tutorials</h1>
      <p className="lead">
        Technical documentation for the SentryFlow Digital Twin Simulation &amp; Validation platform.
      </p>

      <section className="card doc-section">
        <h2>Overview</h2>
        <p>
          SentryFlow is a <strong>Digital Twin Simulation &amp; Validation</strong> platform built with:
        </p>
        <ul>
          <li><strong>Frontend:</strong> React, TypeScript, HTML5, JavaScript (ES6) — supports 1000+ users</li>
          <li><strong>Backend API:</strong> Python (FastAPI) — REST API and gateway to simulation engine</li>
          <li><strong>Simulation engine:</strong> C++ firmware (TCP binary protocol) + Java validation service</li>
          <li><strong>Automation:</strong> Python scripts for testing, validation, and performance benchmarks</li>
          <li><strong>Deployment:</strong> Docker containers for all services</li>
        </ul>
      </section>

      <section className="card doc-section">
        <h2>Quick start</h2>
        <ol>
          <li>Start the stack: <code>docker-compose up -d</code></li>
          <li>Open the UI: <code>http://localhost:3000</code> (or API proxy port)</li>
          <li>Use <strong>Digital Twin Models</strong> to view models, <strong>Validation</strong> to run checks, <strong>Simulation</strong> to run load tests.</li>
        </ol>
      </section>

      <section className="card doc-section">
        <h2>API</h2>
        <p>REST API base: <code>/api</code></p>
        <ul>
          <li><code>GET /api/health</code> — System health (API + simulation engine)</li>
          <li><code>GET /api/stats</code> — Engine metrics (requests, latency, routes)</li>
          <li><code>GET /api/twins</code> — List digital twin models</li>
          <li><code>{'POST /api/validate/{modelId}'}</code> — Run validation</li>
          <li><code>{'POST /api/simulate/{modelId}?requests=N'}</code> — Run simulation</li>
        </ul>
      </section>

      <section className="card doc-section">
        <h2>QA &amp; Testing</h2>
        <p>
          Python automation tests and validation scripts live in <code>tools/</code>:
        </p>
        <ul>
          <li><code>pytest tools/tests/</code> — Protocol and API tests</li>
          <li><code>python tools/traffic_generator.py</code> — Load generation (1000+ requests)</li>
          <li><code>python tools/latency_benchmark.py</code> — Latency validation (&lt; 100 ms target)</li>
          <li><code>python tools/validate_simulation.py</code> — Simulation engine validation</li>
        </ul>
      </section>
    </div>
  );
}
