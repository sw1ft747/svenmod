#ifndef UTILS_H
#define UTILS_H

#ifdef _WIN32
#pragma once
#endif

#include <IUtils.h>

//-----------------------------------------------------------------------------
// CUtils
//-----------------------------------------------------------------------------

class CUtils : public IUtils
{
public:
	CUtils();

	void Init();

	virtual int				GetScreenWidth( void );
	virtual int				GetScreenHeight( void );

	virtual bool			WorldToScreen( Vector &vWorldOrigin, Vector2D &vScreen );
	virtual void			ScreenToWorld( Vector2D &vScreen, Vector &vWorldOrigin );

	virtual CNetMessageParams *GetNetMessageParams( void );
	virtual void			ApplyReadToNetMessageBuffer(CMessageBuffer *buffer);
	virtual void			ApplyReadToNetMessageBuffer(int readcount, int badread);

	virtual const netmsg_t *FindNetworkMessage( int iNetMessageType );
	virtual const netmsg_t *FindNetworkMessage( const char *pszName );
	virtual const usermsg_t *FindUserMessage( const char *pszName );
	virtual const event_t	*FindEventHook( const char *pszName );

	virtual void			PrintChatText( const char *pszMessage, ... );

	virtual void			DrawSetTextColor( float r, float g, float b );
	virtual int				DrawConsoleString( int x, int y, const char *pszFormat, ... );

private:
	int m_iScreenWidth;
	int m_iScreenHeight;

	CNetMessageParams m_NetMessageParams;
};

extern CUtils g_Utils;

#endif // UTILS_H
