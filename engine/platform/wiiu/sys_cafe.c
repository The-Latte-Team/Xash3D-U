/*
sys_cafe.c - wii u backend

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "platform/platform.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <SDL.h>
#include <vpad/input.h>
#include <coreinit/screen.h>
#include <coreinit/cache.h>
#include <whb/proc.h>
#include <whb/log_console.h>
#include <whb/log.h>
#include <whb/sdcard.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <coreinit/exit.h>
#include "dll_cafe.h"

void Platform_ShellExecute( const char *path, const char *parms )
{
	Con_Reportf( S_WARN "Tried to shell execute ;%s; -- not supported\n", path );
}

void Cafe_Init( void ) {
	WHBLogPrintf("---Platform-Init---");
	WHBLogConsoleDraw();
	
	WHBLogPrintf("Setting up dll hack...");
	WHBLogConsoleDraw();
	setup_dll_funcs();

	WHBLogPrintf("Done");
	WHBLogConsoleDraw();
}

void Cafe_Shutdown( void ) //Should I leave this function like this? No. I'm leaving it anyways? Yes.
{ }
