/** Digital Twin model metadata (API-backed). */
export interface DigitalTwinModel {
  id: string;
  name: string;
  description: string;
  version: string;
  status: 'draft' | 'validated' | 'deployed';
  createdAt: string;
  updatedAt: string;
}

/** Validation run result. */
export interface ValidationResult {
  id: string;
  modelId: string;
  passed: boolean;
  checks: ValidationCheck[];
  durationMs: number;
  timestamp: string;
}

export interface ValidationCheck {
  name: string;
  passed: boolean;
  message?: string;
}

/** Simulation run summary. */
export interface SimulationRun {
  id: string;
  modelId: string;
  status: 'running' | 'completed' | 'failed';
  requestsTotal: number;
  latencyMedianMs?: number;
  latencyP95Ms?: number;
  startedAt: string;
  completedAt?: string;
}

/** Engine stats from C++/firmware layer. */
export interface EngineStats {
  totalRequests: number;
  badFrames: number;
  routesInstalled: number;
  uptimeMs: number;
  lastLatencyUs: number;
  avgLatencyUs: number;
}

/** API health. */
export interface HealthStatus {
  status: 'ok' | 'degraded' | 'error';
  api: boolean;
  simulationEngine: boolean;
  timestamp: string;
}
