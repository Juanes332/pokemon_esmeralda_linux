#include "opciones.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool leer_entero(const char* texto, long minimo, long maximo, long* destino) {
	char* fin = NULL;
	long valor = strtol(texto, &fin, 10);
	if (!fin || *fin || valor < minimo || valor > maximo) {
		return false;
	}
	*destino = valor;
	return true;
}

bool opciones_leer(int argc, char** argv, Opciones* opciones) {
	*opciones = (Opciones) { .escala = 4 };
	for (int i = 1; i < argc; ++i) {
		const char* arg = argv[i];
		const char* valor = i + 1 < argc ? argv[i + 1] : NULL;
		long numero = 0;
		if (!strcmp(arg, "--ayuda") || !strcmp(arg, "-h") || !strcmp(arg, "--help")) {
			opciones->ayuda = true;
		} else if (!strcmp(arg, "--pantalla-completa") || !strcmp(arg, "-f")) {
			opciones->pantalla_completa = true;
		} else if (!strcmp(arg, "--escala") && valor && leer_entero(valor, 1, 12, &numero)) {
			opciones->escala = (int) numero;
			++i;
		} else if (!strcmp(arg, "--frames") && valor && leer_entero(valor, 1, 100000000, &numero)) {
			opciones->frames = numero;
			++i;
		} else if (!strcmp(arg, "--captura") && valor) {
			opciones->captura = valor;
			++i;
		} else {
			fprintf(stderr, "Opción no válida: %s (usa --ayuda)\n", arg);
			return false;
		}
	}
	return true;
}

void opciones_mostrar_ayuda(const char* programa) {
	printf("Pokémon Esmeralda para Linux\n\n"
	       "Uso: %s [opciones]\n\n"
	       "  --escala N            tamaño de ventana (1-12, por defecto 4)\n"
	       "  -f, --pantalla-completa\n"
	       "  --frames N            modo prueba: corre N frames sin limitar y sale\n"
	       "  --captura RUTA.bmp    guarda el último frame al salir\n"
	       "  -h, --ayuda\n\n"
	       "Teclas: flechas = cruceta, X = A, Z = B, A = L, S = R,\n"
	       "        Enter = Start, Retroceso = Select\n"
	       "        F5 guardar estado, F8 cargar, 1-9 ranura, F11 pantalla completa,\n"
	       "        Espacio (mantener) acelerar, P pausa, F12 captura, Ctrl+R reiniciar, Esc salir\n",
	       programa);
}
