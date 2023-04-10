#include <pspdisplay.h>
#include <pspiofilemgr.h>
#include <pspmoduleinfo.h>
#include <stdbool.h>
#include <stddef.h>

#include "blue_light_filter.h"

PSP_MODULE_INFO("BlueLightFilterTest", 0, 1, 0);

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272
// Must be a power of 2
#define BUFFER_WIDTH 512

#define VRAM_TOP (void*)0x04000000

extern const int k_pixel_format;
extern const bool k_filter_enabled;

void FillBuffer(uint32_t buffer[], size_t buffer_width, size_t screen_height);

int main() {
  // Initialize
  sceDisplaySetMode(0, SCREEN_WIDTH, SCREEN_HEIGHT);
  sceDisplaySetFrameBuf(0, 0, 0, 1);

  sceDisplayWaitVblankStart();
  // Buffer width must be a power of 2 and is larger than the visible screen
  // width here
  FillBuffer(VRAM_TOP, BUFFER_WIDTH, SCREEN_HEIGHT);
  if (k_filter_enabled) {
    BlueLightFilter(VRAM_TOP, BUFFER_WIDTH, k_pixel_format);
  }
  sceDisplaySetFrameBuf(VRAM_TOP, BUFFER_WIDTH, k_pixel_format, 1);

  // Previous frame should be displayed now
  sceDisplayWaitVblankStart();

#define EMULATOR_DEVCTL__EMIT_SCREENSHOT 0x00000020
  sceIoDevctl("kemulator:", EMULATOR_DEVCTL__EMIT_SCREENSHOT, NULL, 0, NULL, 0);

  return 0;
}