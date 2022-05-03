#include "patterns.h"

namespace Patterns
{
	namespace Hardware
	{
		DEFINE_NULL_PATTERN(build_number);

		DEFINE_NULL_PATTERN(LoadClientDLL);
		DEFINE_NULL_PATTERN(ClientDLL_Init);

		DEFINE_NULL_PATTERN(Sys_InitGame);

		DEFINE_NULL_PATTERN(Host_FilterTime);
		DEFINE_NULL_PATTERN(Host_Shutdown);

		/**
		* Signatures that don't point to the beginning of functions
		*/

		DEFINE_NULL_PATTERN(V_protocol_version);

		DEFINE_NULL_PATTERN(V_EngineStudio_Init);

		DEFINE_NULL_PATTERN(V_VideoMode_Create);

		/**
		* Signatures for various versions
		*/

	#ifdef PLATFORM_WINDOWS
		DEFINE_PATTERN(build_number_5025, "51 A1 ? ? ? ? 56 33 F6 85 C0 0F 85 A4 00 00 00 53 57 33 FF EB 09");

		DEFINE_PATTERN(LoadClientDLL_5025, "FF 74 24 04 E8 ? ? ? ? 83 C4 04 A3 ? ? ? ? 85 C0 75 ? E8 ? ? ? ? 50 FF 74 24 08");
		DEFINE_PATTERN(ClientDLL_Init_5025, "FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 1C 85 C0 74 0A 68 ? ? ? ? FF D0 83 C4 04 E8");

		DEFINE_PATTERN(Sys_InitGame_5025, "83 EC 08 C7 05 ? ? ? ? 00 00 00 00 56 57 8B 7C 24 20 85 FF");

		DEFINE_PATTERN(Host_FilterTime_5025, "D9 EE D9 05 ? ? ? ? 8B 0D ? ? ? ? D8 D1 8B 15 ? ? ? ? DF E0 F6 C4 41 75 41");
		DEFINE_PATTERN(Host_Shutdown_5025, "83 3D ? ? ? ? 00 74 0E 68 ? ? ? ? E8 ? ? ? ? 83 C4 04 C3 83 3D ? ? ? ? 00 56 57 C7 05");

		//=============================================================

		DEFINE_PATTERN(V_protocol_version_5025, "6A ? 68 ? ? ? ? FF D6 68 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 10 85 C0 75");

		DEFINE_PATTERN(V_EngineStudio_Init_5025, "68 ? ? ? ? 68 ? ? ? ? 6A 01 FF D0 83 C4 0C 85 C0");

		DEFINE_PATTERN(V_VideoMode_Create_5025, "A3 ? ? ? ? 8B 4D F4 64 89 0D 00 00 00 00 59 5E 5B 8B E5 5D C3");
	#else

	#endif
	}

	namespace Client
	{
		DEFINE_NULL_PATTERN(GetClientColor);

		DEFINE_NULL_PATTERN(WeaponsResource__GetFirstPos);

		/**
		* Signatures that don't point to the beginning of functions
		*/

		DEFINE_NULL_PATTERN(V___MsgFunc_ServerVer);

		/**
		* Signatures for various versions
		*/

	#ifdef PLATFORM_WINDOWS
		DEFINE_PATTERN(GetClientColor_5025, "8B 4C 24 04 85 C9 7E 35 6B C1 ? 0F BF 80 ? ? ? ? 48 83 F8 03");

		DEFINE_PATTERN(WeaponsResource__GetFirstPos_5025, "6B 54 24 04 68 56 57 33 F6 8B F9 81 C2 ? ? ? ? 8B 02 85 C0 74");

		//=============================================================

		DEFINE_PATTERN(V___MsgFunc_ServerVer_5025, "8D 44 24 40 C6 44 24 3C 00 68 ? ? ? ? 50 0F 11 44 24 48");
	#else

	#endif
	}

	namespace GameUI
	{

	}

	void CheckGameVersion()
	{

	}
}

#ifdef PLATFORM_WINDOWS
//#ifdef PLATFORM_LINUX

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