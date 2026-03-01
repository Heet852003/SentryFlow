import { Routes, Route } from 'react-router-dom';
import Layout from './components/Layout';
import Dashboard from './pages/Dashboard';
import TwinModels from './pages/TwinModels';
import Validation from './pages/Validation';
import Simulation from './pages/Simulation';
import Documentation from './pages/Documentation';

export default function App(): JSX.Element {
  return (
    <Layout>
      <Routes>
        <Route path="/" element={<Dashboard />} />
        <Route path="/models" element={<TwinModels />} />
        <Route path="/validation" element={<Validation />} />
        <Route path="/simulation" element={<Simulation />} />
        <Route path="/docs" element={<Documentation />} />
      </Routes>
    </Layout>
  );
}
