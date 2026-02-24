#!/usr/bin/env bash
set -euo pipefail

bind="${BIND_ADDR:-0.0.0.0}"
port="${PORT:-9000}"

cd "$(dirname "$0")/../firmware"
make all
./build/bin/sentryflow_firmware --bind "$bind" --port "$port"

