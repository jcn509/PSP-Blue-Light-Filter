FROM ubuntu:22.04

ENV DEBIAN_FRONTEND="noninteractive"
ENV PSPDEV /usr/local/pspdev
ENV PATH ${PATH}:${PSPDEV}/bin

# TODO: remove build dependencies once
# PPSSPP is built. Some of these packages
# probably aren't even needed...
RUN \
  apt-get update && \
  apt-get install -y \
    autoconf \
    automake \
    bison \
    build-essential \
    bzip2 \
    clang-format-15 \
    clang-tidy-15 \
    cmake \
    cppcheck \
    doxygen \
    fakeroot \
    flex \
    g++ \
    gcc \
    git \
    gzip \
    libarchive-dev \
    libarchive-tools \
    libcurl4-openssl-dev \
    libelf-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libgpgme-dev \
    libgpgme11 \
    libncurses5-dev \
    libreadline-dev \
    libreadline8 \
    libsdl2-dev \
    libssl-dev \
    libtool-bin \
    libusb-0.1 \
    libusb-dev \
    m4 \
    make \
    ninja-build \
    patch \
    pkgconf \
    python3 \
    python3-pip \
    python3-venv \
    subversion \
    tar \
    tcl \
    texinfo \
    unzip \
    wget \
    xz-utils && \
  rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN git clone --recursive --branch v1.14 --depth 1 https://github.com/hrydgard/ppsspp.git ppsspp_source && \
    mkdir ppsspp && \
    cd ppsspp && \
    cmake \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DHEADLESS=On \
        -DUSE_SYSTEM_FFMPEG=OFF \
        -DUSE_SYSTEM_LIBPNG=OFF \
        -DUSE_SYSTEM_LIBSDL2=ON \
        -DUSE_SYSTEM_LIBZIP=OFF \
        -DUSE_SYSTEM_MINIUPNPC=OFF \
        -DUSE_SYSTEM_SNAPPY=OFF \
        -DUSE_SYSTEM_ZSTD=OFF \
        -DUSING_EGL=ON \
        -DUSING_GLES2=ON \
        -DUSING_QT_UI=OFF \
        ../ppsspp_source && \
    ninja PPSSPPHeadless && \
    cd .. && \
    rm -rf ppsspp_source

ENV PATH ${PATH}:/ppsspp

RUN \
  cd /usr/local && \
  wget https://github.com/pspdev/pspdev/releases/download/latest/pspdev-ubuntu-latest.tar.gz && \
  tar xvf pspdev-ubuntu-latest.tar.gz && \
  rm pspdev-ubuntu-latest.tar.gz

RUN pip install \
  pytest \
  pytest-pylint
