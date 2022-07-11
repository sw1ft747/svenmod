#include "patterns.h"

namespace Patterns
{
	namespace Hardware
	{
	#ifdef PLATFORM_WINDOWS
		DEFINE_PATTERN(g_NetworkMessages, "8B 34 8D ? ? ? ? 85 F6 74");
		DEFINE_PATTERN(MSG_ReadByte, "8B 0D ? ? ? ? 8D 51 01 3B 15 ? ? ? ? 7E ? C7 05 ? ? ? ? ? ? ? ? 83 C8 ? C3 A1 ? ? ? ? 0F B6 04 08");

		DEFINE_PATTERN(Z_Free, "56 8B 74 24 08 85 F6 75 0D 68 ? ? ? ? E8 ? ? ? ? 83 C4 04 83 C6 E8");

		DEFINE_PATTERN(build_number, "51 A1 ? ? ? ? 56 33 F6 85 C0 0F 85 A4 00 00 00 53 57 33 FF EB 09");

		DEFINE_PATTERN(LoadClientDLL, "FF 74 24 04 E8 ? ? ? ? 83 C4 04 A3 ? ? ? ? 85 C0 75 ? E8 ? ? ? ? 50 FF 74 24 08");
		DEFINE_PATTERN(ClientDLL_Init, "FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 1C 85 C0 74 0A 68 ? ? ? ? FF D0 83 C4 04 E8");

		DEFINE_PATTERN(Sys_InitGame, "83 EC 08 C7 05 ? ? ? ? 00 00 00 00 56 57 8B 7C 24 20 85 FF");

		DEFINE_PATTERN(Host_FilterTime, "D9 EE D9 05 ? ? ? ? 8B 0D ? ? ? ? D8 D1 8B 15 ? ? ? ? DF E0 F6 C4 41 75 41");
		DEFINE_PATTERN(Host_Shutdown, "83 3D ? ? ? ? 00 74 0E 68 ? ? ? ? E8 ? ? ? ? 83 C4 04 C3 83 3D ? ? ? ? 00 56 57 C7 05");

		DEFINE_PATTERN(Cvar_DirectSet, "81 EC ? ? 00 00 A1 ? ? ? ? 33 C4 89 84 24 ? ? 00 00 56 8B B4 24 ? ? 00 00 57 8B BC 24 ? ? 00 00 85 FF");
		DEFINE_PATTERN(Cvar_RemoveClientDLLCvars, "56 8B 35 ? ? ? ? 57 33 FF 85 F6 74 2A 53 90");
		DEFINE_PATTERN(Cvar_RemoveClientDLLCmds, "A1 ? ? ? ? 57 33 FF 85 C0 74 24 53 8B 5C 24 0C 56 8B 30");

		/**
		* Signatures that don't point to the beginning of functions
		*/

		DEFINE_PATTERN(V_protocol_version, "6A ? 68 ? ? ? ? FF D6 68 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 10 85 C0 75");

		DEFINE_PATTERN(V_EngineStudio_Init, "68 ? ? ? ? 68 ? ? ? ? 6A 01 FF D0 83 C4 0C 85 C0");

		DEFINE_PATTERN(V_VideoMode_Create, "A3 ? ? ? ? 8B 4D F4 64 89 0D 00 00 00 00 59 5E 5B 8B E5 5D C3");
	#else // Linux

	#endif
	}

	namespace Client
	{
	#ifdef PLATFORM_WINDOWS
		DEFINE_PATTERN(GetClientColor, "8B 4C 24 04 85 C9 7E 35 6B C1 ? 0F BF 80 ? ? ? ? 48 83 F8 03");

		DEFINE_PATTERN(WeaponsResource__GetFirstPos, "6B 54 24 04 68 56 57 33 F6 8B F9 81 C2 ? ? ? ? 8B 02 85 C0 74");

		/**
		* Signatures that don't point to the beginning of functions
		*/

		DEFINE_PATTERN(V___MsgFunc_ServerVer, "8D 44 24 40 C6 44 24 3C 00 68 ? ? ? ? 50 0F 11 44 24 48");
	#else // Linux

	#endif
	}

	namespace GameUI
	{
		DEFINE_PATTERN(CGameConsoleDialog__DPrint, "55 8B EC 56 8B F1 FF B6 ? ? 00 00 8B 8E ? ? 00 00 E8 ? ? ? ? FF 75 08 8B 8E ? ? 00 00 E8 ? ? ? ? 5E 5D C2 04 00");
	}

	void CheckGameVersion()
	{

	}
}

#ifdef PLATFORM_LINUX

namespace Symbols
{
	namespace Hardware
	{
		DEFINE_SYMBOL(build_number, "_Z12build_numberv");

		DEFINE_SYMBOL(LoadClientDLL, "_Z13LoadClientDLLPc_part_0");
		DEFINE_SYMBOL(ClientDLL_Init, "_Z14ClientDLL_Initv");

		DEFINE_SYMBOL(Sys_InitGame, "_Z12Sys_InitGamePcS_Pvi");

		DEFINE_SYMBOL(Host_FilterTime, "_Z15Host_FilterTimef");
		DEFINE_SYMBOL(Host_Shutdown, "_Z13Host_Shutdownv");
	}

	namespace Client
	{
		DEFINE_SYMBOL(GetClientColor, "_Z14GetClientColori");

		DEFINE_SYMBOL(WeaponsResource__GetFirstPos, "_ZN15WeaponsResource11GetFirstPosEi");
	}

	namespace GameUI
	{

	}
}

#endif