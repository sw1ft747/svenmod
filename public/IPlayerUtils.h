#ifndef IPLAYERUTILS_H
#define IPLAYERUTILS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "sys.h"

#include "hl_sdk/engine/APIProxy.h"
#include "hl_sdk/cl_dll/hud.h"

//-----------------------------------------------------------------------------
// Purpose: various helpers to interact with players
//-----------------------------------------------------------------------------

abstract_class IPlayerUtils
{
public:
	virtual						~IPlayerUtils() {}

	virtual extra_player_info_t *GetExtraInfo( int iPlayerIndex ) = 0;
	virtual extra_player_info_t *GetExtraInfo( cl_entity_t *pPlayer ) = 0;

	virtual float				GetHealth( int iPlayerIndex ) = 0;
	virtual float				GetArmor( int iPlayerIndex ) = 0;
	virtual int					GetTeamNumber( int iPlayerIndex ) = 0;
	virtual uint64				GetSteamID( int iPlayerIndex ) = 0;

	virtual void				GetViewAngles( int iPlayerIndex, Vector &vOut ) = 0;
	virtual void				GetViewAngles( cl_entity_t *pPlayer, Vector &vOut ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

extern IPlayerUtils *g_pPlayerUtils;
PLATFORM_INTERFACE IPlayerUtils *PlayerUtils();

#endif // IPLAYERUTILS_H
