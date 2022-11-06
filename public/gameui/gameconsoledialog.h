#ifndef GAMECONSOLE_DIALOG_H
#define GAMECONSOLE_DIALOG_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "color.h"

class CGameConsoleDialog
{
public:
#ifdef PLATFORM_WINDOWS
	char pad_1[280];
#else
	char pad_1[276];
#endif

	void *m_pRichText;

	char pad_2[12];

	Color *m_pDefaultTextColor;
	Color *m_pDevTextColor;
};

#endif // GAMECONSOLE_DIALOG_H