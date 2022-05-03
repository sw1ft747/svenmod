//===== Copyright 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#include <IRegistry.h>
#include <stdio.h>
#include <sys.h>

//-----------------------------------------------------------------------------
// Purpose: Exposes registry interface to rest of launcher
//-----------------------------------------------------------------------------

class CRegistry : public IRegistry
{
public:
							CRegistry( void );
	virtual					~CRegistry( void );

	virtual void			Init( void );
	virtual void			Shutdown( void );
	
	virtual int				ReadInt( const char *key, int defaultValue = 0);
	virtual void			WriteInt( const char *key, int value );

	virtual const char		*ReadString( const char *key, const char *defaultValue = NULL );
	virtual void			WriteString( const char *key, const char *value );

private:
	bool			m_bValid;
#ifdef _WIN32
	HKEY			m_hKey;
#endif
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

static CRegistry s_Registry;
IRegistry *g_pRegistry = (IRegistry *)&s_Registry;

IRegistry *Registry()
{
	return g_pRegistry;
}

#ifdef PLATFORM_WINDOWS

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CRegistry::CRegistry( void )
{
	// Assume failure
	m_bValid	= false;
	m_hKey		= 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CRegistry::~CRegistry( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Read integer from registry
// Input  : *key - 
//			defaultValue - 
// Output : int
//-----------------------------------------------------------------------------
int CRegistry::ReadInt( const char *key, int defaultValue /*= 0*/ )
{
	LONG lResult;           // Registry function result code
	DWORD dwType;           // Type of key
	DWORD dwSize;           // Size of element data

	int value;

	if ( !m_bValid )
	{
		return defaultValue;
	}

	dwSize = sizeof( DWORD );

	lResult = RegQueryValueEx(
		m_hKey,		// handle to key
		key,	// value name
		0,			// reserved
		&dwType,    // type buffer
		(LPBYTE)&value,    // data buffer
		&dwSize );  // size of data buffer

	if (lResult != ERROR_SUCCESS)  // Failure
		return defaultValue;

	if (dwType != REG_DWORD)
		return defaultValue;

	return value;
}

//-----------------------------------------------------------------------------
// Purpose: Save integer to registry
// Input  : *key - 
//			value - 
//-----------------------------------------------------------------------------
void CRegistry::WriteInt( const char *key, int value )
{
	// Size of element data
	DWORD dwSize;           

	if ( !m_bValid )
	{
		return;
	}

	dwSize = sizeof( DWORD );

	RegSetValueEx(
		m_hKey,		// handle to key
		key,	// value name
		0,			// reserved
		REG_DWORD,		// type buffer
		(LPBYTE)&value,    // data buffer
		dwSize );  // size of data buffer
}

//-----------------------------------------------------------------------------
// Purpose: Read string value from registry
// Input  : *key - 
//			*defaultValue - 
// Output : const char
//-----------------------------------------------------------------------------
const char *CRegistry::ReadString( const char *key, const char *defaultValue /* = NULL */ )
{
	LONG lResult;        
	// Type of key
	DWORD dwType;        
	// Size of element data
	DWORD dwSize = 512;           

	static char value[ 512 ];

	value[0] = 0;

	if ( !m_bValid )
	{
		return defaultValue;
	}

	lResult = RegQueryValueEx(
		m_hKey,		// handle to key
		key,	// value name
		0,			// reserved
		&dwType,    // type buffer
		(unsigned char *)value,    // data buffer
		&dwSize );  // size of data buffer

	if ( lResult != ERROR_SUCCESS ) 
	{
		return defaultValue;
	}

	if ( dwType != REG_SZ )
	{
		return defaultValue;
	}

	return value;
}

//-----------------------------------------------------------------------------
// Purpose: Save string to registry
// Input  : *key - 
//			*value - 
//-----------------------------------------------------------------------------
void CRegistry::WriteString( const char *key, const char *value )
{
	DWORD dwSize;           // Size of element data

	if ( !m_bValid )
	{
		return;
	}

	dwSize = (DWORD)( strlen( value ) + 1 );

	RegSetValueEx(
		m_hKey,		// handle to key
		key,	// value name
		0,			// reserved
		REG_SZ,		// type buffer
		(LPBYTE)value,    // data buffer
		dwSize );  // size of data buffer
}

//-----------------------------------------------------------------------------
// Purpose: Open default launcher key based on game directory
//-----------------------------------------------------------------------------
void CRegistry::Init( void )
{
	LONG lResult;
	DWORD dwDisposition;

	char subDir[] = "Software\\Valve\\Half-Life\\Settings\\";

	lResult = RegCreateKeyEx(HKEY_CURRENT_USER, subDir, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisposition);

	if (lResult != ERROR_SUCCESS)
	{
		m_bValid = false;
		return;
	}

	m_bValid = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CRegistry::Shutdown( void )
{
	if ( !m_bValid )
		return;

	// Make invalid
	m_bValid = false;
	RegCloseKey( m_hKey );
}

#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CRegistry::CRegistry( void )
{
	// Assume failure
	m_bValid	= false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CRegistry::~CRegistry( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Read integer from registry
// Input  : *key - 
//			defaultValue - 
// Output : int
//-----------------------------------------------------------------------------
int CRegistry::ReadInt( const char *key, int defaultValue /*= 0*/ )
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Save integer to registry
// Input  : *key - 
//			value - 
//-----------------------------------------------------------------------------
void CRegistry::WriteInt( const char *key, int value )
{
}

//-----------------------------------------------------------------------------
// Purpose: Read string value from registry
// Input  : *key - 
//			*defaultValue - 
// Output : const char
//-----------------------------------------------------------------------------
const char *CRegistry::ReadString( const char *key, const char *defaultValue /* = NULL */ )
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Save string to registry
// Input  : *key - 
//			*value - 
//-----------------------------------------------------------------------------
void CRegistry::WriteString( const char *key, const char *value )
{
}



bool CRegistry::DirectInit( const char *subDirectoryUnderValve )
{

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Open default launcher key based on game directory
//-----------------------------------------------------------------------------
bool CRegistry::Init( const char *platformName )
{
	char subDir[ 512 ];
	snprintf( subDir, sizeof(subDir), "%s\\Settings", platformName );
	return DirectInit( subDir );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CRegistry::Shutdown( void )
{
	if ( !m_bValid )
		return;

	// Make invalid
	m_bValid = false;
}

#endif // PLATFORM_WINDOWS
