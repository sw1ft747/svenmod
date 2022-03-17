#ifndef PLUGINS_MANAGER_H
#define PLUGINS_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include <IClientPlugin.h>
#include <sys.h>

#include <vector>

typedef enum
{
	PLUGIN_RUNNING = 0,
	PLUGIN_PAUSED
} PLUGIN_STATE;

//-----------------------------------------------------------------------------
// Purpose: a single plugin
//-----------------------------------------------------------------------------

class CPlugin
{
public:
	CPlugin();
	~CPlugin();

	bool Load(const char *pszFileName, bool bGlobalLoad);
	void Unload();

	bool Pause();
	bool Unpause();

public:
	IClientPlugin *GetCallback() const;
	PLUGIN_STATE GetStatus() const;
	int GetInterfaceVersion() const;

private:
	HMODULE m_hPluginModule;
	IClientPlugin *m_pPlugin;
	PLUGIN_STATE m_Status;

	int m_iInterfaceVersion;
};

inline IClientPlugin *CPlugin::GetCallback() const
{
	return m_pPlugin;
}

inline PLUGIN_STATE CPlugin::GetStatus() const
{
	return m_Status;
}

inline int CPlugin::GetInterfaceVersion() const
{
	return m_iInterfaceVersion;
}

//-----------------------------------------------------------------------------
// Purpose: manager to handle client-side plugins
//-----------------------------------------------------------------------------

class CPluginsManager
{
	friend class CPluginHelpers;
	friend class CClientHooksHandler;

public:
	CPluginsManager();
	~CPluginsManager();

	CPlugin *FindPluginByIndex(int index);
	int PluginsCount();

	void LoadPlugins();
	void UnloadPlugins();

	bool LoadPlugin(const char *pszFileName, bool bGlobalLoad);
	bool UnloadPlugin(int index);
	
	bool PausePlugin(int index);
	bool UnpausePlugin(int index);
	
	bool PausePlugins();
	bool UnpausePlugins();

	void PrintPlugins();
	void PrintPluginInfo(int index);

public:
	void Frame(int state, double frametime, bool bPostRunCmd);
	void Draw(void);
	void DrawHUD(float time, int intermission);

private:
	std::vector<CPlugin *> m_Plugins;
};

extern CPluginsManager g_PluginsManager;

#endif // PLUGINS_MANAGER_H