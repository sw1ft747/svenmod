#ifndef CLIENT_HOOKS_H
#define CLIENT_HOOKS_H

#ifdef _WIN32
#pragma once
#endif

#include <vector>

#include <IClientHooks.h>
#include <IDetoursAPI.h>

class CClientHooksHandler
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
};

extern CClientHooksHandler g_ClientHooksHandler;

#endif // CLIENT_HOOKS_H