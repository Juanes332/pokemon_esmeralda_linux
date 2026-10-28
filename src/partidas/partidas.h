#pragma once

#include <stdbool.h>
#include <stddef.h>

bool partidas_preparar(void);
const char* partidas_directorio(void);
const char* partidas_ruta_guardado(void);
void partidas_ruta_estado(int ranura, char* destino, size_t tam);
void partidas_ruta_captura(char* destino, size_t tam);
