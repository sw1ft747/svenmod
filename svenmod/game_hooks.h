#ifndef GAME_HOOKS_H
#define GAME_HOOKS_H

#ifdef _WIN32
#pragma once
#endif

#include <vector>

#include <IClientHooks.h>
#include <IDetoursAPI.h>

class CGameHooksHandler
{
public:
	void Init();
	void Shutdown();

	bool RegisterClientHooks(IClientHooks *pClientHooks);
	bool UnregisterClientHooks(IClientHooks *pClientHooks);

	bool RegisterClientPostHooks(IClientHooks *pClientPostHooks);
	bool UnregisterClientPostHooks(IClientHooks *pClientPostHooks);

private:
	DetourHandle_t m_hPaintTraverse;
	DetourHandle_t m_hSCR_BeginLoadingPlaque;
	DetourHandle_t m_hSCR_EndLoadingPlaque;
	DetourHandle_t m_hCL_Disconnect;
	DetourHandle_t m_hR_RenderScene;
	DetourHandle_t m_hUserMsgHook_CurWeapon;
};

extern CGameHooksHandler g_GameHooksHandler;

#endif // GAME_HOOKS_H