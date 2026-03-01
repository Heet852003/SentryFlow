import { useEffect, useState } from 'react';
import { Link } from 'react-router-dom';
import { api } from '../api/client';
import type { HealthStatus, EngineStats } from '../types';
import './Dashboard.css';

export default function Dashboard(): JSX.Element {
  const [health, setHealth] = useState<HealthStatus | null>(null);
  const [stats, setStats] = useState<EngineStats | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    let cancelled = false;
    async function load() {
      try {
        const [h, s] = await Promise.all([api.getHealth(), api.getStats()]);
        if (!cancelled) {
          setHealth(h);
          setStats(s);
        }
      } catch (e) {
        if (!cancelled) setError(e instanceof Error ? e.message : 'Failed to load');
      } finally {
        if (!cancelled) setLoading(false);
      }
    }
    load();
    const id = setInterval(load, 10000);
    return () => {
      cancelled = true;
      clearInterval(id);
    };
  }, []);

  if (loading && !health) {
    return (
      <div className="dashboard">
        <h1>Dashboard</h1>
        <p className="muted">Loading…</p>
      </div>
    );
  }

  return (
    <div className="dashboard">
      <h1>Digital Twin Simulation &amp; Validation</h1>
      <p className="lead">
        Monitor simulation engine health, run validations, and manage digital twin models. Built for scale (1000+ users).
      </p>

      {error && (
        <div className="card card-error">
          <strong>Connection error</strong>: {error}. Ensure the API and simulation engine are running.
        </div>
      )}

      <section className="cards">
        <div className="card">
          <h2>System Health</h2>
          {health ? (
            <ul className="health-list">
              <li>
                <span className={health.api ? 'ok' : 'fail'}>
                  {health.api ? '●' : '○'} API
                </span>
              </li>
              <li>
                <span className={health.simulationEngine ? 'ok' : 'fail'}>
                  {health.simulationEngine ? '●' : '○'} Simulation engine (C++/TCP)
                </span>
              </li>
              <li className="muted">Status: {health.status}</li>
            </ul>
          ) : (
            <p className="muted">—</p>
          )}
        </div>

        <div className="card">
          <h2>Engine Metrics</h2>
          {stats ? (
            <ul className="stats-list">
              <li>Total requests: <strong>{stats.totalRequests.toLocaleString()}</strong></li>
              <li>Bad frames: <strong>{stats.badFrames}</strong></li>
              <li>Routes installed: <strong>{stats.routesInstalled}</strong></li>
              <li>Uptime: <strong>{(stats.uptimeMs / 1000).toFixed(1)}s</strong></li>
              <li>Avg latency: <strong>{(stats.avgLatencyUs / 1000).toFixed(2)} ms</strong></li>
            </ul>
          ) : (
            <p className="muted">—</p>
          )}
        </div>
      </section>

      <section className="actions">
        <Link to="/models" className="btn btn-primary">Digital Twin Models</Link>
        <Link to="/validation" className="btn">Run Validation</Link>
        <Link to="/simulation" className="btn">Simulation Monitor</Link>
        <Link to="/docs" className="btn btn-ghost">Documentation</Link>
      </section>
    </div>
  );
}
