/*
 *  ScanLine
 *
 *  Copyright (C) 2016 Omega2058
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <psputilsforkernel.h>
#include <string.h>
#include <pspdisplay.h>
#include <pspsdk.h>
#include "sctrl.h"

#include "bluelightfilter.h"

PSP_MODULE_INFO("ScanLine", PSP_MODULE_KERNEL, 0, 1);

// Hook into sceDisplaySetFrameBuf and patch the frame buffer?
// should be fairly easy to do - steal code from remote joy lite?


// taken from begin
/*------------------------------------------------------------------------------*/
/* HookNidAddress																*/
/*------------------------------------------------------------------------------*/
void *HookNidAddress( SceModule *mod, char *libname, u32 nid )
{
	int i;

	u32 *ent_next = (u32 *)mod->ent_top;
	u32 *ent_end  = (u32 *)mod->ent_top + (mod->ent_size >> 2);

	while ( ent_next < ent_end ){
		SceLibraryEntryTable *ent = (SceLibraryEntryTable *)ent_next;
		if ( ent->libname != NULL ){
			if ( strcmp( ent->libname, libname ) == 0 ){
				int count = ent->stubcount + ent->vstubcount;
				u32 *nidt = ent->entrytable;
				for ( i=0; i<count; i++ ){
					if ( nidt[i] == nid ){ return( (void *)nidt[count+i] ); }
				}
			}
		}
		ent_next += ent->len;
	}
	return( NULL );
}

/*------------------------------------------------------------------------------*/
/* HookSyscallAddress															*/
/*------------------------------------------------------------------------------*/
void *HookSyscallAddress( void *addr )
{
	u32		**ptr;
	if ( addr == NULL ){ return( NULL ); }
	asm( "cfc0 %0, $12\n" : "=r"(ptr) );
	if ( ptr == NULL ){ return( NULL ); }

	int		i;
	u32		*tbl = ptr[0];
	int		size = (tbl[3]-0x10)/sizeof(u32);
	for ( i=0; i<size; i++ ){
		if ( tbl[4+i] == (u32)addr ){ return( &tbl[4+i] ); }
	}
	return( NULL );
}

/*------------------------------------------------------------------------------*/
/* HookFuncSetting																*/
/*------------------------------------------------------------------------------*/
void HookFuncSetting( void *addr, void *entry )
{
	if ( addr == NULL ){ return; }

	((u32 *)addr)[0] = (u32)entry;
	sceKernelDcacheWritebackInvalidateRange( addr, sizeof( addr ) );
	sceKernelIcacheInvalidateRange( addr, sizeof( addr ) );
}
// taken from end



// Taken from https://github.com/ferion11/remote-joy-lite-fix-DA/blob/master/RemoteJoyLite_psp/hook_display.c begin

/*------------------------------------------------------------------------------*/
/* work																			*/
/*------------------------------------------------------------------------------*/
static int (*sceDisplaySetFrameBuf_Func)( void *, int, int, int ) = NULL;
static int (*sceDisplayWaitVblank_Func)( void ) = NULL;
static int (*sceDisplayWaitVblankCB_Func)( void ) = NULL;
static int (*sceDisplayWaitVblankStart_Func)( void ) = NULL;
static int (*sceDisplayWaitVblankStartCB_Func)( void ) = NULL;

/*------------------------------------------------------------------------------*/
/* MyDisplaySetFrameBuf															*/
/*------------------------------------------------------------------------------*/
static int MyDisplaySetFrameBuf( void *topaddr, int bufferwidth, int pixelformat, int sync )
{
	// int k1 = pspSdkSetK1(0);
	// int interupts = pspSdkDisableInterrupts();
	BlueLightFilter(topaddr, bufferwidth, pixelformat);
	// pspSdkEnableInterrupts(interupts);
	// pspSdkSetK1(k1);
	int ret = sceDisplaySetFrameBuf_Func( topaddr, bufferwidth, pixelformat, sync );
	return( ret );
}

/*------------------------------------------------------------------------------*/
/* MyDisplayWaitVblank															*/
/*------------------------------------------------------------------------------*/
static int MyDisplayWaitVblank( void )
{
	//SendDispEvent();
	int ret = sceDisplayWaitVblank_Func();
	return( ret );
}

/*------------------------------------------------------------------------------*/
/* MyDisplayWaitVblankCB														*/
/*------------------------------------------------------------------------------*/
static int MyDisplayWaitVblankCB( void )
{
	//SendDispEvent();
	int ret = sceDisplayWaitVblankCB_Func();
	return( ret );
}

/*------------------------------------------------------------------------------*/
/* MyDisplayWaitVblankStart														*/
/*------------------------------------------------------------------------------*/
static int MyDisplayWaitVblankStart( void )
{
	//SendDispEvent();
	int ret = sceDisplayWaitVblankStart_Func();
	return( ret );
}

/*------------------------------------------------------------------------------*/
/* MyDisplayWaitVblankStartCB													*/
/*------------------------------------------------------------------------------*/
static int MyDisplayWaitVblankStartCB( void )
{
	//SendDispEvent();
	int ret = sceDisplayWaitVblankStartCB_Func();
	return( ret );
}

/*------------------------------------------------------------------------------*/
/* hookDisplay																	*/
/*------------------------------------------------------------------------------*/
void hookDisplay( void )
{
	SceModule *module = sceKernelFindModuleByName( "sceDisplay_Service" );
	if ( module == NULL ){ return; }

	if ( sceDisplaySetFrameBuf_Func == NULL ){
		sceDisplaySetFrameBuf_Func = HookNidAddress( module, "sceDisplay", 0x289D82FE );
		void *hook_addr = HookSyscallAddress( sceDisplaySetFrameBuf_Func );
		HookFuncSetting( hook_addr, MyDisplaySetFrameBuf );
	}

	// if ( sceDisplayWaitVblank_Func == NULL ){
	// 	sceDisplayWaitVblank_Func = HookNidAddress( module, "sceDisplay", 0x36CDFADE );
	// 	void *hook_addr = HookSyscallAddress( sceDisplayWaitVblank_Func );
	// 	HookFuncSetting( hook_addr, MyDisplayWaitVblank );
	// }

	// if ( sceDisplayWaitVblankCB_Func == NULL ){
	// 	sceDisplayWaitVblankCB_Func = HookNidAddress( module, "sceDisplay", 0x8EB9EC49 );
	// 	void *hook_addr = HookSyscallAddress( sceDisplayWaitVblankCB_Func );
	// 	HookFuncSetting( hook_addr, MyDisplayWaitVblankCB );
	// }

	// if ( sceDisplayWaitVblankStart_Func == NULL ){
	// 	sceDisplayWaitVblankStart_Func = HookNidAddress( module, "sceDisplay", 0x984C27E7 );
	// 	void *hook_addr = HookSyscallAddress( sceDisplayWaitVblankStart_Func );
	// 	HookFuncSetting( hook_addr, MyDisplayWaitVblankStart );
	// }

	// if ( sceDisplayWaitVblankStartCB_Func == NULL ){
	// 	sceDisplayWaitVblankStartCB_Func = HookNidAddress( module, "sceDisplay", 0x46F186C3 );
	// 	void *hook_addr = HookSyscallAddress( sceDisplayWaitVblankStartCB_Func );
	// 	HookFuncSetting( hook_addr, MyDisplayWaitVblankStartCB );
	// }
}

// taken from end

static STMOD_HANDLER previous = NULL;
u32 nextmod = 0;

int module_start_handler(SceModule2 * module)
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

int thread_start(SceSize args, void *argp)
{
	previous = sctrlHENSetStartModuleHandler(module_start_handler);

	//wait until the main game module starts
	while (nextmod != 2)
		sceKernelDelayThread(1000);

	hookDisplay();
	
	return 0;
}

int module_start(SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("ScanLine_Main", thread_start, 0x22, 0x2000, 0, NULL);

	if (thid >= 0)
		sceKernelStartThread(thid, args, argp);

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
