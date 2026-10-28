# Pokémon Esmeralda para Linux

Un único ejecutable nativo de Linux (`dist/pokemon-esmeralda`) que lleva dentro tu ROM de *Pokémon – Edición Esmeralda (España)* y el núcleo de emulación de GBA. Haces doble clic o lo lanzas desde la terminal y el juego arranca, sin abrir ningún emulador ni elegir archivos.

```bash
docker compose run --rm builder   # compila y prueba (ver docs/docker.md)
./dist/pokemon-esmeralda          # a jugar
./scripts/instalar.sh             # opcional: lo añade a ~/.local/bin y al menú de aplicaciones
```

## Cómo funciona

La GBA tiene un procesador ARM7TDMI y un hardware gráfico y de sonido propio, así que no se puede traducir el juego a x86 instrucción por instrucción. El ejecutable incluye un emulador de la consola y la ROM, todo empaquetado:

| Parte | Qué hace | Dónde está |
|---|---|---|
| ROM embebida | Los 16 MB del cartucho se meten en el binario con `.incbin` y se copian a memoria al arrancar. | `src/rom/` |
| Núcleo | [libmgba 0.10.5](https://mgba.io) enlazada de forma estática: emula CPU, vídeo, sonido, guardado Flash y el reloj (RTC) del cartucho. | `src/consola/` |
| Pantalla | Ventana SDL2 de 240×160 con escalado entero (píxeles nítidos) y pantalla completa. | `src/pantalla/` |
| Sonido | Audio estéreo a 48 kHz por SDL2 (PipeWire/PulseAudio/ALSA). | `src/sonido/` |
| Controles | Teclado y mandos (Xbox, PlayStation, Switch Pro… vía SDL GameController), con conexión en caliente. | `src/controles/` |
| Partidas | Guardado del juego y estados rápidos en `~/.local/share/pokemon-esmeralda/`. | `src/partidas/` |
| Arranque | Opciones de línea de comandos y bucle a 59,73 fps, el ritmo real de la GBA. | `src/arranque/` |

Lo único que se carga del sistema es SDL2 (`ldd` muestra solo `libSDL2`, `libm` y `libc`), así que el ejecutable usa Wayland o X11 y el audio que tengas configurados.

### Lo que sabemos del cartucho

Datos sacados de la cabecera de la ROM y de las cadenas de la biblioteca del SDK que lleva dentro:

- **Código de juego:** `BPES` (Pokémon Esmeralda, España), título interno `POKEMON EMER`, fabricante `01` (Nintendo).
- **Entrada:** la primera instrucción (`0xEA00007F`) es un salto ARM a `0x08000204`, donde empieza el código de arranque.
- **Guardado:** Flash de 1 Mbit (`FLASH1M_V103`, 128 KB). Se guarda en `esmeralda.sav`, compatible con mGBA y otros emuladores.
- **Reloj en tiempo real:** chip Seiko S-3511 (`SIIRTC_V001`) conectado por GPIO. Usa la hora de tu equipo, así que las mareas, las bayas y los eventos diarios funcionan.
- **SHA-256 de la ROM:** `e32c82bd10f174cf4019123b36f3ef7729105fb6634d9aa6b61413ee5101a55e`

## Teclas

### Botones de la consola

| GBA | Teclado | Mando |
|---|---|---|
| Cruceta | Flechas | Cruceta o stick izquierdo |
| A (aceptar, hablar) | **X** | A (abajo) |
| B (cancelar, correr con las Deportivas) | **Z** | B (derecha) |
| L | **A** | LB / L1 |
| R | **S** | RB / R1 |
| Start (menú) | **Enter** | Start / Options |
| Select (objeto registrado) | **Retroceso** | Back / Share |

### Funciones del ejecutable

| Tecla | Acción |
|---|---|
| **Espacio** (mantener) / gatillo derecho | Acelerar ×4 (sin sonido mientras se mantiene) |
| **F5** | Guardar estado en la ranura actual |
| **F8** | Cargar estado de la ranura actual |
| **1–9** | Elegir ranura (por defecto, la 1) |
| **F11** o **Alt+Enter** | Pantalla completa |
| **P** o **Pausa** | Pausar / reanudar |
| **F12** | Captura de pantalla (BMP en `capturas/`) |
| **Ctrl+R** | Reiniciar la consola |
| **Esc** | Salir |

El título de la ventana confirma cada acción («Estado 1 guardado», «Pausa»…).

## Opciones

```
./dist/pokemon-esmeralda [opciones]
  --escala N              tamaño de ventana, de 1 a 12 (por defecto 4 → 960×640)
  -f, --pantalla-completa
  --frames N              modo prueba: corre N frames sin limitar velocidad y sale
  --captura RUTA.bmp      guarda el último frame al salir
  -h, --ayuda
```

## Dónde se guarda todo

`~/.local/share/pokemon-esmeralda/` (o `$XDG_DATA_HOME/pokemon-esmeralda/`):

- `esmeralda.sav`: la partida guardada **desde el menú del juego**.
- `estado-1.ss` … `estado-9.ss`: estados rápidos (F5/F8).
- `capturas/`: capturas de pantalla.

## Recomendaciones

- **Guarda también desde el menú del juego.** Los estados rápidos (F5) son una foto de la emulación y pueden dejar de funcionar si cambia la versión de mGBA; el `.sav` es el guardado real del cartucho y se conserva siempre. Haz copia de `esmeralda.sav` de vez en cuando.
- **Cierra con Esc o con la ventana**, no matando el proceso: así el `.sav` se escribe completo.
- **Cuidado al cargar un estado** (F8): sustituye lo que tengas en ese momento. Usa ranuras distintas para no pisar un estado bueno.
- **Un mando se juega mejor.** Si no lo detecta, conéctalo antes de abrir el juego o mira los logs (evento `mando_conectado`).
- **Acelerar (Espacio)** viene bien para subir de nivel o cruzar zonas largas, pero suéltalo en los diálogos para no pasar texto sin leerlo.
- **Si el sonido chasquea**, cierra otras aplicaciones de audio pesadas; el ejecutable descarta el audio que se retrasa más de 100 ms para no desincronizarse.
- **Problemas al arrancar:** el programa escribe un log JSON por stderr. Ejecútalo desde la terminal (`./dist/pokemon-esmeralda 2> log.jsonl`) y busca líneas con `"nivel":"error"`. Cada minuto registra los fps (`"evento":"metricas"`); deberían rondar los 59,7.
- **No compartas el ejecutable.** Lleva dentro la ROM, que tiene copyright de Nintendo/Game Freak/The Pokémon Company. Es solo para tu copia personal del juego.

## Desarrollo

- Compilación, tests y reconstrucción con Docker: [docs/docker.md](docs/docker.md).
- Código en `src/<dominio>/`, ningún archivo pasa de 200 líneas.
- Licencias: mGBA es MPL 2.0 y SDL2 es zlib.
