//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef SDK_IBASEUI_H
#define SDK_IBASEUI_H

#ifdef _WIN32
#pragma once
#endif

#include "../interface.h"

class IEngineSurface;

class IBaseUI : public IBaseInterface
{
public:
    virtual void Initialize(CreateInterfaceFn *factories, int count) = 0;
    virtual void Start(IEngineSurface *engineSurface, int interfaceVersion) = 0;
    virtual void Shutdown() = 0;
    virtual int Key_Event(int down, int keynum, const char *pszCurrentBinding) = 0;
    virtual void CallEngineSurfaceAppHandler(void *event, void *userData) = 0;
    virtual void Paint(int x, int y, int right, int bottom) = 0;
    virtual void HideGameUI() = 0;
    virtual void ActivateGameUI() = 0;
    virtual void HideConsole() = 0;
    virtual void ShowConsole() = 0;
};

#define BASEUI_INTERFACE_VERSION "BaseUI001"

#endif // SDK_IBASEUI_H
