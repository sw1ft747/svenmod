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
		EXTERN_PATTERN(build_number);

		EXTERN_PATTERN(LoadClientDLL);
		EXTERN_PATTERN(ClientDLL_Init);

		EXTERN_PATTERN(Sys_InitGame);

		EXTERN_PATTERN(Host_FilterTime);
		EXTERN_PATTERN(Host_Shutdown);

		/**
		* Signatures that don't point to the beginning of functions
		*/

		EXTERN_PATTERN(V_protocol_version);

		EXTERN_PATTERN(V_EngineStudio_Init);

		EXTERN_PATTERN(V_VideoMode_Create);
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

	}

	void CheckGameVersion();
}

#ifdef PLATFORM_WINDOWS
// #ifdef PLATFORM_LINUX

namespace Symbols
{
#define EXTERN_SYMBOL(sym) extern const char sym[]
#define DEFINE_SYMBOL(sym, s) const char sym[] = s

	namespace Hardware
	{
		EXTERN_SYMBOL(build_number);

		EXTERN_SYMBOL(LoadClientDLL);
		EXTERN_SYMBOL(ClientDLL_Init);

		EXTERN_SYMBOL(Sys_InitGame);

		EXTERN_SYMBOL(Host_FilterTime);
		EXTERN_SYMBOL(Host_Shutdown);
	}

	namespace Client
	{
		EXTERN_SYMBOL(GetClientColor);

		EXTERN_SYMBOL(WeaponsResource__GetFirstPos);
	}

	namespace GameUI
	{

	}
}

#endif

#endif // PATTERNS_H