#!/usr/bin/env bash
# Arranca el juego sin ventana ni audio, corre unos frames y comprueba que la imagen no está vacía.
set -euo pipefail

BIN="${1:-dist/pokemon-esmeralda}"
FRAMES="${FRAMES:-900}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

export SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy XDG_DATA_HOME="$TMP/datos"

fallar() { echo "FALLO: $*" >&2; exit 1; }

"$BIN" --ayuda | grep -q "Pokémon Esmeralda" || fallar "--ayuda no muestra la ayuda"
if "$BIN" --opcion-inventada 2>/dev/null; then fallar "acepta opciones inválidas"; fi

"$BIN" --frames "$FRAMES" --captura "$TMP/final.bmp" 2>"$TMP/log.jsonl" || {
	cat "$TMP/log.jsonl" >&2
	fallar "el ejecutable terminó con error"
}

grep -q "\"evento\":\"rom_cargada\"" "$TMP/log.jsonl" || fallar "no se registró rom_cargada"
grep -q "\"evento\":\"partida_terminada\",\"frames\":$FRAMES" "$TMP/log.jsonl" || fallar "no llegó a $FRAMES frames"
test -s "$TMP/final.bmp" || fallar "no se generó la captura"
test -f "$TMP/datos/pokemon-esmeralda/esmeralda.sav" || fallar "no se creó el archivo de guardado"

# Una pantalla negra o de un solo color indicaría que el juego no arrancó.
colores=$(tail -c +139 "$TMP/final.bmp" | od -An -v -tx4 | tr -s ' ' '\n' | sort -u | wc -l)
[ "$colores" -gt 8 ] || fallar "la captura solo tiene $colores colores"

mkdir -p build
cp "$TMP/final.bmp" build/humo.bmp
echo "OK: $FRAMES frames, $colores colores en la captura (build/humo.bmp)"
