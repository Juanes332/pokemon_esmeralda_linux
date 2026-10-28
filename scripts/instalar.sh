#!/usr/bin/env bash
# Instala el ejecutable en ~/.local/bin y crea el acceso directo del menú de aplicaciones.
set -euo pipefail
cd "$(dirname "$0")/.."

BIN="dist/pokemon-esmeralda"
[ -x "$BIN" ] || { echo "Primero compila: docker compose run --rm builder" >&2; exit 1; }

mkdir -p "$HOME/.local/bin" "$HOME/.local/share/applications"
install -m 755 "$BIN" "$HOME/.local/bin/pokemon-esmeralda"

cat > "$HOME/.local/share/applications/pokemon-esmeralda.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=Pokémon Esmeralda
Comment=Pokémon Edición Esmeralda (GBA) como juego nativo
Exec=$HOME/.local/bin/pokemon-esmeralda
Icon=applications-games
Terminal=false
Categories=Game;
EOF

echo "Instalado en $HOME/.local/bin/pokemon-esmeralda"
