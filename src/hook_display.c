// Code based on https://github.com/ferion11/remote-joy-lite-fix-DA
// and https://wololo.net/talk/viewtopic.php?t=46000

#include <stddef.h>
#include <string.h>

#include <pspdisplay.h>
#include <pspkernel.h>

#include "blue_light_filter.h"


static int (*sceDisplaySetFrameBuf_Func)( void *, int, int, int ) = NULL;


static void *HookNidAddress( SceModule *mod, char *libname, u32 nid )
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


static void *HookSyscallAddress( void *addr )
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


static void HookFuncSetting( void *addr, void *entry )
{
	if ( addr == NULL ){ return; }

	((u32 *)addr)[0] = (u32)entry;
	sceKernelDcacheWritebackInvalidateRange( addr, sizeof( addr ) );
	sceKernelIcacheInvalidateRange( addr, sizeof( addr ) );
}


static int MyDisplaySetFrameBuf( void *topaddr, int bufferwidth, int pixelformat, int sync )
{
	BlueLightFilter(topaddr, bufferwidth, pixelformat);
	return sceDisplaySetFrameBuf_Func( topaddr, bufferwidth, pixelformat, sync );
}


void HookDisplayAndInstallFilter( void )
{
	SceModule *module = sceKernelFindModuleByName( "sceDisplay_Service" );
	if(module == NULL) {
		return;
	}

	if ( sceDisplaySetFrameBuf_Func == NULL ){
		sceDisplaySetFrameBuf_Func = HookNidAddress( module, "sceDisplay", 0x289D82FE );
		void *hook_addr = HookSyscallAddress( (void*)sceDisplaySetFrameBuf_Func );
		HookFuncSetting( hook_addr, (void*)MyDisplaySetFrameBuf );
	}
}