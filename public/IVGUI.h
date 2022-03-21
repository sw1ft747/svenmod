#ifndef IVGUI_H
#define IVGUI_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

#include "vgui2/IVGui.h"
#include "vgui2/ISurface.h"
#include "vgui2/IInput.h"
#include "vgui2/IInputInternal.h"
#include "vgui2/IKeyValues.h"
#include "vgui2/ILocalize.h"
#include "vgui2/IPanel.h"
#include "vgui2/IScheme.h"
#include "vgui2/ISystem.h"

#include "vgui2/IBaseUI.h"
#include "vgui2/IGameUIFuncs.h"
#include "vgui2/IEngineSurface.h"
#include "vgui2/IEngineVGui.h"

//-----------------------------------------------------------------------------
// Purpose: Interface to Valve's GUI
//-----------------------------------------------------------------------------

abstract_class IVGUI
{
public:
	virtual							~IVGUI() {}

	virtual vgui::IVGui				*VGui() = 0;

	virtual IBaseUI					*BaseUI() = 0;
	virtual IGameUIFuncs			*GameUIFuncs() = 0;
	virtual IEngineSurface			*EngineSurface() = 0;

	virtual vgui::IEngineVGui		*EngineVGui() = 0;

	virtual vgui::ISurface			*Surface() = 0;
	virtual vgui::ISchemeManager	*SchemeManager() = 0;
	virtual vgui::ILocalize			*Localize() = 0;

	virtual vgui::IPanel			*Panel() = 0;

	virtual vgui::IInput			*Input() = 0;
	virtual vgui::IInputInternal	*InputInternal() = 0;

	virtual vgui::ISystem			*System() = 0;

	virtual vgui::IKeyValues		*KeyValues() = 0;
};

extern IVGUI *g_pVGUI;
PLATFORM_INTERFACE IVGUI *VGUI();

//-----------------------------------------------------------------------------
// "Traditional" vgui query format
//-----------------------------------------------------------------------------

namespace vgui
{

inline vgui::ISurface *surface()
{
	return VGUI()->Surface();
}

inline vgui::ISchemeManager *scheme()
{
	return VGUI()->SchemeManager();
}

inline vgui::ILocalize *localize()
{
	return VGUI()->Localize();
}

inline vgui::IPanel *panel()
{
	return VGUI()->Panel();
}

inline vgui::IInput *input()
{
	return VGUI()->Input();
}

inline vgui::IInputInternal *inputinternal()
{
	return VGUI()->InputInternal();
}

inline vgui::ISystem *system()
{
	return VGUI()->System();
}

}

#endif // IVGUI_H