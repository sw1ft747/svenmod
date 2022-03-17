//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef SDK_IENGINESURFACE_H
#define SDK_IENGINESURFACE_H

#ifdef _WIN32
#pragma once
#endif

#include "../interface.h"

#include "VGuiVertex.h"

#ifdef CreateFont
#undef CreateFont
#endif

class IEngineSurface : public IBaseInterface
{
public:
	enum EFontFlags
	{
		FONTFLAG_NONE,
		FONTFLAG_ITALIC = 0x001,
		FONTFLAG_UNDERLINE = 0x002,
		FONTFLAG_STRIKEOUT = 0x004,
		FONTFLAG_SYMBOL = 0x008,
		
		// validate these flags
		FONTFLAG_ANTIALIAS = 0x010,
		FONTFLAG_GAUSSIANBLUR = 0x020,
		FONTFLAG_ROTARY = 0x040,
		FONTFLAG_DROPSHADOW = 0x080,
		FONTFLAG_ADDITIVE = 0x100,
		FONTFLAG_OUTLINE = 0x200,
	};

	virtual void PushMakeCurrent(int insets[4], int absExtents[4], int clipRect[4]) = 0;
	virtual void PopMakeCurrent() = 0;
	virtual void DrawSetColor(int r, int g, int b, int a) = 0;
	virtual void DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawSetTextFont(int font) = 0;
	virtual void DrawSetTextColor(int r, int g, int b, int a) = 0;
	virtual void DrawGetTextColor(int &r, int &g, int &b, int &a) = 0;
	virtual void DrawSetTextPos(int x, int y) = 0;
	virtual void DrawGetTextPos(int &x, int &y) = 0;
	virtual void DrawPrintText(const wchar_t *text, int textLen) = 0;
	virtual void DrawUnicodeChar(wchar_t wch, bool additive) = 0;
	virtual void DrawSetTextureFile(int id, const char *filename) = 0;
	virtual void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall) = 0;
	virtual void DrawSetTexture(int id) = 0;
	virtual void DrawGetTextureSize(int id, int &wide, int &tall) = 0;
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawTexturedPolygon(vgui::VGuiVertex *pVertices, int n) = 0;
	virtual int CreateNewTextureID() = 0;
    virtual bool DeleteTextureByID(int id) = 0;
    virtual void DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall) = 0;
	
	// Not valid
	/*
    virtual void DrawSetTextureBGRA(int id, const unsigned char *bgra, int wide, int tall) = 0;
    virtual int CreateFont() = 0;
    virtual bool AddGlyphSetToFont(int font, const char *fontName, int tall, int weight, int flags) = 0;
    virtual bool AddCustomFontFile(const char *fontFileName) = 0;
	virtual int GetFontTall(int font) = 0;
    virtual void GetCharABCwide(int font, int ch, int &a, int &b, int &c) = 0;
    virtual int GetCharacterWidth(int font, int ch) = 0;
    virtual void GetTextSize(int font, const wchar_t *text, int &wide, int &tall) = 0;
	virtual void GetScreenSize(int &wide, int &tall) = 0;
	virtual void SetCursor(int cursor) = 0;
	virtual int GetCursor() = 0;
	virtual void GetCursorPos(int &x, int &y) = 0;
	*/
};

#define ENGINE_SURFACE_INTERFACE_VERSION "EngineSurface007"

#endif // SDK_IENGINESURFACE_H