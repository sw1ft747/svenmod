#ifndef IGAMEUI_H
#define IGAMEUI_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "sys.h"

#include "hl_sdk/engine/APIProxy.h"
#include "IFileSystem.h"

//-----------------------------------------------------------------------------
// Purpose: contains all the functions that the GameUI dll exports
// GameUI_GetInterface() is exported via dll export table to get this table
//-----------------------------------------------------------------------------

abstract_class IGameUI
{
public:
	virtual			~IGameUI() {}

	virtual void 	Initialize( CreateInterfaceFn *factories, int count ) = 0;
	virtual void	Start( cl_enginefuncs_t *engineFuncs, int interfaceVersion, IFileSystem *system ) = 0;
	virtual void	Shutdown( void ) = 0;

	virtual int		ActivateGameUI( void ) = 0;	// activates the menus, returns 0 if it doesn't want to handle it
	virtual int		ActivateDemoUI( void ) = 0;	// activates the demo player, returns 0 if it doesn't want to handle it

	virtual int		HasExclusiveInput( void ) = 0;

	virtual void	RunFrame( void ) = 0;

	virtual void	ConnectToServer( const char *game, int IP, int port ) = 0;
	virtual void	DisconnectFromServer( void ) = 0;
	virtual void	HideGameUI( void ) = 0;

	virtual bool	IsGameUIActive( void ) = 0;

	virtual void	LoadingStarted( const char *resourceType, const char *resourceName ) = 0;
	virtual void	LoadingFinished( const char *resourceType, const char *resourceName ) = 0;

	virtual void	StartProgressBar( const char *progressType, int numProgressPoints ) = 0;
	virtual int		ContinueProgressBar( int progressPoint, float progressFraction ) = 0;
	virtual void	StopProgressBar( bool bError, const char *failureReasonIfAny, const char *extendedReason ) = 0;
	virtual int		SetProgressBarStatusText( const char *statusText ) = 0;
	virtual void	SetSecondaryProgressBar( float progress ) = 0;
	virtual void	SetSecondaryProgressBarText( const char *statusText ) = 0;

	virtual void	ValidateCDKey( bool force, bool inConnect ) = 0;

	virtual void	OnDisconnectFromServer( int eSteamLoginFailure, const char *username ) = 0;
	virtual void	ShowPasswordPromptAndRetry( const char *unk1, bool unk2 ) = 0;
	virtual void	OnExitToDesktop( void ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

#define GAMEUI_INTERFACE_VERSION "GameUI007"

extern IGameUI *g_pGameUI;

#endif // IGAMEUI_H
