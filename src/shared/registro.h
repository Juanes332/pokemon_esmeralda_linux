#pragma once

/* campos_fmt produce un fragmento JSON (p. ej. "\"frames\":%d") o NULL. */
void registro_evento(const char* nivel, const char* evento, const char* campos_fmt, ...);
