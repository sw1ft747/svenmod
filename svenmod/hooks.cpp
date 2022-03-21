#include <IHooks.h>
#include <ICvar.h>
#include <IDetoursAPI.h>

#include <hl_sdk/common/usermsg.h>

#include "game_hooks.h"

extern usermsg_t **g_ppClientUserMsgs;

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