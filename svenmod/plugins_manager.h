#ifndef PLUGINS_MANAGER_H
#define PLUGINS_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include <IClientPlugin.h>
#include <sys.h>

#include <vector>

#include "keyvalues_custom_parser.h"

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

	bool LoadPluginsFile();

	inline bool PluginsLoaded() const { return m_bPluginsLoaded; }
	inline float GetLoadTime() const { return m_flLoadTime; }

	CPlugin *FindPluginByIndex(int index) const;
	int PluginsCount() const;

	void LoadPlugins();
	void LoadPluginsNow();
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
	// Callbacks
	void OnFirstClientdataReceived(client_data_t *pcldata, float flTime);
	void OnBeginLoading(void);
	void OnEndLoading(void);
	void OnDisconnect(void);
	void Frame(int state, double frametime, bool bPostRunCmd);
	void Draw(void);
	void DrawHUD(float time, int intermission);

private:
	bool m_bPluginsLoaded;
	float m_flLoadTime;

	std::vector<CPlugin *> m_Plugins;
	KeyValuesParser::KeyValues *m_pPluginsKV;
};

extern CPluginsManager g_PluginsManager;

#endif // PLUGINS_MANAGER_H