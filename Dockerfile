FROM ubuntu:22.04

ENV DEBIAN_FRONTEND="noninteractive"
ENV PSPDEV /usr/local/pspdev
ENV PATH ${PATH}:${PSPDEV}/bin

RUN \
  apt-get update && \
  apt-get install -y \
    build-essential \
    cmake \
    clang-tidy-15 \
    fakeroot \
    git \
    libarchive-tools \
    libgpgme11 \
    libreadline8 \
    libusb-0.1 \
    pkgconf \
    wget && \
apt-get -y install autoconf automake bison bzip2 cmake doxygen flex g++ gcc git gzip libarchive-dev libcurl4-openssl-dev libelf-dev libgpgme-dev libncurses5-dev libreadline-dev libssl-dev libtool-bin libusb-dev m4 make patch pkg-config python3 python3-venv subversion tar tcl texinfo unzip wget xz-utils && \
    apt-get -y install libsdl2-dev libgl1-mesa-dev libglu1-mesa-dev && \
  rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN git clone --recursive --branch v1.14 --depth 1 https://github.com/hrydgard/ppsspp.git
RUN cd ppsspp && \
    cmake \
        -DHEADLESS=On \
        -DCMAKE_INSTALL_PREFIX=/usr \
		-DCMAKE_BUILD_TYPE=Release \
		-DUSE_SYSTEM_FFMPEG=OFF \
		-DUSE_SYSTEM_LIBPNG=OFF \
		-DUSE_SYSTEM_LIBSDL2=ON \
		-DUSE_SYSTEM_LIBZIP=OFF \
		-DUSE_SYSTEM_MINIUPNPC=OFF \
		-DUSE_SYSTEM_SNAPPY=OFF \
		-DUSE_SYSTEM_ZSTD=OFF \
		-DUSING_QT_UI=OFF \
		-DUSING_GLES2=ON \
		-DUSING_EGL=ON \
        . && \
    make -j"$(nproc)" PPSSPPHeadless

RUN \
  cd /usr/local && \
  wget https://github.com/pspdev/pspdev/releases/download/latest/pspdev-ubuntu-latest.tar.gz && \
  tar xvf pspdev-ubuntu-latest.tar.gz && \
  rm pspdev-ubuntu-latest.tar.gz

RUN apt update && apt install -y python3 python3-pip && pip install pillow pytest xvfbwrapper

ENV PATH ${PATH}:/ppsspp