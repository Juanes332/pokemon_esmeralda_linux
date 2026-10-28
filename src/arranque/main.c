#include "acciones.h"
#include "opciones.h"

#include "../consola/consola.h"
#include "../controles/controles.h"
#include "../pantalla/pantalla.h"
#include "../partidas/partidas.h"
#include "../rom/rom.h"
#include "../shared/registro.h"
#include "../sonido/sonido.h"

#include <SDL2/SDL.h>

#define FRAMES_ACELERADO 4
#define MAX_PARES_AUDIO 4096
/* Si vamos más de estos frames tarde (p. ej. tras suspender el equipo) no se intenta recuperar. */
#define MAX_RETRASO_FRAMES 5

typedef struct {
	long frames;
	Uint64 inicio_ventana;
	long frames_ventana;
} Metricas;

static void medir(Metricas* m) {
	++m->frames;
	++m->frames_ventana;
	Uint64 ahora = SDL_GetPerformanceCounter();
	double segundos = (double) (ahora - m->inicio_ventana) / (double) SDL_GetPerformanceFrequency();
	if (segundos >= 60.0) {
		registro_evento("info", "metricas", "\"fps\":%.2f,\"frames\":%ld", m->frames_ventana / segundos, m->frames);
		m->inicio_ventana = ahora;
		m->frames_ventana = 0;
	}
}

static void emular_frame(bool con_sonido) {
	static int16_t audio[MAX_PARES_AUDIO * 2];
	consola_frame(controles_teclas());
	size_t pares = consola_leer_audio(audio, MAX_PARES_AUDIO);
	if (con_sonido) {
		sonido_encolar(audio, pares);
	}
}

static void esperar_hasta(Uint64* limite, Uint64 periodo) {
	Uint64 ahora = SDL_GetPerformanceCounter();
	if (ahora > *limite + periodo * MAX_RETRASO_FRAMES) {
		*limite = ahora;
	}
	while ((ahora = SDL_GetPerformanceCounter()) < *limite) {
		Uint64 restante_ms = (*limite - ahora) * 1000 / SDL_GetPerformanceFrequency();
		if (restante_ms > 1) {
			SDL_Delay((Uint32) (restante_ms - 1));
		}
	}
	*limite += periodo;
}

static void bucle(const Opciones* opciones) {
	Partida partida = { .ranura = 1 };
	Metricas metricas = { .inicio_ventana = SDL_GetPerformanceCounter() };
	Uint64 periodo = (Uint64) ((double) SDL_GetPerformanceFrequency() / CONSOLA_FPS);
	Uint64 limite = SDL_GetPerformanceCounter();
	bool prueba = opciones->frames > 0;

	while (!partida.salir) {
		SDL_Event evento;
		while (SDL_PollEvent(&evento)) {
			int ranura = 0;
			acciones_aplicar(&partida, controles_accion(&evento, &ranura), ranura);
		}
		if (partida.pausa) {
			pantalla_mostrar(consola_video());
			SDL_Delay(16);
			limite = SDL_GetPerformanceCounter();
			continue;
		}
		bool acelerar = !prueba && controles_acelerar();
		int vueltas = acelerar ? FRAMES_ACELERADO : 1;
		for (int i = 0; i < vueltas; ++i) {
			emular_frame(!acelerar && !prueba);
			medir(&metricas);
		}
		pantalla_mostrar(consola_video());
		if (prueba) {
			partida.salir = metricas.frames >= opciones->frames;
		} else {
			esperar_hasta(&limite, periodo);
		}
	}
	registro_evento("info", "partida_terminada", "\"frames\":%ld", metricas.frames);
}

static bool iniciar_sdl(void) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		registro_evento("error", "sdl_no_iniciado", "\"detalle\":\"%s\"", SDL_GetError());
		return false;
	}
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		registro_evento("warn", "audio_no_iniciado", "\"detalle\":\"%s\"", SDL_GetError());
	}
	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
		registro_evento("warn", "mandos_no_iniciados", "\"detalle\":\"%s\"", SDL_GetError());
	}
	return true;
}

int main(int argc, char** argv) {
	Opciones opciones;
	if (!opciones_leer(argc, argv, &opciones)) {
		return 2;
	}
	if (opciones.ayuda) {
		opciones_mostrar_ayuda(argv[0]);
		return 0;
	}
	if (!partidas_preparar()) {
		registro_evento("error", "directorio_no_creado", "\"ruta\":\"%s\"", partidas_directorio());
		return 1;
	}
	if (!iniciar_sdl()) {
		return 1;
	}
	int codigo = 1;
	if (pantalla_iniciar(opciones.escala, opciones.pantalla_completa) &&
	    consola_iniciar(rom_inicio, rom_tamano(), partidas_ruta_guardado(), SONIDO_TASA)) {
		sonido_iniciar();
		controles_iniciar();
		bucle(&opciones);
		if (opciones.captura && !pantalla_captura_bmp(consola_video(), opciones.captura)) {
			registro_evento("error", "captura_fallida", "\"ruta\":\"%s\"", opciones.captura);
		} else {
			codigo = 0;
		}
	}
	controles_cerrar();
	sonido_cerrar();
	consola_cerrar();
	pantalla_cerrar();
	SDL_Quit();
	return codigo;
}
