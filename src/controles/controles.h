#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
	ACCION_NINGUNA,
	ACCION_SALIR,
	ACCION_PANTALLA_COMPLETA,
	ACCION_PAUSA,
	ACCION_REINICIAR,
	ACCION_GUARDAR_ESTADO,
	ACCION_CARGAR_ESTADO,
	ACCION_ELEGIR_RANURA,
	ACCION_CAPTURA,
} Accion;

void controles_iniciar(void);
uint32_t controles_teclas(void);
bool controles_acelerar(void);
/* Traduce un evento a una acción; con ACCION_ELEGIR_RANURA rellena *ranura. */
Accion controles_accion(const SDL_Event* evento, int* ranura);
void controles_cerrar(void);
