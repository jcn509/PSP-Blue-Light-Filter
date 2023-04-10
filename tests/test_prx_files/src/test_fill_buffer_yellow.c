#include <pspdisplay.h>

#include "set_each_word_in_buffer.h"

extern const int k_pixel_format;

static uint32_t GetYellowWord(const int pixelformat) {
  switch (pixelformat) {
    // 06-bit RGB 5:6:5.
    case PSP_DISPLAY_PIXEL_FORMAT_565:
      return 0b00000111111111110000011111111111;
    // 06-bit RGBA 5:5:5:0.
    case PSP_DISPLAY_PIXEL_FORMAT_5551:
      return 0b10000011111111111000001111111111;
    case PSP_DISPLAY_PIXEL_FORMAT_4444:
      return 0xF0FFF0FF;
    // Must be PSP_DISPLAY_PIXEL_FORMAT_8888
    default:
      return 0xFF00FFFF;
  }
}

void FillBuffer(uint32_t buffer[], const size_t buffer_width,
                const size_t screen_height) {
  SetEachWordInBuffer(buffer, buffer_width, screen_height,
                      GetYellowWord(k_pixel_format));
}