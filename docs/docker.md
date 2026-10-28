# Docker

Todo se compila y se prueba en contenedores. En el host solo hacen falta Docker y SDL2, que es lo único que el ejecutable carga del sistema (`sdl2` o `sdl2-compat` en Arch, `libsdl2-2.0-0` en Debian/Ubuntu).

## Desarrollo (Dev Container)
1. Abre el proyecto en VS Code o Cursor y elige **Reopen in Container**. Desde terminal: `devcontainer up --workspace-folder .`
2. El contenedor es el servicio `builder` de `compose.yaml`: Debian bookworm con gcc, SDL2 y libmgba 0.10.5 compilada como estática en `/opt/mgba`. Dentro puedes usar `make dist`, `make test` y `make clean`.

## Compilar y probar
```bash
docker compose run --rm builder
```
Ejecuta `make clean && make dist test`:
- `make dist` copia la ROM de la raíz a `build/juego.gba`, la embebe con `.incbin` y genera `dist/pokemon-esmeralda`.
- `make test` lanza `tests/humo.sh`: corre 900 frames sin ventana ni audio (`SDL_VIDEODRIVER=dummy`), comprueba que el log registra `rom_cargada` y `partida_terminada`, que se crea el `.sav` y que la captura final tiene color. La captura queda en `build/humo.bmp`.

## Reconstruir desde cero (después de cada cambio)
```bash
./scripts/rebuild.sh
```
El script hace lo siguiente:
1. `docker compose down --volumes --rmi all --remove-orphans`: borra los contenedores, volúmenes e imágenes de **este** proyecto.
2. `docker builder prune --all --force`: borra la build cache.
3. `docker compose build --no-cache --pull`: reconstruye sin caché (vuelve a descargar y compilar mGBA).
4. `docker compose run --rm builder`: compila y pasa el test de humo.

## Servicios
| Servicio | Puerto | Descripción |
|---|---|---|
| builder | — | Compila el ejecutable nativo y ejecuta el test de humo. No expone puertos: el juego se ejecuta en el host. |
