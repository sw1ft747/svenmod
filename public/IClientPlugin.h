#ifndef ICLIENTPLUGIN_H
#define ICLIENTPLUGIN_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "interface.h"
#include "client_state.h"
#include "svenmod_version.h"
#include "ISvenModAPI.h"
#include "IPluginHelpers.h"

typedef enum
{
	PLUGIN_CONTINUE = 0,	// keep going
	PLUGIN_OVERRIDE,		// run the game dll function but use our return value instead
	PLUGIN_STOP,			// don't run the game dll function at all
	PLUGIN_CALL_STOP		// call the game dll function and stop further hooks from executing 
} PLUGIN_RESULT;

//-----------------------------------------------------------------------------
// Purpose: interface for loading and processing client-side plugins
//-----------------------------------------------------------------------------

abstract_class IClientPlugin
{
public:
	virtual						~IClientPlugin() {}

	// Always add this in implementation: return SVENMOD_API_VER;
	virtual api_version_t		GetAPIVersion( void ) = 0;

	// Initialize the plugin to run
	// Return 'false' if there is an error during startup
	virtual bool				Load( CreateInterfaceFn pfnSvenModFactory, ISvenModAPI *pSvenModAPI, IPluginHelpers *pPluginHelpers ) = 0;

	// Called if the plugin was loaded during SvenMod's initialization after attaching detours (via plugins.txt file, @bGlobalLoad is 'true')
	// Otherwise, called right after 'Load' callback (@bGlobalLoad is 'false')
	virtual void				PostLoad( bool bGlobalLoad ) = 0;

	// Called when the plugin should be shutdown
	virtual void				Unload( void ) = 0;

	// Called when the plugin should stop executing
	// Return 'false' if it should not pause
	virtual bool				Pause( void ) = 0;

	// Called when the plugin should start executing again (sometime after a Pause() call)
	virtual void				Unpause( void ) = 0;

	// Called after loading when client received first clientdata from a server
	virtual void				OnFirstClientdataReceived( client_data_t *pcldata, float flTime ) = 0;
	
	// Called when client started loading to the server, loading bar appears
	virtual void				OnBeginLoading( void ) = 0;

	// Called when client has finished loading to the server, game view appears
	virtual void				OnEndLoading( void ) = 0;

	// Called when client was disconnected from a server
	virtual void				OnDisconnect( void ) = 0;

	// Called on each game frame twice (if @bPostRunCmd is 'false', then none of inputs/commands were processed yet)
	virtual void				GameFrame( client_state_t state, double frametime, bool bPostRunCmd ) = 0;

	// Called to draw 2D paints after rendering the game view
	virtual void				Draw( void ) = 0;
	
	// Called to redraw client's HUD
	virtual void				DrawHUD( float time, int intermission ) = 0;

	// Name of the plugin
	virtual const char			*GetName( void ) = 0;

	// Author of the plugin
	virtual const char			*GetAuthor( void ) = 0;

	// Version of the plugin
	virtual const char			*GetVersion( void ) = 0;

	// Description of the plugin
	virtual const char			*GetDescription( void ) = 0;

	// URL of the plugin
	virtual const char			*GetURL( void ) = 0;
	
	// Build date of the plugin
	virtual const char			*GetDate( void ) = 0;

	// Tag of the plugin to log
	virtual const char			*GetLogTag( void ) = 0;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define CLIENT_PLUGIN_INTERFACE_VERSION "ClientPlugin002"

#endif // ICLIENTPLUGIN_H