#ifndef IVGAMEUI_H
#define IVGAMEUI_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

#include "gameui/IGameUI.h"
#include "gameui/IGameConsole.h"
#include "gameui/IMusicManager.h"
#include "gameui/IRunGameEngine.h"
#include "gameui/IVGuiModuleLoader.h"
#include "gameui/ICareerUI.h"

//-----------------------------------------------------------------------------
// Purpose: Interface to GameUI's API
//-----------------------------------------------------------------------------

abstract_class IVGameUI
{
public:
	virtual						~IVGameUI() {}

	virtual IGameUI				*GameUI() = 0;

	virtual IGameConsole		*GameConsole() = 0;

	virtual IMusicManager		*MusicManager() = 0;

	virtual IRunGameEngine		*RunGameEngine() = 0;

	virtual IVGuiModuleLoader	*VGuiModuleLoader() = 0;

	virtual ICareerUI			*CareerUI() = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

extern IVGameUI *g_pVGameUI;
PLATFORM_INTERFACE IVGameUI *VGameUI();

#endif // IVGAMEUI_H