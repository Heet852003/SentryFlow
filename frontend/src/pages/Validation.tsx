import { useEffect, useState } from 'react';
import { useSearchParams } from 'react-router-dom';
import { api } from '../api/client';
import type { DigitalTwinModel, ValidationResult } from '../types';
import './Validation.css';

export default function Validation(): JSX.Element {
  const [searchParams] = useSearchParams();
  const modelId = searchParams.get('model');
  const [models, setModels] = useState<DigitalTwinModel[]>([]);
  const [result, setResult] = useState<ValidationResult | null>(null);
  const [loading, setLoading] = useState(false);
  const [selectedId, setSelectedId] = useState<string | null>(modelId);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    api.getTwins().then(setModels).catch(() => {});
  }, []);

  useEffect(() => {
    if (modelId) setSelectedId(modelId);
  }, [modelId]);

  const runValidation = async () => {
    if (!selectedId) return;
    setError(null);
    setResult(null);
    setLoading(true);
    try {
      const r = await api.validate(selectedId);
      setResult(r);
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Validation failed');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="page">
      <h1>Validation</h1>
      <p className="lead">
        Run validation checks on digital twin models. Validation uses the simulation engine and Java validation service to ensure model correctness and performance.
      </p>

      <div className="card validation-form">
        <label>
          Select model
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
        <button
          type="button"
          className="btn btn-primary"
          disabled={!selectedId || loading}
          onClick={runValidation}
        >
          {loading ? 'Running…' : 'Run validation'}
        </button>
      </div>

      {error && <div className="card card-error">Error: {error}</div>}

      {result && (
        <div className={`card result-card ${result.passed ? 'passed' : 'failed'}`}>
          <h2>{result.passed ? '✓ Validation passed' : '✗ Validation failed'}</h2>
          <p className="muted">Duration: {result.durationMs} ms · {new Date(result.timestamp).toLocaleString()}</p>
          <ul className="checks">
            {result.checks.map((c, i) => (
              <li key={i} className={c.passed ? 'ok' : 'fail'}>
                {c.passed ? '✓' : '✗'} {c.name}
                {c.message && <span className="msg"> — {c.message}</span>}
              </li>
            ))}
          </ul>
        </div>
      )}
    </div>
  );
}
