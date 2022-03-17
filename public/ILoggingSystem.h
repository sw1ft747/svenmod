#ifndef ILOGGINGSYSTEM_H
#define ILOGGINGSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

class IClientPlugin;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define LogMsg(msg, ...) LoggingSystem()->Log_Msg(msg, __VA_ARGS__)
#define LogWarning(msg, ...) LoggingSystem()->Log_Warning(msg, __VA_ARGS__)
#define LogAssert(msg, ...) LoggingSystem()->Log_Assert(msg, __VA_ARGS__)
#define LogError(msg, ...) LoggingSystem()->Log_Error(msg, __VA_ARGS__)

#define PluginLogMsg(plugin, msg, ...) LoggingSystem()->Plugin_Log_Msg(plugin, msg, __VA_ARGS__)
#define PluginLogWarning(plugin, msg, ...) LoggingSystem()->Plugin_Log_Warning(plugin, msg, __VA_ARGS__)
#define PluginLogAssert(plugin, msg, ...) LoggingSystem()->Plugin_Log_Assert(plugin, msg, __VA_ARGS__)
#define PluginLogError(plugin, msg, ...) LoggingSystem()->Plugin_Log_Error(plugin, msg, __VA_ARGS__)

//-----------------------------------------------------------------------------
// Simple logging interface, logs everything in file 'svenmod.log'
// ToDo: must be enhanced
//-----------------------------------------------------------------------------

abstract_class ILoggingSystem
{
public:
	virtual ~ILoggingSystem() {}

	virtual bool Init() = 0;

	virtual void Shutdown() = 0;
	
	virtual bool CanLog() = 0;

	virtual bool IsEnabled() = 0;

	virtual void Log_Msg(const char *pszMessageFormat, ...) = 0;

	virtual void Log_Warning(const char *pszMessageFormat, ...) = 0;
	
	virtual void Log_Assert(const char *pszMessageFormat, ...) = 0;

	virtual void Log_Error(const char *pszMessageFormat, ...) = 0;

	//-----------------------------------------------------------------------------
	// Purpose: log with the plugin's tag
	//-----------------------------------------------------------------------------

	virtual void Plugin_Log_Msg(IClientPlugin *pPlugin, const char *pszMessageFormat, ...) = 0;

	virtual void Plugin_Log_Warning(IClientPlugin *pPlugin, const char *pszMessageFormat, ...) = 0;
	
	virtual void Plugin_Log_Assert(IClientPlugin *pPlugin, const char *pszMessageFormat, ...) = 0;

	virtual void Plugin_Log_Error(IClientPlugin *pPlugin, const char *pszMessageFormat, ...) = 0;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define LOGGINGSYSTEM_INTERFACE_VERSION "LoggingSystem001"

extern ILoggingSystem *g_pLoggingSystem;
PLATFORM_INTERFACE ILoggingSystem *LoggingSystem();

#endif // ILOGGINGSYSTEM_H