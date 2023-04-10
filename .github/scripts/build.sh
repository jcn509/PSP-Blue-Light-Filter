#!/bin/sh

mkdir build &&
cd build &&
psp-cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. &&
ninja
