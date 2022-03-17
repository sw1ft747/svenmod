#ifndef IRUNGAMEENGINE_H
#define IRUNGAMEENGINE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "sys.h"

//-----------------------------------------------------------------------------
// Purpose: Interface to running the game engine
//-----------------------------------------------------------------------------

abstract_class IRunGameEngine
{
public:
	virtual			~IRunGameEngine() {}

	// Returns true if the engine is running, false otherwise.
	// Retruns true no matter what
	virtual bool	IsRunning( void ) = 0;

	// Adds text to the engine command buffer. Only works if IsRunning()
	// returns true on success, false on failure
	virtual bool	AddTextCommand( const char *text ) = 0;

	// runs the engine with the specified command line parameters.  Only works if !IsRunning()
	// Dummy implementation, returns 3
	virtual int		RunEngine( int unk, const char *gameDir, const char *commandLineParams ) = 0;

	// returns true if the player is currently connected to a game server
	virtual bool	IsInGame( void ) = 0;

	// gets information about the server the engine is currently connected to
	// returns 0 no matter what
	virtual int		GetGameInfo( char *infoBuffer, int bufferSize ) = 0;

	// tells the engine our userID
	virtual void	SetTrackerUserID( int trackerID, const char *trackerName ) = 0;

	// this next section could probably moved to another interface
	// iterates users
	// returns the number of user
	virtual int		GetPlayerCount( void ) = 0;

	// returns a playerID for a player
	// playerIndex is in the range [0, GetPlayerCount)
	virtual unsigned int	GetPlayerFriendsID( int playerIndex ) = 0;

	// gets the in-game name of another user, returns NULL if that user doesn't exists
	// returns NULL, dummy implementation
	virtual const char		*GetPlayerName( int friendsID ) = 0;

	// gets the friends name of a player
	// returns NULL, dummy implementation
	virtual const char		*GetPlayerFriendsName( int friendsID ) = 0;

	// returns the engine build number and mod version string for server versioning
	// returns 0 for both, dummy implementation
	virtual unsigned int	GetEngineBuildNumber( void ) = 0;
	virtual const char		*GetProductVersionString( void ) = 0;

	// gets the in-game user ID of another user, return 0 if that user doesn't exists
	virtual int		GetPlayerUserID( int friendsID ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

#define RUNGAMEENGINE_INTERFACE_VERSION "RunGameEngine006"

extern IRunGameEngine *g_pRunGameEngine;

#endif // IRUNGAMEENGINE_H
