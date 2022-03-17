//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef SDK_ICLIENTVGUI_H
#define SDK_ICLIENTVGUI_H

#ifdef _WIN32
#pragma once
#endif

#include "../interface.h"

#include "VGUI2.h"

class IClientVGUI : public IBaseInterface
{
public:
	virtual void Initialize(CreateInterfaceFn *factories, int count) = 0;
	virtual void Start() = 0;
	virtual void SetParent(vgui::VPANEL parent) = 0;
	virtual bool UseVGUI1() = 0;
	virtual void HideScoreBoard() = 0;
	virtual void HideAllVGUIMenu() = 0;
	virtual void ActivateClientUI() = 0;
	virtual void HideClientUI() = 0;
	virtual void Shutdown() = 0;
};

#define CLIENT_VGUI_INTERFACE_VERSION "VClientVGUI001"

#endif // SDK_ICLIENTVGUI_H
