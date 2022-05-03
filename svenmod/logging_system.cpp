#include <ILoggingSystem.h>
#include <ICommandLine.h>
#include <IClientPlugin.h>

#include <interface.h>
#include <sys.h>

#include <stdio.h>
#include <mutex>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define LOGGING_FILE "svenmod.log"
#define MAX_LOGGING_MESSAGE_LENGTH 2048
#define SYSTEM_TIME_EXPAND(time) time.wMonth, time.wDay, time.wYear, time.wHour, time.wMinute, time.wSecond

//-----------------------------------------------------------------------------
// CLoggingSystem
//-----------------------------------------------------------------------------

class CLoggingSystem : public ILoggingSystem
{
public:
	CLoggingSystem();
	virtual ~CLoggingSystem();

	virtual bool Init();

	virtual void Shutdown();

	virtual bool CanLog();

	virtual bool IsEnabled();

	virtual void Log_Msg(const char *pszMessageFormat, ...);

	virtual void Log_Warning(const char *pszMessageFormat, ...);

	virtual void Log_Assert(const char *pszMessageFormat, ...);

	virtual void Log_Error(const char *pszMessageFormat, ...);

	virtual void Plugin_Log_Msg(IClientPlugin *pPlugin, const char *pszMessageFormat, ...);

	virtual void Plugin_Log_Warning(IClientPlugin *pPlugin, const char *pszMessageFormat, ...);

	virtual void Plugin_Log_Assert(IClientPlugin *pPlugin, const char *pszMessageFormat, ...);

	virtual void Plugin_Log_Error(IClientPlugin *pPlugin, const char *pszMessageFormat, ...);

private:
	bool LoggingBegin();
	void LoggingEnd();

	void GetSystemTime();

private:
	FILE *m_hLogFile;

	SYSTEMTIME m_SysTime;
	std::mutex m_hThreadMutex;

	bool m_bInitialized;
};

//-----------------------------------------------------------------------------
// CLoggingSystem implementation
//-----------------------------------------------------------------------------

CLoggingSystem::CLoggingSystem() : m_SysTime()
{
	m_hLogFile = NULL;
	m_bInitialized = false;
}

CLoggingSystem::~CLoggingSystem()
{
}

bool CLoggingSystem::Init()
{
	if ( m_bInitialized )
		return true;

	if ( CommandLine()->CheckParm("-nolog") == NULL )
	{
		m_hLogFile = fopen(LOGGING_FILE, "w");

		if ( !m_hLogFile )
		{
			Sys_ErrorMessage("[SvenMod] Failed to create a log file");
			return false;
		}

		m_bInitialized = true;
		fclose(m_hLogFile);

		Log_Msg("SvenMod log file session started.\n");
	}

	return true;
}

void CLoggingSystem::Shutdown()
{
	if (m_bInitialized)
	{
		Log_Msg("Log file closed.\n");
		m_bInitialized = false;
	}

	if (m_hLogFile)
	{
		fclose(m_hLogFile);
	}
}

bool CLoggingSystem::CanLog()
{
	return m_bInitialized;
}

bool CLoggingSystem::IsEnabled()
{
	return m_bInitialized;
}

//-----------------------------------------------------------------------------
// Logging methods
//-----------------------------------------------------------------------------

void CLoggingSystem::Log_Msg(const char *pszMessageFormat, ...)
{
	if ( !CanLog() )
		return;

	static char szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH] = { 0 };

	m_hThreadMutex.lock();

	if ( LoggingBegin() )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedLog, MAX_LOGGING_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH - 1] = 0;

		GetSystemTime();
		fprintf(m_hLogFile, "L %02d/%02d/%d - %02d:%02d:%02d: %s", SYSTEM_TIME_EXPAND(m_SysTime), szFormattedLog);
	}
	LoggingEnd();

	m_hThreadMutex.unlock();
}

void CLoggingSystem::Log_Warning(const char *pszMessageFormat, ...)
{
	if ( !CanLog() )
		return;

	static char szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH] = { 0 };

	m_hThreadMutex.lock();

	if ( LoggingBegin() )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedLog, MAX_LOGGING_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH - 1] = 0;

		GetSystemTime();
		fprintf(m_hLogFile, "L %02d/%02d/%d - %02d:%02d:%02d: [WARNING] %s", SYSTEM_TIME_EXPAND(m_SysTime), szFormattedLog);
	}
	LoggingEnd();

	m_hThreadMutex.unlock();
}

void CLoggingSystem::Log_Assert(const char *pszMessageFormat, ...)
{
	if ( !CanLog() )
		return;

	static char szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH] = { 0 };

	m_hThreadMutex.lock();

	if ( LoggingBegin() )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedLog, MAX_LOGGING_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH - 1] = 0;

		GetSystemTime();
		fprintf(m_hLogFile, "L %02d/%02d/%d - %02d:%02d:%02d: [ASSERTION FAILED] %s", SYSTEM_TIME_EXPAND(m_SysTime), szFormattedLog);
	}
	LoggingEnd();

	m_hThreadMutex.unlock();
}

void CLoggingSystem::Log_Error(const char *pszMessageFormat, ...)
{
	if ( !CanLog() )
		return;

	static char szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH] = { 0 };

	m_hThreadMutex.lock();

	if ( LoggingBegin() )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedLog, MAX_LOGGING_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH - 1] = 0;

		GetSystemTime();
		fprintf(m_hLogFile, "L %02d/%02d/%d - %02d:%02d:%02d: [ERROR] %s", SYSTEM_TIME_EXPAND(m_SysTime), szFormattedLog);
	}
	LoggingEnd();

	m_hThreadMutex.unlock();
}

//-----------------------------------------------------------------------------
// Logging with plugin's tag
//-----------------------------------------------------------------------------

void CLoggingSystem::Plugin_Log_Msg(IClientPlugin *pPlugin, const char *pszMessageFormat, ...)
{
	if ( !CanLog() )
		return;

	static char szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH] = { 0 };

	m_hThreadMutex.lock();

	if ( LoggingBegin() )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedLog, MAX_LOGGING_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH - 1] = 0;

		GetSystemTime();
		fprintf(m_hLogFile, "L %02d/%02d/%d - %02d:%02d:%02d: [%s] %s", SYSTEM_TIME_EXPAND(m_SysTime), pPlugin->GetLogTag(), szFormattedLog);
	}
	LoggingEnd();

	m_hThreadMutex.unlock();
}

void CLoggingSystem::Plugin_Log_Warning(IClientPlugin *pPlugin, const char *pszMessageFormat, ...)
{
	if ( !CanLog() )
		return;

	static char szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH] = { 0 };

	m_hThreadMutex.lock();

	if ( LoggingBegin() )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedLog, MAX_LOGGING_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH - 1] = 0;

		GetSystemTime();
		fprintf(m_hLogFile, "L %02d/%02d/%d - %02d:%02d:%02d: [WARNING] [%s] %s", SYSTEM_TIME_EXPAND(m_SysTime), pPlugin->GetLogTag(), szFormattedLog);
	}
	LoggingEnd();

	m_hThreadMutex.unlock();
}

void CLoggingSystem::Plugin_Log_Assert(IClientPlugin *pPlugin, const char *pszMessageFormat, ...)
{
	if ( !CanLog() )
		return;

	static char szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH] = { 0 };

	m_hThreadMutex.lock();

	if ( LoggingBegin() )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedLog, MAX_LOGGING_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH - 1] = 0;

		GetSystemTime();
		fprintf(m_hLogFile, "L %02d/%02d/%d - %02d:%02d:%02d: [ASSERTION FAILED] [%s] %s", SYSTEM_TIME_EXPAND(m_SysTime), pPlugin->GetLogTag(), szFormattedLog);
	}
	LoggingEnd();

	m_hThreadMutex.unlock();
}

void CLoggingSystem::Plugin_Log_Error(IClientPlugin *pPlugin, const char *pszMessageFormat, ...)
{
	if ( !CanLog() )
		return;

	static char szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH] = { 0 };

	m_hThreadMutex.lock();

	if ( LoggingBegin() )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedLog, MAX_LOGGING_MESSAGE_LENGTH, pszMessageFormat, args);
		va_end(args);

		szFormattedLog[MAX_LOGGING_MESSAGE_LENGTH - 1] = 0;

		GetSystemTime();
		fprintf(m_hLogFile, "L %02d/%02d/%d - %02d:%02d:%02d: [ERROR] [%s] %s", SYSTEM_TIME_EXPAND(m_SysTime), pPlugin->GetLogTag(), szFormattedLog);
	}
	LoggingEnd();

	m_hThreadMutex.unlock();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool CLoggingSystem::LoggingBegin()
{
	if ( m_bInitialized )
	{
		m_hLogFile = fopen(LOGGING_FILE, "a");

		if ( !m_hLogFile )
			return false;

		return true;
	}

	return false;
}

void CLoggingSystem::LoggingEnd()
{
	if ( m_hLogFile )
	{
		fclose(m_hLogFile);
		m_hLogFile = NULL;
	}
}

void CLoggingSystem::GetSystemTime()
{
	GetLocalTime(&m_SysTime);
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CLoggingSystem g_LoggingSystem;
ILoggingSystem *g_pLoggingSystem = &g_LoggingSystem;

ILoggingSystem *LoggingSystem()
{
	return g_pLoggingSystem;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CLoggingSystem, ILoggingSystem, LOGGINGSYSTEM_INTERFACE_VERSION, g_LoggingSystem);