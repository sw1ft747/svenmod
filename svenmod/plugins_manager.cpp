#pragma warning(disable : 4018)

#include "plugins_manager.h"
#include "plugins_loader.h"

#include <IPluginHelpers.h>
#include <dbg.h>

#define FOR_EACH_PLUGIN(iterName) for (int iterName = 0; iterName < m_Plugins.size(); iterName++)

bool g_bAutoPauseDetours = false;

CPluginsManager g_PluginsManager;
static plugin_info_s s_PluginInfo;

//-----------------------------------------------------------------------------
// CPluginHelpers
//-----------------------------------------------------------------------------

class CPluginHelpers : public IPluginHelpers
{
public:
	virtual int PluginsCount(void);

	// Returns plugin's index, otherwise -1
	virtual int FindPlugin(const char *pszPluginName);

	virtual bool IsPluginRunning(int index);

	// Query information about a plugin
	virtual plugin_info_s *QueryPluginInfo(int index);
	virtual plugin_info_s *QueryPluginInfo(const char *pszPluginName);
};

int CPluginHelpers::PluginsCount(void)
{
	return g_PluginsManager.m_Plugins.size();
}

int CPluginHelpers::FindPlugin(const char *pszPluginName)
{
	for (int i = 0; i < g_PluginsManager.m_Plugins.size(); i++)
	{
		CPlugin *pPlugin = g_PluginsManager.m_Plugins[i];

		if ( !strcmp( pszPluginName, pPlugin->GetCallback()->GetName() ) )
		{
			return i;
		}
	}

	return -1;
}

bool CPluginHelpers::IsPluginRunning(int index)
{
	CPlugin *pPlugin = g_PluginsManager.FindPluginByIndex(index);

	if (pPlugin)
		return (pPlugin->GetStatus() == PLUGIN_RUNNING);

	return false;
}

plugin_info_s *CPluginHelpers::QueryPluginInfo(int index)
{
	CPlugin *pPlugin = g_PluginsManager.FindPluginByIndex(index);

	if (pPlugin)
	{
		s_PluginInfo.name = pPlugin->GetCallback()->GetName();
		s_PluginInfo.author = pPlugin->GetCallback()->GetAuthor();
		s_PluginInfo.version = pPlugin->GetCallback()->GetVersion();
		s_PluginInfo.description = pPlugin->GetCallback()->GetDescription();
		s_PluginInfo.url = pPlugin->GetCallback()->GetURL();
		s_PluginInfo.compile_date = pPlugin->GetCallback()->GetDate();
		s_PluginInfo.log_tag = pPlugin->GetCallback()->GetLogTag();
		s_PluginInfo.iface_version = pPlugin->GetInterfaceVersion();
		s_PluginInfo.api_version = pPlugin->GetCallback()->GetAPIVersion();
		s_PluginInfo.running = (pPlugin->GetStatus() == PLUGIN_RUNNING);

		return &s_PluginInfo;
	}

	return NULL;
}

plugin_info_s *CPluginHelpers::QueryPluginInfo(const char *pszPluginName)
{
	return QueryPluginInfo( FindPlugin(pszPluginName) );
}

CPluginHelpers g_PluginHelpers;
IPluginHelpers *g_pPluginHelpers = (IPluginHelpers *)&g_PluginHelpers;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CPluginHelpers, IPluginHelpers, PLUGIN_HELPERS_INTERFACE_VERSION, g_PluginHelpers);

//-----------------------------------------------------------------------------
// CPlugin
//-----------------------------------------------------------------------------

CPlugin::CPlugin()
{
	m_Status = PLUGIN_RUNNING;

	m_hPluginModule = NULL;
	m_pPlugin = NULL;
	m_iInterfaceVersion = -1;
}

CPlugin::~CPlugin()
{
}

bool CPlugin::Load(const char *pszFileName, bool bGlobalLoad)
{
	int iInterfaceVersion = 1;
	static char szPluginPath[1024];

	snprintf(szPluginPath, sizeof(szPluginPath), "svenmod/plugins/%s", pszFileName);

	m_hPluginModule = Sys_LoadModule(szPluginPath);

	if ( !m_hPluginModule )
	{
	#ifdef PLATFORM_WINDOWS
		Warning("[SvenMod] Unable to load plugin \"../svenmod/plugins/%s%s\"\n", pszFileName, (Sys_GetFileExtension(pszFileName) == NULL ? ".dll" : ""));
		LogWarning("Unable to load plugin \"../svenmod/plugins/%s%s\".\n", pszFileName, (Sys_GetFileExtension(pszFileName) == NULL ? ".dll" : ""));
	#else
		Warning("[SvenMod] Unable to load plugin \"../svenmod/plugins/%s%s\"\n", pszFileName, (Sys_GetFileExtension(pszFileName) == NULL ? ".so" : ""));
		LogWarning("Unable to load plugin \"../svenmod/plugins/%s%s\".\n", pszFileName, (Sys_GetFileExtension(pszFileName) == NULL ? ".so" : ""));
	#endif

		return false;
	}

	CreateInterfaceFn pfnPluginFactory = Sys_GetFactory(m_hPluginModule);

	if ( pfnPluginFactory )
	{
		m_iInterfaceVersion = 1;
		m_pPlugin = reinterpret_cast<IClientPlugin *>(pfnPluginFactory(CLIENT_PLUGIN_INTERFACE_VERSION, NULL));

		if ( !m_pPlugin )
		{
			Warning("[SvenMod] Could not get \"IClientPlugin\" interface from the plugin \"%s\"\n", pszFileName);
			Warning("[SvenMod] The plugin \"%s\" may have a newer version of the interface. In this case, get updated version of SvenMod here: https://github.com/sw1ft747/svenmod\n", pszFileName);

			LogWarning("Could not get \"IClientPlugin\" interface from the plugin \"%s\".\n", pszFileName);
			LogWarning("[SvenMod] The plugin \"%s\" may have a newer version of the interface. In this case, get updated version of SvenMod here: https://github.com/sw1ft747/svenmod\n", pszFileName);

			Sys_UnloadModule(m_hPluginModule);
			return false;
		}

		api_version_s APIver = m_pPlugin->GetAPIVersion();

		if (APIver.major_version != SVENMOD_API_MAJOR_VERSION)
		{
			Sys_ErrorMessage("[SvenMod] API version of the plugin \"%s\" differs with SvenMod's API\n\nAPI version of SvenMod: %s\nAPI version of the plugin: %d.%d\n\nCheck \"svenmod.log\" for details",
							 m_pPlugin->GetName(),
							 SVENMOD_API_VERSION_STRING,
							 APIver.major_version,
							 APIver.minor_version);

			if (APIver.major_version > SVENMOD_API_MAJOR_VERSION)
			{
				LogError("Your version of SvenMod is outdated, update it here: https://github.com/sw1ft747/svenmod\n");

				Sys_Error("[SvenMod] SvenMod uses outdated version of API\n\nUpdate SvenMod here: https://github.com/sw1ft747/svenmod");
			}
			else
			{
				LogError("Plugin \"%s\" uses outdated version of API, update it here: %s\n", m_pPlugin->GetName(), m_pPlugin->GetURL());

				Sys_Error("[SvenMod] Plugin \"%s\" uses outdated version of API\n\nDownload newer version of the plugin here: %s\n\nOtherwise, ask the author (%s) to update it",
								 m_pPlugin->GetName(),
								 m_pPlugin->GetURL(),
								 m_pPlugin->GetAuthor());
			}

			Sys_UnloadModule(m_hPluginModule);
			return false;
		}

		if (APIver.minor_version != SVENMOD_API_MINOR_VERSION)
		{
			PluginLogWarning(m_pPlugin, "API version of the plugin differs with SvenMod's API.");
		}

		if ( !m_pPlugin->Load( Sys_GetFactoryThis(), SvenModAPI(), g_pPluginHelpers ) )
		{
			Warning("[SvenMod] Failed to load plugin \"%s\"\n", pszFileName);
			LogWarning("Failed to load plugin \"%s\".\n", pszFileName);

			Sys_UnloadModule(m_hPluginModule);
			return false;
		}

		if ( !bGlobalLoad )
		{
			m_pPlugin->PostLoad(false);
		}

		return true;
	}
	else
	{
		Warning("[SvenMod] Could not get \"CreateInterface\" factory from plugin \"%s\"\n", pszFileName);
		LogWarning("Could not get \"CreateInterface\" factory from plugin \"%s\".\n", pszFileName);

		Sys_UnloadModule(m_hPluginModule);
	}

	return false;
}

void CPlugin::Unload()
{
	m_pPlugin->Unload();
	Sys_UnloadModule(m_hPluginModule);
}

bool CPlugin::Pause()
{
	if (m_Status == PLUGIN_RUNNING)
	{
		if ( m_pPlugin->Pause() )
		{
			m_Status = PLUGIN_PAUSED;
			return true;
		}

		return false;
	}

	return false;
}

bool CPlugin::Unpause()
{
	if (m_Status == PLUGIN_PAUSED)
	{
		m_pPlugin->Unpause();
		m_Status = PLUGIN_RUNNING;

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// CPluginsManager
//-----------------------------------------------------------------------------

CPluginsManager::CPluginsManager()
{
}

CPluginsManager::~CPluginsManager()
{
	UnloadPlugins();
}

CPlugin *CPluginsManager::FindPluginByIndex(int index)
{
	if (index >= 0 && index < m_Plugins.size())
	{
		return m_Plugins[index];
	}

	return NULL;
}

int CPluginsManager::PluginsCount()
{
	return m_Plugins.size();
}

void CPluginsManager::LoadPlugins()
{
	Assert( m_Plugins.size() == 0 );

	PLUGIN_LOAD_RESULT loadResult;
	plugins_s *pPluginsBase = g_PluginsLoader.LoadFromFile("svenmod/plugins.txt", &loadResult);

	if (loadResult == LOAD_RESULT_MISSING_FILE)
	{
		LogWarning("Missing file \"../svenmod/plugins.txt\" to load plugins.\n");
	}
	else if (loadResult == LOAD_RESULT_FAILED)
	{
		Warning("[SvenMod] Failed to parse plugin-list file \"../svenmod/plugins.txt\". Reason: %s (%d).\n", g_PluginsLoader.GetLastErrorMessage(), g_PluginsLoader.GetLastErrorLine());
		LogWarning("Failed to parse plugin-list file \"../svenmod/plugins.txt\". Reason: %s (%d).\n", g_PluginsLoader.GetLastErrorMessage(), g_PluginsLoader.GetLastErrorLine());
	}
	else
	{
		g_bAutoPauseDetours = true;

		for (plugins_s *pPlugin = pPluginsBase; pPlugin; pPlugin = pPlugin->next)
		{
			if (pPlugin->load)
			{
				LoadPlugin(pPlugin->name, true);
			}
		}

		g_PluginsLoader.FreePlugins(pPluginsBase);

		g_bAutoPauseDetours = false;
		DetoursAPI()->UnpauseAllDetours(); // Attach detours

		for (int i = 0; i < m_Plugins.size(); i++)
		{
			m_Plugins[i]->GetCallback()->PostLoad(true);
		}
	}
}

void CPluginsManager::UnloadPlugins()
{
	for (int i = m_Plugins.size() - 1; i >= 0; --i)
	{
		CPlugin *pPlugin = m_Plugins[i];

		LogMsg("Unloaded plugin \"%s\".\n", pPlugin->GetCallback()->GetName());

		pPlugin->Unload();

		delete pPlugin;
	}

	m_Plugins.clear();
}

bool CPluginsManager::LoadPlugin(const char *pszFileName, bool bGlobalLoad)
{
	LogMsg("Loading plugin \"%s\".\n", pszFileName);

	CPlugin *pPlugin = new CPlugin;

	if ( pPlugin->Load(pszFileName, bGlobalLoad) )
	{
		Msg("[SvenMod] Loaded plugin \"%s\"\n", pszFileName);
		LogMsg("Loaded plugin \"%s\".\n", pszFileName);

		m_Plugins.push_back(pPlugin);
		return true;
	}

	LogMsg("Failed to load plugin \"%s\".\n", pszFileName);

	delete pPlugin;
	return false;
}

bool CPluginsManager::UnloadPlugin(int index)
{
	LogMsg("Unloading plugin \"%d\".\n", index);

	if (index >= 0 && index < m_Plugins.size())
	{
		CPlugin *pPlugin = m_Plugins[index];

		Msg("[SvenMod] Unloaded plugin \"%s\"\n", pPlugin->GetCallback()->GetName());
		LogMsg("Unloaded plugin \"%s\" (%d).\n", pPlugin->GetCallback()->GetName(), index);

		pPlugin->Unload();

		m_Plugins.erase(m_Plugins.begin() + index);
		delete pPlugin;

		return true;
	}

	LogMsg("Failed to unload plugin \"%d\".\n", index);
	return false;
}

bool CPluginsManager::PausePlugin(int index)
{
	if (index >= 0 && index < m_Plugins.size())
	{
		bool bPaused = m_Plugins[index]->Pause();

		if (bPaused)
			Msg("[SvenMod] Plugin \"%s\" has been paused\n", m_Plugins[index]->GetCallback()->GetName());
		else
			Msg("[SvenMod] Unable to pause plugin \"%s\"\n", m_Plugins[index]->GetCallback()->GetName());

		return bPaused;
	}

	return false;
}

bool CPluginsManager::UnpausePlugin(int index)
{
	if (index >= 0 && index < m_Plugins.size())
	{
		bool bUnpaused = m_Plugins[index]->Unpause();

		if (bUnpaused)
			Msg("[SvenMod] Plugin \"%s\" has been unpaused\n", m_Plugins[index]->GetCallback()->GetName());
		else
			Msg("[SvenMod] Plugin \"%s\" is already running\n", m_Plugins[index]->GetCallback()->GetName());

		return bUnpaused;
	}

	return false;
}

bool CPluginsManager::PausePlugins()
{
	bool bPaused = false;

	FOR_EACH_PLUGIN(i)
	{
		if (m_Plugins[i]->GetStatus() == PLUGIN_RUNNING)
		{
			bPaused = PausePlugin(i) || bPaused;
		}
	}

	return bPaused;
}

bool CPluginsManager::UnpausePlugins()
{
	bool bUnpaused = false;

	for (int i = 0; i < m_Plugins.size(); i++)
	{
		if (m_Plugins[i]->GetStatus() == PLUGIN_PAUSED)
		{
			bUnpaused = UnpausePlugin(i) || bUnpaused;
		}
	}

	return bUnpaused;
}

void CPluginsManager::PrintPlugins()
{
	Msg("--------- Loaded Plugins ---------\n");

	FOR_EACH_PLUGIN(i)
	{
		Msg("%d: \"%s\"%s\n", i, m_Plugins[i]->GetCallback()->GetName(), (m_Plugins[i]->GetStatus() == PLUGIN_PAUSED ? " (disabled)" : ""));
	}

	Msg("----------------------------------\n");
}

void CPluginsManager::PrintPluginInfo(int index)
{
	if (index >= 0 && index < m_Plugins.size())
	{
		CPlugin *pPlugin = m_Plugins[index];
		auto APIver = pPlugin->GetCallback()->GetAPIVersion();

		Msg("----------- Plugin Info ----------\n");

		Msg("* Name:              %s\n", pPlugin->GetCallback()->GetName());
		Msg("* Author:            %s\n", pPlugin->GetCallback()->GetAuthor());
		Msg("* Version:           %s\n", pPlugin->GetCallback()->GetVersion());
		Msg("* Description:       %s\n", pPlugin->GetCallback()->GetDescription());
		Msg("* URL:               %s\n", pPlugin->GetCallback()->GetURL());
		Msg("* Compiled in:       %s\n", pPlugin->GetCallback()->GetDate());
		Msg("* Log Tag:           %s\n", pPlugin->GetCallback()->GetLogTag());
		Msg("* Interface Version: %d\n", pPlugin->GetInterfaceVersion());
		Msg("* API Version:       %d.%d\n", APIver.major_version, APIver.minor_version);
		Msg("* State:             %s\n", (pPlugin->GetStatus() == PLUGIN_RUNNING ? "running" : "disabled"));

		Msg("----------------------------------\n");
	}
}

//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

void CPluginsManager::Frame(int state, double frametime, bool bPostRunCmd)
{
	FOR_EACH_PLUGIN(i)
	{
		CPlugin *pPlugin = m_Plugins[i];

		if (pPlugin->GetStatus() == PLUGIN_RUNNING)
		{
			pPlugin->GetCallback()->GameFrame( static_cast<client_state_t>(state), frametime, bPostRunCmd );
		}
	}
}

void CPluginsManager::Draw(void)
{
	FOR_EACH_PLUGIN(i)
	{
		CPlugin *pPlugin = m_Plugins[i];

		if (pPlugin->GetStatus() == PLUGIN_RUNNING)
		{
			PLUGIN_RESULT result = pPlugin->GetCallback()->Draw();

			if (result >= PLUGIN_STOP)
				break;
		}
	}
}

void CPluginsManager::DrawHUD(float time, int intermission)
{
	FOR_EACH_PLUGIN(i)
	{
		CPlugin *pPlugin = m_Plugins[i];

		if (pPlugin->GetStatus() == PLUGIN_RUNNING)
		{
			PLUGIN_RESULT result = pPlugin->GetCallback()->DrawHUD( time, intermission );

			if (result >= PLUGIN_STOP)
				break;
		}
	}
}