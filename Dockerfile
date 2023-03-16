FROM pspdev/pspdev:latest 

RUN apk update && \
    apk add \
        cmake \
        python3 \
        xvfb && \
    apk add ppsspp --repository=http://dl-cdn.alpinelinux.org/alpine/edge/testing/

RUN python3 -m ensurepip && \
    python3 -m pip install --upgrade pip && \
    python3 -m pip install \
        pillow \
        pytest \
        xvfbwrapper
