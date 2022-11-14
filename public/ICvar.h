#ifndef ICVAR_H
#define ICVAR_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "color.h"

#include "hl_sdk/common/cvardef.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

class ConVar;
class ConCommand;
class ConCommandBase;

//-----------------------------------------------------------------------------
// DLL's identifier
//-----------------------------------------------------------------------------

typedef int CVarDLLIdentifier_t;

//-----------------------------------------------------------------------------
// Purpose: interface to ConVars/ConCommands (mostly for them) and engine's cvars/cmds
//-----------------------------------------------------------------------------

abstract_class ICvar
{
public:
	virtual						~ICvar() {}

	// Allocate an unique DLL identifier
	virtual CVarDLLIdentifier_t AllocateDLLIdentifier() = 0;

	// Register, unregister commands
	virtual void				RegisterConCommand( ConCommandBase *pCommandBase ) = 0;
	virtual void				UnregisterConCommand( ConCommandBase *pCommandBase ) = 0;
	virtual void				UnregisterConCommands( CVarDLLIdentifier_t id ) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char			*GetCommandLineValue( const char *pszVariableName ) = 0;

	// Try to find the cvar pointer by name
	virtual cvar_t				*FindCvar( const char *pszName ) = 0;
	virtual const cvar_t		*FindCvar( const char *pszName ) const = 0;
	
	virtual cmd_t				*FindCmd( const char *pszName ) = 0;
	virtual const cmd_t			*FindCmd( const char *pszName ) const = 0;
	
	virtual ConCommandBase		*FindCommandBase( const char *pszName ) = 0;
	virtual const ConCommandBase *FindCommandBase( const char *pszName ) const = 0;

	virtual ConVar				*FindVar( const char *pszName ) = 0;
	virtual const ConVar		*FindVar( const char *pszName ) const = 0;

	virtual ConCommand			*FindCommand( const char *pszName ) = 0;
	virtual const ConCommand	*FindCommand( const char *pszName ) const = 0;

	// Reverts (resets to default value) ConVars which contain a specific flag
	virtual void				RevertFlaggedConVars( int nFlag ) = 0;

	// Print to the console
	virtual bool				CanPrint( void ) const = 0;

	virtual void				ConsoleColorPrint( const Color &clr, const char *pszMessage ) const = 0;
	virtual void				ConsolePrint( const char *pszMessage ) const = 0;
	virtual void				ConsoleDPrint( const char *pszMessage ) const = 0;
	
	virtual void				ConsoleColorPrintf( const Color &clr, const char *pszFormat, ... ) const = 0;
	virtual void				ConsolePrintf( const char *pszFormat, ... ) const = 0;
	virtual void				ConsoleDPrintf( const char *pszFormat, ... ) const = 0;

	// Access to command arguments
	virtual int					ArgC( void ) const = 0;
	virtual const char			**ArgV( void ) const = 0;
	virtual const char			*Arg( int nIndex ) const = 0;

	// Directly set cvar's value
	virtual void				SetValue( cvar_t *pCvar, const char *value ) = 0;
	virtual void				SetValue( cvar_t *pCvar, float value ) = 0;
	virtual void				SetValue( cvar_t *pCvar, int value ) = 0;
	virtual void				SetValue( cvar_t *pCvar, bool value ) = 0;
	virtual void				SetValue( cvar_t *pCvar, Color value ) = 0;

	virtual void				SetValue( const char *pszCvar, const char *value ) = 0;
	virtual void				SetValue( const char *pszCvar, float value ) = 0;
	virtual void				SetValue( const char *pszCvar, int value ) = 0;
	virtual void				SetValue( const char *pszCvar, bool value ) = 0;
	virtual void				SetValue( const char *pszCvar, Color value ) = 0;

	// Get value
	virtual const char			*GetStringFromCvar( cvar_t *pCvar ) = 0;
	virtual float				GetFloatFromCvar( cvar_t *pCvar ) = 0;
	virtual int					GetIntFromCvar( cvar_t *pCvar ) = 0;
	virtual bool				GetBoolFromCvar( cvar_t *pCvar ) = 0;
	virtual Color				GetColorFromCvar( cvar_t *pCvar ) = 0;

	virtual const char			*GetStringFromCvar( const char *pszName ) = 0;
	virtual float				GetFloatFromCvar( const char *pszName ) = 0;
	virtual int					GetIntFromCvar( const char *pszName ) = 0;
	virtual bool				GetBoolFromCvar( const char *pszName ) = 0;
	virtual Color				GetColorFromCvar( const char *pszName ) = 0;
};

#define CVAR_INTERFACE_VERSION "Cvar001"

extern ICvar *g_pCVar;
PLATFORM_INTERFACE ICvar *CVar();

#endif // ICVAR_H