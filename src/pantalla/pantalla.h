#pragma once

#include <stdbool.h>
#include <stdint.h>

bool pantalla_iniciar(int escala, bool completa);
void pantalla_mostrar(const uint32_t* pixeles);
void pantalla_alternar_completa(void);
void pantalla_titulo(const char* estado);
bool pantalla_captura_bmp(const uint32_t* pixeles, const char* ruta);
void pantalla_cerrar(void);
