# Contributing to SentryFlow

Thank you for considering contributing to SentryFlow! This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

By participating in this project, you agree to abide by our Code of Conduct. Please be respectful and considerate of others when contributing.

## How Can I Contribute?

### Reporting Bugs

Bugs are tracked as GitHub issues. Before creating a bug report, please check if the issue has already been reported. When you create a bug report, please include as many details as possible:

- A clear and descriptive title
- Steps to reproduce the issue
- Expected behavior
- Actual behavior
- Screenshots or logs if applicable
- Environment details (OS, browser, etc.)

### Suggesting Enhancements

Enhancement suggestions are also tracked as GitHub issues. When suggesting an enhancement, please include:

- A clear and descriptive title
- A detailed description of the proposed enhancement
- Any potential implementation details
- Why this enhancement would be useful to most users

### Pull Requests

1. Fork the repository
2. Create a new branch for your feature or bugfix
3. Make your changes
4. Run tests to ensure your changes don't break existing functionality
5. Submit a pull request

## Development Setup

### Prerequisites

- Python 3.8+
- Node.js 16+
- Docker and Docker Compose
- PostgreSQL
- Redis
- Kafka
- ClickHouse

### Local Development

1. Clone the repository
   ```bash
   git clone https://github.com/yourusername/sentryflow.git
   cd sentryflow
   ```

2. Set up environment files
   ```bash
   cp backend/.env.example backend/.env
   cp aggregator/.env.example aggregator/.env
   cp frontend/.env.example frontend/.env
   ```

3. Install dependencies
   ```bash
   # Backend
   cd backend
   pip install -r requirements.txt
   
   # Aggregator
   cd ../aggregator
   pip install -r requirements.txt
   
   # Frontend
   cd ../frontend
   npm install
   ```

4. Start services using Docker Compose
   ```bash
   docker-compose up -d postgres redis kafka clickhouse
   ```

5. Set up databases
   ```bash
   cd backend
   python setup_db.py
   
   cd ../aggregator
   python setup_clickhouse.py
   ```

6. Run the services
   ```bash
   # Backend
   cd backend
   uvicorn main:app --reload --host 0.0.0.0 --port 8000
   
   # Aggregator
   cd aggregator
   python batch_consumer.py
   
   # Frontend
   cd frontend
   npm start
   ```

Alternatively, you can use the provided Makefile commands:
```bash
make setup      # Set up the project
make dev-backend    # Run backend in development mode
make dev-aggregator # Run aggregator in development mode
make dev-frontend   # Run frontend in development mode
```

## Testing

Run tests for each component:

```bash
# Backend tests
cd backend
pytest

# Aggregator tests
cd aggregator
pytest

# Frontend tests
cd frontend
npm test
```

Or use the Makefile:
```bash
make test           # Run all tests
make test-backend   # Run backend tests only
make test-aggregator # Run aggregator tests only
make test-frontend  # Run frontend tests only
```

## Code Style

### Python

We follow PEP 8 style guidelines for Python code. Use `flake8` to check your code:

```bash
cd backend  # or aggregator
flake8
```

### JavaScript/React

We use ESLint and Prettier for JavaScript/React code. Check your code with:

```bash
cd frontend
npm run lint
```

## Documentation

Please update the documentation when you make changes to the code. This includes:

- Code comments
- README.md updates
- API documentation
- User guides

## Commit Messages

Write clear and meaningful commit messages. Follow these guidelines:

- Use the present tense ("Add feature" not "Added feature")
- Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit the first line to 72 characters or less
- Reference issues and pull requests after the first line

## Versioning

We use [Semantic Versioning](https://semver.org/) for versioning. For the versions available, see the tags on this repository.

## License

By contributing to SentryFlow, you agree that your contributions will be licensed under the project's MIT License.