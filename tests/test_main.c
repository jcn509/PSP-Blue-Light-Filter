#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <pspdisplay.h>
#include <pspmoduleinfo.h>

#include "../src/bluelightfilter.h"

PSP_MODULE_INFO("BlueLightFilterTest", 0, 1, 0);

#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 272

uint8_t * const pg_vramtop=(uint8_t *)0x04000000;

extern const int k_pixel_format;
extern const bool k_filter_enabled;

static void FillWhite() {
	uint8_t * vram_ptr = pg_vramtop;
	// May need as many as 4 bytes per pixel
	memset (pg_vramtop, UINT8_MAX, 4 * 512 * SCREEN_HEIGHT);
}

static void CreateScreenReadyFile() {
	FILE * const fptr = fopen("ms0:/screen_ready.txt", "w");
	fputs("test", fptr);
	fclose(fptr);
}

int main(unsigned long args, void *argp) {
	// Initialize
	sceDisplaySetMode(0,SCREEN_WIDTH,SCREEN_HEIGHT);
    sceDisplaySetFrameBuf(0,0,0,1);

	sceDisplayWaitVblankStart();
	FillWhite();
	if(k_filter_enabled){
		BlueLightFilter(pg_vramtop, 512, k_pixel_format);
	}
	sceDisplaySetFrameBuf(pg_vramtop, 512, k_pixel_format, 1);

	// Previous frame should be displayed now
	sceDisplayWaitVblankStart();
	// Lets the test runner know that the frame is ready and it should take
	// a screenshot
	CreateScreenReadyFile();

	#define EMULATOR_DEVCTL__EMIT_SCREENSHOT 0x00000020
	sceIoDevctl("kemulator:", EMULATOR_DEVCTL__EMIT_SCREENSHOT, NULL, 0, NULL, 0);

	// // Don't want this test to exit if we run it on real hardware
	// for(;;) {}

	return 0;
}