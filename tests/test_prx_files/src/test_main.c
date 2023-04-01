#include <stdbool.h>

#include <pspdisplay.h>
#include <pspmoduleinfo.h>
#include <pspiofilemgr.h>

#include "blue_light_filter.h"
#include "test_constants.h"

PSP_MODULE_INFO("BlueLightFilterTest", 0, 1, 0);

extern const int k_pixel_format;
extern const bool k_filter_enabled;

void FillBuffer();

int main() {
	// Initialize
	sceDisplaySetMode(0,SCREEN_WIDTH,SCREEN_HEIGHT);
    sceDisplaySetFrameBuf(0,0,0,1);

	sceDisplayWaitVblankStart();
	FillBuffer();
	if(k_filter_enabled){
		BlueLightFilter(VRAM_TOP, BUFFER_WIDTH, k_pixel_format);
	}
	sceDisplaySetFrameBuf(VRAM_TOP, BUFFER_WIDTH, k_pixel_format, 1);

	// Previous frame should be displayed now
	sceDisplayWaitVblankStart();

	#define EMULATOR_DEVCTL__EMIT_SCREENSHOT 0x00000020
	sceIoDevctl("kemulator:", EMULATOR_DEVCTL__EMIT_SCREENSHOT, NULL, 0, NULL, 0);

	return 0;
}