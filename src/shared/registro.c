#include "registro.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

void registro_evento(const char* nivel, const char* evento, const char* campos_fmt, ...) {
	char marca[32];
	time_t ahora = time(NULL);
	struct tm utc;
	gmtime_r(&ahora, &utc);
	strftime(marca, sizeof(marca), "%Y-%m-%dT%H:%M:%SZ", &utc);

	fprintf(stderr, "{\"ts\":\"%s\",\"nivel\":\"%s\",\"evento\":\"%s\"", marca, nivel, evento);
	if (campos_fmt) {
		va_list args;
		va_start(args, campos_fmt);
		fputc(',', stderr);
		vfprintf(stderr, campos_fmt, args);
		va_end(args);
	}
	fputs("}\n", stderr);
}
