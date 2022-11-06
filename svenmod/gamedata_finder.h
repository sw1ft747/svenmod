#ifndef GAMEDATA_FINDER_H
#define GAMEDATA_FINDER_H

#ifdef _WIN32
#pragma once
#endif

#include <hl_sdk/common/sizebuf.h>

class CGameDataFinder
{
public:
	// SvenMod

	void FindClientVersion(const char **pClientVersion);
	void FindClientState(int **pClientState);

	void FindProtocolVersion(int *pProtocolVersion);
	void FindFrametime(double **pRealtime, double **pClientTime, double **pFrametime, void *pfnHost_FilterTime = NULL);

	void FindClientFuncs(cl_clientfuncs_t **pClientFuncs, void *pfnLoadClientDLL = NULL);

	void FindEngineStudio(engine_studio_api_t **pEngineStudio, r_studio_interface_t **pStudioAPI, void *pEngineStudioInit = NULL);
	void FindEngineClient(IEngineClient **pEngineClient);
	void FindStudioModelRenderer(CStudioModelRenderer **pStudioRenderer);
	void FindPlayerMove(playermove_t **pPlayerMove, void *pfnClientDLL_Init = NULL);
	void FindVideoMode(IVideoMode ***ppVideoMode, IVideoMode **pVideoMode, void *pVideoMode_Create = NULL);
	void FindUserMessages(usermsg_t ***ppClientUserMsgs);
	void FindEventHooks(event_t **ppEventHooks);
	void FindNetworkMessages(netmsg_t **pNetworkMessages, sizebuf_t **pNetMesasge, int **pReadCount, int **pBadRead);
	void FindExtraPlayerInfo(extra_player_info_t **pPlayerExtraInfo);
	void FindWeaponsResource(WeaponsResource **pWeaponsResource);

	// CVar

	void *FindZ_Free();
	void *FindCvar_DirectSet();
	void FindCvarList(cvar_t ***ppCvarList);
	void FindCmdList(cmd_t ***ppCmdList);
	void FindCommandArgs(int **pArgC, const char ***ppArgV);
	void FindConsolePrint(void **pRichText__InsertColorChange, void **pRichText__InsertString);

	// etc

	void *FindSCR_BeginLoadingPlaque();
	void *FindSCR_EndLoadingPlaque();
	void *FindCL_Disconnect();
	void *FindR_RenderScene();
};

extern CGameDataFinder g_GameDataFinder;

#endif // GAMEDATA_FINDER_H