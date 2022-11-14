#ifndef ISVENMODAPI_H
#define ISVENMODAPI_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "api_version.h"
#include "client_state.h"
#include "sys.h"

#include "hl_sdk/engine/APIProxy.h"
#include "hl_sdk/common/r_studioint.h"
#include "hl_sdk/pm_shared/pm_shared.h"
#include "hl_sdk/cl_dll/StudioModelRenderer.h"

#include "ICommandLine.h"
#include "ICvar.h"
#include "IRender.h"
#include "IEngineClient.h"
#include "IFileSystem.h"
#include "IRegistry.h"
#include "IMemoryUtils.h"
#include "IDetoursAPI.h"
#include "IHooks.h"
#include "IClient.h"
#include "IUtils.h"
#include "IPlayerUtils.h"
#include "IClientWeapon.h"
#include "IInventory.h"
#include "ILoggingSystem.h"
#include "IConfigManager.h"
#include "IPluginHelpers.h"
#include "IVGUI.h"
#include "IVGameUI.h"
#include "IVideoMode.h"

//-----------------------------------------------------------------------------
// Struct declaration
//-----------------------------------------------------------------------------

typedef struct client_version_s
{
	const char *string;

	int version;

	int major_version;
	int minor_version;
} client_version_t;

typedef struct modules_s
{
	HMODULE SvenMod = 0;
	HMODULE Hardware = 0;
	HMODULE Client = 0;
	HMODULE FileSystem = 0;
	HMODULE GameUI = 0;
	HMODULE VGUI = 0;
	HMODULE VGUI2 = 0;
	HMODULE SDL2 = 0;
	HMODULE VSTDLib = 0;
	HMODULE Tier0 = 0;
	HMODULE OpenGL = 0;
	HMODULE SteamAPI = 0;
} modules_t;

//-----------------------------------------------------------------------------
// Bind SvenMod's API to global variables
//-----------------------------------------------------------------------------

abstract_class ISvenModAPI;

void BindApiToGlobals(ISvenModAPI *pSvenMod);

//-----------------------------------------------------------------------------
// API interface
//-----------------------------------------------------------------------------

abstract_class ISvenModAPI
{
public:
	//-----------------------------------------------------------------------------
	// Basic info
	//-----------------------------------------------------------------------------

	// SvenMod's version of API
	virtual void					GetVersion(int *major, int *minor) = 0;

	// Engine's build number, protocol version and client's version
	virtual int						GetEngineBuild() = 0;
	virtual int						GetProtocolVersion() = 0;
	virtual client_version_t		*GetClientVersion() = 0;

	virtual client_state_t			GetClientState() = 0;

	virtual const char				*GetBaseDirectory() = 0;

	//-----------------------------------------------------------------------------
	// Application's API
	//-----------------------------------------------------------------------------

	virtual ICommandLine			*CommandLine() = 0;
	virtual IFileSystem				*FileSystem() = 0;
	virtual IRegistry				*Registry() = 0;

	//-----------------------------------------------------------------------------
	// SvenMod's API
	//-----------------------------------------------------------------------------

	virtual IMemoryUtils			*MemoryUtils() = 0;
	virtual IDetoursAPI				*DetoursAPI() = 0;
	virtual ILoggingSystem			*LoggingSystem() = 0;
	virtual IConfigManager			*ConfigManager() = 0;
	virtual IPluginHelpers			*PluginHelpers() = 0;

	virtual modules_t				*Modules() = 0;

	virtual ICvar					*CVar() = 0;
	virtual IRender					*Render() = 0;
	virtual IHooks					*Hooks() = 0;

	virtual IClient					*Client() = 0;
	virtual IClientWeapon			*ClientWeapon() = 0;
	virtual IPlayerUtils			*PlayerUtils() = 0;
	virtual IInventory				*Inventory() = 0;
	virtual IUtils					*Utils() = 0;

	//-----------------------------------------------------------------------------
	// Game's API
	//-----------------------------------------------------------------------------

	virtual IVGUI					*VGUI() = 0;
	virtual IVGameUI				*VGameUI() = 0;

	virtual cl_clientfuncs_t		*ClientFuncs() = 0;
	virtual cl_enginefuncs_t		*EngineFuncs() = 0;

	virtual IEngineClient			*EngineClient() = 0;

	virtual engine_studio_api_t		*EngineStudio() = 0;
	virtual r_studio_interface_t	*StudioAPI() = 0;
	virtual CStudioModelRenderer	*StudioRenderer() = 0;

	virtual triangleapi_t			*TriangleAPI() = 0;
	virtual efx_api_t				*EffectsAPI() = 0;
	virtual event_api_t				*EventAPI() = 0;
	virtual demo_api_t				*DemoAPI() = 0;
	virtual net_api_t				*NetAPI() = 0;
	virtual IVoiceTweak				*VoiceTweak() = 0;

	virtual playermove_t			*PlayerMove() = 0;

	virtual IVideoMode				*VideoMode() = 0;
};

// Interface version! Not an API's version!
#define SVENMOD_API_INTERFACE_VERSION "SvenModAPI003"

extern modules_t *g_pModules;
extern ISvenModAPI *g_pSvenModAPI;
PLATFORM_INTERFACE ISvenModAPI *SvenModAPI();

#endif // ISVENMODAPI_H
