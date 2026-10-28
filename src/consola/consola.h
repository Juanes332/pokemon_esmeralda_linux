#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CONSOLA_ANCHO 240
#define CONSOLA_ALTO 160
/* Reloj de la GBA (16,78 MHz) entre ciclos por frame: ~59,73 fps. */
#define CONSOLA_FPS (16777216.0 / 280896.0)

enum {
	TECLA_A = 1 << 0,
	TECLA_B = 1 << 1,
	TECLA_SELECT = 1 << 2,
	TECLA_START = 1 << 3,
	TECLA_DERECHA = 1 << 4,
	TECLA_IZQUIERDA = 1 << 5,
	TECLA_ARRIBA = 1 << 6,
	TECLA_ABAJO = 1 << 7,
	TECLA_R = 1 << 8,
	TECLA_L = 1 << 9,
};

bool consola_iniciar(const void* rom, size_t tam, const char* ruta_guardado, int tasa_audio);
void consola_frame(uint32_t teclas);
const uint32_t* consola_video(void);
size_t consola_leer_audio(int16_t* destino, size_t max_pares);
bool consola_guardar_estado(const char* ruta);
bool consola_cargar_estado(const char* ruta);
void consola_reiniciar(void);
void consola_cerrar(void);
