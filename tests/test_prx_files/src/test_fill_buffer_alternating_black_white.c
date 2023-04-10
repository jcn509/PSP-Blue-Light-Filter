#include <pspdisplay.h>
#include <stdbool.h>
#include <stddef.h>

#include "set_each_word_in_buffer.h"

extern const int k_pixel_format;

void FillBuffer(uint32_t buffer[], const size_t buffer_width,
                const size_t screen_height) {
  if (k_pixel_format == PSP_DISPLAY_PIXEL_FORMAT_8888) {
    bool odd = false;
    // May end up writing beyond the end of the visible part of the buffer but
    // that is OK as the emulator does not complain
    for (size_t offset = 0; offset < buffer_width * screen_height; offset++) {
      const uint32_t value = odd ? UINT32_MAX : 0;
      *(buffer + offset) = value;
      odd = !odd;
    }
  } else {
    SetEachWordInBuffer(buffer, buffer_width, screen_height, 0xFFFF0000);
  }
}