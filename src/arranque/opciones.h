#pragma once

#include <stdbool.h>

typedef struct {
	int escala;
	bool pantalla_completa;
	/* Con frames > 0 corre sin limitar velocidad y sale al llegar: modo prueba. */
	long frames;
	const char* captura;
	bool ayuda;
} Opciones;

bool opciones_leer(int argc, char** argv, Opciones* opciones);
void opciones_mostrar_ayuda(const char* programa);
