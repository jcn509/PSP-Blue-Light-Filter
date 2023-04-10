#include "set_each_word_in_buffer.h"

void FillBuffer(uint32_t buffer[], const size_t buffer_width,
                const size_t screen_height) {
  SetEachWordInBuffer(buffer, buffer_width, screen_height, 0);
}