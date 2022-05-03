#include "utils.h"

#include <IVideoMode.h>
#include <hl_sdk/engine/APIProxy.h>

//-----------------------------------------------------------------------------
// CUtils implementation
//-----------------------------------------------------------------------------

CUtils::CUtils()
{
	m_iScreenWidth = 0;
	m_iScreenHeight = 0;
}

void CUtils::Init()
{
	vmode_t *mode = g_pVideoMode->GetCurrentMode();

	m_iScreenWidth = mode->width;
	m_iScreenHeight = mode->height;
}

bool CUtils::IsValidSteam64ID(uint64 steamid)
{
	return (steamid & 76561197960265728uLL) != 0;
}

int CUtils::GetScreenWidth(void)
{
	return m_iScreenWidth;
}

int CUtils::GetScreenHeight(void)
{
	return m_iScreenHeight;
}

bool CUtils::WorldToScreen(Vector &vWorldOrigin, Vector2D &vScreen)
{
	int iResult = g_pTriangleAPI->WorldToScreen(vWorldOrigin, vScreen);

	if ( !iResult && vScreen.x <= 1 && vScreen.y <= 1 && vScreen.x >= -1 && vScreen.y >= -1 )
	{
		vScreen.x = (m_iScreenWidth / 2 * vScreen.x) + (vScreen.x + m_iScreenWidth / 2);
		vScreen.y = -(m_iScreenHeight / 2 * vScreen.y) + (vScreen.y + m_iScreenHeight / 2);

		return true;
	}

	return false;
}

void CUtils::ScreenToWorld(Vector2D &vScreen, Vector &vWorldOrigin)
{
	g_pTriangleAPI->ScreenToWorld(vScreen, vWorldOrigin);
}

void CUtils::DrawSetTextColor(float r, float g, float b)
{
	g_pEngineFuncs->DrawSetTextColor(r, g, b);
}

int CUtils::DrawConsoleString(int x, int y, const char *pszFormat, ...)
{
	static char szFormattedMsg[4096] = { 0 };

	va_list args;
	va_start(args, pszFormat);
	vsnprintf(szFormattedMsg, sizeof(szFormattedMsg), pszFormat, args);
	va_end(args);

	szFormattedMsg[sizeof(szFormattedMsg) - 1] = 0;

	return g_pEngineFuncs->DrawConsoleString(x, y, szFormattedMsg);
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CUtils g_Utils;
IUtils *g_pUtils = (IUtils *)&g_Utils;

IUtils *Utils()
{
	return g_pUtils;
}
