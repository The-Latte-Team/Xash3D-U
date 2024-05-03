/*
main.c - Wii U Xash3D Launcher

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifdef __WIIU__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <malloc.h>
#include <dirent.h>
#include <unistd.h>

#include <vpad/input.h>
#include <coreinit/screen.h>
#include <coreinit/cache.h>
#include <whb/proc.h>
#include <whb/log_console.h>
#include <whb/log.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <whb/sdcard.h>
#include "cafe_utils.h"
#include "dll_cafe.h"
#include "crtlib.h"

#include <SDL.h>
#include "SDL_audio.h"
#include <SDL2/SDL_image.h>

#define HOMEBREW_APP_PATH "wiiu/apps/xash3DU"

#if XASH_EMSCRIPTEN
#include <emscripten.h>
#endif
#include "build.h"
#include "common.h"
#include "vid_common.h"
#include "ref_common.h"

#define E_GAME	"XASH3D_GAME" // default env dir to start from
#ifndef XASH_GAMEDIR
#define XASH_GAMEDIR	"wiiu/apps/xash3DU/valve"
#endif

#if XASH_WIN32
#error "Single-binary or dedicated builds aren't supported for Windows!"
#endif

#undef XASH_DLL_LOADER

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

    WHBLogPrintf("Loading game...");
    WHBLogConsoleDraw();

	return Host_Main( szArgc, szArgv, game, 0, Sys_ChangeGame );
}

int main(int argc, char **argv)
{
    // Init launcher
    WHBProcInit();
    WHBLogConsoleInit();
    WHBLogConsoleSetColor(0x000000);

    WHBLogPrintf("Half-Life");
    WHBLogConsoleDraw();

    VPADStatus status;
    VPADReadError error;
    bool vpad_fatal = false;

    /*SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *img = NULL;
	int w, h;*/

    //Valve intro shit
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    /*win = SDL_CreateWindow("n/a", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN ); //What if
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);

    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8 *wavBuffer;
    
    SDL_LoadWAV("wiiu/apps/xash3DU/valve.wav", &wavSpec, &wavBuffer, &wavLength);
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

    int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);*/

    bool valveFolderAvailable = false;
    
    DIR *dir = opendir(HOMEBREW_APP_PATH "/valve");
    if (dir != NULL)
    {
        valveFolderAvailable = true;
        closedir(dir);
    }
    else
    {
        WHBLogConsoleSetColor(0x993333FF);
        WHBLogPrintf("No Valve folder found!");
        WHBLogPrintf("Put your Valve folder files in: sd:/" HOMEBREW_APP_PATH "/valve");
        valveFolderAvailable = false;

        WHBLogConsoleDraw();
    }

    bool displayed = false;
    /*bool playedAudio = false;

    int freakingAlpha = 0;
    bool fadeIn = true;
    bool fadeOut = false;*/

    while (WHBProcIsRunning())
    {
        /*img = IMG_LoadTexture(renderer, "wiiu/apps/xash3DU/valve.png");
	    SDL_QueryTexture(img, NULL, NULL, &w, &h);

	    SDL_Rect texr; 
        texr.x = (1280 - w) / 2;
        texr.y = (720 - h) / 2;
        texr.w = w;
        texr.h = h;
        if(fadeIn && !fadeOut){
            if(freakingAlpha < 255){
                freakingAlpha += 0.01;
            }
            else if(freakingAlpha >= 255){
                freakingAlpha == 255;
                SDL_Delay(6500);
                fadeIn = false;
                fadeOut = true;
            }
        }
        if(fadeOut && !fadeIn){
            freakingAlpha -= 0.01;
        }

        if (img) {
            // Set the alpha of the texture
            SDL_SetTextureAlphaMod(img, freakingAlpha);
        }

        SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, img, NULL, &texr);
		SDL_RenderPresent(renderer);

        if(!playedAudio){
            SDL_PauseAudioDevice(deviceId, 0);

            SDL_Delay(10000);

            SDL_CloseAudioDevice(deviceId);
            SDL_FreeWAV(wavBuffer);
            playedAudio = true;

            SDL_SetTextureAlphaMod(img, 0x00);
            SDL_DestroyTexture(img);
            WHBLogPrintf(" real");
            WHBLogConsoleDraw();
        }*/

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

        char *valveSDCardPath;
        if(valveFolderAvailable && !displayed){
            valveSDCardPath = WHBGetSdCardMountPath();
            Q_strncpy(valveSDCardPath, "/wiiu/apps/xash3DU/valve/", sizeof( valveSDCardPath ));
            chdir(valveSDCardPath);

            //Launch the game
            szArgc = argc;
	        szArgv = argv;
	        Sys_Start();


            WHBLogPrintf("If we're here, game didn't load");
            WHBLogConsoleDraw();
            displayed = true;
        }
    }

    SDL_Quit();
    WHBLogConsoleFree();
    WHBUnmountSdCard();
    WHBProcShutdown();
    return 0;
}

#endif // __WIIU__