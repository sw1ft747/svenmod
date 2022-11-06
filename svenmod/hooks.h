#ifndef HOOKS_H
#define HOOKS_H

#ifdef _WIN32
#pragma once
#endif

#include <IHooks.h>

#include "data_struct/hashtable.h"

//-----------------------------------------------------------------------------
// CHooks
//-----------------------------------------------------------------------------

class CHooks : public IHooks
{
	friend DECLARE_FUNC(void, __cdecl, HOOKED_Cvar_DirectSet, cvar_t *pCvar, const char *pszValue);

public:
	CHooks();

	void					Init();
	void					Shutdown();

	virtual bool			RegisterClientHooks(IClientHooks *pClientHooks);
	virtual bool			RegisterClientPostHooks(IClientHooks *pClientHooks);

	virtual bool			UnregisterClientHooks(IClientHooks *pClientHooks);
	virtual bool			UnregisterClientPostHooks(IClientHooks *pClientHooks);

	virtual bool			HookCvarChange(cvar_t *pCvar, CvarChangeHookFn pfnCvarChangeHook);
	virtual bool			UnhookCvarChange(cvar_t *pCvar, CvarChangeHookFn pfnCvarChangeHook);

	virtual DetourHandle_t	HookNetworkMessage(int iType, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook);
	virtual DetourHandle_t	HookNetworkMessage(const char *pszName, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook);
	virtual DetourHandle_t	HookNetworkMessage(netmsg_t *pNetMsg, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook);
	virtual bool			UnhookNetworkMessage(DetourHandle_t hNetMsgHook);

	virtual DetourHandle_t	HookUserMessage(const char *pszName, UserMsgHookFn pfnUserMsgHook, UserMsgHookFn *ppfnOriginalUserMsgHook);
	virtual DetourHandle_t	HookUserMessage(usermsg_t *pUserMsg, UserMsgHookFn pfnUserMsgHook, UserMsgHookFn *ppfnOriginalUserMsgHook);
	virtual bool			UnhookUserMessage(DetourHandle_t hUserMsgHook);
	
	virtual DetourHandle_t	HookEvent(const char *pszName, EventHookFn pfnEventHook, EventHookFn *ppfnOriginalEventHook);
	virtual DetourHandle_t	HookEvent(event_t *pEventHook, EventHookFn pfnEventHook, EventHookFn *ppfnOriginalEventHook);
	virtual bool			UnhookEvent(DetourHandle_t hEventHook);

	virtual DetourHandle_t	HookConsoleCommand(const char *pszName, CommandCallbackFn pfnCommandCallback, CommandCallbackFn *ppfnOriginalCommandCallback);
	virtual DetourHandle_t	HookConsoleCommand(cmd_t *pCommand, CommandCallbackFn pfnCommandCallback, CommandCallbackFn *ppfnOriginalCommandCallback);
	virtual bool			UnhookConsoleCommand(DetourHandle_t hCommandCallback);
	
private:
	void CallCvarChangeChain(cvar_t *pCvar, const char *pszOldValue, float flOldValue);

private:
	CHashTable<cvar_t *, std::vector<CvarChangeHookFn>> m_hCvarChangeHooks;
	DetourHandle_t m_hCvar_DirectSet;
};

extern CHooks g_Hooks;

#endif // HOOKS_H
