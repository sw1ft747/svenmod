#include <ISvenModAPI.h>
#include <sys.h>

#include "gameui_iface.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

IGameUI *g_pGameUI = NULL;
IGameConsole *g_pGameConsole = NULL;
IMusicManager *g_pMusicManager = NULL;
IRunGameEngine *g_pRunGameEngine = NULL;
IVGuiModuleLoader *g_pVGuiModuleLoader = NULL;
ICareerUI *g_pCareerUI = NULL;

//-----------------------------------------------------------------------------
// CVGameUI implementation
//-----------------------------------------------------------------------------

CVGameUI::CVGameUI()
{
}

void CVGameUI::Init()
{
	CreateInterfaceFn pfnGameUIFactory = Sys_GetFactory( SvenModAPI()->Modules()->GameUI );

	if ( !pfnGameUIFactory )
	{
		Sys_Error("[SvenMod] Can't locate GameUI's CreateInterface factory");
		return;
	}

	g_pGameUI = (IGameUI *)pfnGameUIFactory(GAMEUI_INTERFACE_VERSION, NULL);

	if ( !g_pGameUI )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IGameUI");
		return;
	}

	g_pGameConsole = (IGameConsole *)pfnGameUIFactory(GAMECONSOLE_INTERFACE_VERSION, NULL);

	if ( !g_pGameConsole )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IGameConsole");
		return;
	}
	
	g_pMusicManager = (IMusicManager *)pfnGameUIFactory(MUSICMANAGER_INTERFACE_VERSION, NULL);

	if ( !g_pMusicManager )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IMusicManager");
		return;
	}
	
	g_pRunGameEngine = (IRunGameEngine *)pfnGameUIFactory(RUNGAMEENGINE_INTERFACE_VERSION, NULL);

	if ( !g_pRunGameEngine )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IRunGameEngine");
		return;
	}
	
	g_pVGuiModuleLoader = (IVGuiModuleLoader *)pfnGameUIFactory(VGUIMODULELOADER_INTERFACE_VERSION, NULL);

	if ( !g_pVGuiModuleLoader )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IVGuiModuleLoader");
		return;
	}
	
	g_pCareerUI = (ICareerUI *)pfnGameUIFactory(CAREERUI_INTERFACE_VERSION, NULL);

	if ( !g_pCareerUI )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "ICareerUI");
		return;
	}
}

IGameUI *CVGameUI::GameUI()
{
	return g_pGameUI;
}

IGameConsole *CVGameUI::GameConsole()
{
	return g_pGameConsole;
}

IMusicManager *CVGameUI::MusicManager()
{
	return g_pMusicManager;
}

IRunGameEngine *CVGameUI::RunGameEngine()
{
	return g_pRunGameEngine;
}

IVGuiModuleLoader *CVGameUI::VGuiModuleLoader()
{
	return g_pVGuiModuleLoader;
}

ICareerUI *CVGameUI::CareerUI()
{
	return g_pCareerUI;
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CVGameUI g_VGameUI;
IVGameUI *g_pVGameUI = (IVGameUI *)&g_VGameUI;

IVGameUI *VGameUI()
{
	return g_pVGameUI;
}