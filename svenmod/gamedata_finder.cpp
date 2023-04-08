#include <ISvenModAPI.h>
#include <IMemoryUtils.h>
#include <memutils/patterns.h>

#include "patterns.h"
#include "gamedata_finder.h"

extern ud_t g_inst;
CGameDataFinder g_GameDataFinder;

//-----------------------------------------------------------------------------
// SvenMod
//-----------------------------------------------------------------------------

void CGameDataFinder::FindClientVersion(const char **pClientVersion)
{
#ifdef PLATFORM_WINDOWS
	void *p__MsgFuncServerVer = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Client, Patterns::Client::V___MsgFunc_ServerVer );

	if ( p__MsgFuncServerVer == NULL)
	{
		Sys_Error("[SvenMod] Can't locate \"client's version\"");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, p__MsgFuncServerVer, 32, 48);

	while ( MemoryUtils()->Disassemble(&g_inst) )
	{
		if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
		{
			*pClientVersion = reinterpret_cast<const char *>(g_inst.operand[0].lval.udword);
			break;
		}
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindClientState(int **pClientState)
{
#ifdef PLATFORM_WINDOWS
	MemoryUtils()->InitDisasm(&g_inst, g_pEngineFuncs->pNetAPI->Status, 32, 36);

	while ( MemoryUtils()->Disassemble(&g_inst) )
	{
		if (g_inst.mnemonic == UD_Icmp && g_inst.operand[0].type == UD_OP_MEM && g_inst.operand[0].size == 32 && g_inst.operand[1].type == UD_OP_IMM)
		{
			*pClientState = reinterpret_cast<int *>(g_inst.operand[0].lval.udword);
			break;
		}
	}

	if ( !(*pClientState) )
	{
		Sys_Error("[SvenMod] Failed to get \"client state\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindProtocolVersion(int *pProtocolVersion)
{
#ifdef PLATFORM_WINDOWS
	void *pfnProtocolVersion = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::V_protocol_version );

	if ( pfnProtocolVersion )
	{
		MemoryUtils()->InitDisasm(&g_inst, pfnProtocolVersion, 32, 15);

		if ( MemoryUtils()->Disassemble(&g_inst) && g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM )
		{
			*pProtocolVersion = (int)g_inst.operand[0].lval.udword;
		}
	}
	else
	{
		Sys_Error("[SvenMod] Failed to locate \"protocol version\"");
	}

	if ( *pProtocolVersion == 0 )
	{
		Sys_Error("[SvenMod] Can't get \"protocol version\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindClientFuncs(cl_clientfuncs_t **pClientFuncs, void *pfnLoadClientDLL /* = NULL */)
{
#ifdef PLATFORM_WINDOWS
	if ( pfnLoadClientDLL == NULL )
	{
		pfnLoadClientDLL = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::LoadClientDLL );
	}

	if ( pfnLoadClientDLL )
	{
		MemoryUtils()->InitDisasm(&g_inst, pfnLoadClientDLL, 32);

		bool bFoundKeyInstruction = false;

		while ( MemoryUtils()->Disassemble(&g_inst) )
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
					*pClientFuncs = reinterpret_cast<cl_clientfuncs_t *>(g_inst.operand[0].lval.udword);
					break;
				}
			}
		}
	}
	else
	{
		Sys_Error("[SvenMod] Couldn't find function \"LoadClientDLL\"");
	}

	if ( !(*pClientFuncs) )
	{
		Sys_Error("[SvenMod] Failed to get \"cl_clientfuncs\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindFrametime(double **pRealtime, double **pClientTime, double **pFrametime, void *pfnHost_FilterTime /* = NULL */)
{
#ifdef PLATFORM_WINDOWS
	if ( pfnHost_FilterTime == NULL )
	{
		pfnHost_FilterTime = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::Host_FilterTime );
	}

	if ( pfnHost_FilterTime )
	{
		bool bFoundFST = false;

		MemoryUtils()->InitDisasm(&g_inst, pfnHost_FilterTime, 32, 128);

		while (MemoryUtils()->Disassemble(&g_inst)) {
			if (g_inst.mnemonic == UD_Ifst && g_inst.operand[0].type == UD_OP_MEM && !bFoundFST)
			{
				*pFrametime = reinterpret_cast<double *>(g_inst.operand[0].lval.udword);
				bFoundFST = true;
			}
			else if (g_inst.mnemonic == UD_Ifadd && g_inst.operand[0].type == UD_OP_MEM && bFoundFST)
			{
				*pRealtime = reinterpret_cast<double *>(g_inst.operand[0].lval.udword);
				break;
			}
		}

		MemoryUtils()->InitDisasm(&g_inst, g_pEngineFuncs->GetClientTime, 32, 16);

		if ( MemoryUtils()->Disassemble(&g_inst) && g_inst.mnemonic == UD_Ifld )
		{
			*pClientTime = reinterpret_cast<double *>(g_inst.operand[0].lval.udword);
		}
	}
	else
	{
		Sys_Error("[SvenMod] Couldn't find function \"Host_FilterTime\"");
	}

	if ( !(*pRealtime) )
	{
		Sys_Error("[SvenMod] Failed to get \"realtime\"");
	}

	if ( !(*pClientTime) )
	{
		Sys_Error("[SvenMod] Failed to get \"r_refdef_time\"");
	}

	if ( !(*pFrametime) )
	{
		Sys_Error("[SvenMod] Failed to get \"r_refdef_frametime\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindEngineStudio(engine_studio_api_t **pEngineStudio, r_studio_interface_t **pStudioAPI, void *pEngineStudioInit /* = NULL */)
{
#ifdef PLATFORM_WINDOWS
	if ( pEngineStudioInit == NULL )
	{
		pEngineStudioInit = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::V_EngineStudio_Init );
	}

	if ( pEngineStudioInit )
	{
		bool bFoundFirstPush = false;
		bool bFoundSecondPush = false;

		MemoryUtils()->InitDisasm(&g_inst, pEngineStudioInit, 32, 70);

		while ( MemoryUtils()->Disassemble(&g_inst) )
		{
			if ( !bFoundFirstPush )
			{
				if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
				{
					*pEngineStudio = reinterpret_cast<engine_studio_api_t *>(g_inst.operand[0].lval.udword);

					bFoundFirstPush = true;
					continue;
				}
			}
			else if ( !bFoundSecondPush )
			{
				if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
				{
					*pStudioAPI = *reinterpret_cast<r_studio_interface_t **>(g_inst.operand[0].lval.udword);

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
		}
	}
	else
	{
		Sys_Error("[SvenMod] Can't locate \"engine_studio_api\"");
	}

	if ( !(*pEngineStudio) )
	{
		Sys_Error("[SvenMod] Failed to get \"engine_studio_api\"");
	}

	if ( !(*pStudioAPI) )
	{
		Sys_Error("[SvenMod] Failed to get \"r_studio_interface\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindEngineClient(IEngineClient **pEngineClient)
{
#ifdef PLATFORM_WINDOWS
	CreateInterfaceFn pfnHardwareFactory = Sys_GetFactory( SvenModAPI()->Modules()->Hardware );

	if ( !pfnHardwareFactory )
	{
		Sys_Error("[SvenMod] Can't locate hardware's \"CreateInterface\" factory");
		return;
	}

	*pEngineClient = reinterpret_cast<IEngineClient *>(pfnHardwareFactory(ENGINECLIENT_INTERFACE_VERSION, NULL));

	if ( !(*pEngineClient) )
	{
		Sys_Error("[SvenMod] Failed to get \"IEngineClient\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindStudioModelRenderer(CStudioModelRenderer **pStudioRenderer)
{
#ifdef PLATFORM_WINDOWS
	bool bFoundFirstECX = false;

	MemoryUtils()->InitDisasm(&g_inst, g_pClientFuncs->HUD_GetStudioModelInterface, 32, 128);

	while ( MemoryUtils()->Disassemble(&g_inst) )
	{
		if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_ECX && g_inst.operand[1].type == UD_OP_IMM)
		{
			if ( !bFoundFirstECX )
			{
				bFoundFirstECX = true;
			}
			else
			{
				*pStudioRenderer = reinterpret_cast<CStudioModelRenderer *>(g_inst.operand[1].lval.udword);
				break;
			}
		}
	}

	if ( !(*pStudioRenderer) )
	{
		Sys_Error("[SvenMod] Failed to get \"StudioModelRenderer\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindPlayerMove(playermove_t **pPlayerMove, void *pfnClientDLL_Init /* = NULL */)
{
#ifdef PLATFORM_WINDOWS
	if ( pfnClientDLL_Init == NULL )
	{
		pfnClientDLL_Init = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::ClientDLL_Init );
	}

	if ( pfnClientDLL_Init )
	{
		MemoryUtils()->InitDisasm(&g_inst, pfnClientDLL_Init, 32, 32);

		while ( MemoryUtils()->Disassemble(&g_inst) )
		{
			if (g_inst.mnemonic == UD_Ipush && g_inst.operand[0].type == UD_OP_IMM)
			{
				*pPlayerMove = reinterpret_cast<playermove_t *>(g_inst.operand[0].lval.udword);
				break;
			}
		}
	}
	else
	{
		Sys_Error("[SvenMod] Can't locate \"playermove\"");
	}

	if ( !(*pPlayerMove) )
	{
		Sys_Error("[SvenMod] Failed to get \"playermove\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindVideoMode(IVideoMode ***ppVideoMode, IVideoMode **pVideoMode, void *pVideoMode_Create /* = NULL */)
{
#ifdef PLATFORM_WINDOWS
	if ( pVideoMode_Create == NULL )
	{
		pVideoMode_Create = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::V_VideoMode_Create );
	}

	if ( pVideoMode_Create )
	{
		MemoryUtils()->InitDisasm(&g_inst, pVideoMode_Create, 32, 64);

		while ( MemoryUtils()->Disassemble(&g_inst) )
		{
			if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_MEM && g_inst.operand[1].type == UD_OP_REG && g_inst.operand[1].base == UD_R_EAX)
			{
				*ppVideoMode = reinterpret_cast<IVideoMode **>(g_inst.operand[0].lval.udword);
				break;
			}
		}
	}
	else
	{
		Sys_Error("[SvenMod] Can't locate \"videomode\"");
	}

	if ( !(*ppVideoMode) )
	{
		// Can be null if heap don't have enough memory to allocate it
		// Videomode allocates before initialization of SvenMod, so.. everything should be fine..
		Sys_Error("[SvenMod] Failed to get \"videomode\"");
	}
	else
	{
		*pVideoMode = **ppVideoMode;
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindUserMessages(usermsg_t ***ppClientUserMsgs)
{
#ifdef PLATFORM_WINDOWS
	int iDisassembledBytes = 0;
	unsigned char *pfnHookUserMsg = (unsigned char *)g_pEngineFuncs->HookUserMsg;

	MemoryUtils()->InitDisasm(&g_inst, pfnHookUserMsg, 32, 32);

	while ( iDisassembledBytes = MemoryUtils()->Disassemble(&g_inst) )
	{
		if (g_inst.mnemonic == UD_Icall)
		{
			void *pfnHookServerMsg = MemoryUtils()->CalcAbsoluteAddress( pfnHookUserMsg );

			MemoryUtils()->InitDisasm(&g_inst, pfnHookServerMsg, 32, 48);

			while ( MemoryUtils()->Disassemble(&g_inst) )
			{
				if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_ESI && g_inst.operand[1].type == UD_OP_MEM)
				{
					*ppClientUserMsgs = reinterpret_cast<usermsg_t **>(g_inst.operand[1].lval.udword);
					break;
				}

			}

			break;
		}

		pfnHookUserMsg += iDisassembledBytes;
	}

	if ( !(*ppClientUserMsgs) )
	{
		Sys_Error("[SvenMod] Failed to get \"client's user messages\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindEventHooks(event_t **ppEventHooks)
{
#ifdef PLATFORM_WINDOWS
	unsigned char *pfnHookEvent = (unsigned char *)g_pEngineFuncs->HookEvent;

	MemoryUtils()->InitDisasm(&g_inst, pfnHookEvent, 32, 16);

	if ( MemoryUtils()->Disassemble(&g_inst) )
	{
		if ( g_inst.mnemonic == UD_Ijmp )
		{
			DEFINE_PATTERN(g_pEventHooks_sig, "56 8B 35 ? ? ? ? 85 F6 74 ? 8B 46 04");

			void *pHookEvent = MemoryUtils()->CalcAbsoluteAddress( pfnHookEvent );
			pHookEvent = MemoryUtils()->FindPatternWithin( SvenModAPI()->Modules()->Hardware, g_pEventHooks_sig, pHookEvent, (unsigned char *)pHookEvent + 128 );

			if ( pHookEvent != NULL )
			{
				MemoryUtils()->InitDisasm(&g_inst, pHookEvent, 32, 32);

				while ( MemoryUtils()->Disassemble(&g_inst) )
				{
					if ( g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_ESI && g_inst.operand[1].type == UD_OP_MEM )
					{
						*ppEventHooks = reinterpret_cast<event_t *>(g_inst.operand[1].lval.udword);
						break;
					}
				}

				if ( *ppEventHooks == NULL )
				{
					Sys_Error("[SvenMod] Failed to get \"g_pEventHooks\"");
				}
			}
			else
			{
				Sys_Error("[SvenMod] Failed to locate \"g_pEventHooks\"");
			}
		}
		else
		{
			Sys_Error("[SvenMod] Failed to locate JMP op-code on function \"HookEvent\"");
		}
	}
	else
	{
		Sys_Error("[SvenMod] Failed to disassemble \"HookEvent\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindNetworkMessages(netmsg_t **pNetworkMessages, sizebuf_t **pNetMesasge, int **pReadCount, int **pBadRead)
{
#ifdef PLATFORM_WINDOWS
	void *pNetworkMessagesSig = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::g_NetworkMessages );

	if ( !pNetworkMessagesSig )
	{
		Sys_Error("[SvenMod] Can't locate \"g_NetworkMessages\"");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, pNetworkMessagesSig, 32, 32);
	MemoryUtils()->Disassemble(&g_inst);

	if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_ESI && g_inst.operand[1].type == UD_OP_MEM &&
		g_inst.operand[1].index == UD_R_ECX && g_inst.operand[1].scale == 4 && g_inst.operand[1].offset == 32)
	{
		*pNetworkMessages = reinterpret_cast<netmsg_t *>(g_inst.operand[1].lval.udword - sizeof(void *));
	}

	if ( !(*pNetworkMessages) )
	{
		Sys_Error("[SvenMod] Failed to get \"g_NetworkMessages\"");
	}

	void *pfnMSG_ReadByte = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::MSG_ReadByte );

	if ( !pfnMSG_ReadByte )
	{
		Sys_Error("[SvenMod] Can't locate \"MSG_ReadByte\"");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, pfnMSG_ReadByte, 32, 32);

	while ( MemoryUtils()->Disassemble(&g_inst) )
	{
		if ( g_inst.mnemonic == UD_Imov )
		{
			if ( g_inst.operand[0].type == UD_OP_REG )
			{
				if ( g_inst.operand[0].base == UD_R_ECX && g_inst.operand[1].type == UD_OP_MEM )
				{
					*pReadCount = reinterpret_cast<int *>(g_inst.operand[1].lval.udword);
				}
			}
			else if ( g_inst.operand[0].type == UD_OP_MEM )
			{
				if ( g_inst.operand[1].type == UD_OP_IMM && g_inst.operand[1].lval.udword == 1 )
				{
					*pBadRead = reinterpret_cast<int *>(g_inst.operand[0].lval.udword);
					break;
				}
			}
		}
		else if ( g_inst.mnemonic == UD_Icmp )
		{
			if ( g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_EDX && g_inst.operand[1].type == UD_OP_MEM )
			{
				*pNetMesasge = reinterpret_cast<sizebuf_t *>(g_inst.operand[1].lval.udword - sizeof(void *) * 4);
			}
		}
	}

	if ( !(*pReadCount) )
	{
		Sys_Error("[SvenMod] Failed to get \"msg_readcount\"");
	}

	if ( !(*pBadRead) )
	{
		Sys_Error("[SvenMod] Failed to get \"msg_badread\"");
	}

	if ( !(*pNetMesasge) )
	{
		Sys_Error("[SvenMod] Failed to get \"net_message\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindExtraPlayerInfo(extra_player_info_t **pPlayerExtraInfo)
{
#ifdef PLATFORM_WINDOWS
	void *pfnGetClientColor = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Client, Patterns::Client::GetClientColor );

	if ( !pfnGetClientColor )
	{
		Sys_Error("[SvenMod] Couldn't find function \"GetClientColor\"");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, pfnGetClientColor, 32, 48);

	while ( MemoryUtils()->Disassemble(&g_inst) )
	{
		if (g_inst.mnemonic == UD_Imovsx && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_EAX && g_inst.operand[1].type == UD_OP_MEM)
		{
			*pPlayerExtraInfo = reinterpret_cast<extra_player_info_t *>(g_inst.operand[1].lval.udword - 0xA);
			break;
		}
	}

	if ( !(*pPlayerExtraInfo) )
	{
		Sys_Error("[SvenMod] Failed to get \"extra_player_info\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindWeaponsResource(WeaponsResource **pWeaponsResource)
{
#ifdef PLATFORM_WINDOWS
	void *pfmWeaponsResource__GetFirstPos = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Client, Patterns::Client::WeaponsResource__GetFirstPos );

	if ( !pfmWeaponsResource__GetFirstPos )
	{
		Sys_Error("[SvenMod] Couldn't find function \"WeaponsResource::GetFirstPos\"");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, pfmWeaponsResource__GetFirstPos, 32, 24);

	while ( MemoryUtils()->Disassemble(&g_inst) )
	{
		if (g_inst.mnemonic == UD_Iadd && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_EDX && g_inst.operand[1].type == UD_OP_IMM)
		{
			*pWeaponsResource = reinterpret_cast<WeaponsResource *>(g_inst.operand[1].lval.udword);
			break;
		}
	}

	if ( !(*pWeaponsResource) )
	{
		Sys_Error("[SvenMod] Failed to get \"WeaponsResource\"");
	}
#else
#error Implement Linux equivalent
#endif
}

//-----------------------------------------------------------------------------
// CVar
//-----------------------------------------------------------------------------

void *CGameDataFinder::FindZ_Free()
{
#ifdef PLATFORM_WINDOWS
	void *pfnZ_Free = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::Z_Free );
#else
	void *pfnZ_Free = MemoryUtils()->ResolveSymbol( SvenModAPI()->Modules()->Hardware, Symbols::Hardware::Z_Free );
#endif

	if ( pfnZ_Free == NULL )
	{
		Sys_Error("[SvenMod] Couldn't find function \"Z_Free\"");
	}

	return pfnZ_Free;
}

void *CGameDataFinder::FindCvar_DirectSet()
{
#ifdef PLATFORM_WINDOWS
	void *pfnCvar_DirectSet = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::Cvar_DirectSet );
#else
	void *pfnCvar_DirectSet = MemoryUtils()->ResolveSymbol( SvenModAPI()->Modules()->Hardware, Symbols::Hardware::Cvar_DirectSet );
#endif

	if ( pfnCvar_DirectSet == NULL )
	{
		Sys_Error("[SvenMod] Couldn't find function \"Cvar_DirectSet\"");
	}

	return pfnCvar_DirectSet;
}

void CGameDataFinder::FindCvarList(cvar_t ***ppCvarList)
{
#ifdef PLATFORM_WINDOWS
	void *pfnCvar_RemoveClientDLLCvars = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::Cvar_RemoveClientDLLCvars );
#else
	void *pfnCvar_RemoveClientDLLCvars = MemoryUtils()->ResolveSymbol( SvenModAPI()->Modules()->Hardware, Symbols::Hardware::Cvar_RemoveClientDLLCvars );
#endif

	if ( pfnCvar_RemoveClientDLLCvars == NULL )
	{
		Sys_Error("[SvenMod] Couldn't find function \"Cvar_RemoveClientDLLCvars\"");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, pfnCvar_RemoveClientDLLCvars, 32, 32);

	while ( MemoryUtils()->Disassemble(&g_inst) )
	{
		if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_ESI && g_inst.operand[1].type == UD_OP_MEM)
		{
			*ppCvarList = reinterpret_cast<cvar_t **>(g_inst.operand[1].lval.udword);
			break;
		}
	}

	if ( !(*ppCvarList) )
	{
		Sys_Error("[SvenMod] Failed to get \"g_ppCvarList\"");
	}
}

void CGameDataFinder::FindCmdList(cmd_t ***ppCmdList)
{
#ifdef PLATFORM_WINDOWS
	void *pfnCvar_RemoveClientDLLCmds = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::Cmd_RemoveClientDLLCmds );
#else
	void *pfnCvar_RemoveClientDLLCmds = MemoryUtils()->ResolveSymbol( SvenModAPI()->Modules()->Hardware, Symbols::Hardware::Cmd_RemoveClientDLLCmds );
#endif

	if ( pfnCvar_RemoveClientDLLCmds == NULL )
	{
		Sys_Error("[SvenMod] Couldn't find function \"Cvar_RemoveClientDLLCmds\"");
		return;
	}

	MemoryUtils()->InitDisasm(&g_inst, pfnCvar_RemoveClientDLLCmds, 32, 32);

	while ( MemoryUtils()->Disassemble(&g_inst) )
	{
		if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_EAX && g_inst.operand[1].type == UD_OP_MEM)
		{
			*ppCmdList = reinterpret_cast<cmd_t **>(g_inst.operand[1].lval.udword);
			break;
		}
	}

	if ( !(*ppCmdList) )
	{
		Sys_Error("[SvenMod] Failed to get \"g_ppCmdList\"");
	}
}

void CGameDataFinder::FindCommandArgs(int **pArgC, const char ***ppArgV)
{
#ifdef PLATFORM_WINDOWS
	if ( *((unsigned char *)g_pEngineFuncs->Cmd_Argc) == 0xE9 ) // JMP opcode
	{
		void *pCmd_Argc = MemoryUtils()->CalcAbsoluteAddress( g_pEngineFuncs->Cmd_Argc );

		MemoryUtils()->InitDisasm(&g_inst, pCmd_Argc, 32, 16);

		while ( MemoryUtils()->Disassemble(&g_inst) )
		{
			if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_EAX && g_inst.operand[1].type == UD_OP_MEM)
			{
				*pArgC = reinterpret_cast<int *>(g_inst.operand[1].lval.udword);
				break;
			}
		}
	}

	if ( !(*pArgC) )
	{
		Sys_Error("[SvenMod] Failed to get \"g_pArgC\"");
	}

	if ( *((unsigned char *)g_pEngineFuncs->Cmd_Argv) == 0xE9 ) // JMP opcode
	{
		void *pCmd_Argv = MemoryUtils()->CalcAbsoluteAddress( g_pEngineFuncs->Cmd_Argv );

		MemoryUtils()->InitDisasm(&g_inst, pCmd_Argv, 32, 96);

		while ( MemoryUtils()->Disassemble(&g_inst) )
		{
			if (g_inst.mnemonic == UD_Imov && g_inst.operand[0].type == UD_OP_REG && g_inst.operand[0].base == UD_R_EAX && g_inst.operand[1].type == UD_OP_MEM &&
				g_inst.operand[1].index == UD_R_EAX && g_inst.operand[1].scale == 4 && g_inst.operand[1].offset == 32)
			{
				*ppArgV = reinterpret_cast<const char **>(g_inst.operand[1].lval.udword);
				break;
			}
		}
	}

	if ( !(*ppArgV) )
	{
		Sys_Error("[SvenMod] Failed to get \"g_ppArgV\"");
	}
#else
#error Implement Linux equivalent
#endif
}

void CGameDataFinder::FindConsolePrint(void **pRichText__InsertColorChange, void **pRichText__InsertString)
{
#ifdef PLATFORM_WINDOWS
	bool bFoundFirstCall = false;
	int iDisassembledBytes = 0;

	void *pCGameConsoleDialog__DPrint = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->GameUI, Patterns::GameUI::CGameConsoleDialog__DPrint );
	//void *pCGameConsoleDialog__DPrint = MemoryUtils()->ResolveSymbol( SvenModAPI()->Modules()->GameUI, Symbols::Hardware::CGameConsoleDialog__DPrint );

	if ( !pCGameConsoleDialog__DPrint )
	{
		Sys_Error("[SvenMod] Couldn't find \"CGameConsoleDialog::DPrint\"");
		return;
	}

	unsigned char *p = (unsigned char *)pCGameConsoleDialog__DPrint;

	MemoryUtils()->InitDisasm(&g_inst, pCGameConsoleDialog__DPrint, 32, 128);

	while ( iDisassembledBytes = MemoryUtils()->Disassemble(&g_inst) )
	{
		if (g_inst.mnemonic == UD_Icall && g_inst.operand[0].type == UD_OP_JIMM)
		{
			if ( !bFoundFirstCall )
			{
				*pRichText__InsertColorChange = MemoryUtils()->CalcAbsoluteAddress(p);
				bFoundFirstCall = true;
			}
			else
			{
				*pRichText__InsertString = MemoryUtils()->CalcAbsoluteAddress(p);
				break;
			}
		}

		p += iDisassembledBytes;
	}

	if ( !(*pRichText__InsertColorChange) )
	{
		Sys_Error("[SvenMod] Failed to get \"vgui2::RichText::InsertColorChange\"");
	}

	if ( !(*pRichText__InsertString) )
	{
		Sys_Error("[SvenMod] Failed to get \"vgui2::RichText::InsertString\"");
	}
#else
#error Implement Linux equivalent
#endif
}


//-----------------------------------------------------------------------------
// etc
//-----------------------------------------------------------------------------

void *CGameDataFinder::FindSCR_BeginLoadingPlaque()
{
#ifdef PLATFORM_WINDOWS
	void *pfnSCR_BeginLoadingPlaque = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::SCR_BeginLoadingPlaque );
#else
	void *pfnSCR_BeginLoadingPlaque = MemoryUtils()->ResolveSymbol( SvenModAPI()->Modules()->Hardware, Symbols::Hardware::SCR_BeginLoadingPlaque );
#endif

	if ( pfnSCR_BeginLoadingPlaque == NULL )
	{
		Sys_Error("[SvenMod] Couldn't find function \"SCR_BeginLoadingPlaque\"");
	}

	return pfnSCR_BeginLoadingPlaque;
}

void *CGameDataFinder::FindSCR_EndLoadingPlaque()
{
#ifdef PLATFORM_WINDOWS
	void *pfnSCR_EndLoadingPlaque = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::SCR_EndLoadingPlaque );
#else
	void *pfnSCR_EndLoadingPlaque = MemoryUtils()->ResolveSymbol( SvenModAPI()->Modules()->Hardware, Symbols::Hardware::SCR_EndLoadingPlaque );
#endif

	if ( pfnSCR_EndLoadingPlaque == NULL )
	{
		Sys_Error("[SvenMod] Couldn't find function \"SCR_EndLoadingPlaque\"");
	}

	return pfnSCR_EndLoadingPlaque;
}

void *CGameDataFinder::FindCL_Disconnect()
{
#ifdef PLATFORM_WINDOWS
	void *pfnCL_Disconnect = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::CL_Disconnect );
#else
	void *pfnCL_Disconnect = MemoryUtils()->ResolveSymbol( SvenModAPI()->Modules()->Hardware, Symbols::Hardware::CL_Disconnect );
#endif

	if ( pfnCL_Disconnect == NULL )
	{
		Sys_Error("[SvenMod] Couldn't find function \"CL_Disconnect\"");
	}

	return pfnCL_Disconnect;
}

void *CGameDataFinder::FindR_RenderScene()
{
#ifdef PLATFORM_WINDOWS
	void *pfnR_RenderScene = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::R_RenderScene );
#else
	void *pfnR_RenderScene = MemoryUtils()->ResolveSymbol( SvenModAPI()->Modules()->Hardware, Symbols::Hardware::R_RenderScene );
#endif

	if ( pfnR_RenderScene == NULL )
	{
		Sys_Error("[SvenMod] Couldn't find function \"R_RenderScene\"");
	}

	return pfnR_RenderScene;
}
