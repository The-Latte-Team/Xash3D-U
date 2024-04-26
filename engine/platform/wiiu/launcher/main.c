//
// Copyright(C) 2021 Terry Hearst
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
//	Wii U Launcher/WAD picker implementation
//

#ifdef __WIIU__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <malloc.h>
#include <dirent.h>

#include <vpad/input.h>
#include <coreinit/screen.h>
#include <coreinit/cache.h>
#include <whb/proc.h>
#include <whb/log_console.h>
#include <whb/log.h>

#define HOMEBREW_APP_PATH "wiiu/apps/xash3DU"

#if XASH_SDLMAIN
#include <SDL.h>
#endif
#if XASH_EMSCRIPTEN
#include <emscripten.h>
#endif
#include "build.h"
#include "common.h"
#include "vid_common.h"

#define E_GAME	"XASH3D_GAME" // default env dir to start from
#ifndef XASH_GAMEDIR
#define XASH_GAMEDIR	"wiiu/apps/xash3DU/valve"
#endif

#if XASH_WIN32
#error "Single-binary or dedicated builds aren't supported for Windows!"
#endif

static char        szGameDir[128]; // safe place to keep gamedir
static int         szArgc;
static char        **szArgv;

// Global variables
bool launcherRunning = true;

static void Sys_ChangeGame( const char *progname )
{
	// a1ba: may never be called within engine
	// if platform supports execv() function
	Q_strncpy( szGameDir, progname, sizeof( szGameDir ));
	Host_Shutdown( );
	exit( Host_Main( szArgc, szArgv, szGameDir, 1, &Sys_ChangeGame ) );
}

static int Sys_Start( void )
{
	const char *game = getenv( E_GAME );

	if( !game )
		game = XASH_GAMEDIR;

	Q_strncpy( szGameDir, game, sizeof( szGameDir ));

	return Host_Main( szArgc, szArgv, game, 0, Sys_ChangeGame );
}

int main(int argc, char **argv)
{
    // Init launcher
    WHBProcInit();
    WHBLogConsoleInit();
    WHBLogConsoleSetColor(0x000000);

    WHBLogPrintf("Xash3D U");
    WHBLogConsoleDraw();

    VPADStatus status;
    VPADReadError error;
    bool vpad_fatal = false;

    bool valveFolderAvailable = false;
    
    glw_state.software = true; //force it to be always software
    DIR *dir = opendir(HOMEBREW_APP_PATH "/valve");
    if (dir != NULL)
    {
        valveFolderAvailable = true;
        closedir(dir);
    }
    else
    {
        WHBLogPrintf("No Valve folder found!");
        WHBLogPrintf("Put your Valve folder files in: sd:/" HOMEBREW_APP_PATH "/valve");
        valveFolderAvailable = false;

        WHBLogConsoleDraw();
    }

    // I need this variable because with out it, WHBProcIsRunning becomes true
    // again before exiting, causing a crash
    int wbhRunning = true;
    bool displayed = false;

    while (launcherRunning && (wbhRunning = WHBProcIsRunning()))
    {
        // Poll input
        VPADRead(VPAD_CHAN_0, &status, 1, &error);
        switch (error) {
            case VPAD_READ_SUCCESS: {
                break;
            }
            case VPAD_READ_NO_SAMPLES: {
                continue;
            }
            case VPAD_READ_INVALID_CONTROLLER: {
                WHBLogPrint("Gamepad disconnected!");
                vpad_fatal = true;
                break;
            }
            default: {
                WHBLogPrintf("Unknown VPAD error! %08X", error);
                vpad_fatal = true;
                break;
            }
        }

        if (vpad_fatal) break;

        if(valveFolderAvailable && !displayed){
            WHBLogPrintf("Loading game...");
            WHBLogConsoleDraw();
            //Launch the game
            szArgc = argc;
	        szArgv = argv;
	        //Sys_Start(); //we don't wanna launch yet
            
            WHBLogPrintf("If we're here, game didn't load");
            displayed = true;
        }
    }

    WHBLogConsoleFree();

    if (!wbhRunning)
    {
        WHBProcShutdown();
        exit(0);
    }
}

#endif // __WIIU__