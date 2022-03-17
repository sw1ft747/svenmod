#ifndef ICAREERUI_H
#define ICAREERUI_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "sys.h"

class MapInfo;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------

// Dummy interface
abstract_class ICareerUI
{
public:
	virtual			~ICareerUI() {}

	// Everything returns 0
	virtual int		IsPlayingMatch( void ) = 0;
	virtual int		GetCurrentTaskVec( void ) = 0;
	virtual int		PlayAsCT( void ) = 0;
	virtual int		GetReputationGained( void ) = 0;
	virtual int		GetNumMapsUnlocked( void ) = 0;
	virtual int		DoesWinUnlockAll( void ) = 0;
	virtual int		GetRoundTimeLength( void ) = 0;
	virtual int		GetWinfastLength( void ) = 0;

	// Returns 1
	virtual int		GetDifficulty( void ) = 0;

	virtual int		GetCurrentMapTriplet( MapInfo *mapInfo ) = 0;

	virtual void	OnRoundEndMenuOpen( bool unk ) = 0;
	virtual void	OnMatchEndMenuOpen( bool unk ) = 0;
	virtual void	OnRoundEndMenuClose( bool unk ) = 0;
	virtual void	OnMatchEndMenuClose( bool unk ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

#define CAREERUI_INTERFACE_VERSION "CareerUI001"

extern ICareerUI *g_pCareerUI;

#endif // ICAREERUI_H
