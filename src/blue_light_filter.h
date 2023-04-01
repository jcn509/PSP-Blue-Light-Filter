#pragma once

#include <stdint.h>

int GetPixelSizeBytes(const int pixelformat);

int GetPixelsPerWord(const int pixelformat);

uint32_t GetBlueLightWordBitMap(const int pixelformat);

void BlueLightFilter(void * const topaddr, const int bufferwidth, const int pixelformat);
