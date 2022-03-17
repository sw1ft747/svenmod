#ifndef IGAMECONSOLE_H
#define IGAMECONSOLE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "sys.h"
#include "color.h"

#include "vgui2/VGUI2.h"

//-----------------------------------------------------------------------------
// Purpose: interface to game/dev console
//-----------------------------------------------------------------------------

abstract_class IGameConsole
{
public:
	virtual			~IGameConsole() {}

	// activates the console, makes it visible and brings it to the foreground
	virtual void	Activate( void ) = 0;

	virtual void	Initialize( void ) = 0;

	// hides the console
	virtual void	Hide( void ) = 0;

	// clears the console
	virtual void	Clear( void ) = 0;

	// return true if the console has focus
	virtual bool	IsConsoleVisible( void ) = 0;

	// prints a message to the console
	virtual void	Printf( const char *format, ... ) = 0;

	// printes a debug message to the console
	virtual void	DPrintf( const char *format, ... ) = 0;

	// printes a debug message to the console
	//virtual void	ColorPrintf( Color &clr, const char *format, ... ) = 0;

	virtual void	SetParent( vgui::VPANEL parent ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

#define GAMECONSOLE_INTERFACE_VERSION "GameConsole003"

extern IGameConsole *g_pGameConsole;

#endif // IGAMECONSOLE_H
