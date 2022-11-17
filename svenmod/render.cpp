#include "render.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#error Implement Linux equivalent
#endif

#include <gl/GL.h>
#include <algorithm>

#include <hl_sdk/engine/APIProxy.h>

extern double *g_pRealtime;

//-----------------------------------------------------------------------------
// Draw point
//-----------------------------------------------------------------------------

CDrawPoint::CDrawPoint(const Vector &vPoint, const Color &color, float size) : m_color(color)
{
	m_vecOrigin = vPoint;
	m_flSize = size;
}

void CDrawPoint::Draw()
{
	glEnable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable( GL_TEXTURE_2D );

	glColor4ub( m_color.r, m_color.g, m_color.b, m_color.a );
	glPointSize( m_flSize );

	glBegin( GL_POINTS );
		glVertex3f( VectorExpand(m_vecOrigin) );
	glEnd();

	glPointSize( 1.f );

	glEnable( GL_TEXTURE_2D );

	glDisable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
}

bool CDrawPoint::ShouldStopDraw()
{
	return false;
}

const Vector &CDrawPoint::GetDrawOrigin() const
{
	return m_vecOrigin;
}

//-----------------------------------------------------------------------------
// Draw line
//-----------------------------------------------------------------------------

CDrawLine::CDrawLine(const Vector &vStart, const Vector &vEnd, const Color &color, float width) : m_color(color)
{
	m_vecOrigin = vStart + (vEnd - vStart) * 0.5f;
	m_vecStart = vStart;
	m_vecEnd = vEnd;
	m_flWidth = width;
}

void CDrawLine::Draw()
{
	glEnable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable( GL_TEXTURE_2D );

	glColor4ub( m_color.r, m_color.g, m_color.b, m_color.a );
	glLineWidth( m_flWidth );

	glBegin( GL_LINES );
		glVertex3f( VectorExpand(m_vecStart) );
		glVertex3f( VectorExpand(m_vecEnd) );
	glEnd();

	glLineWidth( 1.f );

	glEnable( GL_TEXTURE_2D );

	glDisable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
}

bool CDrawLine::ShouldStopDraw()
{
	return false;
}

const Vector &CDrawLine::GetDrawOrigin() const
{
	return m_vecOrigin;
}

//-----------------------------------------------------------------------------
// Draw box
//-----------------------------------------------------------------------------

CDrawBox::CDrawBox(const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Color &color) : m_color(color)
{
	if ( vOrigin.x == 0.f && vOrigin.y == 0.f && vOrigin.z == 0.f )
	{
		m_vecDrawOrigin = vMins + (vMaxs - vMins) * 0.5f;
	}
	else
	{
		m_vecDrawOrigin = vOrigin;
	}

	m_vecOrigin = vOrigin;
	m_vecMins = vMins;
	m_vecMaxs = vMaxs;
}

void CDrawBox::Draw()
{
	glEnable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable( GL_TEXTURE_2D );

	Vector vecPoints[8];

	Vector vecMins = m_vecMins;
	Vector vecMaxs = m_vecMaxs;

	VectorAdd( vecMins, m_vecOrigin, vecMins );
	VectorAdd( vecMaxs, m_vecOrigin, vecMaxs );

	// Build points of box
	vecPoints[0].x = vecMins.x;
	vecPoints[0].y = vecMins.y;
	vecPoints[0].z = vecMins.z;

	vecPoints[1].x = vecMins.x;
	vecPoints[1].y = vecMaxs.y;
	vecPoints[1].z = vecMins.z;

	vecPoints[2].x = vecMaxs.x;
	vecPoints[2].y = vecMaxs.y;
	vecPoints[2].z = vecMins.z;

	vecPoints[3].x = vecMaxs.x;
	vecPoints[3].y = vecMins.y;
	vecPoints[3].z = vecMins.z;

	vecPoints[4].x = vecMins.x;
	vecPoints[4].y = vecMins.y;
	vecPoints[4].z = vecMaxs.z;

	vecPoints[5].x = vecMins.x;
	vecPoints[5].y = vecMaxs.y;
	vecPoints[5].z = vecMaxs.z;

	vecPoints[6].x = vecMaxs.x;
	vecPoints[6].y = vecMaxs.y;
	vecPoints[6].z = vecMaxs.z;

	vecPoints[7].x = vecMaxs.x;
	vecPoints[7].y = vecMins.y;
	vecPoints[7].z = vecMaxs.z;

	glColor4ub( m_color.r, m_color.g, m_color.b, m_color.a );

	for (int i = 0; i < 4; i++)
	{
		int j = (i + 1) % 4;

		glBegin( GL_TRIANGLE_STRIP );
			glVertex3f( VectorExpand(vecPoints[i]) );
			glVertex3f( VectorExpand(vecPoints[j]) );
			glVertex3f( VectorExpand(vecPoints[i + 4]) );
			glVertex3f( VectorExpand(vecPoints[j + 4]) );
		glEnd();
	}

	// Bottom
	glBegin( GL_TRIANGLE_STRIP );
		glVertex3f( VectorExpand(vecPoints[2]) );
		glVertex3f( VectorExpand(vecPoints[1]) );
		glVertex3f( VectorExpand(vecPoints[3]) );
		glVertex3f( VectorExpand(vecPoints[0]) );
	glEnd();

	// Top
	glBegin( GL_TRIANGLE_STRIP );
		glVertex3f( VectorExpand(vecPoints[4]) );
		glVertex3f( VectorExpand(vecPoints[5]) );
		glVertex3f( VectorExpand(vecPoints[7]) );
		glVertex3f( VectorExpand(vecPoints[6]) );
	glEnd();

	glEnable( GL_TEXTURE_2D );

	glDisable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
}

bool CDrawBox::ShouldStopDraw()
{
	return false;
}

const Vector &CDrawBox::GetDrawOrigin() const
{
	return m_vecDrawOrigin;
}

//-----------------------------------------------------------------------------
// Draw box angles
//-----------------------------------------------------------------------------

CDrawBoxAngles::CDrawBoxAngles(const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Vector &vAngles, const Color &color) : m_color(color)
{
	if ( vOrigin.x == 0.f && vOrigin.y == 0.f && vOrigin.z == 0.f )
	{
		m_vecDrawOrigin = vMins + (vMaxs - vMins) * 0.5f;
	}
	else
	{
		m_vecDrawOrigin = vOrigin;
	}

	m_vecOrigin = vOrigin;
	m_vecAngles = vAngles;
	m_vecMins = vMins;
	m_vecMaxs = vMaxs;
}

void CDrawBoxAngles::Draw()
{
	glEnable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable( GL_TEXTURE_2D );

	Vector vecPoints[8];

	Vector vecMins = m_vecMins;
	Vector vecMaxs = m_vecMaxs;

	// Build points of box
	vecPoints[0].x = vecMins.x;
	vecPoints[0].y = vecMins.y;
	vecPoints[0].z = vecMins.z;

	vecPoints[1].x = vecMins.x;
	vecPoints[1].y = vecMaxs.y;
	vecPoints[1].z = vecMins.z;

	vecPoints[2].x = vecMaxs.x;
	vecPoints[2].y = vecMaxs.y;
	vecPoints[2].z = vecMins.z;

	vecPoints[3].x = vecMaxs.x;
	vecPoints[3].y = vecMins.y;
	vecPoints[3].z = vecMins.z;

	vecPoints[4].x = vecMins.x;
	vecPoints[4].y = vecMins.y;
	vecPoints[4].z = vecMaxs.z;

	vecPoints[5].x = vecMins.x;
	vecPoints[5].y = vecMaxs.y;
	vecPoints[5].z = vecMaxs.z;

	vecPoints[6].x = vecMaxs.x;
	vecPoints[6].y = vecMaxs.y;
	vecPoints[6].z = vecMaxs.z;

	vecPoints[7].x = vecMaxs.x;
	vecPoints[7].y = vecMins.y;
	vecPoints[7].z = vecMaxs.z;

	Vector temp;
	float localSpaceToWorld[3][4];

	AngleMatrix( m_vecAngles, localSpaceToWorld );

	localSpaceToWorld[0][3] = m_vecOrigin[0];
	localSpaceToWorld[1][3] = m_vecOrigin[1];
	localSpaceToWorld[2][3] = m_vecOrigin[2];

	for (int i = 0; i < 8; i++)
	{
		VectorTransform(vecPoints[i], localSpaceToWorld, temp);

		vecPoints[i] = temp;
	}

	glColor4ub( m_color.r, m_color.g, m_color.b, m_color.a );

	for (int i = 0; i < 4; i++)
	{
		int j = (i + 1) % 4;

		glBegin( GL_TRIANGLE_STRIP );
			glVertex3f( VectorExpand(vecPoints[i]) );
			glVertex3f( VectorExpand(vecPoints[j]) );
			glVertex3f( VectorExpand(vecPoints[i + 4]) );
			glVertex3f( VectorExpand(vecPoints[j + 4]) );
		glEnd();
	}

	// Bottom
	glBegin( GL_TRIANGLE_STRIP );
		glVertex3f( VectorExpand(vecPoints[2]) );
		glVertex3f( VectorExpand(vecPoints[1]) );
		glVertex3f( VectorExpand(vecPoints[3]) );
		glVertex3f( VectorExpand(vecPoints[0]) );
	glEnd();

	// Top
	glBegin( GL_TRIANGLE_STRIP );
		glVertex3f( VectorExpand(vecPoints[4]) );
		glVertex3f( VectorExpand(vecPoints[5]) );
		glVertex3f( VectorExpand(vecPoints[7]) );
		glVertex3f( VectorExpand(vecPoints[6]) );
	glEnd();

	glEnable( GL_TEXTURE_2D );

	glDisable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
}

bool CDrawBoxAngles::ShouldStopDraw()
{
	return false;
}

const Vector &CDrawBoxAngles::GetDrawOrigin() const
{
	return m_vecDrawOrigin;
}

//-----------------------------------------------------------------------------
// CRender implementation
//-----------------------------------------------------------------------------

CRender::CRender()
{
	m_vecRenderOrigin.Zero();
}

CRender::~CRender()
{
	DrawClear();
}

void CRender::AddDrawContext(IDrawContext *pContext, float duration)
{
	if ( pContext == NULL )
		return;

	if ( duration < 0.f )
		duration = 0.f;

	draw_context_t draw_context;
	
	draw_context.pDrawContext = pContext;
	draw_context.flDuration = static_cast<float>(*g_pRealtime) + duration;
	draw_context.flDistanceSqr = 0.f;

	m_vDrawContext.push_back( draw_context );
}

void CRender::DrawPoint(const Vector &vPoint, const Color &color, float size, float duration)
{
	if ( duration < 0.f )
		duration = 0.f;

	draw_context_t draw_context;

	IDrawContext *pDrawContext = new CDrawPoint(vPoint, color, size);

	draw_context.pDrawContext = pDrawContext;
	draw_context.flDuration = static_cast<float>(*g_pRealtime) + duration;
	draw_context.flDistanceSqr = 0.f;

	m_vDrawContext.push_back( draw_context );
}

void CRender::DrawLine(const Vector &vStart, const Vector &vEnd, const Color &color, float width, float duration)
{
	if ( duration < 0.f )
		duration = 0.f;

	draw_context_t draw_context;

	IDrawContext *pDrawContext = new CDrawLine(vStart, vEnd, color, width);

	draw_context.pDrawContext = pDrawContext;
	draw_context.flDuration = static_cast<float>(*g_pRealtime) + duration;
	draw_context.flDistanceSqr = 0.f;

	m_vDrawContext.push_back( draw_context );
}

void CRender::DrawBox(const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Color &color, float duration)
{
	if ( duration < 0.f )
		duration = 0.f;

	draw_context_t draw_context;

	IDrawContext *pDrawContext = new CDrawBox(vOrigin, vMins, vMaxs, color);

	draw_context.pDrawContext = pDrawContext;
	draw_context.flDuration = static_cast<float>(*g_pRealtime) + duration;
	draw_context.flDistanceSqr = 0.f;

	m_vDrawContext.push_back( draw_context );
}

void CRender::DrawBoxAngles(const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Vector &vAngles, const Color &color, float duration)
{
	if ( duration < 0.f )
		duration = 0.f;

	draw_context_t draw_context;

	IDrawContext *pDrawContext = new CDrawBoxAngles(vOrigin, vMins, vMaxs, vAngles, color);

	draw_context.pDrawContext = pDrawContext;
	draw_context.flDuration = static_cast<float>(*g_pRealtime) + duration;
	draw_context.flDistanceSqr = 0.f;

	m_vDrawContext.push_back( draw_context );
}

void CRender::DrawClear(void)
{
	for (size_t i = 0; i < m_vDrawContext.size(); i++)
	{
		delete m_vDrawContext[i].pDrawContext;
	}

	m_vDrawContext.clear();
}

void CRender::Draw()
{
	if ( m_vDrawContext.empty() )
		return;

	// TODO: optimize
	for (size_t i = 0; i < m_vDrawContext.size(); i++)
	{
		m_vDrawContext[i].flDistanceSqr = (m_vDrawContext[i].pDrawContext->GetDrawOrigin() - m_vecRenderOrigin).LengthSqr();
	}

	std::sort( m_vDrawContext.begin(), m_vDrawContext.end() );

	for (size_t i = 0; i < m_vDrawContext.size(); i++)
	{
		draw_context_t &draw_context = m_vDrawContext[i];

		if ( draw_context.flDuration < static_cast<float>(*g_pRealtime) || draw_context.pDrawContext->ShouldStopDraw() )
		{
			delete draw_context.pDrawContext;

			m_vDrawContext.erase( m_vDrawContext.begin() + i );
			i--;

			continue;
		}

		draw_context.pDrawContext->Draw();
	}
}

void CRender::SetRenderOrigin(const Vector &vOrigin)
{
	m_vecRenderOrigin = vOrigin;
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CRender g_Render;
IRender *g_pRender = (IRender *)&g_Render;

IRender *Render()
{
	return g_pRender;
}