#include "sonido.h"

#include "../shared/registro.h"

#include <SDL2/SDL.h>

/* Más de ~100 ms en cola se nota como retraso; se descarta para recuperar sincronía. */
#define MAX_COLA_BYTES (SONIDO_TASA / 10 * 2 * sizeof(int16_t))

static SDL_AudioDeviceID dispositivo;

bool sonido_iniciar(void) {
	SDL_AudioSpec deseado = { 0 };
	deseado.freq = SONIDO_TASA;
	deseado.format = AUDIO_S16SYS;
	deseado.channels = 2;
	deseado.samples = 1024;
	dispositivo = SDL_OpenAudioDevice(NULL, 0, &deseado, NULL, 0);
	if (!dispositivo) {
		registro_evento("warn", "sonido_no_disponible", "\"detalle\":\"%s\"", SDL_GetError());
		return false;
	}
	SDL_PauseAudioDevice(dispositivo, 0);
	registro_evento("info", "sonido_listo", "\"tasa\":%d", SONIDO_TASA);
	return true;
}

void sonido_encolar(const int16_t* muestras, size_t pares) {
	if (!dispositivo || !pares) {
		return;
	}
	if (SDL_GetQueuedAudioSize(dispositivo) > MAX_COLA_BYTES) {
		SDL_ClearQueuedAudio(dispositivo);
	}
	SDL_QueueAudio(dispositivo, muestras, (Uint32) (pares * 2 * sizeof(int16_t)));
}

void sonido_vaciar(void) {
	if (dispositivo) {
		SDL_ClearQueuedAudio(dispositivo);
	}
}

void sonido_pausar(bool pausa) {
	if (dispositivo) {
		SDL_PauseAudioDevice(dispositivo, pausa);
	}
}

void sonido_cerrar(void) {
	if (dispositivo) {
		SDL_CloseAudioDevice(dispositivo);
	}
}
