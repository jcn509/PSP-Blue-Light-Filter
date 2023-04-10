#!/bin/sh

mkdir build &&
cd build &&
psp-cmake -DCMAKE_BUILD_TYPE=Release .. &&
make
