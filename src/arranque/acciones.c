#include "acciones.h"

#include "../consola/consola.h"
#include "../pantalla/pantalla.h"
#include "../partidas/partidas.h"
#include "../shared/registro.h"
#include "../sonido/sonido.h"

#include <stdio.h>

static void avisar(const Partida* partida, const char* mensaje) {
	char texto[128];
	snprintf(texto, sizeof(texto), "%s%s", partida->pausa ? "Pausa · " : "", mensaje ? mensaje : "");
	pantalla_titulo(texto);
}

static void guardar_estado(Partida* partida) {
	char ruta[4300];
	partidas_ruta_estado(partida->ranura, ruta, sizeof(ruta));
	bool ok = consola_guardar_estado(ruta);
	registro_evento(ok ? "info" : "error", "estado_guardado", "\"ranura\":%d,\"ok\":%s", partida->ranura,
	                ok ? "true" : "false");
	char mensaje[64];
	snprintf(mensaje, sizeof(mensaje), ok ? "Estado %d guardado" : "No se pudo guardar el estado %d",
	         partida->ranura);
	avisar(partida, mensaje);
}

static void cargar_estado(Partida* partida) {
	char ruta[4300];
	partidas_ruta_estado(partida->ranura, ruta, sizeof(ruta));
	bool ok = consola_cargar_estado(ruta);
	registro_evento(ok ? "info" : "warn", "estado_cargado", "\"ranura\":%d,\"ok\":%s", partida->ranura,
	                ok ? "true" : "false");
	sonido_vaciar();
	char mensaje[64];
	snprintf(mensaje, sizeof(mensaje), ok ? "Estado %d cargado" : "Ranura %d vacía", partida->ranura);
	avisar(partida, mensaje);
}

static void capturar(Partida* partida) {
	char ruta[4300];
	partidas_ruta_captura(ruta, sizeof(ruta));
	bool ok = pantalla_captura_bmp(consola_video(), ruta);
	registro_evento(ok ? "info" : "error", "captura", "\"ruta\":\"%s\",\"ok\":%s", ruta, ok ? "true" : "false");
	avisar(partida, ok ? "Captura guardada" : "No se pudo guardar la captura");
}

void acciones_aplicar(Partida* partida, Accion accion, int ranura) {
	char mensaje[64];
	switch (accion) {
	case ACCION_SALIR:
		partida->salir = true;
		break;
	case ACCION_PANTALLA_COMPLETA:
		pantalla_alternar_completa();
		break;
	case ACCION_PAUSA:
		partida->pausa = !partida->pausa;
		sonido_pausar(partida->pausa);
		registro_evento("info", partida->pausa ? "pausa" : "reanudar", NULL);
		avisar(partida, NULL);
		break;
	case ACCION_REINICIAR:
		consola_reiniciar();
		sonido_vaciar();
		registro_evento("info", "reinicio", NULL);
		avisar(partida, "Reiniciado");
		break;
	case ACCION_GUARDAR_ESTADO:
		guardar_estado(partida);
		break;
	case ACCION_CARGAR_ESTADO:
		cargar_estado(partida);
		break;
	case ACCION_ELEGIR_RANURA:
		partida->ranura = ranura;
		snprintf(mensaje, sizeof(mensaje), "Ranura %d", ranura);
		avisar(partida, mensaje);
		break;
	case ACCION_CAPTURA:
		capturar(partida);
		break;
	case ACCION_NINGUNA:
		break;
	}
}
