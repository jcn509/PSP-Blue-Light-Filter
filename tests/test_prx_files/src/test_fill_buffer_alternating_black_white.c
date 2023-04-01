#include <stdbool.h>
#include <stddef.h>

#include <pspdisplay.h>

#include "set_each_word_in_buffer.h"
#include "test_constants.h"

extern const int k_pixel_format;

void FillBuffer() {
    if(k_pixel_format == PSP_DISPLAY_PIXEL_FORMAT_8888) {
	    bool even = false;
        // May end up writing beyond the end of the visible part of the buffer but that
        // is OK as the emulator does not complain
        for(size_t offset = 0; offset < BUFFER_WIDTH * SCREEN_HEIGHT; offset++) {
            const uint32_t value = even ? UINT32_MAX : 0;
            *((uint32_t*)VRAM_TOP + offset) = value;
            even = !even;
        }
    }
    else {
        SetEachWordInBuffer(0xFFFF0000);
    }
}