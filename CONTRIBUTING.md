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

- Linux or WSL2 recommended (the firmware uses `epoll`)
- GCC/Clang + `make`
- Python 3.10+

### Local Development

1. Clone the repository
   ```bash
   git clone https://github.com/yourusername/sentryflow.git
   cd sentryflow
   ```

2. Build and run the firmware
```bash
cd firmware
make all
make run
```

## Testing

Run firmware unit tests and self-test:

```bash
cd firmware
make test
```

Run Python tooling tests:

```bash
cd tools
python -m venv .venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate
pip install -r requirements.txt
python -m pytest
```

## Code Style

### C/C++

- Prefer small, testable modules under `firmware/src/`
- Keep platform-specific code in the platform layer (`platform_linux.c`, `hal_linux.c`)
- Avoid compiler extensions unless necessary

### Python

- Keep protocol handling in `tools/sentryflow_protocol.py` and `tools/sentryflow_client.py`
- Add tests under `tools/tests/`

## Documentation

Please update the documentation when you make changes to the code. This includes:

- Code comments
- README.md updates
- `docs/` (protocol/routing/architecture)

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