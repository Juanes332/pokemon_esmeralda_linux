#pragma once

#include "../controles/controles.h"

#include <stdbool.h>

typedef struct {
	bool salir;
	bool pausa;
	int ranura;
} Partida;

void acciones_aplicar(Partida* partida, Accion accion, int ranura);
