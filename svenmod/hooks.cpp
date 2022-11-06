#include <ICvar.h>
#include <dbg.h>

#include <hl_sdk/common/usermsg.h>
#include <hl_sdk/common/protocol.h>

#include "hooks.h"
#include "game_hooks.h"
#include "cvar.h"

extern usermsg_t **g_ppClientUserMsgs;
extern event_t *g_pEventHooks;
extern netmsg_t *g_pNetworkMessages;

//-----------------------------------------------------------------------------
// Cvar_DirectSet hook
//-----------------------------------------------------------------------------

DECLARE_HOOK(void, __cdecl, Cvar_DirectSet, cvar_t *pCvar, const char *pszValue);

DECLARE_FUNC(void, __cdecl, HOOKED_Cvar_DirectSet, cvar_t *pCvar, const char *pszValue)
{
	const char *pszOldValue = strdup( pCvar->string );
	float flOldValue = pCvar->value;

	ORIG_Cvar_DirectSet( pCvar, pszValue );

	g_Hooks.CallCvarChangeChain( pCvar, pszOldValue, flOldValue );

	free( (void *)pszOldValue );
}

//-----------------------------------------------------------------------------
// CHooks implementation
//-----------------------------------------------------------------------------

CHooks::CHooks() : m_hCvarChangeHooks(127)
{
}

void CHooks::Init()
{
	m_hCvar_DirectSet = DetoursAPI()->DetourFunction( g_CVar.GetCvar_DirectSet(), HOOKED_Cvar_DirectSet, GET_FUNC_PTR(ORIG_Cvar_DirectSet) );

	if ( m_hCvar_DirectSet == DETOUR_INVALID_HANDLE )
	{
		Sys_Error("[SvenMod] Failed to hook function \"Cvar_DirectSet\"");
	}
}

void CHooks::Shutdown()
{
	DetoursAPI()->RemoveDetour( m_hCvar_DirectSet );

	for (int i = 0; i < m_hCvarChangeHooks.Count(); i++)
	{
		HashTableIterator_t it = m_hCvarChangeHooks.First(i);

		while ( m_hCvarChangeHooks.IsValidIterator(it) )
		{
			std::vector<CvarChangeHookFn> &hooks = m_hCvarChangeHooks.ValueAt(i, it);

			hooks.clear();

			it = m_hCvarChangeHooks.Next(i, it);
		}
	}

	m_hCvarChangeHooks.Purge();
}

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
// Cvar
//-----------------------------------------------------------------------------

void CHooks::CallCvarChangeChain(cvar_t *pCvar, const char *pszOldValue, float flOldValue)
{
	std::vector<CvarChangeHookFn> *pCvarChangeHooks = m_hCvarChangeHooks.Find(pCvar);

	if ( pCvarChangeHooks != NULL )
	{
		for (size_t i = 0; i < pCvarChangeHooks->size(); i++)
		{
			pCvarChangeHooks->at(i)( pCvar, pszOldValue, flOldValue );
		}
	}
}

bool CHooks::HookCvarChange(cvar_t *pCvar, CvarChangeHookFn pfnCvarChangeHook)
{
	if ( pCvar != NULL && pfnCvarChangeHook != NULL )
	{
		std::vector<CvarChangeHookFn> *pCvarChangeHooks = m_hCvarChangeHooks.Find(pCvar);

		if ( pCvarChangeHooks != NULL )
		{
			if ( std::find( pCvarChangeHooks->begin(), pCvarChangeHooks->end(), pfnCvarChangeHook ) != pCvarChangeHooks->end() )
			{
				Warning("[SvenMod] CHooks::HookCvarChange: cvar change hook (0x%X) for cvar \"%s\" (0x%X) is already registered\n", pfnCvarChangeHook, pCvar->name, pCvar);
			}
			else
			{
				pCvarChangeHooks->push_back( pfnCvarChangeHook );
			}
		}
		else
		{
			if ( m_hCvarChangeHooks.Insert( pCvar, std::vector<CvarChangeHookFn>() ) )
			{
				m_hCvarChangeHooks.Find(pCvar)->push_back( pfnCvarChangeHook );
			}
			else
			{
				Warning("[SvenMod] CHooks::HookCvarChange: failed to register cvar change hook (0x%X) for cvar \"%s\" (0x%X)\n", pfnCvarChangeHook, pCvar->name, pCvar);
			}
		}
	}

	return false;
}

bool CHooks::UnhookCvarChange(cvar_t *pCvar, CvarChangeHookFn pfnCvarChangeHook)
{
	if ( pCvar != NULL && pfnCvarChangeHook != NULL )
	{
		std::vector<CvarChangeHookFn> *pCvarChangeHooks = m_hCvarChangeHooks.Find(pCvar);

		if ( pCvarChangeHooks != NULL )
		{
			std::vector<CvarChangeHookFn>::iterator it;

			if ( (it = std::find( pCvarChangeHooks->begin(), pCvarChangeHooks->end(), pfnCvarChangeHook )) != pCvarChangeHooks->end() )
			{
				pCvarChangeHooks->erase( it );

				if ( pCvarChangeHooks->size() == 0 )
				{
					m_hCvarChangeHooks.Remove( pCvar );
				}
			}
			else
			{
				Warning("[SvenMod] CHooks::HookCvarChange: cvar change hook (0x%X) for cvar \"%s\" (0x%X) is not registered\n", pfnCvarChangeHook, pCvar->name, pCvar);
			}
		}
		else
		{
			Warning("[SvenMod] CHooks::HookCvarChange: cvar change hook (0x%X) for cvar \"%s\" (0x%X) is not registered\n", pfnCvarChangeHook, pCvar->name, pCvar);
		}
	}

	return false;
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
// Event Hook
//-----------------------------------------------------------------------------

DetourHandle_t CHooks::HookEvent(const char *pszName, EventHookFn pfnEventHook, EventHookFn *ppfnOriginalEventHook)
{
	event_t *pEventHook = g_pEventHooks;

	while (pEventHook)
	{
		if (pEventHook->name)
		{
			if ( !stricmp(pszName, pEventHook->name) )
			{
				event_t **ppEventHook = &pEventHook;
				constexpr size_t index = offsetof(event_t, function) / sizeof(void *);

				return DetoursAPI()->DetourVirtualFunction(ppEventHook, index, pfnEventHook, (void **)ppfnOriginalEventHook);
			}
		}

		pEventHook = pEventHook->next;
	}

	Warning("[SvenMod] CHooks::HookEvent: bad event hook name (%s)\n", pszName);

	return DETOUR_INVALID_HANDLE;
}

DetourHandle_t CHooks::HookEvent(event_t *pEventHook, EventHookFn pfnEventHook, EventHookFn *ppfnOriginalEventHook)
{
	event_t **ppEventHook = &pEventHook;
	constexpr size_t index = offsetof(event_t, function) / sizeof(void *);

	return DetoursAPI()->DetourVirtualFunction(ppEventHook, index, pfnEventHook, (void **)ppfnOriginalEventHook);
}

bool CHooks::UnhookEvent(DetourHandle_t hEventHook)
{
	return DetoursAPI()->RemoveDetour( hEventHook );
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