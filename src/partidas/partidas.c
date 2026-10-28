#include "partidas.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define NOMBRE_JUEGO "pokemon-esmeralda"

static char directorio[4096];
static char guardado[4200];

static bool crear_directorios(char* ruta) {
	for (char* p = ruta + 1; *p; ++p) {
		if (*p != '/') {
			continue;
		}
		*p = '\0';
		bool fallo = mkdir(ruta, 0755) != 0 && errno != EEXIST;
		*p = '/';
		if (fallo) {
			return false;
		}
	}
	return mkdir(ruta, 0755) == 0 || errno == EEXIST;
}

bool partidas_preparar(void) {
	const char* xdg = getenv("XDG_DATA_HOME");
	const char* home = getenv("HOME");
	if (xdg && *xdg) {
		snprintf(directorio, sizeof(directorio), "%s/" NOMBRE_JUEGO, xdg);
	} else if (home && *home) {
		snprintf(directorio, sizeof(directorio), "%s/.local/share/" NOMBRE_JUEGO, home);
	} else {
		snprintf(directorio, sizeof(directorio), "./" NOMBRE_JUEGO);
	}
	snprintf(guardado, sizeof(guardado), "%s/esmeralda.sav", directorio);

	char capturas[4200];
	snprintf(capturas, sizeof(capturas), "%s/capturas", directorio);
	return crear_directorios(directorio) && crear_directorios(capturas);
}

const char* partidas_directorio(void) {
	return directorio;
}

const char* partidas_ruta_guardado(void) {
	return guardado;
}

void partidas_ruta_estado(int ranura, char* destino, size_t tam) {
	snprintf(destino, tam, "%s/estado-%d.ss", directorio, ranura);
}

void partidas_ruta_captura(char* destino, size_t tam) {
	char marca[32];
	time_t ahora = time(NULL);
	struct tm local;
	localtime_r(&ahora, &local);
	strftime(marca, sizeof(marca), "%Y%m%d-%H%M%S", &local);
	snprintf(destino, tam, "%s/capturas/esmeralda-%s.bmp", directorio, marca);
}
