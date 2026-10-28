#pragma once

#include <stddef.h>

extern const unsigned char rom_inicio[];
extern const unsigned char rom_fin[];

static inline size_t rom_tamano(void) {
	return (size_t) (rom_fin - rom_inicio);
}
