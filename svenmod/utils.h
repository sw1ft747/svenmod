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

	virtual bool			IsValidSteam64ID( uint64 steamid );

	virtual int				GetScreenWidth( void );
	virtual int				GetScreenHeight( void );

	virtual bool			WorldToScreen( Vector &vWorldOrigin, Vector2D &vScreen );
	virtual void			ScreenToWorld( Vector2D &vScreen, Vector &vWorldOrigin );

	virtual int				DrawConsoleString( int x, int y, const char *pszFormat, ... );

private:
	int m_iScreenWidth;
	int m_iScreenHeight;
};

extern CUtils g_Utils;

#endif // UTILS_H