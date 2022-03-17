#ifndef PLUGINS_LOADER_H
#define PLUGINS_LOADER_H

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef enum
{
	LOAD_RESULT_OK = 0,
	LOAD_RESULT_FAILED,
	LOAD_RESULT_MISSING_FILE
} PLUGIN_LOAD_RESULT;

struct plugins_s
{
	struct plugins_s *next;

	const char *name;
	int load;
};

//-----------------------------------------------------------------------------
// Purpose: load plugins from a file
//-----------------------------------------------------------------------------

class CPluginsLoader
{
public:
	plugins_s *LoadFromFile(const char *pszFileName, PLUGIN_LOAD_RESULT *pResult);
	void FreePlugins(plugins_s *pPluginsBase);

public:
	int GetLastErrorLine();
	const char *GetLastErrorMessage();
};

extern CPluginsLoader g_PluginsLoader;

#endif // PLUGINS_LOADER_H