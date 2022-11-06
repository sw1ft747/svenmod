#ifndef DEBUG_H
#define DEBUG_H

#ifdef _WIN32
#pragma once
#endif

#include <vector>
#include <IDebug.h>

enum DebugType
{
	DEBUG_POINT = 0,
	DEBUG_LINE,
	DEBUG_BOX,
	DEBUG_BOX_ANGLES
};

//-----------------------------------------------------------------------------
// CDebugContext
//-----------------------------------------------------------------------------

class CDebugDrawContext
{
public:
	CDebugDrawContext(const Color &color, float duration);

public:
	int m_type;

	Vector m_vecOrigin;
	Vector m_vecAngles;

	union
	{
		Vector m_vecStart;
		Vector m_vecMins;
	};

	union
	{
		Vector m_vecEnd;
		Vector m_vecMaxs;
	};

	Color m_color;
	float m_flSize;

	float m_flDrawTime;
};

//-----------------------------------------------------------------------------
// CDebug
//-----------------------------------------------------------------------------

class CDebug : IDebug
{
public:
	virtual ~CDebug();

	virtual void DrawPoint( const Vector &vPoint, const Color &color, float size, float duration );
	virtual void DrawLine( const Vector &vStart, const Vector &vEnd, const Color &color, float width, float duration );
	virtual void DrawBox( const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Color &color, float duration );
	virtual void DrawBoxAngles( const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Vector &vAngles, const Color &color, float duration );
	virtual void DrawClear( void );

public:
	void Draw();

private:
	std::vector<CDebugDrawContext *> m_vDebugContext;
};

extern CDebug g_Debug;

#endif // DEBUG_H