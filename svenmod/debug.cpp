#include "debug.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#error Implement Linux equivalent
#endif
#include <gl/GL.h>

#include <hl_sdk/engine/APIProxy.h>

//-----------------------------------------------------------------------------
// CDebugDrawContext implementation
//-----------------------------------------------------------------------------

CDebugDrawContext::CDebugDrawContext(const Color &color, float duration) : m_color(color)
{
	if ( duration < 0.f )
		duration = 0.f;

	m_flDrawTime = g_pEngineFuncs->GetClientTime() + duration;
}

//-----------------------------------------------------------------------------
// CDebug implementation
//-----------------------------------------------------------------------------

CDebug::~CDebug()
{
	DrawClear();
}

void CDebug::DrawPoint(const Vector &vPoint, const Color &color, float size, float duration)
{
	CDebugDrawContext *pDebugContext = new CDebugDrawContext(color, duration);

	pDebugContext->m_type = DEBUG_POINT;

	pDebugContext->m_vecOrigin = vPoint;
	pDebugContext->m_flSize = size;

	m_vDebugContext.push_back( pDebugContext );
}

void CDebug::DrawLine(const Vector &vStart, const Vector &vEnd, const Color &color, float width, float duration)
{
	CDebugDrawContext *pDebugContext = new CDebugDrawContext(color, duration);

	pDebugContext->m_type = DEBUG_LINE;

	pDebugContext->m_vecStart = vStart;
	pDebugContext->m_vecEnd = vEnd;
	pDebugContext->m_flSize = width;

	m_vDebugContext.push_back( pDebugContext );
}

void CDebug::DrawBox(const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Color &color, float duration)
{
	CDebugDrawContext *pDebugContext = new CDebugDrawContext(color, duration);

	pDebugContext->m_type = DEBUG_BOX;

	pDebugContext->m_vecOrigin = vOrigin;
	pDebugContext->m_vecMins = vMins;
	pDebugContext->m_vecMaxs = vMaxs;

	m_vDebugContext.push_back( pDebugContext );
}

void CDebug::DrawBoxAngles(const Vector &vOrigin, const Vector &vMins, const Vector &vMaxs, const Vector &vAngles, const Color &color, float duration)
{
	CDebugDrawContext *pDebugContext = new CDebugDrawContext(color, duration);

	pDebugContext->m_type = DEBUG_BOX_ANGLES;

	pDebugContext->m_vecOrigin = vOrigin;
	pDebugContext->m_vecAngles = vAngles;
	pDebugContext->m_vecMins = vMins;
	pDebugContext->m_vecMaxs = vMaxs;

	m_vDebugContext.push_back( pDebugContext );
}

void CDebug::DrawClear(void)
{
	for (size_t i = 0; i < m_vDebugContext.size(); i++)
	{
		delete m_vDebugContext[i];
	}

	m_vDebugContext.clear();
}

void CDebug::Draw()
{
	for (size_t i = 0; i < m_vDebugContext.size(); i++)
	{
		CDebugDrawContext *pDebugContext = m_vDebugContext[i];

		if ( pDebugContext->m_flDrawTime < g_pEngineFuncs->GetClientTime() )
		{
			m_vDebugContext.erase(m_vDebugContext.begin() + i);
			i--;

			delete pDebugContext;
			continue;
		}

		int type = pDebugContext->m_type;

		glEnable( GL_BLEND );
		glDisable( GL_ALPHA_TEST );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );

		glDisable( GL_TEXTURE_2D );

		if (type == DEBUG_POINT)
		{
			glColor4ub( pDebugContext->m_color.r, pDebugContext->m_color.g, pDebugContext->m_color.b, pDebugContext->m_color.a );
			glPointSize( pDebugContext->m_flSize );

			glBegin( GL_POINTS );
				glVertex3f( VectorExpand(pDebugContext->m_vecOrigin) );
			glEnd();

			glPointSize( 1.f );
		}
		else if (type == DEBUG_LINE)
		{
			glColor4ub( pDebugContext->m_color.r, pDebugContext->m_color.g, pDebugContext->m_color.b, pDebugContext->m_color.a );
			glLineWidth( pDebugContext->m_flSize );

			glBegin( GL_LINES );
				glVertex3f( VectorExpand(pDebugContext->m_vecStart) );
				glVertex3f( VectorExpand(pDebugContext->m_vecEnd) );
			glEnd();

			glLineWidth( 1.f );
		}
		else if (type == DEBUG_BOX || type == DEBUG_BOX_ANGLES)
		{
			Vector vecPoints[8];

			Vector vecMins = pDebugContext->m_vecMins;
			Vector vecMaxs = pDebugContext->m_vecMaxs;

			if ( type == DEBUG_BOX )
			{
				VectorAdd( vecMins, pDebugContext->m_vecOrigin, vecMins );
				VectorAdd( vecMaxs, pDebugContext->m_vecOrigin, vecMaxs );
			}

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
			
			if ( type == DEBUG_BOX_ANGLES )
			{
				Vector temp;
				float localSpaceToWorld[3][4];

				AngleMatrix( pDebugContext->m_vecAngles, localSpaceToWorld );

				localSpaceToWorld[0][3] = pDebugContext->m_vecOrigin[0];
				localSpaceToWorld[1][3] = pDebugContext->m_vecOrigin[1];
				localSpaceToWorld[2][3] = pDebugContext->m_vecOrigin[2];

				for (int i = 0; i < 8; i++)
				{
					VectorTransform(vecPoints[i], localSpaceToWorld, temp);

					vecPoints[i] = temp;
				}
			}

			glColor4ub( pDebugContext->m_color.r, pDebugContext->m_color.g, pDebugContext->m_color.b, pDebugContext->m_color.a );

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
		}

		glEnable( GL_TEXTURE_2D );

		glDisable( GL_BLEND );
		glDisable( GL_ALPHA_TEST );
	}
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CDebug g_Debug;
IDebug *g_pDebug = (IDebug *)&g_Debug;

IDebug *Debug()
{
	return g_pDebug;
}