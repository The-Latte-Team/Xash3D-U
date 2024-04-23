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

#if XASH_EMSCRIPTEN
#ifdef EMSCRIPTEN_LIB_FS
	// For some unknown reason emscripten refusing to load libraries later
	COM_LoadLibrary( "menu", 0 );
	COM_LoadLibrary( "server", 0 );
	COM_LoadLibrary( "client", 0 );
#endif
#if XASH_DEDICATED
	// NodeJS support for debug
	EM_ASM(try {
		FS.mkdir( '/xash' );
		FS.mount( NODEFS, { root: '.'}, '/xash' );
		FS.chdir( '/xash' );
	} catch( e ) { };);
#endif
#elif XASH_IOS
	IOS_LaunchDialog();
#endif

	return Host_Main( szArgc, szArgv, game, 0, Sys_ChangeGame );
}

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

// Global variables
bool launcherRunning = true;

char **foundWads = NULL;
int foundWadsCount = 0;
int selectedWadIndex = 0;

/*void launcherUpdate(VPADStatus status)
{
    if (foundWadsCount > 0)
    {
        if (status.trigger & (VPAD_BUTTON_UP | VPAD_STICK_L_EMULATION_UP))
            selectedWadIndex--;
        if (status.trigger & (VPAD_BUTTON_DOWN | VPAD_STICK_L_EMULATION_DOWN))
            selectedWadIndex++;

        if (selectedWadIndex < 0)
            selectedWadIndex = foundWadsCount - 1;
        else if (selectedWadIndex >= foundWadsCount)
            selectedWadIndex = 0;
    }
}*/

/*void launcherDraw(OSScreenID screenID)
{
    OSScreenPutFontEx(screenID, 0, 0, "Xash3DU");

    if (foundWadsCount > 0)
    {
        OSScreenPutFontEx(screenID, 0, 2, "Press up and down to select a WAD");
        OSScreenPutFontEx(screenID, 0, 3, "Press + to start playing");
        for (int i = 0; i < foundWadsCount; i++)
        {
            int y = i + 5;
            OSScreenPutFontEx(screenID, 5, y, foundWads[i]);
            if (selectedWadIndex == i)
                OSScreenPutFontEx(screenID, 2, y, ">>");
        }
    }
    else
    {
        OSScreenPutFontEx(screenID, 5, 2, "No WAD files found!");
        OSScreenPutFontEx(screenID, 5, 3, "Put your WADs in: sd:/" HOMEBREW_APP_PATH "/wads");
        OSScreenPutFontEx(screenID, 0, 5, "Press + to exit");
    }
}*/

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
    // Scan for WADs
    foundWads = NULL;
    foundWadsCount = 0;
    struct dirent *files;
    DIR *dir = opendir(HOMEBREW_APP_PATH "/valve");
    if (dir != NULL)
    {
        while ((files = readdir(dir)) != NULL)
        {
            foundWadsCount++;
            foundWads = realloc(foundWads, foundWadsCount * sizeof(char *));
            foundWads[foundWadsCount - 1] = strdup(files->d_name);
        }
        valveFolderAvailable = true;
        /*szArgc = argc;
	    szArgv = argv;
	    return Sys_Start();*/
        closedir(dir);
    }
    else
    {
        WHBLogPrintf("No Valve folder found!");
        WHBLogPrintf("Put your Valve folder files in: sd:/" HOMEBREW_APP_PATH "/valve");
        WHBLogPrintf("Reopen the app when you have them on the SD Card");
        valveFolderAvailable = false;

        WHBLogConsoleDraw();
    }
    //WHBLogPrintf("Press + to start playing");

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
	        Sys_Start();

            displayed = true;
        }
        //launcherUpdate(status);
    }

    WHBLogConsoleFree();

    if (foundWadsCount > 0)
    {
        if (wbhRunning)
        {
            // Build up "myargc" and "myargv" from stuff user selected
            /*char *origArgv0 = myargv[0];

            myargc = 3;
            myargv = malloc(myargc * sizeof(char *));

            myargv[0] = origArgv0;
            myargv[1] = "-iwad";
            myargv[2] = strdup(foundWads[selectedWadIndex]);*/
        }

        // Free found WADs
        for (int i = 0; i < foundWadsCount; i++)
        {
            free(foundWads[i]);
        }
        free(foundWads);
    }

    if (!wbhRunning || foundWadsCount == 0)
    {
        WHBProcShutdown();
        exit(0);
    }
}

#endif // __WIIU__