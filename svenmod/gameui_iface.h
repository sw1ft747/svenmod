#ifndef VGAMEUI_IFACE_H
#define VGAMEUI_IFACE_H

#ifdef _WIN32
#pragma once
#endif

#include <IVGameUI.h>

//-----------------------------------------------------------------------------
// CVGameUI
//-----------------------------------------------------------------------------

class CVGameUI : public IVGameUI
{
public:
	CVGameUI();

	void Init();

	virtual IGameUI *GameUI();

	virtual IGameConsole *GameConsole();

	virtual IMusicManager *MusicManager();

	virtual IRunGameEngine *RunGameEngine();

	virtual IVGuiModuleLoader *VGuiModuleLoader();

	virtual ICareerUI *CareerUI();
};

extern CVGameUI g_VGameUI;

#endif // VGAMEUI_IFACE_H