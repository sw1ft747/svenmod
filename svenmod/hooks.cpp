#include <IHooks.h>
#include <ICvar.h>
#include <IDetoursAPI.h>
#include <dbg.h>

#include <hl_sdk/common/usermsg.h>
#include <hl_sdk/common/protocol.h>

#include "game_hooks.h"

extern usermsg_t **g_ppClientUserMsgs;
extern netmsg_t *g_pNetworkMessages;

//-----------------------------------------------------------------------------
// CHooks
//-----------------------------------------------------------------------------

class CHooks : public IHooks
{
public:
	virtual bool			RegisterClientHooks(IClientHooks *pClientHooks);
	virtual bool			RegisterClientPostHooks(IClientHooks *pClientHooks);

	virtual bool			UnregisterClientHooks(IClientHooks *pClientHooks);
	virtual bool			UnregisterClientPostHooks(IClientHooks *pClientHooks);

	virtual DetourHandle_t	HookNetworkMessage(int iType, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook);
	virtual DetourHandle_t	HookNetworkMessage(const char *pszName, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook);
	virtual DetourHandle_t	HookNetworkMessage(netmsg_t *pNetMsg, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook);
	virtual bool			UnhookNetworkMessage(DetourHandle_t hNetMsgHook);

	virtual DetourHandle_t	HookUserMessage(const char *pszName, UserMsgHookFn pfnUserMsgHook, UserMsgHookFn *ppfnOriginalUserMsgHook);
	virtual DetourHandle_t	HookUserMessage(usermsg_t *pUserMsg, UserMsgHookFn pfnUserMsgHook, UserMsgHookFn *ppfnOriginalUserMsgHook);
	virtual bool			UnhookUserMessage(DetourHandle_t hUserMsgHook);

	virtual DetourHandle_t	HookConsoleCommand(const char *pszName, CommandCallbackFn pfnCommandCallback, CommandCallbackFn *ppfnOriginalCommandCallback);
	virtual DetourHandle_t	HookConsoleCommand(cmd_t *pCommand, CommandCallbackFn pfnCommandCallback, CommandCallbackFn *ppfnOriginalCommandCallback);
	virtual bool			UnhookConsoleCommand(DetourHandle_t hCommandCallback);
};

//-----------------------------------------------------------------------------
// CHooks implementation
//-----------------------------------------------------------------------------

bool CHooks::RegisterClientHooks(IClientHooks *pClientHooks)
{
	return g_GameHooksHandler.RegisterClientHooks(pClientHooks);
}

bool CHooks::RegisterClientPostHooks(IClientHooks *pClientHooks)
{
	return g_GameHooksHandler.RegisterClientPostHooks(pClientHooks);
}

bool CHooks::UnregisterClientHooks(IClientHooks *pClientHooks)
{
	return g_GameHooksHandler.UnregisterClientHooks(pClientHooks);
}

bool CHooks::UnregisterClientPostHooks(IClientHooks *pClientHooks)
{
	return g_GameHooksHandler.UnregisterClientPostHooks(pClientHooks);
}

//-----------------------------------------------------------------------------
// Network Message
//-----------------------------------------------------------------------------

DetourHandle_t CHooks::HookNetworkMessage(int iType, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook)
{
	if (iType > SVC_NOP && iType <= SVC_LASTMSG)
	{
		netmsg_t *pNetMsg = &g_pNetworkMessages[iType];

		netmsg_t **ppNetMsg = &pNetMsg;
		constexpr size_t index = offsetof(netmsg_t, function) / sizeof(void *);

		return DetoursAPI()->DetourVirtualFunction(ppNetMsg, index, pfnNetMsgHook, (void **)ppfnOriginalNetMsgHook);
	}
	else
	{
		Warning("[SvenMod] CHooks::HookNetworkMessage: bad network message type (%d)\n", iType);
	}

	return DETOUR_INVALID_HANDLE;
}

DetourHandle_t CHooks::HookNetworkMessage(const char *pszName, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook)
{
	for (int i = SVC_NOP + 1; i <= SVC_LASTMSG; i++)
	{
		netmsg_t *pNetMsg = &g_pNetworkMessages[i];

		if ( !stricmp(pszName, pNetMsg->name) )
		{
			netmsg_t **ppNetMsg = &pNetMsg;
			constexpr size_t index = offsetof(netmsg_t, function) / sizeof(void *);

			return DetoursAPI()->DetourVirtualFunction(ppNetMsg, index, pfnNetMsgHook, (void **)ppfnOriginalNetMsgHook);
		}
	}

	Warning("[SvenMod] CHooks::HookNetworkMessage: bad network message name (%s)\n", pszName);

	return DETOUR_INVALID_HANDLE;
}

DetourHandle_t CHooks::HookNetworkMessage(netmsg_t *pNetMsg, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook)
{
	netmsg_t **ppNetMsg = &pNetMsg;
	constexpr size_t index = offsetof(netmsg_t, function) / sizeof(void *);

	return DetoursAPI()->DetourVirtualFunction(ppNetMsg, index, pfnNetMsgHook, (void **)ppfnOriginalNetMsgHook);
}

bool CHooks::UnhookNetworkMessage(DetourHandle_t hNetMsgHook)
{
	return DetoursAPI()->RemoveDetour(hNetMsgHook);
}

//-----------------------------------------------------------------------------
// User Message
//-----------------------------------------------------------------------------

DetourHandle_t CHooks::HookUserMessage(const char *pszName, UserMsgHookFn pfnUserMsgHook, UserMsgHookFn *ppfnOriginalUserMsgHook)
{
	// HACK: let DetoursAPI think we're going to hook a virtual function
	
	usermsg_t *pUserMsg = *g_ppClientUserMsgs;

	while (pUserMsg)
	{
		if ( !stricmp(pszName, pUserMsg->name) )
		{
			usermsg_t **ppUserMsg = &pUserMsg;
			constexpr size_t index = offsetof(usermsg_t, function) / sizeof(void *);

			return DetoursAPI()->DetourVirtualFunction(ppUserMsg, index, pfnUserMsgHook, (void **)ppfnOriginalUserMsgHook);
		}

		pUserMsg = pUserMsg->next;
	}

	Warning("[SvenMod] CHooks::HookUserMessage: bad user message name (%s)\n", pszName);

	return DETOUR_INVALID_HANDLE;
}

DetourHandle_t CHooks::HookUserMessage(usermsg_t *pUserMsg, UserMsgHookFn pfnUserMsgHook, UserMsgHookFn *ppfnOriginalUserMsgHook)
{
	usermsg_t **ppUserMsg = &pUserMsg;
	constexpr size_t index = offsetof(usermsg_t, function) / sizeof(void *);

	return DetoursAPI()->DetourVirtualFunction(ppUserMsg, index, pfnUserMsgHook, (void **)ppfnOriginalUserMsgHook);
}

bool CHooks::UnhookUserMessage(DetourHandle_t hUserMsgHook)
{
	return DetoursAPI()->RemoveDetour( hUserMsgHook );
}

//-----------------------------------------------------------------------------
// Console Command
//-----------------------------------------------------------------------------

DetourHandle_t CHooks::HookConsoleCommand(const char *pszName, CommandCallbackFn pfnCommandCallback, CommandCallbackFn *ppfnOriginalCommandCallback)
{
	cmd_t *pCmd = CVar()->FindCmd(pszName);

	if ( pCmd )
	{
		cmd_t **ppCmd = &pCmd;
		constexpr size_t index = offsetof(cmd_t, function) / sizeof(void *);

		return DetoursAPI()->DetourVirtualFunction(ppCmd, index, pfnCommandCallback, (void **)ppfnOriginalCommandCallback);
	}

	return DETOUR_INVALID_HANDLE;
}

DetourHandle_t CHooks::HookConsoleCommand(cmd_t *pCommand, CommandCallbackFn pfnCommandCallback, CommandCallbackFn *ppfnOriginalCommandCallback)
{
	cmd_t **ppCommand = &pCommand;
	constexpr size_t index = offsetof(cmd_t, function) / sizeof(void *);

	return DetoursAPI()->DetourVirtualFunction(pCommand, index, pfnCommandCallback, (void **)ppfnOriginalCommandCallback);
}

bool CHooks::UnhookConsoleCommand(DetourHandle_t hCommandCallback)
{
	return DetoursAPI()->RemoveDetour( hCommandCallback );
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CHooks g_Hooks;
IHooks *g_pHooks = (IHooks *)&g_Hooks;

IHooks *Hooks()
{
	return g_pHooks;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CHooks, IHooks, HOOKS_INTERFACE_VERSION, g_Hooks);