import subprocess
import sys
import os
from pathlib import Path


def run(cmd: list[str], *, env: dict[str, str] | None = None) -> int:
    print(f"+ {' '.join(cmd)}")
    return subprocess.call(cmd, env=env)


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    tools_dir = root / "tools"

    rc = 0

    # Basic Python syntax check
    for py in tools_dir.glob("*.py"):
        if run([sys.executable, "-m", "py_compile", str(py)]) != 0:
            rc = 1

    # Run pytest for tooling
    env = os.environ.copy()
    env["PYTHONPATH"] = str(tools_dir) + os.pathsep + env.get("PYTHONPATH", "")
    if run([sys.executable, "-m", "pytest", str(tools_dir / "tests")], env=env) != 0:
        rc = 1

    return rc


if __name__ == "__main__":
    raise SystemExit(main())

