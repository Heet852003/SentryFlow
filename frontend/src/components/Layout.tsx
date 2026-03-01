import { Link, useLocation } from 'react-router-dom';
import './Layout.css';

const nav = [
  { to: '/', label: 'Dashboard' },
  { to: '/models', label: 'Digital Twin Models' },
  { to: '/validation', label: 'Validation' },
  { to: '/simulation', label: 'Simulation' },
  { to: '/docs', label: 'Documentation' },
];

interface LayoutProps {
  children: React.ReactNode;
}

export default function Layout({ children }: LayoutProps): JSX.Element {
  const location = useLocation();

  return (
    <div className="layout">
      <header className="header">
        <Link to="/" className="logo">
          <span className="logo-icon">◇</span>
          SentryFlow
        </Link>
        <p className="tagline">Digital Twin Simulation &amp; Validation</p>
        <nav className="nav">
          {nav.map(({ to, label }) => (
            <Link
              key={to}
              to={to}
              className={location.pathname === to ? 'nav-link active' : 'nav-link'}
            >
              {label}
            </Link>
          ))}
        </nav>
      </header>
      <main className="main">{children}</main>
      <footer className="footer">
        <span>SentryFlow 2025</span>
        <span>React · TypeScript · Python · Docker · C++ · Java</span>
      </footer>
    </div>
  );
}
