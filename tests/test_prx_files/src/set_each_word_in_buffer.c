#include "set_each_word_in_buffer.h"

#include <stddef.h>

#include "test_constants.h"


void SetEachWordInBuffer(const uint32_t value) {
	// We may end up setting many bytes beyond the end of the visible buffer but that's fine
    // as the emulator does not complain
    for(size_t offset = 0; offset < BUFFER_WIDTH * SCREEN_HEIGHT; offset++) {
        *((uint32_t*)VRAM_TOP + offset) = value; 
    }
}