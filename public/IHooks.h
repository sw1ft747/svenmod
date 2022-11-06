#ifndef IHOOKS_H
#define IHOOKS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "sys.h"

#include "IDetoursAPI.h"

#include "hl_sdk/common/cvardef.h"
#include "hl_sdk/common/usermsg.h"
#include "hl_sdk/common/netmsg.h"
#include "hl_sdk/common/event.h"

class IClientHooks;

typedef void (*CvarChangeHookFn)(cvar_t *pCvar, const char *pszOldValue, float flOldValue);

//-----------------------------------------------------------------------------
// Purpose: interface to hook various game functions, structures, etc.
//-----------------------------------------------------------------------------

abstract_class IHooks
{
public:
	virtual					~IHooks() {}

	virtual bool			RegisterClientHooks( IClientHooks *pClientHooks ) = 0;
	virtual bool			RegisterClientPostHooks( IClientHooks *pClientHooks ) = 0;

	virtual bool			UnregisterClientHooks( IClientHooks *pClientHooks ) = 0;
	virtual bool			UnregisterClientPostHooks( IClientHooks *pClientHooks ) = 0;

	virtual bool			HookCvarChange( cvar_t *pCvar, CvarChangeHookFn pfnCvarChangeHook ) = 0;
	virtual bool			UnhookCvarChange( cvar_t *pCvar, CvarChangeHookFn pfnCvarChangeHook ) = 0;

	virtual DetourHandle_t	HookNetworkMessage( int iType, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook ) = 0;
	virtual DetourHandle_t	HookNetworkMessage( const char *pszName, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook ) = 0;
	virtual DetourHandle_t	HookNetworkMessage( netmsg_t *pNetMsg, NetMsgHookFn pfnNetMsgHook, NetMsgHookFn *ppfnOriginalNetMsgHook ) = 0;
	virtual bool			UnhookNetworkMessage( DetourHandle_t hNetMsgHook ) = 0;
	
	virtual DetourHandle_t	HookUserMessage( const char *pszName, UserMsgHookFn pfnUserMsgHook, UserMsgHookFn *ppfnOriginalUserMsgHook ) = 0;
	virtual DetourHandle_t	HookUserMessage( usermsg_t *pUserMsg, UserMsgHookFn pfnUserMsgHook, UserMsgHookFn *ppfnOriginalUserMsgHook ) = 0;
	virtual bool			UnhookUserMessage( DetourHandle_t hUserMsgHook ) = 0;
	
	virtual DetourHandle_t	HookEvent( const char *pszName, EventHookFn pfnEventHook, EventHookFn *ppfnOriginalEventHook ) = 0;
	virtual DetourHandle_t	HookEvent( event_t *pEventHook, EventHookFn pfnEventHook, EventHookFn *ppfnOriginalEventHook ) = 0;
	virtual bool			UnhookEvent( DetourHandle_t hEventHook ) = 0;

	virtual DetourHandle_t	HookConsoleCommand( const char *pszName, CommandCallbackFn pfnCommandCallback, CommandCallbackFn *ppfnOriginalCommandCallback ) = 0;
	virtual DetourHandle_t	HookConsoleCommand( cmd_t *pCommand, CommandCallbackFn pfnCommandCallback, CommandCallbackFn *ppfnOriginalCommandCallback ) = 0;
	virtual bool			UnhookConsoleCommand( DetourHandle_t hCommandCallback ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

#define HOOKS_INTERFACE_VERSION "Hooks003"

extern IHooks *g_pHooks;
PLATFORM_INTERFACE IHooks *Hooks();

#endif // IHOOKS_H
