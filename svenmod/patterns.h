#ifndef PATTERNS_H
#define PATTERNS_H

#ifdef _WIN32
#pragma once
#endif

#include <memutils/patterns.h>
#include <platform.h>

namespace Patterns
{
	namespace Hardware
	{
		EXTERN_PATTERN(g_NetworkMessages);
		EXTERN_PATTERN(MSG_ReadByte);

		EXTERN_PATTERN(Z_Free);

		EXTERN_PATTERN(build_number);

		EXTERN_PATTERN(LoadClientDLL);
		EXTERN_PATTERN(ClientDLL_Init);

		EXTERN_PATTERN(Sys_InitGame);

		EXTERN_PATTERN(R_RenderScene);

		EXTERN_PATTERN(SCR_BeginLoadingPlaque);
		EXTERN_PATTERN(SCR_EndLoadingPlaque);

		EXTERN_PATTERN(CL_Disconnect);

		EXTERN_PATTERN(Host_FilterTime);
		EXTERN_PATTERN(Host_Shutdown);

		EXTERN_PATTERN(Cvar_DirectSet);
		EXTERN_PATTERN(Cvar_RemoveClientDLLCvars);
		EXTERN_PATTERN(Cmd_RemoveClientDLLCmds);

		/**
		* Signatures that don't point to the beginning of functions
		*/

		EXTERN_PATTERN(V_protocol_version);

		EXTERN_PATTERN(V_EngineStudio_Init);

		EXTERN_PATTERN(V_VideoMode_Create);

		EXTERN_PATTERN(g_pEventHooks);
	}

	namespace Client
	{
		EXTERN_PATTERN(GetClientColor);

		EXTERN_PATTERN(WeaponsResource__GetFirstPos);

		/**
		* Signatures that don't point to the beginning of functions
		*/

		EXTERN_PATTERN(V___MsgFunc_ServerVer);
	}

	namespace GameUI
	{
		EXTERN_PATTERN(CGameConsoleDialog__DPrint);
	}
}

#ifdef PLATFORM_LINUX

namespace Symbols
{
#define EXTERN_SYMBOL(sym) extern const char sym[]
#define DEFINE_SYMBOL(sym, s) const char sym[] = s

	namespace Hardware
	{
		EXTERN_SYMBOL(MSG_ReadByte);

		EXTERN_SYMBOL(Z_Free);

		EXTERN_SYMBOL(build_number);

		EXTERN_SYMBOL(LoadClientDLL);

		EXTERN_SYMBOL(Sys_InitGame);

		EXTERN_SYMBOL(Host_FilterTime);
		EXTERN_SYMBOL(Host_Shutdown);

		EXTERN_SYMBOL(R_RenderScene);

		EXTERN_SYMBOL(SCR_BeginLoadingPlaque);
		EXTERN_SYMBOL(SCR_EndLoadingPlaque);

		EXTERN_SYMBOL(CL_Disconnect);

		EXTERN_SYMBOL(Cvar_DirectSet);
		EXTERN_SYMBOL(Cvar_RemoveClientDLLCvars);
		EXTERN_SYMBOL(Cmd_RemoveClientDLLCmds);
	}

	namespace Client
	{
		EXTERN_SYMBOL(GetClientColor);

		EXTERN_SYMBOL(WeaponsResource__GetFirstPos);
	}

	namespace GameUI
	{
		EXTERN_SYMBOL(CGameConsoleDialog__DPrint);
	}
}

#endif

#endif // PATTERNS_H