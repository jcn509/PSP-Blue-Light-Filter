#include "set_each_word_in_buffer.h"

#include <string.h>

void SetEachWordInBuffer(uint32_t buffer[], const size_t buffer_width,
                         const size_t screen_height, const uint32_t value) {
  // We may end up setting many bytes beyond the end of the visible buffer but
  // that's fine
  // as the emulator does not complain
  for (size_t offset = 0; offset < buffer_width * screen_height; offset++) {
    *(buffer + offset) = value;
  }
}