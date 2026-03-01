import { useEffect, useState } from 'react';
import { useSearchParams } from 'react-router-dom';
import { api } from '../api/client';
import type { DigitalTwinModel, SimulationRun, EngineStats } from '../types';
import './Simulation.css';

export default function Simulation(): JSX.Element {
  const [searchParams] = useSearchParams();
  const modelId = searchParams.get('model');
  const [models, setModels] = useState<DigitalTwinModel[]>([]);
  const [run, setRun] = useState<SimulationRun | null>(null);
  const [stats, setStats] = useState<EngineStats | null>(null);
  const [loading, setLoading] = useState(false);
  const [selectedId, setSelectedId] = useState<string | null>(modelId);
  const [requests, setRequests] = useState(1000);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    api.getTwins().then(setModels).catch(() => {});
  }, []);

  useEffect(() => {
    if (modelId) setSelectedId(modelId);
  }, [modelId]);

  const refreshStats = () => {
    api.getStats().then(setStats).catch(() => {});
  };

  useEffect(() => {
    refreshStats();
    const id = setInterval(refreshStats, 5000);
    return () => clearInterval(id);
  }, []);

  const startSimulation = async () => {
    if (!selectedId) return;
    setError(null);
    setRun(null);
    setLoading(true);
    try {
      const r = await api.runSimulation(selectedId, requests);
      setRun(r);
      refreshStats();
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Simulation failed');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="page">
      <h1>Simulation Monitor</h1>
      <p className="lead">
        Run and monitor digital twin simulations against the C++ simulation engine. Target &lt; 100 ms latency at 1000+ requests.
      </p>

      <div className="card sim-form">
        <label>
          Model
          <select
            value={selectedId ?? ''}
            onChange={(e) => setSelectedId(e.target.value || null)}
          >
            <option value="">— Select —</option>
            {models.map((m) => (
              <option key={m.id} value={m.id}>{m.name}</option>
            ))}
          </select>
        </label>
        <label>
          Requests
          <input
            type="number"
            min={100}
            max={100000}
            value={requests}
            onChange={(e) => setRequests(parseInt(e.target.value, 10) || 1000)}
          />
        </label>
        <button
          type="button"
          className="btn btn-primary"
          disabled={!selectedId || loading}
          onClick={startSimulation}
        >
          {loading ? 'Running…' : 'Run simulation'}
        </button>
      </div>

      {error && <div className="card card-error">Error: {error}</div>}

      {stats && (
        <div className="card">
          <h2>Engine live stats</h2>
          <ul className="stats-list">
            <li>Total requests: <strong>{stats.totalRequests.toLocaleString()}</strong></li>
            <li>Avg latency: <strong>{(stats.avgLatencyUs / 1000).toFixed(2)} ms</strong></li>
            <li>Uptime: <strong>{(stats.uptimeMs / 1000).toFixed(0)} s</strong></li>
          </ul>
        </div>
      )}

      {run && (
        <div className="card result-card">
          <h2>Last run</h2>
          <p>Status: <strong>{run.status}</strong></p>
          <p>Requests: {run.requestsTotal}</p>
          {run.latencyMedianMs != null && <p>Median latency: {run.latencyMedianMs.toFixed(2)} ms</p>}
          {run.latencyP95Ms != null && <p>P95 latency: {run.latencyP95Ms.toFixed(2)} ms</p>}
          <p className="muted">Started: {new Date(run.startedAt).toLocaleString()}</p>
        </div>
      )}
    </div>
  );
}
