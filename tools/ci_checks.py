import subprocess
import sys
from pathlib import Path


def run(cmd: list[str]) -> int:
    print(f"+ {' '.join(cmd)}")
    return subprocess.call(cmd)


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    tools_dir = root / "tools"

    rc = 0

    # Basic Python syntax check
    for py in tools_dir.glob("*.py"):
        if run([sys.executable, "-m", "py_compile", str(py)]) != 0:
            rc = 1

    # Optional: run pytest if available
    if (tools_dir / "tests").exists():
        if run([sys.executable, "-m", "pytest", str(tools_dir / "tests")]) != 0:
            rc = 1

    return rc


if __name__ == "__main__":
    raise SystemExit(main())

