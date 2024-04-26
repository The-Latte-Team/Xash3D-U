/*
ref_common.h - Xash3D render dll API
Copyright (C) 2019 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#pragma once
#if !defined REF_COMMON_H && !defined REF_DLL
#define REF_COMMON_H

#include "ref_api.h"

#define RP_LOCALCLIENT( e )	((e) != NULL && (e)->index == ( cl.playernum + 1 ) && e->player )

struct ref_state_s
{
	qboolean initialized;

	HINSTANCE hInstance;
	ref_interface_t dllFuncs;

	// depends on build configuration
	int numRenderers;
	const char **shortNames;
	const char **readableNames;
};

extern struct ref_state_s ref;
extern ref_globals_t refState;

// handy API wrappers
void R_GetTextureParmss( int *w, int *h, int texnum );
#define REF_GET_PARM( parm, arg ) ref.dllFuncs.RefGetParm( (parm), (arg) )
#define GL_LoadTextureInternal( name, pic, flags ) ref.dllFuncs.GL_LoadTextureFromBuffer( (name), (pic), (flags), false )
#define GL_UpdateTextureInternal( name, pic, flags ) ref.dllFuncs.GL_LoadTextureFromBuffer( (name), (pic), (flags), true )
#define R_GetBuiltinTexture( name ) ref.dllFuncs.GL_LoadTexture( (name), 0, 0, 0 )

void GL_RenderFrame( const struct ref_viewpass_s *rvp );

// common engine and renderer cvars
extern convar_t	r_decals;
extern convar_t	r_adjust_fov;
extern convar_t gl_clear;
//vars that help with compiling
extern convar_t	*r_fullbright;
extern convar_t	*r_norefresh;
extern convar_t	*tracerred;
extern convar_t	*tracergreen;
extern convar_t	*tracerblue;
extern convar_t	*traceralpha;
extern convar_t	*r_sprite_lighting;
extern convar_t	*r_sprite_lerping;
extern convar_t	*r_drawviewmodel;
extern convar_t	*r_dynamic;
extern convar_t	*r_lightmap;
extern convar_t	*r_lighting_modulate;
extern convar_t	*r_glowshellfreq;
extern convar_t	*r_speeds;
extern convar_t	*r_drawentities;
extern convar_t	*cl_himodels;

qboolean Init( void );
void Shutdown( void );

extern triangleapi_t gTriApi;

#endif // REF_COMMON_H
