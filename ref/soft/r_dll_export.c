#include <stdlib.h>
/*
vid_sdl.c - SDL vid component
Copyright (C) 2018 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#include "r_local.h"

ref_api_t      gEngfuncs2;
ref_globals_t *gpGlobals2;
ref_client_t  *gp_cl2;
ref_host_t    *gp_host2;
gl_globals_t tr2;
ref_speeds_t r_stats2;
poolhandle_t r_temppool2;
viddef_t vid2;
static void GAME_EXPORT R_ClearScreen2( void )
{

}

static const byte * GAME_EXPORT R_GetTextureOriginalBuffer2( unsigned int idx )
{
	image_t *glt = R_GetTexture( idx );

	if( !glt || !glt->original || !glt->original->buffer )
		return NULL;

	return glt->original->buffer;
}

/*
=============
CL_FillRGBA

=============
*/
static void GAME_EXPORT CL_FillRGBA2( float _x, float _y, float _w, float _h, int r, int g, int b, int a )
{
	vid2.rendermode = kRenderTransAdd;
	_TriColor4ub(r,g,b,a);
	Draw_Fill(_x,_y,_w,_h);
}

/*
=============
pfnFillRGBABlend

=============
*/
static void GAME_EXPORT CL_FillRGBABlend2( float _x, float _y, float _w, float _h, int r, int g, int b, int a )
{
	vid2.rendermode = kRenderTransAlpha;
	_TriColor4ub(r,g,b,a);
	Draw_Fill(_x,_y,_w,_h);
}
void Mod_UnloadTextures( model_t *mod );

static qboolean GAME_EXPORT Mod_ProcessRenderData2( model_t *mod, qboolean create, const byte *buf )
{
	qboolean loaded = true;

	if( create )
	{


		switch( mod->type )
		{
			case mod_studio:
				 //Mod_LoadStudioModel( mod, buf, loaded );
				break;
			case mod_sprite:
				Mod_LoadSpriteModel( mod, buf, &loaded, mod->numtexinfo );
				break;
			case mod_alias:
				//Mod_LoadAliasModel( mod, buf, &loaded );
				break;
			case mod_brush:
				// Mod_LoadBrushModel( mod, buf, loaded );
				break;

			default: gEngfuncs2.Host_Error( "Mod_LoadModel: unsupported type %d\n", mod->type );
		}
	}

	if( loaded && gEngfuncs2.drawFuncs->Mod_ProcessUserData )
		gEngfuncs2.drawFuncs->Mod_ProcessUserData( mod, create, buf );

	if( !create )
		Mod_UnloadTextures( mod );

	return loaded;
}

static int GL_RefGetParm2( int parm, int arg )
{
	image_t *glt;

	switch( parm )
	{
	case PARM_TEX_WIDTH:
		glt = R_GetTexture( arg );
		return glt->width;
	case PARM_TEX_HEIGHT:
		glt = R_GetTexture( arg );
		return glt->height;
	case PARM_TEX_SRC_WIDTH:
		glt = R_GetTexture( arg );
		return glt->srcWidth;
	case PARM_TEX_SRC_HEIGHT:
		glt = R_GetTexture( arg );
		return glt->srcHeight;
	case PARM_TEX_GLFORMAT:
		glt = R_GetTexture( arg );
		return 0; //glt->format;
	case PARM_TEX_ENCODE:
		glt = R_GetTexture( arg );
		return 0; //glt->encode;
	case PARM_TEX_MIPCOUNT:
		glt = R_GetTexture( arg );
		return glt->numMips;
	case PARM_TEX_DEPTH:
		glt = R_GetTexture( arg );
		return glt->depth;
	case PARM_TEX_SKYBOX:
		Assert( arg >= 0 && arg < 6 );
		return tr2.skyboxTextures[arg];
	case PARM_TEX_SKYTEXNUM:
		return 0; //tr.skytexturenum;
	case PARM_TEX_LIGHTMAP:
		arg = bound( 0, arg, MAX_LIGHTMAPS - 1 );
		return tr2.lightmapTextures[arg];
	case PARM_TEX_TARGET:
		glt = R_GetTexture( arg );
		return 0; //glt->target;
	case PARM_TEX_TEXNUM:
		glt = R_GetTexture( arg );
		return 0; //glt->texnum;
	case PARM_TEX_FLAGS:
		glt = R_GetTexture( arg );
		return glt->flags;
	case PARM_TEX_MEMORY:
		return R_TexMemory();
	case PARM_ACTIVE_TMU:
		return  0; //glState.activeTMU;
	case PARM_LIGHTSTYLEVALUE:
		arg = bound( 0, arg, MAX_LIGHTSTYLES - 1 );
		return tr2.lightstylevalue[arg];
	case PARM_MAX_IMAGE_UNITS:
		return 0; //GL_MaxTextureUnits();
	case PARM_REBUILD_GAMMA:
		return 0;
	case PARM_SURF_SAMPLESIZE:
		if( arg >= 0 && arg < WORLDMODEL->numsurfaces )
			return gEngfuncs2.Mod_SampleSizeForFace( &WORLDMODEL->surfaces[arg] );
		return LM_SAMPLE_SIZE;
	case PARM_GL_CONTEXT_TYPE:
		return 0; //glConfig.context;
	case PARM_GLES_WRAPPER:
		return 0; //glConfig.wrapper;
	case PARM_STENCIL_ACTIVE:
		return 0; //glState.stencilEnabled;
	case PARM_SKY_SPHERE:
		return 0; // ref_soft doesn't support sky sphere
	case PARM_TEX_FILTERING:
		return 0; // ref_soft doesn't do filtering in general
	default:
		return ENGINE_GET_PARM_( parm, arg );
	}
	return 0;
}

static void GAME_EXPORT R_GetDetailScaleForTexture2( int texture, float *xScale, float *yScale )
{
	image_t *glt = R_GetTexture( texture );

	if( xScale ) *xScale = glt->xscale;
	if( yScale ) *yScale = glt->yscale;
}

static void GAME_EXPORT R_GetExtraParmsForTexture2( int texture, byte *red, byte *green, byte *blue, byte *density )
{
	image_t *glt = R_GetTexture( texture );

	if( red ) *red = glt->fogParams[0];
	if( green ) *green = glt->fogParams[1];
	if( blue ) *blue = glt->fogParams[2];
	if( density ) *density = glt->fogParams[3];
}


static void GAME_EXPORT R_SetCurrentEntity2( cl_entity_t *ent )
{
	RI.currententity = ent;

	// set model also
	if( RI.currententity != NULL )
	{
		RI.currentmodel = RI.currententity->model;
	}
}

static void GAME_EXPORT R_SetCurrentModel2( model_t *mod )
{
	RI.currentmodel = mod;
}

static float GAME_EXPORT R_GetFrameTime2( void )
{
	return tr2.frametime;
}

static const char * GAME_EXPORT GL_TextureName2( unsigned int texnum )
{
	return R_GetTexture( texnum )->name;
}

static const byte * GAME_EXPORT GL_TextureData2( unsigned int texnum )
{
	rgbdata_t *pic = R_GetTexture( texnum )->original;

	if( pic != NULL )
		return pic->buffer;
	return NULL;
}

static void Mod_BrushUnloadTextures2( model_t *mod )
{
	int i;


	gEngfuncs2.Con_Printf("Unloading world\n");
	tr2.map_unload = true;

	for( i = 0; i < mod->numtextures; i++ )
	{
		texture_t *tx = mod->textures[i];
		if( !tx || tx->gl_texturenum == tr2.defaultTexture )
			continue;	// free slot

		GL_FreeTexture( tx->gl_texturenum );	// main texture
		GL_FreeTexture( tx->fb_texturenum );	// luma texture
	}
}

void Mod_UnloadTextures2( model_t *mod )
{
	int		i, j;

	Assert( mod != NULL );

	switch( mod->type )
	{
	case mod_studio:
		//Mod_StudioUnloadTextures( mod->cache.data );
		break;
	case mod_alias:
		//Mod_AliasUnloadTextures( mod->cache.data );
		break;
	case mod_brush:
		Mod_BrushUnloadTextures2( mod );
		break;
	case mod_sprite:
		Mod_SpriteUnloadTextures2( mod->cache.data );
		break;
	default: gEngfuncs2.Host_Error( "Mod_UnloadModel: unsupported type %d\n", mod->type );
	}
}

static void GAME_EXPORT R_ProcessEntData2( qboolean allocate, cl_entity_t *entities, unsigned int max_entities )
{
	tr2.entities = entities;
	tr2.max_entities = max_entities;
}

static void GAME_EXPORT R_Flush2( unsigned int flags )
{
	// stub
}

// stubs

static void GAME_EXPORT GL_SetTexCoordArrayMode2( uint mode )
{

}

static void GAME_EXPORT GL_BackendStartFrame2( void )
{

}

static void GAME_EXPORT GL_BackendEndFrame2( void )
{

}


void GAME_EXPORT GL_SetRenderMode2(int mode)
{
	vid2.rendermode = mode;
	/// TODO: table shading/blending???
	/// maybe, setup block drawing function pointers here
}

static void GAME_EXPORT R_ShowTextures2( void )
{
	// textures undone too
}

void GAME_EXPORT R_SetupSky2(const char *skyboxname)
{

}

qboolean GAME_EXPORT VID_CubemapShot2(const char *base, uint size, const float *vieworg, qboolean skyshot)
{
	// cubemaps? in my softrender???
	return false;
}

void R_InitSkyClouds2(mip_t *mt, texture_t *tx, qboolean custom_palette)
{

}

static void GAME_EXPORT GL_SubdivideSurface2( model_t *mod, msurface_t *fa )
{

}

static void GAME_EXPORT DrawSingleDecal2(decal_t *pDecal, msurface_t *fa)
{

}

static void GAME_EXPORT GL_SelectTexture2(int texture)
{

}

static void GAME_EXPORT GL_LoadTexMatrixExt2(const float *glmatrix)
{

}

static void GAME_EXPORT GL_LoadIdentityTexMatrix2( void )
{

}

static void GAME_EXPORT GL_CleanUpTextureUnits2(int last)
{

}

static void GAME_EXPORT GL_TexGen2(unsigned int coord, unsigned int mode)
{

}

static void GAME_EXPORT GL_TextureTarget2(uint target)
{

}

void GAME_EXPORT Mod_SetOrthoBounds2(const float *mins, const float *maxs)
{

}

qboolean GAME_EXPORT R_SpeedsMessage2(char *out, size_t size)
{
	return false;
}

byte *GAME_EXPORT Mod_GetCurrentVis2( void )
{
	return NULL;
}

static const char *R_GetConfigName2( void )
{
	return "ref_soft"; // software specific cvars will go to ref_soft.cfg
}

static void* GAME_EXPORT R_GetProcAddress2( const char *name )
{
	return gEngfuncs2.GL_GetProcAddress( name );
}

ref_interface_t gReffuncs2 =
{
	R_Init,
	R_Shutdown,
	R_GetConfigName2,
	R_SetDisplayTransform,

	GL_SetupAttributes,
	GL_InitExtensions,
	GL_ClearExtensions,

	R_GammaChanged,
	R_BeginFrame,
	R_RenderScene,
	R_EndFrame,
	R_PushScene,
	R_PopScene,
	GL_BackendStartFrame2,
	GL_BackendEndFrame2,

	R_ClearScreen2,
	R_AllowFog,
	GL_SetRenderMode,

	R_AddEntity,
	CL_AddCustomBeam,
	R_ProcessEntData2,
	R_Flush2,

	R_ShowTextures2,

	R_GetTextureOriginalBuffer2,
	GL_LoadTextureFromBuffer,
	GL_ProcessTexture,
	R_SetupSky,

	R_Set2DMode,
	R_DrawStretchRaw,
	R_DrawStretchPic,
	R_DrawTileClear,
	CL_FillRGBA2,
	CL_FillRGBABlend2,
	R_WorldToScreen,

	VID_ScreenShot,
	VID_CubemapShot,

	R_LightPoint,

	R_DecalShoot,
	R_DecalRemoveAll,
	R_CreateDecalList,
	R_ClearAllDecals,

	R_StudioEstimateFrame,
	R_StudioLerpMovement,
	CL_InitStudioAPI,

	R_InitSkyClouds,
	GL_SubdivideSurface2,
	CL_RunLightStyles,

	R_GetSpriteParms,
	R_GetSpriteTexture,

	Mod_LoadMapSprite,
	Mod_ProcessRenderData2,
	Mod_StudioLoadTextures,

	CL_DrawParticles,
	CL_DrawTracers,
	CL_DrawBeams,
	R_BeamCull,

	GL_RefGetParm2,
	R_GetDetailScaleForTexture2,
	R_GetExtraParmsForTexture2,
	R_GetFrameTime2,

	R_SetCurrentEntity2,
	R_SetCurrentModel2,

	GL_FindTexture,
	GL_TextureName2,
	GL_TextureData2,
	GL_LoadTexture,
	GL_CreateTexture,
	GL_LoadTextureArray,
	GL_CreateTextureArray,
	GL_FreeTexture,

	DrawSingleDecal2,
	R_DecalSetupVerts,
	R_EntityRemoveDecals,

	R_UploadStretchRaw,

	GL_Bind,
	GL_SelectTexture2,
	GL_LoadTexMatrixExt2,
	GL_LoadIdentityTexMatrix2,
	GL_CleanUpTextureUnits2,
	GL_TexGen2,
	GL_TextureTarget2,
	GL_SetTexCoordArrayMode2,
	GL_UpdateTexSize,
	NULL,
	NULL,

	CL_DrawParticlesExternal,
	R_LightVec,
	R_StudioGetTexture,

	R_RenderFrame,
	Mod_SetOrthoBounds,
	R_SpeedsMessage,
	Mod_GetCurrentVis,
	R_NewMap,
	R_ClearScene,
	R_GetProcAddress2,

	TriRenderMode2,
	TriBegin,
	TriEnd,
	_TriColor4f,
	_TriColor4ub,
	TriTexCoord2f,
	TriVertex3fv,
	TriVertex3f,
	TriFog,
	R_ScreenToWorld,
	TriGetMatrix,
	TriFogParams,
	TriCullFace2,

	VGUI_DrawInit,
	VGUI_DrawShutdown,
	VGUI_SetupDrawingText,
	VGUI_SetupDrawingRect,
	VGUI_SetupDrawingImage,
	VGUI_BindTexture,
	VGUI_EnableTexture,
	VGUI_CreateTexture,
	VGUI_UploadTexture,
	VGUI_UploadTextureBlock,
	VGUI_DrawQuad,
	VGUI_GetTextureSizes,
	VGUI_GenerateTexture,
};

int EXPORT GetRefAPI2( int version, ref_interface_t *funcs, ref_api_t *engfuncs, ref_globals_t *globals );
int EXPORT GetRefAPI2( int version, ref_interface_t *funcs, ref_api_t *engfuncs, ref_globals_t *globals )
{
	if( version != REF_API_VERSION )
		return 0;

	// fill in our callbacks
	memcpy( funcs, &gReffuncs2, sizeof( ref_interface_t ));
	memcpy( &gEngfuncs2, engfuncs, sizeof( ref_api_t ));
	gpGlobals2 = globals;

	gp_cl2 = (ref_client_t *)ENGINE_GET_PARM( PARM_GET_CLIENT_PTR );
	gp_host2 = (ref_host_t *)ENGINE_GET_PARM( PARM_GET_HOST_PTR );

	return REF_API_VERSION;
}


extern int dll_register(const char *name, dllexport_t *exports);

dllexport_t cafe_graphics_exports[] = {
    { "GetRefAPI", (void*)GetRefAPI2 },
    { NULL, NULL },
};

extern int setup_dll_funcs( void )
{
    return dll_register( "libref_soft.so", cafe_graphics_exports );
}