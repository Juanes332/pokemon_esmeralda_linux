#include "consola.h"

#include "../shared/registro.h"

#include <fcntl.h>
#include <stdio.h>
#include <mgba/core/blip_buf.h>
#include <mgba/core/core.h>
#include <mgba/core/log.h>
#include <mgba/core/serialize.h>
#include <mgba/gba/core.h>
#include <mgba-util/vfs.h>

static struct mCore* nucleo;
static color_t video[CONSOLA_ANCHO * CONSOLA_ALTO];
static blip_t* canal_izq;
static blip_t* canal_der;

/* El log de mGBA por defecto imprime cada acceso raro a memoria; solo interesan los errores. */
static void registrar_mgba(struct mLogger* logger, int categoria, enum mLogLevel nivel, const char* fmt, va_list args) {
	(void) logger;
	if (!(nivel & (mLOG_FATAL | mLOG_ERROR))) {
		return;
	}
	char mensaje[256];
	vsnprintf(mensaje, sizeof(mensaje), fmt, args);
	for (char* c = mensaje; *c; ++c) {
		if (*c == '"' || *c == '\\' || (unsigned char) *c < 0x20) {
			*c = '\'';
		}
	}
	registro_evento("error", "mgba", "\"categoria\":\"%s\",\"mensaje\":\"%s\"", mLogCategoryName(categoria), mensaje);
}

static struct mLogger logger_mgba = { .log = registrar_mgba };

bool consola_iniciar(const void* rom, size_t tam, const char* ruta_guardado, int tasa_audio) {
	mLogSetDefaultLogger(&logger_mgba);
	nucleo = GBACoreCreate();
	if (!nucleo || !nucleo->init(nucleo)) {
		registro_evento("error", "nucleo_no_iniciado", NULL);
		return false;
	}
	mCoreInitConfig(nucleo, NULL);
	nucleo->setVideoBuffer(nucleo, video, CONSOLA_ANCHO);
	nucleo->setAudioBufferSize(nucleo, 4096);

	/* Copia escribible: el RTC del cartucho (GPIO) escribe sus pines dentro del espacio de la ROM. */
	if (!nucleo->loadROM(nucleo, VFileMemChunk(rom, tam))) {
		registro_evento("error", "rom_no_cargada", "\"bytes\":%zu", tam);
		return false;
	}
	struct VFile* guardado = VFileOpen(ruta_guardado, O_CREAT | O_RDWR);
	if (!guardado || !nucleo->loadSave(nucleo, guardado)) {
		registro_evento("warn", "guardado_no_disponible", "\"ruta\":\"%s\"", ruta_guardado);
	}

	canal_izq = nucleo->getAudioChannel(nucleo, 0);
	canal_der = nucleo->getAudioChannel(nucleo, 1);
	blip_set_rates(canal_izq, nucleo->frequency(nucleo), tasa_audio);
	blip_set_rates(canal_der, nucleo->frequency(nucleo), tasa_audio);

	nucleo->reset(nucleo);
	registro_evento("info", "rom_cargada", "\"bytes\":%zu,\"guardado\":\"%s\"", tam, ruta_guardado);
	return true;
}

void consola_frame(uint32_t teclas) {
	nucleo->setKeys(nucleo, teclas);
	nucleo->runFrame(nucleo);
}

const uint32_t* consola_video(void) {
	return video;
}

size_t consola_leer_audio(int16_t* destino, size_t max_pares) {
	int disponibles = blip_samples_avail(canal_izq);
	if (disponibles <= 0) {
		return 0;
	}
	if ((size_t) disponibles > max_pares) {
		disponibles = (int) max_pares;
	}
	blip_read_samples(canal_izq, destino, disponibles, 1);
	blip_read_samples(canal_der, destino + 1, disponibles, 1);
	return (size_t) disponibles;
}

bool consola_guardar_estado(const char* ruta) {
	struct VFile* vf = VFileOpen(ruta, O_CREAT | O_TRUNC | O_RDWR);
	if (!vf) {
		return false;
	}
	bool ok = mCoreSaveStateNamed(nucleo, vf, SAVESTATE_SCREENSHOT | SAVESTATE_RTC | SAVESTATE_METADATA);
	vf->close(vf);
	return ok;
}

bool consola_cargar_estado(const char* ruta) {
	struct VFile* vf = VFileOpen(ruta, O_RDONLY);
	if (!vf) {
		return false;
	}
	bool ok = mCoreLoadStateNamed(nucleo, vf, SAVESTATE_RTC);
	vf->close(vf);
	return ok;
}

void consola_reiniciar(void) {
	nucleo->reset(nucleo);
}

void consola_cerrar(void) {
	if (!nucleo) {
		return;
	}
	nucleo->unloadROM(nucleo);
	nucleo->deinit(nucleo);
	nucleo = NULL;
}
