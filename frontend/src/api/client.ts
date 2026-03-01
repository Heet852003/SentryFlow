const API_BASE = '/api';

async function request<T>(
  path: string,
  options: RequestInit = {}
): Promise<T> {
  const url = path.startsWith('http') ? path : `${API_BASE}${path}`;
  const res = await fetch(url, {
    ...options,
    headers: {
      'Content-Type': 'application/json',
      ...options.headers,
    },
  });
  if (!res.ok) {
    const err = await res.json().catch(() => ({ detail: res.statusText }));
    throw new Error((err as { detail?: string }).detail ?? `HTTP ${res.status}`);
  }
  return res.json() as Promise<T>;
}

export const api = {
  getHealth: () => request<import('../types').HealthStatus>('/health'),
  getStats: () => request<import('../types').EngineStats>('/stats'),
  getTwins: () => request<import('../types').DigitalTwinModel[]>('/twins'),
  getTwin: (id: string) =>
    request<import('../types').DigitalTwinModel>(`/twins/${id}`),
  validate: (modelId: string) =>
    request<import('../types').ValidationResult>(`/validate/${modelId}`, {
      method: 'POST',
    }),
  runSimulation: (modelId: string, requests?: number) =>
    request<import('../types').SimulationRun>(
      `/simulate/${modelId}?requests=${requests ?? 1000}`,
      { method: 'POST' }
    ),
  getSimulation: (runId: string) =>
    request<import('../types').SimulationRun>(`/simulate/run/${runId}`),
};
