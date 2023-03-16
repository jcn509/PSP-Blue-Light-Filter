#include <pspdisplay.h>



static inline int GetPixelsPerWord(const int pixelformat) {
	switch(pixelformat){
		// 16-bit RGB 5:6:5.
		case PSP_DISPLAY_PIXEL_FORMAT_565:
		// 16-bit RGBA 5:5:5:1.
		case PSP_DISPLAY_PIXEL_FORMAT_5551:
		case PSP_DISPLAY_PIXEL_FORMAT_4444:
			return 2;
		case PSP_DISPLAY_PIXEL_FORMAT_8888:
			return 1;
	}
}

static inline int GetPixelSizeBytes(const int pixelformat) {
	switch(pixelformat){
		// 16-bit RGB 5:6:5.
		case PSP_DISPLAY_PIXEL_FORMAT_565:
		// 16-bit RGBA 5:5:5:1.
		case PSP_DISPLAY_PIXEL_FORMAT_5551:
		case PSP_DISPLAY_PIXEL_FORMAT_4444:
			return 2;
		case PSP_DISPLAY_PIXEL_FORMAT_8888:
			return 4;
	}
}

static inline uint32_t GetBlueLightWordBitMap(const int pixelformat){
	switch(pixelformat){
		// 16-bit RGB 5:6:5.
		case PSP_DISPLAY_PIXEL_FORMAT_565:
            return 0b00000111111111110000011111111111;
		// 16-bit RGBA 5:5:5:1.
		case PSP_DISPLAY_PIXEL_FORMAT_5551:
			// Crappy blue light filter - just no blue
				return 0b10000011111111111000001111111111;
		case PSP_DISPLAY_PIXEL_FORMAT_4444:
			// Crappy blue light filter - just no blue
			return 0xF0FFF0FF;
		case PSP_DISPLAY_PIXEL_FORMAT_8888:
			// Crappy blue light filter - just no blue
			return 0xFF00FFFF;
	}
}

static inline void BlueLightFilter(void * const topaddr, const int bufferwidth, const int pixelformat) {
	const int pixels_per_word = GetPixelsPerWord(pixelformat);
	const int pixel_size_bytes = GetPixelSizeBytes(pixelformat);
	const uint32_t blue_light_bitmap = GetBlueLightWordBitMap(pixelformat);

	int mode_unused;
	int width = 480;
	int height = 272;
	sceDisplayGetMode(&mode_unused, &width, &height);

	// Process one word at a time for speed
	const int bytes_per_buffer_row = bufferwidth * pixel_size_bytes;
	for (int row_offset = 0; row_offset < (height * bytes_per_buffer_row); row_offset += bytes_per_buffer_row) {
		for (int column_offset = 0; column_offset < (width*4)/pixels_per_word; column_offset+=4) {
			const int word_offset = row_offset + column_offset;
			*((uint32_t*)(topaddr + word_offset)) &= blue_light_bitmap;			
		}
	}
}
