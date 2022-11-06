#ifndef IPLUGINHELPERS_H
#define IPLUGINHELPERS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "api_version.h"

//-----------------------------------------------------------------------------
// Plugin's info
//-----------------------------------------------------------------------------

typedef struct plugin_info_s
{
	const char *name;
	const char *author;
	const char *version;
	const char *description;
	const char *url;

	const char *compile_date;
	const char *log_tag;

	int iface_version;
	api_version_t api_version;

	bool running;
} plugin_info_t;

//-----------------------------------------------------------------------------
// Purpose: helpers for loaded plugins
//-----------------------------------------------------------------------------

abstract_class IPluginHelpers
{
public:
	virtual					~IPluginHelpers() {}

	// Get current count of loaded plugins, both running and paused
	virtual int				PluginsCount( void ) = 0;

	// Returns plugin's valid index, otherwise -1
	virtual int				FindPlugin( const char *pszPluginName ) = 0;

	virtual bool			IsPluginRunning( int index ) = 0;

	// Query information about a plugin
	virtual plugin_info_t	*QueryPluginInfo( int index ) = 0;
	virtual plugin_info_t	*QueryPluginInfo( const char *pszPluginName ) = 0;
};

extern IPluginHelpers *g_pPluginHelpers;

#define PLUGIN_HELPERS_INTERFACE_VERSION "PluginHelpers001"

#endif // IPLUGINHELPERS_H