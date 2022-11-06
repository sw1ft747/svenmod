#ifndef IUTILS_H
#define IUTILS_H

#ifdef _WIN32
#pragma once
#endif

#include "hl_sdk/common/event.h"
#include "hl_sdk/common/usermsg.h"
#include "hl_sdk/common/netmsg.h"
#include "hl_sdk/common/sizebuf.h"

#include "platform.h"
#include "steamtypes.h"
#include "sys.h"

#include "math/vector.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

class CMessageBuffer;

//-----------------------------------------------------------------------------
// CNetMessageParams
//-----------------------------------------------------------------------------

class CNetMessageParams
{
public:
	const sizebuf_t *buffer;
	int readcount;
	bool badread;
};

//-----------------------------------------------------------------------------
// Purpose: utility functions
//-----------------------------------------------------------------------------

abstract_class IUtils
{
public:
	virtual					~IUtils() {}

	virtual int				GetScreenWidth( void ) = 0;
	virtual int				GetScreenHeight( void ) = 0;

	virtual bool			WorldToScreen( Vector &vWorldOrigin, Vector2D &vScreen ) = 0;
	virtual void			ScreenToWorld( Vector2D &vScreen, Vector &vWorldOrigin ) = 0;

	virtual CNetMessageParams *GetNetMessageParams( void ) = 0;
	virtual void			ApplyReadToNetMessageBuffer( CMessageBuffer *buffer ) = 0;
	virtual void			ApplyReadToNetMessageBuffer( int readcount, int badread ) = 0;

	virtual const netmsg_t *FindNetworkMessage( int iNetMessageType ) = 0;
	virtual const netmsg_t *FindNetworkMessage( const char *pszName ) = 0;
	virtual const usermsg_t *FindUserMessage( const char *pszName ) = 0;
	virtual const event_t	*FindEventHook( const char *pszName ) = 0;

	virtual void			PrintChatText( const char *pszMessage, ... ) = 0;

	virtual void			DrawSetTextColor( float r, float g, float b ) = 0;
	virtual int				DrawConsoleString( int x, int y, const char *pszFormat, ... ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

extern IUtils *g_pUtils;
PLATFORM_INTERFACE IUtils *Utils();

#endif // IUTILS_H
