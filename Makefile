MGBA ?= /opt/mgba
BIN := dist/pokemon-esmeralda

CFLAGS ?= -O2 -g -Wall -Wextra -std=gnu11
CFLAGS += -I$(MGBA)/include $(shell sdl2-config --cflags)
LDLIBS := $(MGBA)/lib/libmgba.a $(shell sdl2-config --libs) -lm -lpthread

FUENTES := $(wildcard src/*/*.c)
OBJETOS := $(FUENTES:src/%.c=build/%.o) build/rom/rom_embebida.o

.PHONY: dist test clean

dist: $(BIN)

$(BIN): $(OBJETOS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(LDLIBS)

build/%.o: src/%.c $(wildcard src/*/*.h)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

build/rom/rom_embebida.o: src/rom/rom_embebida.S build/juego.gba
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $<

# El nombre de la ROM lleva espacios, que make no sabe manejar como dependencia.
build/juego.gba:
	@mkdir -p build
	@rom="$$(ls *.gba | head -n1)"; test -n "$$rom" || { echo "No hay ninguna ROM .gba en la raíz" >&2; exit 1; }; \
	  echo "ROM: $$rom"; cp "$$rom" $@

test: $(BIN)
	./tests/humo.sh $(BIN)

clean:
	rm -rf build dist
