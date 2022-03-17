//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef SDK_IVGUI_H
#define SDK_IVGUI_H

#ifdef _WIN32
#pragma once
#endif

#include "../interface.h"

#include "VGUI2.h"

class KeyValues;

namespace vgui
{
	class IVGui : public IBaseInterface
	{
	public:
		virtual bool Init(CreateInterfaceFn *factoryList, int numFactories) = 0;
		virtual void Shutdown() = 0;
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual bool IsRunning() = 0;
		virtual void RunFrame() = 0;
		virtual void ShutdownMessage(unsigned int shutdownID) = 0;
		virtual VPANEL AllocPanel() = 0;
		virtual void FreePanel(VPANEL panel) = 0;
		virtual void DPrintf(const char *format, ...) = 0;
		virtual void DPrintf2(const char *format, ...) = 0;
		virtual void SpewAllActivePanelNames() = 0;
		virtual HPanel PanelToHandle(VPANEL panel) = 0;
		virtual VPANEL HandleToPanel(HPanel index) = 0;
		virtual void MarkPanelForDeletion(VPANEL panel) = 0;
		virtual void AddTickSignal(VPANEL panel, int intervalMilliseconds) = 0;
		virtual void RemoveTickSignal(VPANEL panel) = 0;
		virtual void PostMessage(VPANEL target, KeyValues *params, VPANEL from, float delaySeconds) = 0;
		virtual HContext CreateContext() = 0;
		virtual void DestroyContext(HContext context) = 0;
		virtual void AssociatePanelWithContext(HContext context, VPANEL pRoot) = 0;
		virtual void ActivateContext(HContext context) = 0;
		virtual void SetSleep(bool state) = 0;
		virtual bool GetShouldVGuiControlSleep() = 0;
	};
}

#define VGUI_IVGUI_INTERFACE_VERSION "VGUI_ivgui006"

#endif // SDK_IVGUI_H
