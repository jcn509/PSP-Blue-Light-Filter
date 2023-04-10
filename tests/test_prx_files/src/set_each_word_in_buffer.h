#pragma once

#include <stddef.h>
#include <stdint.h>

void SetEachWordInBuffer(uint32_t buffer[], size_t buffer_width,
                         size_t screen_height, uint32_t value);
