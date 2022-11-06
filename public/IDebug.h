#ifndef IDEBUG_H
#define IDEBUG_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

#include "math/vector.h"
#include "color.h"

//-----------------------------------------------------------------------------
// Purpose: debug stuff
//-----------------------------------------------------------------------------

abstract_class IDebug
{
public:
	virtual			~IDebug() {}

	virtual void	DrawPoint( const Vector &vPoint, const Color &color, float size = 24.f, float duration = 0.f ) = 0;
	virtual void	DrawLine( const Vector &vStart, const Vector &vEnd, const Color &color, float width = 2.f, float duration = 0.f ) = 0;
	virtual void	DrawBox( const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Color &color, float duration = 0.f ) = 0;
	virtual void	DrawBoxAngles( const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Vector &vAngles, const Color &color, float duration = 0.f ) = 0;
	virtual void	DrawClear( void ) = 0;

	inline void DrawPoint( const Vector &vPoint, float r, float g, float b, float alpha, float size = 24.f, float duration = 0.f )
	{
		DrawPoint(vPoint, { r, g, b, alpha }, size, duration);
	}
	inline void DrawLine( const Vector &vStart, const Vector &vEnd, float r, float g, float b, float alpha, float width = 2.f, float duration = 0.f )
	{
		DrawLine(vStart, vEnd, { r, g, b, alpha }, width, duration);
	}
	inline void DrawBox( const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, float r, float g, float b, float alpha, float duration = 0.f )
	{
		DrawBox(vOrigin, vMins, vMaxs, { r, g, b, alpha }, duration);
	}
	inline void DrawBoxAngles( const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Vector &vAngles, float r, float g, float b, float alpha, float duration = 0.f )
	{
		DrawBoxAngles(vOrigin, vMins, vMaxs, vAngles, { r, g, b, alpha }, duration);
	}
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

extern IDebug *g_pDebug;
PLATFORM_INTERFACE IDebug *Debug();

#endif // IDEBUG_H
