#ifndef VGUI_IFACE_H
#define VGUI_IFACE_H

#ifdef _WIN32
#pragma once
#endif

#include <IVGUI.h>

//-----------------------------------------------------------------------------
// CVGUI
//-----------------------------------------------------------------------------

class CVGUI : public IVGUI
{
public:
	CVGUI();

	void Init();

	virtual vgui::IVGui *VGui();

	virtual IBaseUI *BaseUI();
	virtual IGameUIFuncs *GameUIFuncs();
	virtual IEngineSurface *EngineSurface();

	virtual vgui::IEngineVGui *EngineVGui();

	virtual vgui::ISurface *Surface();
	virtual vgui::ISchemeManager *SchemeManager();
	virtual vgui::ILocalize *Localize();

	virtual vgui::IPanel *Panel();

	virtual vgui::IInput *Input();
	virtual vgui::IInputInternal *InputInternal();

	virtual vgui::ISystem *System();

	virtual vgui::IKeyValues *KeyValues();

private:
	vgui::IVGui *m_pVGui;

	IBaseUI *m_pBaseUI;
	IGameUIFuncs *m_pGameUIFuncs;
	IEngineSurface *m_pEngineSurface;

	vgui::IEngineVGui *m_pEngineVGui;

	vgui::ISurface *m_pSurface;
	vgui::ISchemeManager *m_pSchemeManager;
	vgui::ILocalize *m_pLocalize;

	vgui::IPanel *m_pPanel;

	vgui::IInput *m_pInput;
	vgui::IInputInternal *m_pInputInternal;

	vgui::ISystem *m_pSystem;

	vgui::IKeyValues *m_pKeyValues;
};

extern CVGUI g_VGUI;

#endif // VGUI_IFACE_H