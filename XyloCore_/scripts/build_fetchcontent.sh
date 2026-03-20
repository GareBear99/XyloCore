#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
cmake --preset fetchcontent
cmake --build --preset fetchcontent --config Release -j
