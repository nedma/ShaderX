// ENG-Types.h: interface for the CTypes class.
// Simple types for Engine.
//
//////////////////////////////////////////////////////////////////////

#pragma once

//Global resource ID's (do not use)
static const DWORD D3_GRI_GlobalStart               = 0xF0000000;
static const DWORD D3_GRI_TemporaryStart            = 0xF2000000;

//Textures
static const DWORD D3_GRI_Tex_Black                 = D3_GRI_GlobalStart + 1;
static const DWORD D3_GRI_Tex_White                 = D3_GRI_GlobalStart + 2;
static const DWORD D3_GRI_Tex_Gray                  = D3_GRI_GlobalStart + 3;
static const DWORD D3_GRI_Tex_ShininessLightMap     = D3_GRI_GlobalStart + 4;
static const DWORD D3_GRI_Tex_DefaultSpotLight      = D3_GRI_GlobalStart + 5;
static const DWORD D3_GRI_Tex_DefaultNotmalMap      = D3_GRI_GlobalStart + 6;
static const DWORD D3_GRI_Tex_DefaultEnvMap         = D3_GRI_GlobalStart + 7;
static const DWORD D3_GRI_Tex_FresnelMap            = D3_GRI_GlobalStart + 8;

static const DWORD D3_GRI_Tex_ShininessStart        = D3_GRI_GlobalStart + 9;   //100 textures from this point are reserved for shininess maps
#define GetShininessID(n) (DWORD)(D3_GRI_Tex_ShininessStart + (DWORD)n)   //Use this macro to obtain correct shininess level

static const DWORD D3_GRI_Tex_ShadowMarchingLookup  = D3_GRI_GlobalStart + 200;   //shadow map lookup - marching one
static const DWORD D3_GRI_Tex_CircularNoise         = D3_GRI_GlobalStart + 201;   //shadow map circular noise

//Materials
static const DWORD D3_GRI_Mat_PixelPhong            = D3_GRI_GlobalStart + 1;
static const DWORD D3_GRI_Mat_PixelNoOutput         = D3_GRI_GlobalStart + 2;
static const DWORD D3_GRI_Mat_ShadowBuffer          = D3_GRI_GlobalStart + 3;
static const DWORD D3_GRI_Mat_ShadowBuffer2         = D3_GRI_GlobalStart + 4;
static const DWORD D3_GRI_Mat_PostProcess           = D3_GRI_GlobalStart + 5;

//############################################################################
//Forward declarations


