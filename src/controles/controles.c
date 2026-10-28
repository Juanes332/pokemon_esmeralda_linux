#include "controles.h"

#include "../consola/consola.h"
#include "../shared/registro.h"

#define MAX_MANDOS 4
#define ZONA_MUERTA 12000

static SDL_GameController* mandos[MAX_MANDOS];

static const struct { SDL_Scancode tecla; uint32_t boton; } teclado[] = {
	{ SDL_SCANCODE_X, TECLA_A },          { SDL_SCANCODE_Z, TECLA_B },
	{ SDL_SCANCODE_A, TECLA_L },          { SDL_SCANCODE_S, TECLA_R },
	{ SDL_SCANCODE_RETURN, TECLA_START }, { SDL_SCANCODE_BACKSPACE, TECLA_SELECT },
	{ SDL_SCANCODE_UP, TECLA_ARRIBA },    { SDL_SCANCODE_DOWN, TECLA_ABAJO },
	{ SDL_SCANCODE_LEFT, TECLA_IZQUIERDA }, { SDL_SCANCODE_RIGHT, TECLA_DERECHA },
};

static const struct { SDL_GameControllerButton boton_mando; uint32_t boton; } mando[] = {
	{ SDL_CONTROLLER_BUTTON_A, TECLA_A },
	{ SDL_CONTROLLER_BUTTON_B, TECLA_B },
	{ SDL_CONTROLLER_BUTTON_LEFTSHOULDER, TECLA_L },
	{ SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, TECLA_R },
	{ SDL_CONTROLLER_BUTTON_START, TECLA_START },
	{ SDL_CONTROLLER_BUTTON_BACK, TECLA_SELECT },
	{ SDL_CONTROLLER_BUTTON_DPAD_UP, TECLA_ARRIBA },
	{ SDL_CONTROLLER_BUTTON_DPAD_DOWN, TECLA_ABAJO },
	{ SDL_CONTROLLER_BUTTON_DPAD_LEFT, TECLA_IZQUIERDA },
	{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT, TECLA_DERECHA },
};

static void abrir_mando(int indice) {
	if (!SDL_IsGameController(indice)) {
		return;
	}
	for (int i = 0; i < MAX_MANDOS; ++i) {
		if (!mandos[i]) {
			mandos[i] = SDL_GameControllerOpen(indice);
			if (mandos[i]) {
				registro_evento("info", "mando_conectado", "\"nombre\":\"%s\"", SDL_GameControllerName(mandos[i]));
			}
			return;
		}
	}
}

static void cerrar_mando(SDL_JoystickID id) {
	for (int i = 0; i < MAX_MANDOS; ++i) {
		if (mandos[i] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(mandos[i])) == id) {
			SDL_GameControllerClose(mandos[i]);
			mandos[i] = NULL;
			registro_evento("info", "mando_desconectado", NULL);
		}
	}
}

void controles_iniciar(void) {
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		abrir_mando(i);
	}
}

static uint32_t teclas_mando(SDL_GameController* m) {
	uint32_t teclas = 0;
	for (size_t i = 0; i < SDL_arraysize(mando); ++i) {
		if (SDL_GameControllerGetButton(m, mando[i].boton_mando)) {
			teclas |= mando[i].boton;
		}
	}
	int x = SDL_GameControllerGetAxis(m, SDL_CONTROLLER_AXIS_LEFTX);
	int y = SDL_GameControllerGetAxis(m, SDL_CONTROLLER_AXIS_LEFTY);
	teclas |= x < -ZONA_MUERTA ? TECLA_IZQUIERDA : x > ZONA_MUERTA ? TECLA_DERECHA : 0;
	teclas |= y < -ZONA_MUERTA ? TECLA_ARRIBA : y > ZONA_MUERTA ? TECLA_ABAJO : 0;
	return teclas;
}

uint32_t controles_teclas(void) {
	const Uint8* estado = SDL_GetKeyboardState(NULL);
	uint32_t teclas = 0;
	for (size_t i = 0; i < SDL_arraysize(teclado); ++i) {
		if (estado[teclado[i].tecla]) {
			teclas |= teclado[i].boton;
		}
	}
	for (int i = 0; i < MAX_MANDOS; ++i) {
		if (mandos[i]) {
			teclas |= teclas_mando(mandos[i]);
		}
	}
	/* La GBA no admite direcciones opuestas a la vez; algunos juegos fallan con ellas. */
	if ((teclas & TECLA_IZQUIERDA) && (teclas & TECLA_DERECHA)) {
		teclas &= ~(uint32_t) (TECLA_IZQUIERDA | TECLA_DERECHA);
	}
	if ((teclas & TECLA_ARRIBA) && (teclas & TECLA_ABAJO)) {
		teclas &= ~(uint32_t) (TECLA_ARRIBA | TECLA_ABAJO);
	}
	return teclas;
}

bool controles_acelerar(void) {
	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE]) {
		return true;
	}
	for (int i = 0; i < MAX_MANDOS; ++i) {
		if (mandos[i] && SDL_GameControllerGetAxis(mandos[i], SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > ZONA_MUERTA) {
			return true;
		}
	}
	return false;
}

static Accion accion_tecla(const SDL_KeyboardEvent* k, int* ranura) {
	SDL_Keycode tecla = k->keysym.sym;
	if (tecla >= SDLK_1 && tecla <= SDLK_9) {
		*ranura = (int) (tecla - SDLK_0);
		return ACCION_ELEGIR_RANURA;
	}
	switch (tecla) {
	case SDLK_ESCAPE: return ACCION_SALIR;
	case SDLK_F11: return ACCION_PANTALLA_COMPLETA;
	case SDLK_RETURN: return (k->keysym.mod & KMOD_ALT) ? ACCION_PANTALLA_COMPLETA : ACCION_NINGUNA;
	case SDLK_p:
	case SDLK_PAUSE: return ACCION_PAUSA;
	case SDLK_r: return (k->keysym.mod & KMOD_CTRL) ? ACCION_REINICIAR : ACCION_NINGUNA;
	case SDLK_F5: return ACCION_GUARDAR_ESTADO;
	case SDLK_F8: return ACCION_CARGAR_ESTADO;
	case SDLK_F12: return ACCION_CAPTURA;
	default: return ACCION_NINGUNA;
	}
}

Accion controles_accion(const SDL_Event* evento, int* ranura) {
	switch (evento->type) {
	case SDL_QUIT: return ACCION_SALIR;
	case SDL_CONTROLLERDEVICEADDED: abrir_mando(evento->cdevice.which); return ACCION_NINGUNA;
	case SDL_CONTROLLERDEVICEREMOVED: cerrar_mando(evento->cdevice.which); return ACCION_NINGUNA;
	case SDL_KEYDOWN: return evento->key.repeat ? ACCION_NINGUNA : accion_tecla(&evento->key, ranura);
	default: return ACCION_NINGUNA;
	}
}

void controles_cerrar(void) {
	for (int i = 0; i < MAX_MANDOS; ++i) {
		if (mandos[i]) {
			SDL_GameControllerClose(mandos[i]);
			mandos[i] = NULL;
		}
	}
}
