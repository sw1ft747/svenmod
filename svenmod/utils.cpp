#include "utils.h"

#include <dbg.h>
#include <messagebuffer.h>
#include <client_state.h>
#include <IVideoMode.h>

#include <hl_sdk/common/protocol.h>
#include <hl_sdk/engine/APIProxy.h>

extern int *g_pClientState;

extern usermsg_t **g_ppClientUserMsgs;
extern event_t *g_pEventHooks;
extern netmsg_t *g_pNetworkMessages;

extern sizebuf_t *g_pNetMessage;
extern int *g_pNetMessageReadCount;
extern int *g_pNetMessageBadRead;

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

//-----------------------------------------------------------------------------
// Screen utilities
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
// Network Message
//-----------------------------------------------------------------------------

CNetMessageParams *CUtils::GetNetMessageParams(void)
{
	m_NetMessageParams.buffer = g_pNetMessage;
	m_NetMessageParams.readcount = *g_pNetMessageReadCount;
	m_NetMessageParams.badread = !!(*g_pNetMessageBadRead);

	return &m_NetMessageParams;
}

void CUtils::ApplyReadToNetMessageBuffer(CMessageBuffer *buffer)
{
	*g_pNetMessageReadCount = buffer->GetReadCount();
	*g_pNetMessageBadRead = !buffer->ReadOK();
}

void CUtils::ApplyReadToNetMessageBuffer(int readcount, int badread)
{
	*g_pNetMessageReadCount = readcount;
	*g_pNetMessageBadRead = badread;
}

const netmsg_t *CUtils::FindNetworkMessage(int iType)
{
	if (iType > SVC_NOP && iType <= SVC_LASTMSG)
	{
		return const_cast<const netmsg_t *>(&g_pNetworkMessages[iType]);
	}
	else
	{
		Warning("[SvenMod] CUtils::FindNetworkMessage: bad network message type (%d)\n", iType);
	}

	return NULL;
}

const netmsg_t *CUtils::FindNetworkMessage(const char *pszName)
{
	for (int i = SVC_NOP + 1; i <= SVC_LASTMSG; i++)
	{
		netmsg_t *pNetMsg = &g_pNetworkMessages[i];

		if ( !stricmp(pszName, pNetMsg->name) )
		{
			return const_cast<const netmsg_t *>(pNetMsg);
		}
	}

	Warning("[SvenMod] CHooks::HookNetworkMessage: bad network message name (%s)\n", pszName);

	return NULL;
}

//-----------------------------------------------------------------------------
// User Message
//-----------------------------------------------------------------------------

const usermsg_t *CUtils::FindUserMessage(const char *pszName)
{
	usermsg_t *pUserMsg = *g_ppClientUserMsgs;

	while (pUserMsg)
	{
		if ( !stricmp(pszName, pUserMsg->name) )
		{
			return const_cast<const usermsg_t *>(pUserMsg);
		}

		pUserMsg = pUserMsg->next;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Event Hook
//-----------------------------------------------------------------------------

const event_t *CUtils::FindEventHook(const char *pszName)
{
	event_t *pEventHook = g_pEventHooks;

	while (pEventHook)
	{
		if (pEventHook->name)
		{
			if ( !stricmp(pszName, pEventHook->name) )
			{
				return const_cast<const event_t *>(pEventHook);
			}
		}

		pEventHook = pEventHook->next;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Print to client's chat, not visible to others and when playing back demo
//-----------------------------------------------------------------------------

void CUtils::PrintChatText(const char *pszMessage, ...)
{
	size_t len;
	CMessageBuffer msgbuffer;

	static char buffer[1024];
	static char szFormattedMsg[1024];
	static const usermsg_t *pSayText = NULL;

	if ( *g_pClientState < CLS_LOADING || pszMessage == NULL || ( len = strlen(pszMessage) ) == 0 )
		return;

	if ( pSayText == NULL )
		pSayText = FindUserMessage("SayText");

	va_list args;
	va_start(args, pszMessage);
	vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszMessage, args);
	va_end(args);

	szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

	msgbuffer.Init( buffer, M_ARRAYSIZE(buffer) );
	msgbuffer.WriteByte( 0 );
	msgbuffer.WriteString( szFormattedMsg );

	buffer[M_ARRAYSIZE(buffer) - 1] = 0;

	//Msg( pszMessage );
	pSayText->function( "SayText", msgbuffer.GetBuffer()->cursize, msgbuffer.GetBuffer()->data );
}

//-----------------------------------------------------------------------------
// Draw utilities
//-----------------------------------------------------------------------------

void CUtils::DrawSetTextColor(float r, float g, float b)
{
	g_pEngineFuncs->DrawSetTextColor(r, g, b);
}

int CUtils::DrawConsoleString(int x, int y, const char *pszFormat, ...)
{
	static char szFormattedMsg[4096] = { 0 };

	va_list args;
	va_start(args, pszFormat);
	vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszFormat, args);
	va_end(args);

	szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

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
