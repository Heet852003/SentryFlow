$ErrorActionPreference = "Stop"

$bind = if ($env:BIND_ADDR) { $env:BIND_ADDR } else { "0.0.0.0" }
$port = if ($env:PORT) { $env:PORT } else { "9000" }

Set-Location (Join-Path $PSScriptRoot "..\\firmware")
make all
.\build\bin\sentryflow_firmware --bind $bind --port $port

