#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."

docker compose down --volumes --rmi all --remove-orphans
docker builder prune --all --force
docker compose build --no-cache --pull
docker compose run --rm builder
ls -lh dist/
