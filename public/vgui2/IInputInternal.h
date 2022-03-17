//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef SDK_IINPUTINTERNAL_H
#define SDK_IINPUTINTERNAL_H

#ifdef _WIN32
#pragma once
#endif

#include "IInput.h"

namespace vgui
{
	class IInputInternal : public IInput
	{
	public:
		virtual void RunFrame() = 0;
		virtual void UpdateMouseFocus(int x, int y)  = 0;
		virtual void PanelDeleted(VPANEL panel) = 0;
		virtual void InternalCursorMoved(int x, int y) = 0;
		virtual void InternalMousePressed(MouseCode code) = 0;
		virtual void InternalMouseDoublePressed(MouseCode code) = 0;
		virtual void InternalMouseReleased(MouseCode code) = 0;
		virtual void InternalMouseWheeled(int delta) = 0;
		virtual void InternalKeyCodePressed(KeyCode code) = 0;
		virtual void InternalKeyCodeTyped(KeyCode code) = 0;
		virtual void InternalKeyTyped(wchar_t unichar) = 0;
		virtual void InternalKeyCodeReleased(KeyCode code) = 0;
		virtual HInputContext CreateInputContext() = 0;
		virtual void DestroyInputContext(HInputContext context) = 0;
		virtual void AssociatePanelWithInputContext(HInputContext context, VPANEL pRoot) = 0;
		virtual void ActivateInputContext(HInputContext context) = 0;
	};
}

#define VGUI_INPUTINTERNAL_INTERFACE_VERSION "VGUI_InputInternal001"

#endif // SDK_IINPUTINTERNAL_H