#include <dbg.h>
#include <color.h>

#include <hl_sdk/engine/APIProxy.h>

#include <gameui/IGameConsole.h>
#include <gameui/gameconsole.h>

#include <IMemoryUtils.h>
#include <ISvenModAPI.h>

#include <memutils/patterns.h>

#include "dbg_sm.h"

#define MAX_CON_PRINT_MESSAGE_LENGTH 8192

static cvar_t *s_pDeveloper = NULL;
static bool s_bDbgInitialized = false;

//-----------------------------------------------------------------------------
// Colors
//-----------------------------------------------------------------------------

const Color g_MsgPrintColor = { 232, 232, 232, 255 };
const Color g_WarningPrintColor = { 255, 90, 90, 255 };

//-----------------------------------------------------------------------------
// Init debugging
//-----------------------------------------------------------------------------

void DbgInit()
{
	s_pDeveloper = g_pEngineFuncs->GetCvarPointer("developer");

	if ( !s_pDeveloper )
	{
		Sys_Error("[SvenMod] Can't get \"developer\" cvar..");
		return;
	}

	s_bDbgInitialized = true;
}

void DbgShutdown()
{
	s_bDbgInitialized = false;
}

//-----------------------------------------------------------------------------
// Debug functions
//-----------------------------------------------------------------------------

void Msg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	_vsnprintf(szFormattedMsg, MAX_CON_PRINT_MESSAGE_LENGTH, pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH - 1] = 0;

	g_pCVar->ConsolePrint(szFormattedMsg);
}

void Warning(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	_vsnprintf(szFormattedMsg, MAX_CON_PRINT_MESSAGE_LENGTH, pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH - 1] = 0;

	g_pCVar->ConsoleColorPrint(g_WarningPrintColor, szFormattedMsg);
}

void DevMsg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH] = { 0 };

	if ( g_pCVar->CanPrint() && bool(s_pDeveloper->value) )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		_vsnprintf(szFormattedMsg, MAX_CON_PRINT_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH - 1] = 0;

		g_pCVar->ConsoleDPrint(szFormattedMsg);
	}
}

void DevWarning(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH] = { 0 };

	if ( g_pCVar->CanPrint() && bool(s_pDeveloper->value) )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		_vsnprintf(szFormattedMsg, MAX_CON_PRINT_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH - 1] = 0;

		g_pCVar->ConsoleColorPrint(g_WarningPrintColor, szFormattedMsg);
	}
}

void ConColorMsg(const Color &clr, const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	_vsnprintf(szFormattedMsg, MAX_CON_PRINT_MESSAGE_LENGTH, pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH - 1] = 0;

	g_pCVar->ConsoleColorPrint(clr, szFormattedMsg);
}

void ConMsg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	_vsnprintf(szFormattedMsg, MAX_CON_PRINT_MESSAGE_LENGTH, pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH - 1] = 0;

	g_pCVar->ConsolePrint(szFormattedMsg);
}

void ConDMsg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH] = { 0 };

	if ( g_pCVar->CanPrint() && bool(s_pDeveloper->value) )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		_vsnprintf(szFormattedMsg, MAX_CON_PRINT_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedMsg[MAX_CON_PRINT_MESSAGE_LENGTH - 1] = 0;

		g_pCVar->ConsolePrint(szFormattedMsg);
	}
}