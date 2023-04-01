// Code based on https://github.com/ferion11/remote-joy-lite-fix-DA
// and https://wololo.net/talk/viewtopic.php?t=46000

#include <string.h>

#include "hook_display.h"
#include "sctrl.h"

PSP_MODULE_INFO("PSPBlueLightFilter", PSP_MODULE_KERNEL, 0, 1);


static STMOD_HANDLER previous = NULL;
static u32 nextmod = 0;

static int module_start_handler(SceModule2 * module)
{
	int ret = previous ? previous(module) : 0;

	//signal that the main game module has loaded.
	if (nextmod == 1)
		nextmod++;

	//signal that sceKernelLibrary has loaded, the next module should be the main game module.
	if (!strcmp(module->modname, "sceKernelLibrary"))
		nextmod++;

	return ret;
}

static int thread_start(SceSize /*args*/, void */*argp*/)
{
	previous = sctrlHENSetStartModuleHandler(module_start_handler);

	//wait until the main game module starts
	while (nextmod != 2)
		sceKernelDelayThread(1000);

	HookDisplayAndInstallFilter();
	
	return 0;
}

int module_start(SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("ScanLine_Main", thread_start, 0x22, 0x2000, 0, NULL);

	if (thid >= 0)
		sceKernelStartThread(thid, args, argp);

	return 0;
}

int module_stop(SceSize /*args*/, void */*argp*/)
{
	return 0;
}
