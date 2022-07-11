#include <ISvenModAPI.h>
#include <sys.h>

#include "vgui_iface.h"

//-----------------------------------------------------------------------------
// CVGUI implementation
//-----------------------------------------------------------------------------

CVGUI::CVGUI()
{
	m_pVGui = NULL;
	m_pBaseUI = NULL;
	m_pGameUIFuncs = NULL;
	m_pEngineSurface = NULL;
	m_pEngineVGui = NULL;
	m_pSurface = NULL;
	m_pSchemeManager = NULL;
	m_pLocalize = NULL;
	m_pPanel = NULL;
	m_pInput = NULL;
	m_pInputInternal = NULL;
	m_pSystem = NULL;
	m_pKeyValues = NULL;
}

void CVGUI::Init()
{
	CreateInterfaceFn pfnVGUI2Factory = Sys_GetFactory( SvenModAPI()->Modules()->VGUI2 );

	if ( !pfnVGUI2Factory )
	{
		Sys_Error("[SvenMod] Can't locate vgui2's CreateInterface factory");
		return;
	}

	CreateInterfaceFn pfnHardwareFactory = Sys_GetFactory( SvenModAPI()->Modules()->Hardware );

	if ( !pfnHardwareFactory )
	{
		Sys_Error("[SvenMod] Can't locate hardware's CreateInterface factory");
		return;
	}

	m_pBaseUI = (IBaseUI *)pfnHardwareFactory(BASEUI_INTERFACE_VERSION, NULL);

	if ( !m_pBaseUI )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IBaseUI");
		return;
	}

	m_pGameUIFuncs = (IGameUIFuncs *)pfnHardwareFactory(ENGINE_GAMEUIFUNCS_INTERFACE_VERSION, NULL);

	if ( !m_pGameUIFuncs )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IGameUIFuncs");
		return;
	}

	m_pEngineSurface = (IEngineSurface *)pfnHardwareFactory(ENGINE_SURFACE_INTERFACE_VERSION, NULL);

	if ( !m_pEngineSurface )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IEngineSurface");
		return;
	}

	m_pEngineVGui = (vgui::IEngineVGui *)pfnHardwareFactory(VENGINE_VGUI_VERSION, NULL);

	if ( !m_pEngineVGui )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IEngineVGui");
		return;
	}

	m_pVGui = (vgui::IVGui *)pfnVGUI2Factory(VGUI_IVGUI_INTERFACE_VERSION, NULL);

	if ( !m_pVGui )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IVGui");
		return;
	}

	m_pSurface = (vgui::ISurface *)pfnHardwareFactory(VGUI_SURFACE_INTERFACE_VERSION, NULL);

	if ( !m_pSurface )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "ISurface");
		return;
	}

	m_pSchemeManager = (vgui::ISchemeManager *)pfnVGUI2Factory(VGUI_SCHEME_INTERFACE_VERSION, NULL);

	if ( !m_pSchemeManager )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "ISchemeManager");
		return;
	}

	m_pLocalize = (vgui::ILocalize *)pfnVGUI2Factory(VGUI_LOCALIZE_INTERFACE_VERSION, NULL);

	if ( !m_pLocalize )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "ILocalize");
		return;
	}

	m_pPanel = (vgui::IPanel *)pfnVGUI2Factory(VGUI_PANEL_INTERFACE_VERSION, NULL);

	if ( !m_pPanel )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IPanel");
		return;
	}

	m_pInput = (vgui::IInput *)pfnVGUI2Factory(VGUI_INPUT_INTERFACE_VERSION, NULL);

	if ( !m_pInput )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IInput");
		return;
	}

	m_pInputInternal = (vgui::IInputInternal *)pfnVGUI2Factory(VGUI_INPUTINTERNAL_INTERFACE_VERSION, NULL);

	if ( !m_pInputInternal )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IInputInternal");
		return;
	}

	m_pSystem = (vgui::ISystem *)pfnVGUI2Factory(VGUI_SYSTEM_INTERFACE_VERSION, NULL);

	if ( !m_pSystem )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "ISystem");
		return;
	}

	m_pKeyValues = (vgui::IKeyValues *)pfnVGUI2Factory(VGUI_KEYVALUES_INTERFACE_VERSION, NULL);

	if ( !m_pKeyValues )
	{
		Sys_Error("[SvenMod] Failed to get %s interface", "IKeyValues");
		return;
	}
}

vgui::IVGui *CVGUI::VGui()
{
	return m_pVGui;
}

IBaseUI *CVGUI::BaseUI()
{
	return m_pBaseUI;
}

IGameUIFuncs *CVGUI::GameUIFuncs()
{
	return m_pGameUIFuncs;
}

IEngineSurface *CVGUI::EngineSurface()
{
	return m_pEngineSurface;
}

vgui::IEngineVGui *CVGUI::EngineVGui()
{
	return m_pEngineVGui;
}

vgui::ISurface *CVGUI::Surface()
{
	return m_pSurface;
}

vgui::ISchemeManager *CVGUI::SchemeManager()
{
	return m_pSchemeManager;
}

vgui::ILocalize *CVGUI::Localize()
{
	return m_pLocalize;
}

vgui::IPanel *CVGUI::Panel()
{
	return m_pPanel;
}

vgui::IInput *CVGUI::Input()
{
	return m_pInput;
}

vgui::IInputInternal *CVGUI::InputInternal()
{
	return m_pInputInternal;
}

vgui::ISystem *CVGUI::System()
{
	return m_pSystem;
}

vgui::IKeyValues *CVGUI::KeyValues()
{
	return m_pKeyValues;
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CVGUI g_VGUI;
IVGUI *g_pVGUI = (IVGUI *)&g_VGUI;

IVGUI *VGUI()
{
	return g_pVGUI;
}