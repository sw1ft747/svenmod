//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $NoKeywords: $
//===========================================================================//

#ifndef CONVAR_H
#define CONVAR_H

#ifdef _WIN32
#pragma once
#endif

#include "color.h"
#include "ICvar.h"

#include "hl_sdk/common/const.h"
#include "hl_sdk/common/cvardef.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

class ConVar;
class CCommand;
class ConCommand;
class ConCommandBase;
class IConCommandBaseAccessor;

//-----------------------------------------------------------------------------
// Called when a ConCommand needs to execute
//-----------------------------------------------------------------------------

typedef void (*FnCommandCallback_t)(void);

//-----------------------------------------------------------------------------
// Purpose: register/unregister cvars
//-----------------------------------------------------------------------------

void ConVar_Register(int nCVarFlag = FCVAR_CLIENTDLL, IConCommandBaseAccessor *pAccessor = NULL);
void ConVar_Unregister();

//-----------------------------------------------------------------------------
// Purpose: prints description about a cvar
//-----------------------------------------------------------------------------

void ConVar_PrintDescription(const ConCommandBase *pVar);

//-----------------------------------------------------------------------------
// Sources of console commands
//-----------------------------------------------------------------------------

enum cmd_source_t
{
	src_client = 0,	// came in over a net connection as a clc_stringcmd
	src_command		// from the command buffer
};

//-----------------------------------------------------------------------------
// Any executable that wants to use ConVars need to implement one of
// these to hook up access to console variables.
//-----------------------------------------------------------------------------

class IConCommandBaseAccessor
{
public:
	virtual bool RegisterConCommandBase(ConCommandBase *pVar) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: The base console invoked command/cvar interface
//-----------------------------------------------------------------------------

class ConCommandBase
{
	friend class CCvar;
	friend class ConVar;
	friend class ConCommand;
	
	friend void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor);
	friend void ConVar_Unregister();

public:
	ConCommandBase( void );
	ConCommandBase( const char *pszName, const char *pszHelpString = NULL, int flags = 0 );

	virtual						~ConCommandBase();

	virtual void				Create( const char *pszName, const char *pszDescription, int flags );

	virtual bool				IsCommand( void ) const;

	virtual bool				IsFlagSet( int flag ) const;
	virtual void				AddFlags( int flags );
	virtual void				RemoveFlags( int flags );
	virtual int					GetFlags( void ) const;

	virtual const char			*GetName( void ) const;
	virtual const char			*GetHelpText( void ) const;

	virtual bool				IsRegistered( void ) const;

	const ConCommandBase		*GetNext( void ) const;
	ConCommandBase				*GetNext( void );

public:
	// Returns the DLL identifier
	virtual CVarDLLIdentifier_t	GetDLLIdentifier( void ) const;

	virtual void				Init();
	void						Shutdown();

private:
	ConCommandBase				*m_pNext;

	const char					*m_pszName;
	const char					*m_pszHelpString;

	bool						m_bRegistered;

protected:
	int							m_nFlags;

protected:
	// ConVars add themselves to this list for the executable. 
	// Then ConVar_Register runs through  all the console variables 
	// and registers them into a global list stored in vstdlib.dll
	static ConCommandBase		*s_pConCommandBases;

	// ConVars in this executable use this 'global' to access values.
	static IConCommandBaseAccessor *s_pAccessor;
};

//-----------------------------------------------------------------------------
// Command tokenizer
//-----------------------------------------------------------------------------

class CCommand
{
public:
	CCommand( void );
	CCommand( int nArgC, const char **ppArgV );

	int				ArgC( void ) const;					// Count of arguments
	const char		**ArgV( void ) const;				// Arguments pointer
	const char		*operator[]( int nIndex ) const;	// Gets at arguments
	const char		*Arg( int nIndex ) const;			// Gets at arguments
	
	// Helper functions to parse arguments to commands.
	const char		*FindArg( const char *pszName ) const;
	int				FindArgInt( const char *pszName, int nDefaultVal ) const;

private:
	int				m_nArgc;
	const char		**m_ppArgv;
};

inline int CCommand::ArgC(void) const
{
	return m_nArgc;
}

inline const char **CCommand::ArgV(void) const
{
	return m_nArgc ? (const char **)m_ppArgv : NULL;
}

inline const char *CCommand::Arg(int nIndex) const
{
	if ( nIndex < 0 || nIndex >= m_nArgc )
		return "";

	return m_ppArgv[nIndex];
}

inline const char *CCommand::operator[](int nIndex) const
{
	return Arg( nIndex );
}

//-----------------------------------------------------------------------------
// Purpose: The console invoked command
//-----------------------------------------------------------------------------

class ConCommand : public ConCommandBase
{
	friend class CCvar;

public:
	ConCommand( const char *pszName, FnCommandCallback_t pfnCallback, const char *pszHelpString = 0, int flags = 0 );

	virtual				~ConCommand();

	virtual bool		IsCommand( void ) const;

	virtual bool		IsFlagSet( int flag ) const;
	virtual void		AddFlags( int flags );
	virtual void		RemoveFlags( int flags );
	virtual int			GetFlags( void ) const;

	cmd_t				*GetCmdPointer( void ) const;

private:
	cmd_t				*m_pCommand;
	FnCommandCallback_t m_pfnCallback;
};

//-----------------------------------------------------------------------------
// Purpose: A console variable
//-----------------------------------------------------------------------------

class ConVar : public ConCommandBase
{
	friend class CCvar;

public:
	ConVar( const char *pszName, const char *pszDefaultValue, int flags = 0);
	ConVar( const char *pszName, const char *pszDefaultValue, int flags, const char *pszHelpString );
	ConVar( const char *pszName, const char *pszDefaultValue, int flags, const char *pszHelpString, bool bMin, float fMin, bool bMax, float fMax );

	virtual						~ConVar();

	virtual void				Create( const char *pszName, const char *pszDefaultValue, int flags = 0,
									   const char *pszHelpString = 0, bool bMin = false, float fMin = 0.0, bool bMax = false, float fMax = false );

	virtual bool				IsCommand( void ) const;

	virtual bool				IsFlagSet( int flag ) const;
	virtual void				AddFlags( int flags );
	virtual void				RemoveFlags( int flags );
	virtual int					GetFlags( void ) const;

	cvar_t						*GetCvarPointer( void ) const;

	// Retrieve value
	float						GetFloat( void ) const;
	int							GetInt( void ) const;
	Color						GetColor( void ) const;
	bool						GetBool( void ) const;
	const char					*GetString( void ) const;
	
	// Set value
	void						SetValue( const char *value );
	void						SetValue( float value );
	void						SetValue( int value );
	void						SetValue( bool value );
	void						SetValue( Color value );
	
	// Reset to default value
	void						Revert( void );

	// True if it has a min/max setting
	bool						HasMin( void ) const;
	bool						HasMax( void ) const;

	bool						GetMin( float &minVal ) const;
	bool						GetMax( float &maxVal ) const;

	float						GetMinValue( void ) const;
	float						GetMaxValue( void ) const;

	const char					*GetDefault( void ) const;
	void						SetDefault( const char *pszDefault );

	// Clamp cvar's current value
	bool						Clamp( void );

	// Clamp given value
	bool						ClampValue( float &value );

protected:
	// The actual pointer to cvar
	cvar_t						*m_pCvar;

	// Static data
	const char					*m_pszDefaultValue;
	
	// Min/Max values
	bool						m_bHasMin;
	bool						m_bHasMax;
	float						m_fMinVal;
	float						m_fMaxVal;
};

//-----------------------------------------------------------------------------
// Purpose: Utility macros to quicky generate a simple console command
//-----------------------------------------------------------------------------

#define CON_COMMAND( name, description ) \
	static void name( const CCommand &args ); \
	static void command_wrapper__##name() { CCommand args( CVar()->ArgC(), CVar()->ArgV() ); name(args); } \
	static ConCommand name##_command( #name, command_wrapper__##name, description ); \
	static void name( const CCommand &args )

#define CON_COMMAND_F( name, description, flags ) \
	static void name( const CCommand &args ); \
	static void command_wrapper__##name() { CCommand args( CVar()->ArgC(), CVar()->ArgV() ); name(args); } \
	static ConCommand name##_command( #name, command_wrapper__##name, description, flags ); \
	static void name( const CCommand &args )

#define CON_COMMAND_EXTERN( name, _funcname, description ) \
	void _funcname( const CCommand &args ); \
	static void command_wrapper__##name() { CCommand args( CVar()->ArgC(), CVar()->ArgV() ); _funcname(args); } \
	static ConCommand name##_command( #name, command_wrapper__##name, description ); \
	void _funcname( const CCommand &args )

#define CON_COMMAND_EXTERN_F( name, _funcname, description, flags ) \
	void _funcname( const CCommand &args ); \
	static void command_wrapper__##name() { CCommand args( CVar()->ArgC(), CVar()->ArgV() ); _funcname(args); } \
	static ConCommand name##_command( #name, _funcname, description, flags ); \
	void _funcname( const CCommand &args )

//-----------------------------------------------------------------------------
// Utility macros without CCommand wrapper
//-----------------------------------------------------------------------------

#define CON_COMMAND_NO_WRAPPER( name, description ) \
	static void name(); \
	static ConCommand name##_command( #name, name, description ); \
	static void name()

#define CON_COMMAND_F_NO_WRAPPER( name, description, flags ) \
	static void name(); \
	static ConCommand name##_command( #name, name, description, flags ); \
	static void name()

#define CON_COMMAND_EXTERN_NO_WRAPPER( name, _funcname, description ) \
	void _funcname(); \
	static ConCommand name##_command( #name, _funcname, description ); \
	void _funcname()

#define CON_COMMAND_EXTERN_F_NO_WRAPPER( name, _funcname, description, flags ) \
	void _funcname(); \
	static ConCommand name##_command( #name, _funcname, description, flags ); \
	void _funcname()

#endif // CONVAR_H