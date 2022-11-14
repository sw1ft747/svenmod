#ifndef RENDER_H
#define RENDER_H

#ifdef _WIN32
#pragma once
#endif

#include <vector>
#include <IRender.h>

//-----------------------------------------------------------------------------
// Draw context
//-----------------------------------------------------------------------------

struct draw_context_t
{
	inline bool operator <(const draw_context_t &draw_context) const
	{
		return (flDistanceSqr > draw_context.flDistanceSqr);
	}

	IDrawContext *pDrawContext;
	float flDuration;
	float flDistanceSqr;
};

//-----------------------------------------------------------------------------
// Draw point
//-----------------------------------------------------------------------------

class CDrawPoint : public IDrawContext
{
public:
	CDrawPoint(const Vector &vPoint, const Color &color, float size);
	virtual ~CDrawPoint() {}

	virtual void Draw() override;
	virtual bool ShouldStopDraw() override;

	virtual const Vector &GetDrawOrigin() const override;

private:
	Vector m_vecOrigin;

	Color m_color;
	float m_flSize;
};

//-----------------------------------------------------------------------------
// Draw line
//-----------------------------------------------------------------------------

class CDrawLine : public IDrawContext
{
public:
	CDrawLine(const Vector &vStart, const Vector &vEnd, const Color &color, float width);
	virtual ~CDrawLine() {}

	virtual void Draw() override;
	virtual bool ShouldStopDraw() override;

	virtual const Vector &GetDrawOrigin() const override;

private:
	Vector m_vecOrigin;

	Vector m_vecStart;
	Vector m_vecEnd;

	Color m_color;
	float m_flWidth;
};

//-----------------------------------------------------------------------------
// Draw box
//-----------------------------------------------------------------------------

class CDrawBox : public IDrawContext
{
public:
	CDrawBox(const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Color &color);
	virtual ~CDrawBox() {}

	virtual void Draw() override;
	virtual bool ShouldStopDraw() override;

	virtual const Vector &GetDrawOrigin() const override;

private:
	Vector m_vecDrawOrigin;
	Vector m_vecOrigin;

	Vector m_vecMins;
	Vector m_vecMaxs;

	Color m_color;
};

//-----------------------------------------------------------------------------
// Draw box angles
//-----------------------------------------------------------------------------

class CDrawBoxAngles : public IDrawContext
{
public:
	CDrawBoxAngles(const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Vector &vAngles, const Color &color);
	virtual ~CDrawBoxAngles() {}

	virtual void Draw() override;
	virtual bool ShouldStopDraw() override;

	virtual const Vector &GetDrawOrigin() const override;

private:
	Vector m_vecDrawOrigin;

	Vector m_vecOrigin;
	Vector m_vecAngles;

	Vector m_vecMins;
	Vector m_vecMaxs;

	Color m_color;
};

//-----------------------------------------------------------------------------
// CRender
//-----------------------------------------------------------------------------

class CRender : IRender
{
public:
	CRender();
	virtual ~CRender();

	virtual void AddDrawContext(IDrawContext *pContext, float duration);

	virtual void DrawPoint( const Vector &vPoint, const Color &color, float size, float duration );
	virtual void DrawLine( const Vector &vStart, const Vector &vEnd, const Color &color, float width, float duration );
	virtual void DrawBox( const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Color &color, float duration );
	virtual void DrawBoxAngles( const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Vector &vAngles, const Color &color, float duration );
	virtual void DrawClear( void );

public:
	void Draw();
	void SetRenderOrigin(const Vector &vOrigin);

private:
	std::vector<draw_context_t> m_vDrawContext;
	Vector m_vecRenderOrigin;
};

extern CRender g_Render;

#endif // RENDER_H