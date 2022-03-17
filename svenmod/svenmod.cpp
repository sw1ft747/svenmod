#include <interface.h>
#include <ISvenMod.h>
#include <ISvenModAPI.h>
#include <ICommandLine.h>
#include <IEngineClient.h>
#include <IInventory.h>
#include <IFileSystem.h>
#include <IRegistry.h>
#include <ILoggingSystem.h>
#include <IMemoryUtils.h>
#include <IDetoursAPI.h>
#include <IVideoMode.h>
#include <ICvar.h>

#include <dbg.h>
#include <convar.h>
#include <svenmod_version.h>
#include <memutils/patterns.h>

#include <hl_sdk/engine/APIProxy.h>
#include <hl_sdk/common/r_studioint.h>
#include <hl_sdk/pm_shared/pm_shared.h>
#include <hl_sdk/cl_dll/StudioModelRenderer.h>
#include <hl_sdk/cl_dll/ammohistory.h>
#include <hl_sdk/cl_dll/hud.h>

#include "cvar_sm.h"
#include "dbg_sm.h"
#include "plugins_manager.h"
#include "client_hooks_handler.h"

// Patterns
DEFINE_PATTERN(LoadClientDLL, "FF 74 24 04 E8 ? ? ? ? 83 C4 04 A3 ? ? ? ? 85 C0 75 ? E8 ? ? ? ? 50 FF 74 24 08");
DEFINE_PATTERN(ClientDLL_Init, "FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 1C 85 C0 74 0A 68 ? ? ? ? FF D0 83 C4 04 E8");
DEFINE_PATTERN(Sys_Init, "83 EC 08 C7 05 ? ? ? ? 00 00 00 00 56 57 8B 7C 24 20 85 FF");
DEFINE_PATTERN(Host_FilterTime, "D9 EE D9 05 ? ? ? ? 8B 0D ? ? ? ? D8 D1 8B 15 ? ? ? ? DF E0 F6 C4 41 75 41");
DEFINE_PATTERN(Host_Shutdown, "83 3D ? ? ? ? 00 74 0E 68 ? ? ? ? E8 ? ? ? ? 83 C4 04 C3 83 3D ? ? ? ? 00 56 57 C7 05");
DEFINE_PATTERN(EngineStudio_Init, "68 ? ? ? ? 68 ? ? ? ? 6A 01 FF D0 83 C4 0C 85 C0");
DEFINE_PATTERN(VideoMode_Create, "A3 ? ? ? ? 8B 4D F4 64 89 0D 00 00 00 00 59 5E 5B 8B E5 5D C3");
DEFINE_PATTERN(GetClientColor, "8B 4C 24 04 85 C9 7E 35 6B C1 ? 0F BF 80 ? ? ? ? 48 83 F8 03");
DEFINE_PATTERN(WeaponsResource__GetFirstPos, "6B 54 24 04 68 56 57 33 F6 8B F9 81 C2 ? ? ? ? 8B 02 85 C0 74");

DEFINE_PATTERN(build_number_sig, "51 A1 ? ? ? ? 56 33 F6 85 C0 0F 85 A4 00 00 00 53 57 33 FF EB 09");
DEFINE_PATTERN(protocol_version_sig, "6A 30 68 ? ? ? ? FF D6 68 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 10 85 C0 75");
DEFINE_PATTERN(__MsgFunc_ServerVer, "8D 44 24 40 C6 44 24 3C 00 68 ? ? ? ? 50 0F 11 44 24 48");

// Game interfaces
cl_enginefuncs_t g_EngineFuncs;

cl_enginefuncs_t *g_pEngineFuncs = NULL;
cl_clientfuncs_t *g_pClientFuncs = NULL;
engine_studio_api_t *g_pEngineStudio = NULL;
r_studio_interface_t *g_pStudioAPI = NULL;
CStudioModelRenderer *g_pStudioRenderer = NULL;
IEngineClient *g_pEngineClient = NULL;
triangleapi_t *g_pTriangleAPI = NULL;
efx_api_t *g_pEffectsAPI = NULL;
event_api_t *g_pEventAPI = NULL;
demo_api_t *g_pDemoAPI = NULL;
net_api_t *g_pNetAPI = NULL;
IVoiceTweak *g_pVoiceTweak = NULL;
playermove_t *g_pPlayerMove = NULL;
IVideoMode *g_pVideoMode = NULL;
IVideoMode **g_ppVideoMode = NULL;
usermsg_t **g_ppClientUserMsgs = NULL;

WeaponsResource *g_pWeaponsResource = NULL;
extra_player_info_t *g_pPlayerExtraInfo = NULL;

// Common stuff
double *g_pRealtime = NULL;
double *g_pFrametime = NULL;

const char *g_pszBaseDirectory = NULL;

static const char *s_pszClientVersion = NULL;
client_version_s g_ClientVersion;
int g_iEngineBuild = 0;
int g_iProtocolVersion = 0;

int *g_pClientState = NULL;
modules_s g_Modules;

// Disassemble struct
ud_t g_inst;

// Shared interfaces
ICommandLine *g_pCommandLine = NULL;
IFileSystem *g_pFileSystem = NULL;
IRegistry *g_pRegistry = NULL;

//-----------------------------------------------------------------------------
// Main class
//-----------------------------------------------------------------------------

class CSvenMod : public ISvenMod
{
	friend int __cdecl HOOKED_Sys_Init(CreateInterfaceFn, const char *, void *, int);
	friend void __cdecl HOOKED_LoadClientDLL(char *);

public:
	// ISvenMod interface
	bool Init(ICommandLine *pCommandLine, IFileSystem *pFileSystem, IRegistry *pRegistry) override;
	void Shutdown() override;

private:
	// Callbacks
	void SystemPostInit();
	bool StartSystems();

	// Lookup for stuff
	void FindEngineStudio();
	void FindPlayerMove();
	void FindVideoMode();
	void FindStudioModelRenderer();
	void FindClientVersion();
	void FindEngineClient();
	void FindUserMsgs();
	void FindExtraPlayerInfo();
	void FindWeaponsResource();

	void FindClientState();
	void FindFrametime();
	void FindProtocolVersion();

	// Utilities
	bool FindSignatures();
	bool AttachDetours();

	void CheckClientVersion();

private:
	void *m_pfnLoadClientDLL;
	void *m_pfnClientDLL_Init;
	void *m_pfnSys_Init;
	void *m_pfnHost_FilterTime;
	void *m_pfnHost_Shutdown;
	void *m_pfnEngineStudioInit;
	void *m_pfnVideoMode_Create;
	void *m_pfn__MsgFunc_ServerVer;

	DetourHandle_t m_hLoadClientDLL;
	DetourHandle_t m_hSys_Init;
	DetourHandle_t m_hHost_FilterTime;
	DetourHandle_t m_hHost_Shutdown;
};

CSvenMod g_SvenMod;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

DECLARE_FUNC_PTR(int, __cdecl, build_number);

//-----------------------------------------------------------------------------
// Hooks
//-----------------------------------------------------------------------------

DECLARE_HOOK(int, __cdecl, Sys_Init, CreateInterfaceFn, const char *, void *, int);
DECLARE_HOOK(qboolean, __cdecl, Host_FilterTime, float);
DECLARE_HOOK(void, __cdecl, Host_Shutdown);
DECLARE_HOOK(int, __cdecl, Initialize, cl_enginefuncs_t *, int);
DECLARE_HOOK(void, __cdecl, LoadClientDLL, char *);

DECLARE_FUNC(int, __cdecl, HOOKED_Sys_Init, CreateInterfaceFn appSystemFactory, const char *pBaseDir, void *pwnd, int bIsDedicated)
{
	if ( bIsDedicated )
	{
		Sys_Error("[SvenMod] Running as dedicated server is not allowed");
	}

	int initialized = ORIG_Sys_Init(appSystemFactory, pBaseDir, pwnd, bIsDedicated);

	if ( initialized )
	{
		g_pszBaseDirectory = strdup(pBaseDir);
		g_SvenMod.SystemPostInit();
	}
	else
	{
		Sys_Error("[SvenMod] Engine failed initialization");
	}

	DetoursAPI()->RemoveDetour( g_SvenMod.m_hSys_Init );

	return initialized;
}

DECLARE_FUNC(qboolean, __cdecl, HOOKED_Host_FilterTime, float time)
{
	qboolean simulate = ORIG_Host_FilterTime(time);

	if ( simulate )
	{
		g_PluginsManager.Frame( *g_pClientState, *g_pFrametime, false );
	}

	return simulate;
}

DECLARE_FUNC(void, __cdecl, HOOKED_Host_Shutdown)
{
	// Unload all plugins before client DLL will be unloaded
	g_PluginsManager.UnloadPlugins();
	g_ClientHooksHandler.Shutdown();

	CvarDisablePrint();

	ORIG_Host_Shutdown();
}

DECLARE_FUNC(int, __cdecl, HOOKED_Initialize, cl_enginefuncs_t *pEngineFuncs, int iVersion)
{
	memcpy(&g_EngineFuncs, pEngineFuncs, sizeof(cl_enginefuncs_t));
	g_pEngineFuncs = &g_EngineFuncs;

	if ( iVersion != CLDLL_INTERFACE_VERSION )
	{
		LogWarning("Client's DLL version differs.\n");
	}

	int initialized = ORIG_Initialize(pEngineFuncs, iVersion);

	if ( !initialized )
	{
		Sys_Error("Client's DLL failed initialization");
	}

	g_pClientFuncs->Initialize = ORIG_Initialize;

	return initialized;
}

DECLARE_FUNC(void, __cdecl, HOOKED_LoadClientDLL, char *pszLibFileName)
{
	ORIG_LoadClientDLL(pszLibFileName);

	// If we were able to get here then client's library was linked correctly

	MemoryUtils()->InitDisasm(&g_inst, g_SvenMod.m_pfnLoadClientDLL, 32);

	bool bFoundKeyInstruction = false;

	do
	{
		if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_MEM && g_inst.operand[1].type == UD_OP_IMM && g_inst.operand[1].lval.ubyte == 1)
		{
			bFoundKeyInstruction = true;
			continue;
		}

		if (bFoundKeyInstruction)
		{
			if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_MEM && g_inst.operand[1].type == UD_OP_IMM)
			{
				g_pClientFuncs = reinterpret_cast<cl_clientfuncs_t *>(g_inst.operand[0].lval.udword);
				break;
			}
		}

	} while ( MemoryUtils()->Disassemble(&g_inst) );

	if ( !g_pClientFuncs )
	{
		Sys_Error("[SvenMod] Failed to get cl_clientfuncs");
	}
	else
	{
		ORIG_Initialize = g_pClientFuncs->Initialize;
		g_pClientFuncs->Initialize = HOOKED_Initialize;
	}

	DetoursAPI()->RemoveDetour( g_SvenMod.m_hLoadClientDLL );
}

//-----------------------------------------------------------------------------
// SvenMod implementation
//-----------------------------------------------------------------------------

bool CSvenMod::Init(ICommandLine *pCommandLine, IFileSystem *pFileSystem, IRegistry *pRegistry)
{
	// Sequence of calls
	// Launcher >> CSvenMod::Init >> LoadClientDLL (hook) >> cl_clientfuncs_s::Initialize (hook) >> Sys_Init (hook) >> CSvenMod::SystemPostInit >> CSvenMod::StartSystems

	g_pCommandLine = pCommandLine;
	g_pFileSystem = pFileSystem;
	g_pRegistry = pRegistry;

	DetoursAPI()->Init();

	if ( !LoggingSystem()->Init() )
		return false;

	LogMsg("Initializing SvenMod...\n");

	if ( !FindSignatures() )
		return false;

	if ( !AttachDetours() )
		return false;

	return true;
}

// Initialize the rest part
void CSvenMod::SystemPostInit()
{
	// Collect modules
#ifdef PLATFORM_WINDOWS
	HMODULE hSvenMod = Sys_GetModuleHandle("svenmod.dll");
	HMODULE hClientDLL = Sys_GetModuleHandle("client.dll");
	HMODULE hFileSystem = Sys_GetModuleHandle("filesystem_stdio.dll");
	HMODULE hGameUI = Sys_GetModuleHandle("GameUI.dll");
	HMODULE hVGUI = Sys_GetModuleHandle("vgui.dll");
	HMODULE hVGUI2 = Sys_GetModuleHandle("vgui2.dll");
	HMODULE hSDL2 = Sys_GetModuleHandle("SDL2.dll");
	HMODULE hVSTDLib = Sys_GetModuleHandle("vstdlib.dll");
	HMODULE hTier0 = Sys_GetModuleHandle("tier0.dll");
	HMODULE hOpenGL = Sys_GetModuleHandle("opengl32.dll");
	HMODULE hSteamAPI = Sys_GetModuleHandle("steam_api.dll");
#else
	HMODULE hSvenMod = Sys_GetModuleHandle("svenmod.so");
	HMODULE hClientDLL = Sys_GetModuleHandle("client.so");
	HMODULE hFileSystem = Sys_GetModuleHandle("filesystem_stdio.so");
	HMODULE hGameUI = Sys_GetModuleHandle("gameui.so");
	HMODULE hVGUI = Sys_GetModuleHandle("vgui.so");
	HMODULE hVGUI2 = Sys_GetModuleHandle("vgui2.so");
	HMODULE hSDL2 = Sys_GetModuleHandle("libSDL2-2.0.so.0");
	HMODULE hVSTDLib = Sys_GetModuleHandle("libvstdlib.so");
	HMODULE hTier0 = Sys_GetModuleHandle("libtier0.so");
	HMODULE hOpenGL = Sys_GetModuleHandle("opengl32.so");
	HMODULE hSteamAPI = Sys_GetModuleHandle("steam_api.so");
#endif

	g_Modules.SvenMod = hSvenMod;
	g_Modules.Client = hClientDLL;
	g_Modules.FileSystem = hFileSystem;
	g_Modules.GameUI = hGameUI;
	g_Modules.VGUI = hVGUI;
	g_Modules.VGUI2 = hVGUI2;
	g_Modules.SDL2 = hSDL2;
	g_Modules.VSTDLib = hVSTDLib;
	g_Modules.Tier0 = hTier0;
	g_Modules.OpenGL = hOpenGL;
	g_Modules.SteamAPI = hSteamAPI;

	FindClientVersion();
	CheckClientVersion();

	FindFrametime();
	FindProtocolVersion();

	FindClientState();
	FindEngineStudio();
	FindEngineClient();
	FindStudioModelRenderer();
	FindPlayerMove();
	FindVideoMode();
	FindUserMsgs();
	FindExtraPlayerInfo();
	FindWeaponsResource();

	VGUI()->Init();
	VGameUI()->Init();
	Inventory()->Init();

	CvarInit();
	DbgInit();

	g_pTriangleAPI = g_pEngineFuncs->pTriAPI;
	g_pEffectsAPI = g_pEngineFuncs->pEfxAPI;
	g_pEventAPI = g_pEngineFuncs->pEventAPI;
	g_pDemoAPI = g_pEngineFuncs->pDemoAPI;
	g_pNetAPI = g_pEngineFuncs->pNetAPI;
	g_pVoiceTweak = g_pEngineFuncs->pVoiceTweak;

	// Let's not change engine functions so easily.. Just detour them for proper work of the game and SvenMod
	MemoryUtils()->VirtualProtect( g_pEngineStudio, sizeof(engine_studio_api_t), PAGE_EXECUTE_READ, NULL );
	MemoryUtils()->VirtualProtect( g_pStudioAPI, sizeof(r_studio_interface_t), PAGE_EXECUTE_READ, NULL );
	MemoryUtils()->VirtualProtect( g_pEngineFuncs, sizeof(cl_enginefuncs_t), PAGE_EXECUTE_READ, NULL );
	MemoryUtils()->VirtualProtect( g_pClientFuncs, sizeof(cl_clientfuncs_t), PAGE_EXECUTE_READ, NULL );

	MemoryUtils()->VirtualProtect( g_pTriangleAPI, sizeof(triangleapi_t), PAGE_EXECUTE_READ, NULL );
	MemoryUtils()->VirtualProtect( g_pEffectsAPI, sizeof(efx_api_t), PAGE_EXECUTE_READ, NULL );
	MemoryUtils()->VirtualProtect( g_pEventAPI, sizeof(event_api_t), PAGE_EXECUTE_READ, NULL );
	MemoryUtils()->VirtualProtect( g_pDemoAPI, sizeof(demo_api_t), PAGE_EXECUTE_READ, NULL );
	MemoryUtils()->VirtualProtect( g_pNetAPI, sizeof(net_api_t), PAGE_EXECUTE_READ, NULL );
	MemoryUtils()->VirtualProtect( g_pVoiceTweak, sizeof(IVoiceTweak), PAGE_EXECUTE_READ, NULL );

	MemoryUtils()->VirtualProtect( &g_ClientVersion, sizeof(client_version_s), PAGE_EXECUTE_READ, NULL );
	MemoryUtils()->VirtualProtect( &g_Modules, sizeof(modules_s), PAGE_EXECUTE_READ, NULL );

	g_iEngineBuild = build_number();

	LogMsg("Initialization finished.\n");

	if ( !g_SvenMod.StartSystems() )
	{
		Sys_Error("[SvenMod] Startup of systems was failed");
	}
}

bool CSvenMod::StartSystems()
{
	ConColorMsg({ 40, 255, 40, 255 }, "Loaded SvenMod v%s (API version: v%s)\n", SVENMOD_VERSION_STRING, SVENMOD_API_VERSION_STRING);
	LogMsg("SvenMod v%s.\n", SVENMOD_VERSION_STRING_FULL);

	ConVar_Register();

	g_ClientHooksHandler.Init();
	g_PluginsManager.LoadPlugins();

	return true;
}

void CSvenMod::Shutdown()
{
	DetoursAPI()->RemoveDetour( m_hHost_FilterTime );
	DetoursAPI()->RemoveDetour( m_hHost_Shutdown );

	ConVar_Unregister();

	CvarShutdown();
	g_pLoggingSystem->Shutdown();

	free( (void *)g_pszBaseDirectory );
	free( (void *)g_ClientVersion.string );
}

//-----------------------------------------------------------------------------
// Find stuff
//-----------------------------------------------------------------------------

void CSvenMod::FindClientState()
{
	MemoryUtils()->InitDisasm(&g_inst, g_pEngineFuncs->pNetAPI->Status, 32, 36);

	do
	{
		if (g_inst.mnemonic == UD_Icmp && g_inst.operand[0].type == UD_OP_MEM && g_inst.operand[0].size == 32 && g_inst.operand[1].type == UD_OP_IMM)
		{
			g_pClientState = reinterpret_cast<int *>(g_inst.operand[0].lval.udword);
			break;
		}

	} while ( MemoryUtils()->Disassemble(&g_inst) );

	if ( !g_pClientState )
	{
		Sys_Error("[SvenMod] Failed to get client state");
	}
}

void CSvenMod::FindClientVersion()
{
	if ( (m_pfn__MsgFunc_ServerVer = MemoryUtils()->FindPattern(SvenModAPI()->Modules()->Client, __MsgFunc_ServerVer)) == NULL)
	{
		Sys_Error("[SvenMod] Can't locate client's version");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, g_SvenMod.m_pfn__MsgFunc_ServerVer, 32, 48);

	do
	{
		if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
		{
			s_pszClientVersion = reinterpret_cast<const char *>(g_inst.operand[0].lval.udword);
			break;
		}

	} while ( MemoryUtils()->Disassemble(&g_inst) );
}

void CSvenMod::FindEngineStudio()
{
	bool bFoundFirstPush = false;
	bool bFoundSecondPush = false;

	MemoryUtils()->InitDisasm(&g_inst, g_SvenMod.m_pfnEngineStudioInit, 32, 70);

	do
	{
		if ( !bFoundFirstPush )
		{
			if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
			{
				g_pEngineStudio = reinterpret_cast<engine_studio_api_t *>(g_inst.operand[0].lval.udword);

				bFoundFirstPush = true;
				continue;
			}
		}
		else if ( !bFoundSecondPush )
		{
			if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
			{
				g_pStudioAPI = reinterpret_cast<r_studio_interface_t *>(g_inst.operand[0].lval.udword);

				bFoundSecondPush = true;
				continue;
			}

			break;
		}
		else
		{
			if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
			{
				int iVersion = g_inst.operand[0].lval.ubyte;

				if ( iVersion != STUDIO_INTERFACE_VERSION )
				{
					LogWarning("Studio's API version differs.\n");
				}
			}

			break;
		}

	} while ( MemoryUtils()->Disassemble(&g_inst) );

	if ( !g_pEngineStudio )
	{
		Sys_Error("[SvenMod] Failed to get engine_studio_api");
	}
}

void CSvenMod::FindStudioModelRenderer()
{
	bool bFoundFirstECX = false;

	MemoryUtils()->InitDisasm(&g_inst, g_pClientFuncs->HUD_GetStudioModelInterface, 32, 128);

	do
	{
		if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_ECX && g_inst.operand[1].type == UD_OP_IMM)
		{
			if (!bFoundFirstECX)
			{
				bFoundFirstECX = true;
			}
			else
			{
				g_pStudioRenderer = reinterpret_cast<CStudioModelRenderer *>(g_inst.operand[1].lval.udword);
				break;
			}
		}

	} while (MemoryUtils()->Disassemble(&g_inst));

	if ( !g_pStudioRenderer )
	{
		Sys_Error("[SvenMod] Failed to get StudioModelRenderer");
	}
}

void CSvenMod::FindPlayerMove()
{
	MemoryUtils()->InitDisasm(&g_inst, g_SvenMod.m_pfnClientDLL_Init, 32, 32);

	do
	{
		if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
		{
			g_pPlayerMove = reinterpret_cast<playermove_t *>(g_inst.operand[0].lval.udword);
			break;
		}

	} while ( MemoryUtils()->Disassemble(&g_inst) );

	if ( !g_pPlayerMove )
	{
		Sys_Error("[SvenMod] Failed to get playermove");
	}
}

void CSvenMod::FindVideoMode()
{
	MemoryUtils()->InitDisasm(&g_inst, g_SvenMod.m_pfnVideoMode_Create, 32, 64);

	do
	{
		if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_MEM && g_inst.operand[1].type == UD_OP_REG && g_inst.operand[1].base == UD_R_EAX)
		{
			g_ppVideoMode = reinterpret_cast<IVideoMode **>(g_inst.operand[0].lval.udword);
			break;
		}

	} while ( MemoryUtils()->Disassemble(&g_inst) );

	if ( !g_ppVideoMode )
	{
		// Can be null if heap don't have enough memory to allocate it
		// Videomode allocates before initialization of SvenMod, so.. everything should be fine..
		Sys_Error("[SvenMod] Failed to get videomode");
	}
	else
	{
		g_pVideoMode = *g_ppVideoMode;
	}
}

void CSvenMod::FindEngineClient()
{
	CreateInterfaceFn pfnHardwareFactory = Sys_GetFactory( SvenModAPI()->Modules()->Hardware );

	if ( !pfnHardwareFactory )
	{
		Sys_Error("[SvenMod] Can't locate hardware's CreateInterface factory");
		return;
	}

	g_pEngineClient = reinterpret_cast<IEngineClient *>(pfnHardwareFactory(ENGINECLIENT_INTERFACE_VERSION, NULL));

	if ( !g_pEngineClient )
	{
		Sys_Error("[SvenMod] Failed to get IEngineClient");
	}
}

void CSvenMod::FindUserMsgs()
{
	int iDisassembledBytes = 0;
	unsigned char *pHookUserMsg = (unsigned char *)g_pEngineFuncs->HookUserMsg;

	MemoryUtils()->InitDisasm(&g_inst, pHookUserMsg, 32, 32);

	do
	{
		if (g_inst.mnemonic == UD_Icall)
		{
			void *pfnHookServerMsg = MemoryUtils()->CalcAbsoluteAddress( pHookUserMsg );

			MemoryUtils()->InitDisasm(&g_inst, pfnHookServerMsg, 32, 48);

			do
			{
				if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_ESI && g_inst.operand[1].type == UD_OP_MEM)
				{
					g_ppClientUserMsgs = reinterpret_cast<usermsg_t **>(g_inst.operand[1].lval.udword);
					break;
				}

			} while ( MemoryUtils()->Disassemble(&g_inst) );

			break;
		}

		pHookUserMsg += iDisassembledBytes;

	} while ( iDisassembledBytes = MemoryUtils()->Disassemble(&g_inst) );

	if ( !g_ppClientUserMsgs )
	{
		Sys_Error("[SvenMod] Failed to get client's user messages");
	}
}

void CSvenMod::FindExtraPlayerInfo()
{
	void *pGetClientColor = MemoryUtils()->FindPattern(SvenModAPI()->Modules()->Client, GetClientColor);

	if ( !pGetClientColor )
	{
		Sys_Error("[SvenMod] Couldn't find function GetClientColor");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, pGetClientColor, 32, 48);

	do
	{
		if (g_inst.mnemonic == UD_Imovsx && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_EAX && g_inst.operand[1].type == UD_OP_MEM)
		{
			g_pPlayerExtraInfo = reinterpret_cast<extra_player_info_t *>(g_inst.operand[1].lval.udword);
			break;
		}

	} while ( MemoryUtils()->Disassemble(&g_inst) );

	if ( !g_pPlayerExtraInfo)
	{
		Sys_Error("[SvenMod] Failed to get extra_player_info");
	}
}

void CSvenMod::FindWeaponsResource()
{
	void *pWeaponsResource__GetFirstPos = MemoryUtils()->FindPattern(SvenModAPI()->Modules()->Client, WeaponsResource__GetFirstPos);

	if ( !pWeaponsResource__GetFirstPos )
	{
		Sys_Error("[SvenMod] Couldn't find function WeaponsResource::GetFirstPos");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, pWeaponsResource__GetFirstPos, 32, 24);

	do
	{
		if (g_inst.mnemonic == UD_Iadd && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_EDX && g_inst.operand[1].type == UD_OP_IMM)
		{
			g_pWeaponsResource = reinterpret_cast<WeaponsResource *>(g_inst.operand[1].lval.udword);
			break;
		}

	} while ( MemoryUtils()->Disassemble(&g_inst) );

	if ( !g_pWeaponsResource )
	{
		Sys_Error("[SvenMod] Failed to get WeaponsResource");
	}
}

void CSvenMod::FindFrametime()
{
	bool bFoundFST = false;

	MemoryUtils()->InitDisasm(&g_inst, m_pfnHost_FilterTime, 32, 128);

	do
	{
		if (g_inst.mnemonic == UD_Ifst)
		{
			if ( g_inst.operand[0].type == UD_OP_MEM && !bFoundFST )
			{
				g_pFrametime = reinterpret_cast<double *>(g_inst.operand[0].lval.udword);
				bFoundFST = true;
			}
		}
		else if (g_inst.mnemonic == UD_Ifadd)
		{
			if ( g_inst.operand[0].type == UD_OP_MEM && bFoundFST )
			{
				g_pRealtime = reinterpret_cast<double *>(g_inst.operand[0].lval.udword);
				break;
			}
		}

	} while ( MemoryUtils()->Disassemble(&g_inst) );

	if ( !g_pRealtime )
	{
		Sys_Error("[SvenMod] Failed to get realtime");
	}

	if ( !g_pFrametime )
	{
		Sys_Error("[SvenMod] Failed to get r_refdef_frametime");
	}
}

void CSvenMod::FindProtocolVersion()
{
	void *pProtocolVersion = MemoryUtils()->FindPattern(SvenModAPI()->Modules()->Hardware, protocol_version_sig);

	if (pProtocolVersion)
	{
		MemoryUtils()->InitDisasm(&g_inst, pProtocolVersion, 32, 15);

		if ( MemoryUtils()->Disassemble(&g_inst) )
		{
			if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
			{
				g_iProtocolVersion = (int)g_inst.operand[0].lval.udword;
			}
		}
	}

	if ( !g_iProtocolVersion )
	{
		Sys_Error("[SvenMod] Can't get protocol version");
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

bool CSvenMod::FindSignatures()
{
	HMODULE hHardwareDLL = Sys_GetModuleHandle("hw.dll");

	g_Modules.Hardware = hHardwareDLL;

	if ( (build_number = (build_numberFn)MemoryUtils()->FindPattern(hHardwareDLL, build_number_sig)) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Couldn't find function build_number");
		return false;
	}
	
	if ( (m_pfnLoadClientDLL = MemoryUtils()->FindPattern(hHardwareDLL, LoadClientDLL)) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Couldn't find function LoadClientDLL");
		return false;
	}
	
	if ( (m_pfnClientDLL_Init = MemoryUtils()->FindPattern(hHardwareDLL, ClientDLL_Init)) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Can't locate playermove");
		return false;
	}
	
	if ( (m_pfnSys_Init = MemoryUtils()->FindPattern(hHardwareDLL, Sys_Init)) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Couldn't find function Sys_Init");
		return false;
	}
	
	if ( (m_pfnHost_FilterTime = MemoryUtils()->FindPattern(hHardwareDLL, Host_FilterTime)) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Couldn't find function Host_FilterTime");
		return false;
	}
	
	if ( (m_pfnHost_Shutdown = MemoryUtils()->FindPattern(hHardwareDLL, Host_Shutdown)) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Couldn't find function Host_Shutdown");
		return false;
	}
	
	if ( (m_pfnEngineStudioInit = MemoryUtils()->FindPattern(hHardwareDLL, EngineStudio_Init)) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Can't locate engine_studio_api");
		return false;
	}
	
	if ( (m_pfnVideoMode_Create = MemoryUtils()->FindPattern(hHardwareDLL, VideoMode_Create)) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Can't locate videomode");
		return false;
	}

	return true;
}

bool CSvenMod::AttachDetours()
{
	m_hLoadClientDLL = DetoursAPI()->DetourFunction( m_pfnLoadClientDLL, HOOKED_LoadClientDLL, GET_FUNC_PTR(ORIG_LoadClientDLL) );

	if ( m_hLoadClientDLL == DETOUR_INVALID_HANDLE )
	{
		Sys_ErrorMessage("[SvenMod] Failed to hook function LoadClientDLL");
		return false;
	}

	m_hSys_Init = DetoursAPI()->DetourFunction( m_pfnSys_Init, HOOKED_Sys_Init, GET_FUNC_PTR(ORIG_Sys_Init) );
	
	if ( m_hSys_Init == DETOUR_INVALID_HANDLE )
	{
		Sys_ErrorMessage("[SvenMod] Failed to hook function Sys_Init");
		return false;
	}

	m_hHost_FilterTime = DetoursAPI()->DetourFunction( m_pfnHost_FilterTime, HOOKED_Host_FilterTime, GET_FUNC_PTR(ORIG_Host_FilterTime) );
	
	if ( m_hHost_FilterTime == DETOUR_INVALID_HANDLE )
	{
		Sys_ErrorMessage("[SvenMod] Failed to hook function Host_FilterTime");
		return false;
	}
	
	m_hHost_Shutdown = DetoursAPI()->DetourFunction( m_pfnHost_Shutdown, HOOKED_Host_Shutdown, GET_FUNC_PTR(ORIG_Host_Shutdown) );
	
	if ( m_hHost_Shutdown == DETOUR_INVALID_HANDLE )
	{
		Sys_ErrorMessage("[SvenMod] Failed to hook function Host_Shutdown");
		return false;
	}

	return true;
}

void CSvenMod::CheckClientVersion()
{
	if ( !s_pszClientVersion )
	{
	L_FAIL:
		Sys_Error("[SvenMod] Failed to get client's version");
		return;
	}

	g_ClientVersion.string = strdup(s_pszClientVersion);

	if ( !g_ClientVersion.string )
		goto L_FAIL;

	char *buffer = const_cast<char *>(g_ClientVersion.string);
	char *pszFirstSeparator = NULL;

	while (*buffer)
	{
		if ( *buffer == '.' )
		{
			pszFirstSeparator = buffer;
			break;
		}

		buffer++;
	}

	if ( !pszFirstSeparator )
	{
		LogWarning("Tried to find client's version in format 'XX.XX' but got this: '%s'.\n", g_ClientVersion.string);
		g_ClientVersion.major_version = atoi(g_ClientVersion.string);
	}
	else
	{
		*pszFirstSeparator = 0;
		g_ClientVersion.major_version = atoi(g_ClientVersion.string);
		*pszFirstSeparator = '.';

		pszFirstSeparator += 1;

		if (!*pszFirstSeparator)
		{
			Sys_Error("[SvenMod] Unexpected end of client's version");
		}
		else
		{
			g_ClientVersion.minor_version = atoi(pszFirstSeparator);
		}
	}

	g_ClientVersion.version = g_ClientVersion.major_version * 1000 + g_ClientVersion.minor_version;
}

//-----------------------------------------------------------------------------
// Export the interfaces
//-----------------------------------------------------------------------------

ICommandLine *CommandLine()
{
	return g_pCommandLine;
}

IFileSystem *FileSystem()
{
	return g_pFileSystem;
}

IRegistry *Registry()
{
	return g_pRegistry;
}

IVideoMode *VideoMode()
{
	if (*g_ppVideoMode)
	{
		return *g_ppVideoMode;
	}

	Sys_Error("[SvenMod] VideoMode is NULL.\nReport asap.");

	return NULL;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSvenMod, ISvenMod, SVENMOD_INTERFACE_VERSION, g_SvenMod);