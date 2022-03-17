//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef SDK_ISCHEME_H
#define SDK_ISCHEME_H

#ifdef _WIN32
#pragma once
#endif

#include "../interface.h"
#include "../color.h"

#include "VGUI2.h"

namespace vgui
{
	class IBorder;
	class IImage;

	class IScheme : public IBaseInterface
	{
	public:
		virtual const char *GetResourceString(const char *stringName) = 0;
		virtual IBorder *GetBorder(const char *borderName) = 0;
		virtual HFont GetFont(const char *fontName, bool proportional) = 0;
		virtual Color GetColor(const char *colorName, Color defaultColor) = 0;
	};

	class ISchemeManager : public IBaseInterface
	{
	public:
		virtual HScheme LoadSchemeFromFile(const char *fileName, const char *tag) = 0;
		virtual void ReloadSchemes() = 0;
		virtual HScheme GetDefaultScheme() = 0;
		virtual HScheme GetScheme(const char *tag) = 0;
		virtual IImage *GetImage(const char *imageName, bool hardwareFiltered) = 0;
		virtual HTexture GetImageID(const char *imageName, bool hardwareFiltered) = 0;
		virtual class IScheme *GetIScheme(HScheme scheme) = 0;
		virtual void Shutdown(bool full) = 0;
		virtual int GetProportionalScaledValue(int normalizedValue) = 0;
		virtual int GetProportionalNormalizedValue(int scaledValue) = 0;
	};
}

#define VGUI_SCHEME_INTERFACE_VERSION "VGUI_Scheme009"

#endif // SDK_ISCHEME_H