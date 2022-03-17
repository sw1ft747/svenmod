/***
*
*	Copyright (c) 2003', Valve LLC. All rights reserved.
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

#ifndef SDK_HUD_PLAYER_INFO_H
#define SDK_HUD_PLAYER_INFO_H

#ifdef _WIN32
#pragma once
#endif

#include "global_consts.h"
#include "hud.h"

#include "../engine/cdll_int.h"

extern hud_player_info_t	g_PlayerInfoList[MAX_PLAYERS_HUD + 1];	   // player info from the engine
extern extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS_HUD + 1];   // additional player info sent directly to the client dll
extern team_info_t			g_TeamInfo[MAX_TEAMS + 1];
extern int					g_IsSpectator[MAX_PLAYERS_HUD + 1];

// pointers
extern hud_player_info_t	*g_pPlayerInfoList;
extern extra_player_info_t  *g_pPlayerExtraInfo;
extern team_info_t			*g_pTeamInfo;
extern int					*g_pIsSpectator;

#endif