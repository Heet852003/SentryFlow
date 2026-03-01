import { useEffect, useState } from 'react';
import { Link } from 'react-router-dom';
import { api } from '../api/client';
import type { DigitalTwinModel } from '../types';
import './TwinModels.css';

export default function TwinModels(): JSX.Element {
  const [models, setModels] = useState<DigitalTwinModel[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    api
      .getTwins()
      .then(setModels)
      .catch((e) => setError(e instanceof Error ? e.message : 'Failed'))
      .finally(() => setLoading(false));
  }, []);

  if (loading) {
    return (
      <div className="page">
        <h1>Digital Twin Models</h1>
        <p className="muted">Loading…</p>
      </div>
    );
  }

  return (
    <div className="page">
      <h1>Digital Twin Models</h1>
      <p className="lead">
        Create and manage digital twin models for simulation and validation. Each model can be validated and run against the simulation engine.
      </p>

      {error && (
        <div className="card card-error">Error: {error}</div>
      )}

      <div className="model-grid">
        {models.length === 0 && !error ? (
          <p className="muted">No models yet. The API may seed demo models on first run.</p>
        ) : (
          models.map((m) => (
            <div key={m.id} className="card model-card">
              <div className="model-header">
                <h2>{m.name}</h2>
                <span className={`badge badge-${m.status}`}>{m.status}</span>
              </div>
              <p className="model-desc">{m.description}</p>
              <p className="model-meta">v{m.version} · Updated {new Date(m.updatedAt).toLocaleDateString()}</p>
              <div className="model-actions">
                <Link to={`/validation?model=${m.id}`} className="btn btn-sm">Validate</Link>
                <Link to={`/simulation?model=${m.id}`} className="btn btn-sm btn-primary">Simulate</Link>
              </div>
            </div>
          ))
        )}
      </div>
    </div>
  );
}
