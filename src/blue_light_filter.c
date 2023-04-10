#include "blue_light_filter.h"

#include <pspdisplay.h>

static int GetPixelSizeBytes(const int pixelformat) {
  if (pixelformat == PSP_DISPLAY_PIXEL_FORMAT_8888) {
    return 4;
  }
  return 2;
}

static uint32_t GetBlueLightWordBitMap(const int pixelformat) {
  switch (pixelformat) {
    // 16-bit RGB 5:6:5.
    case PSP_DISPLAY_PIXEL_FORMAT_565:
      return 0b00000111111111110000011111111111;
    // 16-bit RGBA 5:5:5:1.
    case PSP_DISPLAY_PIXEL_FORMAT_5551:
      return 0b10000011111111111000001111111111;
    case PSP_DISPLAY_PIXEL_FORMAT_4444:
      return 0xF0FFF0FF;
    // Must be PSP_DISPLAY_PIXEL_FORMAT_8888
    default:
      return 0xFF00FFFF;
  }
}

void BlueLightFilter(uint8_t* const topaddr, const int bufferwidth,
                     const int pixelformat) {
  // bufferwidth is a power of 2 and must be positive
  const uint32_t bufferwidth_unsigned = (uint32_t)bufferwidth;

  // Currently not bothering to check if the size is different as its not clear
  // what we should do if a different size is set
  // if we do want this check it may be faster to hook sceDisplaySetMode
  // int mode_unused;
  const uint32_t width = 480;
  const uint32_t height = 272;
  // sceDisplayGetMode(&mode_unused, &width, &height);

  const uint32_t pixel_size_bytes = GetPixelSizeBytes(pixelformat);
  const uint32_t bytes_per_buffer_row = bufferwidth_unsigned * pixel_size_bytes;
  const uint32_t screen_width_bytes = width * pixel_size_bytes;
  const uint32_t buffer_size_bytes = height * bytes_per_buffer_row;

  const uint32_t blue_light_bitmap = GetBlueLightWordBitMap(pixelformat);

  // Process one word at a time for speed
  // It was easier to do the maths in terms of bytes so using char pointers
  for (uint8_t* current_word_row = topaddr;
       current_word_row < topaddr + buffer_size_bytes;
       current_word_row += bytes_per_buffer_row) {
    for (uint8_t* current_word = current_word_row;
         current_word < current_word_row + screen_width_bytes;
         current_word += sizeof(uint32_t)) {
      *((uint32_t*)current_word) &= blue_light_bitmap;
    }
  }
}
