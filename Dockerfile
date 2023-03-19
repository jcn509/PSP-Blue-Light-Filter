
FROM pspdev/pspdev:latest 


RUN apk update && \
    apk add \
        cmake \
        python3 \
        xvfb

RUN python3 -m ensurepip && \
    python3 -m pip install --upgrade pip && \
    python3 -m pip install \
        pillow \
        pytest \
        xvfbwrapper \
        meson


# # Install all needed deps and compile the mesa llvmpipe driver from source.
# RUN set -xe; \
#     apk --update add --no-cache --virtual .runtime-deps bison wayland-dev libxrandr-dev wayland-protocols flex glslang ninja xvfb llvm15-libs xdpyinfo; \
#     apk add --no-cache --virtual .build-deps llvm-dev build-base zlib-dev glproto xorg-server-dev python3-dev; \
#     python3 -m pip install mako ; \
#     mkdir -p /var/tmp/build; \
#     cd /var/tmp/build; \
#     wget --no-check-certificate "https://mesa.freedesktop.org/archive/mesa-23.0.0.tar.xz"; \
#     tar xfv mesa-23.0.0.tar.xz; \
#     rm mesa-23.0.0.tar.xz; \
#     cd mesa-23.0.0; \
#     mkdir _build ; \
#     # Not sure how to do -Denable-glx=gallium-xlib -Ddri=disabled
#     meson -Dgallium-drivers=swrast -Dgbm=disabled -Degl=disabled -Dosmesa=true -Dprefix=/usr/local _build ; \
#     cd _build ; \
#     meson configure ; \
#     ninja ; \
#     ninja install ; \
#     cd ../.. ; \
#     rm -rf mesa-23.0.0; \
#     apk del .build-deps;

# TODO: delete the source once done.
# TODO: uninstall build deps (git etc.)
# TODO: clear package cache
RUN apk add git
RUN git clone --recursive --branch v1.14.4 --depth 1 https://github.com/hrydgard/ppsspp.git
RUN apk add build-base wget git bash cmake directfb-dev python3 glu-dev sdl2-dev libstdc++ glu-dev && \
    export CFLAGS=-I/usr/include/directfb/ && \
    export CXXFLAGS=-I/usr/include/directfb/ && \
    cd ppsspp && \
    cmake -DHEADLESS=On . && \
    make -j"$(nproc)" PPSSPPHeadless