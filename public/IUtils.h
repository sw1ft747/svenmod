#ifndef IUTILS_H
#define IUTILS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "steamtypes.h"
#include "sys.h"

#include "math/vector.h"

//-----------------------------------------------------------------------------
// Purpose: utility functions
//-----------------------------------------------------------------------------

abstract_class IUtils
{
public:
	virtual					~IUtils() {}

	virtual bool			IsValidSteam64ID( uint64 steamid ) = 0;

	virtual int				GetScreenWidth( void ) = 0;
	virtual int				GetScreenHeight( void ) = 0;

	virtual bool			WorldToScreen( Vector &vWorldOrigin, Vector2D &vScreen ) = 0;
	virtual void			ScreenToWorld( Vector2D &vScreen, Vector &vWorldOrigin ) = 0;

	virtual int				DrawConsoleString( int x, int y, const char *pszFormat, ... ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

extern IUtils *g_pUtils;
PLATFORM_INTERFACE IUtils *Utils();

#endif // IUTILS_H
