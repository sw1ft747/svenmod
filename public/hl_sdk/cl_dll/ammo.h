/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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

#ifndef SDK_AMMO_H
#define SDK_AMMO_H

#ifdef _WIN32
#pragma once
#endif

#include "../common/Platform.h"
#include "../engine/cdll_int.h"

#define MAX_WEAPON_NAME 128


#define WEAPON_FLAGS_SELECTONEMPTY 1

#define WEAPON_IS_ONTARGET 0x40

struct WEAPON
{
	char szName[MAX_WEAPON_NAME];
	int iAmmoType;
	int iAmmo2Type;
	int iMax1;
	int iMax2;
	int iSlot;
	int iSlotPos;
	int iFlags;
	int iId;
	int iClip;

	int iCount; // # of items in plist

	VHSPRITE hActive;
	Rect rcActive;
	VHSPRITE hInactive;
	Rect rcInactive;
	VHSPRITE hAmmo;
	Rect rcAmmo;
	VHSPRITE hAmmo2;
	Rect rcAmmo2;
	VHSPRITE hCrosshair;
	Rect rcCrosshair;
	VHSPRITE hAutoaim;
	Rect rcAutoaim;
	VHSPRITE hZoomedCrosshair;
	Rect rcZoomedCrosshair;
	VHSPRITE hZoomedAutoaim;
	Rect rcZoomedAutoaim;
};

typedef int AMMO;

#endif // SDK_AMMO_H