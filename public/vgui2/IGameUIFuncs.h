//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef SDK_IGAMEUIFUNCS_H
#define SDK_IGAMEUIFUNCS_H

#ifdef _WIN32
#pragma once
#endif

#include "../interface.h"
#include "../modes.h"

#include "VGUI2.h"

class IGameUIFuncs : public IBaseInterface
{
public:
    virtual bool IsKeyDown(const char *keyname, bool &isdown) = 0;
    virtual const char *Key_NameForKey(int keynum) = 0;
    virtual const char *Key_BindingForKey(int keynum) = 0;
    virtual vgui::KeyCode GetVGUI2KeyCodeForBind(const char *bind) = 0;
    virtual void GetVideoModes(vmode_t **liststart, int *count) = 0;
    virtual void GetCurrentVideoMode(int *wide, int *tall, int *bpp) = 0;
    virtual void GetCurrentRenderer(char *name, int namelen, int *windowed, int *hdmodels, int *addons_folder, int *vid_level) = 0;
    virtual bool IsConnectedToVACSecureServer() = 0;
    virtual int Key_KeyStringToKeyNum(const char *pchKey) = 0;
};

#define ENGINE_GAMEUIFUNCS_INTERFACE_VERSION "VENGINE_GAMEUIFUNCS_VERSION001"

#endif // SDK_IGAMEUIFUNCS_H
