#ifndef IMUSICMANAGER_H
#define IMUSICMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "sys.h"

typedef int TrackType;

//-----------------------------------------------------------------------------
// Purpose: music manager?
//-----------------------------------------------------------------------------

abstract_class IMusicManager
{
public:
	virtual			~IMusicManager() {}

	virtual void	StartTrack( TrackType type, bool unk1 ) = 0;
	virtual void	StopTrack( void ) = 0;

	virtual void	FadeOutTrack( void ) = 0;

	virtual bool	IsPlayingTrack( void ) = 0;
	virtual bool	IsPlayingTrack( TrackType type ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

#define MUSICMANAGER_INTERFACE_VERSION "MusicManager001"

extern IMusicManager *g_pMusicManager;

#endif // IMUSICMANAGER_H
