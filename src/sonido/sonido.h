#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SONIDO_TASA 48000

bool sonido_iniciar(void);
void sonido_encolar(const int16_t* muestras, size_t pares);
void sonido_vaciar(void);
void sonido_pausar(bool pausa);
void sonido_cerrar(void);
