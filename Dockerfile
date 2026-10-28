FROM debian:bookworm-slim AS base
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential cmake git ca-certificates pkg-config libsdl2-dev \
    && rm -rf /var/lib/apt/lists/*

FROM base AS mgba
ARG MGBA_VERSION=0.10.5
RUN git clone --depth 1 --branch ${MGBA_VERSION} https://github.com/mgba-emu/mgba.git /tmp/mgba
RUN cmake -S /tmp/mgba -B /tmp/mgba/build \
      -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/mgba \
      -DBUILD_STATIC=ON -DBUILD_SHARED=OFF -DBUILD_QT=OFF -DBUILD_SDL=OFF \
      -DBUILD_GL=OFF -DBUILD_GLES2=OFF -DBUILD_GLES3=OFF -DUSE_EPOXY=OFF \
      -DUSE_FFMPEG=OFF -DUSE_ZLIB=OFF -DUSE_PNG=OFF -DUSE_LIBZIP=OFF \
      -DUSE_MINIZIP=OFF -DUSE_SQLITE3=OFF -DUSE_ELF=OFF -DUSE_LUA=OFF \
      -DUSE_EDITLINE=OFF -DUSE_DEBUGGERS=OFF -DUSE_GDB_STUB=OFF -DUSE_LZMA=OFF \
      -DUSE_DISCORD_RPC=OFF -DM_CORE_GB=OFF -DSKIP_GIT=ON \
    && cmake --build /tmp/mgba/build -j"$(nproc)" \
    && cmake --install /tmp/mgba/build

FROM base AS builder
COPY --from=mgba /opt/mgba /opt/mgba
WORKDIR /work
CMD ["make", "dist"]
