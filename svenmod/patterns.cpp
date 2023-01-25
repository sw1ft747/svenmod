#include "patterns.h"

namespace Patterns
{
	namespace Hardware
	{
	#ifdef PLATFORM_WINDOWS

	#if defined(SC_NEWEST) || defined(SC_5_25)
		DEFINE_PATTERN(MSG_ReadByte, "8B 0D ? ? ? ? 8D 51 01 3B 15 ? ? ? ? 7E ? C7 05 ? ? ? ? ? ? ? ? 83 C8 ? C3 A1 ? ? ? ? 0F B6 04 08");

		DEFINE_PATTERN(Z_Free, "56 8B 74 24 08 85 F6 75 ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? 83 C6");

		DEFINE_PATTERN(build_number, "51 A1 ? ? ? ? 56 33 F6");

		DEFINE_PATTERN(LoadClientDLL, "FF 74 24 04 E8 ? ? ? ? 83 C4 ? A3 ? ? ? ? 85 C0 75");

		DEFINE_PATTERN(Sys_InitGame, "83 EC ? C7 05 ? ? ? ? ? ? ? ? 56");

		DEFINE_PATTERN(R_RenderScene, "83 EC ? E8 ? ? ? ? 85 C0 74");

		DEFINE_PATTERN(SCR_BeginLoadingPlaque, "6A ? E8 ? ? ? ? A1 ? ? ? ? 83 C4 ? 83 F8");
		DEFINE_PATTERN(SCR_EndLoadingPlaque, "C7 05 ? ? ? ? ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? C7 05");

		DEFINE_PATTERN(CL_Disconnect, "83 EC ? A1 ? ? ? ? 33 C4 89 44 24 14 DD ? ? ? ? ? DD");

		DEFINE_PATTERN(Host_FilterTime, "D9 ? D9 ? ? ? ? ? 8B 0D");
		DEFINE_PATTERN(Host_Shutdown, "83 3D ? ? ? ? ? 74 ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? C3 83 3D");

		DEFINE_PATTERN(Cvar_DirectSet, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 08 04 00 00");
		DEFINE_PATTERN(Cvar_RemoveClientDLLCvars, "56 8B 35 ? ? ? ? 57 33 FF 85 F6 74 ? 53");
		DEFINE_PATTERN(Cmd_RemoveClientDLLCmds, "A1 ? ? ? ? 57 33 FF 85 C0 74");

		/**
		* Signatures that don't point to the beginning of functions
		*/

		DEFINE_PATTERN(V_protocol_version, "6A ? 68 ? ? ? ? FF D6 68 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 10 85 C0 75");

		DEFINE_PATTERN(V_EngineStudio_Init, "68 ? ? ? ? 68 ? ? ? ? 6A ? FF D0 83 C4 ? 85 C0 75 ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? E8 ? ? ? ? 68");

		DEFINE_PATTERN(V_VideoMode_Create, "A3 ? ? ? ? 8B 4D F4 64 89 0D 00 00 00 00 59 5E 5B 8B E5 5D C3");

		DEFINE_PATTERN(ClientDLL_Init, "FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 ? 85 C0");

		DEFINE_PATTERN(g_pEventHooks, "56 8B 35 ? ? ? ? 85 F6 74 ? 8B 46 04");
		DEFINE_PATTERN(g_NetworkMessages, "8B 34 8D ? ? ? ? 85 F6 74");
	#else // 5.22
		DEFINE_PATTERN(MSG_ReadByte, "8B 0D ? ? ? ? 8D 51 01 3B 15 ? ? ? ? 7E ? C7 05 ? ? ? ? ? ? ? ? 83 C8 ? C3 A1 ? ? ? ? 0F B6 04 08");

		DEFINE_PATTERN(Z_Free, "56 8B 74 24 08 85 F6 75 ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? 83 C6");

		DEFINE_PATTERN(build_number, "A1 ? ? ? ? 83 EC ? 56");

		DEFINE_PATTERN(LoadClientDLL, "FF 74 24 04 E8 ? ? ? ? 83 C4 ? A3 ? ? ? ? 85 C0 75");

		DEFINE_PATTERN(Sys_InitGame, "55 8B EC 83 E4 ? 83 EC ? C7 05");

		DEFINE_PATTERN(R_RenderScene, "83 EC ? A1 ? ? ? ? 33 C4 89 44 24 20 E8");

		DEFINE_PATTERN(SCR_BeginLoadingPlaque, "6A ? E8 ? ? ? ? A1 ? ? ? ? 83 C4 ? 83 F8");
		DEFINE_PATTERN(SCR_EndLoadingPlaque, "C7 05 ? ? ? ? ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? C7 05");

		DEFINE_PATTERN(CL_Disconnect, "83 EC ? A1 ? ? ? ? 33 C4 89 44 24 14 DD ? ? ? ? ? DD");

		DEFINE_PATTERN(Host_FilterTime, "51 D9 ? D9 ? ? ? ? ? 8B 0D");
		DEFINE_PATTERN(Host_Shutdown, "83 3D ? ? ? ? ? 74 ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? C3 83 3D");

		DEFINE_PATTERN(Cvar_DirectSet, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 08 04 00 00");
		DEFINE_PATTERN(Cvar_RemoveClientDLLCvars, "56 8B 35 ? ? ? ? 57 33 FF 85 F6 74 ? 53");
		DEFINE_PATTERN(Cmd_RemoveClientDLLCmds, "A1 ? ? ? ? 57 33 FF 85 C0 74");

		/**
		* Signatures that don't point to the beginning of functions
		*/

		DEFINE_PATTERN(V_protocol_version, "6A ? 68 ? ? ? ? FF D6 68");

		DEFINE_PATTERN(V_EngineStudio_Init, "68 ? ? ? ? 68 ? ? ? ? 6A ? FF D0 83 C4 ? 85 C0 75 ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? E8 ? ? ? ? 68");

		DEFINE_PATTERN(V_VideoMode_Create, "A3 ? ? ? ? 8B 4D F4");

		DEFINE_PATTERN(ClientDLL_Init, "FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 ? 85 C0");

		DEFINE_PATTERN(g_pEventHooks, "8B 35 ? ? ? ? 85 F6 74 ? 8B 46 04");
		DEFINE_PATTERN(g_NetworkMessages, "8B 34 8D ? ? ? ? 85 F6");
	#endif

	#else // Linux

	#endif
	}

	namespace Client
	{
	#ifdef PLATFORM_WINDOWS

	#if defined(SC_NEWEST) || defined(SC_5_25)
		DEFINE_PATTERN(GetClientColor, "8B 4C 24 04 85 C9 7E");

		DEFINE_PATTERN(WeaponsResource__GetFirstPos, "6B 54 24 04 ? 56 57 33 F6 8B F9 81 C2 ? ? ? ? 8B 02 85 C0 74");

		/**
		* Signatures that don't point to the beginning of functions
		*/

		DEFINE_PATTERN(V___MsgFunc_ServerVer, "8D 44 24 40 C6 44 24 3C 00 68 ? ? ? ? 50 0F 11 44 24 48");
	#else // 5.22
		DEFINE_PATTERN(GetClientColor, "8B 44 24 04 85 C0 7E");

		DEFINE_PATTERN(WeaponsResource__GetFirstPos, "53 55 56 57 8B 7C 24 14 6B FF");

		/**
		* Signatures that don't point to the beginning of functions
		*/

		DEFINE_PATTERN(V___MsgFunc_ServerVer, "8D 54 24 1C 68 ? ? ? ? 52 89 44 24 24");
	#endif

	#else // Linux

	#endif
	}

	namespace GameUI
	{
	#if defined(SC_NEWEST) || defined(SC_5_25)
		DEFINE_PATTERN(CGameConsoleDialog__DPrint, "55 8B EC 56 8B F1 FF B6 2C 01 00 00");
	#else // 5.22
		DEFINE_PATTERN(CGameConsoleDialog__DPrint, "55 8B EC 56 8B F1 FF B6 2C 01 00 00");
	#endif
	}
}

#ifdef PLATFORM_LINUX

namespace Symbols
{
	namespace Hardware
	{
		DEFINE_SYMBOL(MSG_ReadByte, "_Z12MSG_ReadBytev");

		DEFINE_SYMBOL(Z_Free, "_Z6Z_FreePv");

		DEFINE_SYMBOL(build_number, "_Z12build_numberv");

		DEFINE_SYMBOL(LoadClientDLL, "_Z13LoadClientDLLPc_part_0");

		DEFINE_SYMBOL(Sys_InitGame, "_Z12Sys_InitGamePcS_Pvi");

		DEFINE_SYMBOL(Host_FilterTime, "_Z15Host_FilterTimef");
		DEFINE_SYMBOL(Host_Shutdown, "_Z13Host_Shutdownv");

		DEFINE_SYMBOL(R_RenderScene, "_Z13R_RenderScenev");

		DEFINE_SYMBOL(SCR_BeginLoadingPlaque, "_Z22SCR_BeginLoadingPlaquei");
		DEFINE_SYMBOL(SCR_EndLoadingPlaque, "_Z20SCR_EndLoadingPlaquev");

		DEFINE_SYMBOL(CL_Disconnect, "_Z13CL_Disconnectv");

		DEFINE_SYMBOL(Cvar_DirectSet, "_Z14Cvar_DirectSetP6cvar_sPc");
		DEFINE_SYMBOL(Cvar_RemoveClientDLLCvars, "_Z25Cvar_RemoveClientDLLCvarsv");
		DEFINE_SYMBOL(Cmd_RemoveClientDLLCmds, "_Z23Cmd_RemoveClientDLLCmdsv");
	}

	namespace Client
	{
		DEFINE_SYMBOL(GetClientColor, "_Z14GetClientColori");

		DEFINE_SYMBOL(WeaponsResource__GetFirstPos, "_ZN15WeaponsResource11GetFirstPosEi");
	}

	namespace GameUI
	{
		DEFINE_SYMBOL(CGameConsoleDialog__DPrint, "_ZN18CGameConsoleDialog6DPrintEPKc");
	}
}

#endif