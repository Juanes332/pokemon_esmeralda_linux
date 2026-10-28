#include "pantalla.h"

#include "../consola/consola.h"
#include "../shared/registro.h"

#include <SDL2/SDL.h>

#define TITULO "Pokémon Esmeralda"

static SDL_Window* ventana;
static SDL_Renderer* render;
static SDL_Texture* textura;

bool pantalla_iniciar(int escala, bool completa) {
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	Uint32 banderas = SDL_WINDOW_RESIZABLE | (completa ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	ventana = SDL_CreateWindow(TITULO, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                           CONSOLA_ANCHO * escala, CONSOLA_ALTO * escala, banderas);
	if (!ventana) {
		registro_evento("error", "ventana_no_creada", "\"detalle\":\"%s\"", SDL_GetError());
		return false;
	}
	render = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
	if (!render) {
		render = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_SOFTWARE);
	}
	if (!render) {
		registro_evento("error", "render_no_creado", "\"detalle\":\"%s\"", SDL_GetError());
		return false;
	}
	SDL_RenderSetLogicalSize(render, CONSOLA_ANCHO, CONSOLA_ALTO);
	SDL_RenderSetIntegerScale(render, SDL_TRUE);
	/* mGBA entrega píxeles XBGR8888: en little-endian son los bytes R,G,B,X. */
	textura = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
	                            CONSOLA_ANCHO, CONSOLA_ALTO);
	if (!textura) {
		registro_evento("error", "textura_no_creada", "\"detalle\":\"%s\"", SDL_GetError());
		return false;
	}
	SDL_RendererInfo info;
	SDL_GetRendererInfo(render, &info);
	registro_evento("info", "pantalla_lista", "\"render\":\"%s\",\"escala\":%d", info.name, escala);
	return true;
}

void pantalla_mostrar(const uint32_t* pixeles) {
	SDL_UpdateTexture(textura, NULL, pixeles, CONSOLA_ANCHO * sizeof(uint32_t));
	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);
	SDL_RenderCopy(render, textura, NULL, NULL);
	SDL_RenderPresent(render);
}

void pantalla_alternar_completa(void) {
	bool completa = SDL_GetWindowFlags(ventana) & SDL_WINDOW_FULLSCREEN_DESKTOP;
	SDL_SetWindowFullscreen(ventana, completa ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_ShowCursor(completa ? SDL_ENABLE : SDL_DISABLE);
}

void pantalla_titulo(const char* estado) {
	char titulo[256];
	if (estado && *estado) {
		SDL_snprintf(titulo, sizeof(titulo), TITULO " — %s", estado);
	} else {
		SDL_snprintf(titulo, sizeof(titulo), TITULO);
	}
	SDL_SetWindowTitle(ventana, titulo);
}

bool pantalla_captura_bmp(const uint32_t* pixeles, const char* ruta) {
	SDL_Surface* superficie = SDL_CreateRGBSurfaceWithFormatFrom(
		(void*) pixeles, CONSOLA_ANCHO, CONSOLA_ALTO, 32, CONSOLA_ANCHO * 4, SDL_PIXELFORMAT_XBGR8888);
	if (!superficie) {
		return false;
	}
	bool ok = SDL_SaveBMP(superficie, ruta) == 0;
	SDL_FreeSurface(superficie);
	return ok;
}

void pantalla_cerrar(void) {
	if (textura) SDL_DestroyTexture(textura);
	if (render) SDL_DestroyRenderer(render);
	if (ventana) SDL_DestroyWindow(ventana);
}
