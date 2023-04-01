#include <pspdisplay.h>

#include "set_each_word_in_buffer.h"

extern const int k_pixel_format;

static uint32_t GetBlueWord(const int pixelformat) {
    switch(pixelformat){
		// 16-bit RGB 5:6:5.
		case PSP_DISPLAY_PIXEL_FORMAT_565:
            return 0b11111000000000001111100000000000;
		// 16-bit RGBA 5:5:5:1.
		case PSP_DISPLAY_PIXEL_FORMAT_5551:
			return 0b01111100000000000111110000000000;
		case PSP_DISPLAY_PIXEL_FORMAT_4444:
			return 0x0F000F00;
		// Must be PSP_DISPLAY_PIXEL_FORMAT_8888
		default:
			return 0x00FF0000;
	}
}

void FillBuffer() {
	SetEachWordInBuffer(GetBlueWord(k_pixel_format));
}