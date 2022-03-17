#ifndef GAMECONSOLE_H
#define GAMECONSOLE_H

#ifdef _WIN32
#pragma once
#endif

#include "IGameConsole.h"
#include "gameconsoledialog.h"

class CGameConsole : public IGameConsole
{
public:
	CGameConsole();
	~CGameConsole();

	inline bool					IsInitialized() { return m_bInitialized; }
	inline void					SetInitialized(bool state) { m_bInitialized = state; }

	inline CGameConsoleDialog	*GetGameConsoleDialog() { return m_pConsole; }

private:
	bool m_bInitialized;
	CGameConsoleDialog *m_pConsole;
};

#endif // GAMECONSOLE_H