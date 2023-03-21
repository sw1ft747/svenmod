#ifndef SVENMOD_H
#define SVENMOD_H

#ifdef _WIN32
#pragma once
#endif

#include <ISvenMod.h>
#include <ISvenModAPI.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

extern double *g_pRealtime;
extern double *g_pClientTime;
extern double *g_pFrametime;

extern const char *g_pszBaseDirectory;

extern client_version_t g_ClientVersion;
extern int g_iEngineBuild;
extern int g_iProtocolVersion;

extern int *g_pClientState;

extern modules_t g_Modules;

//-----------------------------------------------------------------------------
// Main class
//-----------------------------------------------------------------------------

class CSvenMod : public ISvenMod
{
	friend int __cdecl HOOKED_Sys_InitGame(CreateInterfaceFn, const char *, void *, int);
	friend void __cdecl HOOKED_LoadClientDLL(char *);

public:
	// ISvenMod interface
	bool Init(ICommandLine *pCommandLine, IFileSystem *pFileSystem, IRegistry *pRegistry) override;
	void Shutdown() override;

private:
	// Callbacks
	void InitSystems();
	bool StartSystems();

	// Utilities
	void CollectHardwareModule();
	void CollectModules();

	bool FindSignatures();
	bool AttachDetours();

	void CheckClientVersion();

private:
	void *m_pfnLoadClientDLL;
	void *m_pfnClientDLL_Init;
	void *m_pfnSys_InitGame;
	void *m_pfnHost_FilterTime;
	void *m_pfnHost_Shutdown;
	void *m_pfnEngineStudioInit;
	void *m_pfnVideoMode_Create;
	void *m_pfn__MsgFunc_ServerVer;

	DetourHandle_t m_hLoadClientDLL;
	DetourHandle_t m_hSys_InitGame;
	DetourHandle_t m_hHost_FilterTime;
	DetourHandle_t m_hHost_Shutdown;
};

#endif // SVENMOD_H