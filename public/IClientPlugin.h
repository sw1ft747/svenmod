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

	// Add this in implementation: return SVENMOD_API_VER;
	virtual api_version_s		GetAPIVersion( void ) = 0;

	// Initialize the plugin to run
	// Return 'false' if there is an error during startup
	virtual bool				Load( CreateInterfaceFn pfnSvenModFactory, ISvenModAPI *pSvenModAPI, IPluginHelpers *pPluginHelpers ) = 0;

	// Called if plugin was loaded during SvenMod's initialization after attaching detours (via plugins.txt file, @bGlobalLoad is 'true')
	// Otherwise, called right after 'Load' callback (@bGlobalLoad is 'false')
	virtual void				PostLoad( bool bGlobalLoad ) = 0;

	// Called when the plugin should be shutdown
	virtual void				Unload( void ) = 0;

	// Called when a plugins execution is stopped but the plugin is not unloaded
	// Return 'false' if it should not pause
	virtual bool				Pause( void ) = 0;

	// Called when a plugin should start executing again (sometime after a Pause() call)
	virtual void				Unpause( void ) = 0;

	// Called on each game frame twice (if @bPostRunCmd is 'false', then none of inputs/commands were processed yet)
	virtual void				GameFrame( client_state_t state, double frametime, bool bPostRunCmd ) = 0;
	
	// Called to draw 2D paints after rendering the game view, it's only callback that you can handle to influence on other plugins
	// @PLUGIN_CONTINUE - don't abort the call sequence
	// @PLUGIN_STOP and @PLUGIN_CALL_STOP - don't call the further plugins callbacks
	virtual PLUGIN_RESULT		Draw( void ) = 0;
	
	// Called to redraw client's HUD, same callback as @Draw method
	virtual PLUGIN_RESULT		DrawHUD( float time, int intermission ) = 0;

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

#define CLIENT_PLUGIN_INTERFACE_VERSION "ClientPlugin001"

#endif // ICLIENTPLUGIN_H