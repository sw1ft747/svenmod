/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
//  cdll_int.h
//
// 4-23-98
// JOHN:  client dll interface declarations
//

#ifndef SDK_CDLL_INT_H
#define SDK_CDLL_INT_H

#ifdef _WIN32
#pragma once
#endif

#include "../../steamtypes.h"
#include "../common/const.h"
#include "../common/ref_params.h"
#include "../common/r_efx.h"
#include "../common/studio_event.h"
#include "../common/common_types.h"

// this file is included by both the engine and the client-dll,
// so make sure engine declarations aren't done twice

typedef int VHSPRITE; // handle to a graphic

#define SCRINFO_SCREENFLASH 1
#define SCRINFO_STRETCHED 2

typedef struct SCREENINFO_s
{
	int iSize;
	int iWidth;
	int iHeight;
	int iFlags;
	int iCharHeight;
	short charWidths[256];
} SCREENINFO;


typedef struct client_data_s
{
	// fields that cannot be modified  (ie. have no effect if changed)
	Vector origin;

	// fields that can be changed by the cldll
	Vector viewangles;
	int iWeaponBits;
	//	int		iAccessoryBits;
	float fov; // field of view
} client_data_t;

typedef struct client_sprite_s
{
	char szName[64];
	char szSprite[64];
	int hspr;
	int iRes;
	Rect rc;
} client_sprite_t;



typedef struct hud_player_info_s
{
	char* name;
	short ping;
	byte thisplayer; // true if this is the calling player

	byte spectator;
	byte packetloss;

	char* model;
	short topcolor;
	short bottomcolor;

	uint64 m_nSteamID;

	// In client's dll the structure's size is 56.. ( v13 = (const char **)&g_PlayerInfoList[14 * clientIndex]; )
	//char pad_1[24];
} hud_player_info_t;

#include "../common/in_buttons.h"

#define CLDLL_INTERFACE_VERSION 7

//#include "server.h" // server_static_t define for apiproxy
#include "APIProxy.h"

#endif