#ifndef IENGINEAPI_H
#define IENGINEAPI_H

#ifdef _WIN32
#pragma once
#endif

#include <interface.h>

#ifdef PLATFORM_LINUX
typedef void *HINSTANCE;
#endif

enum
{
	RUN_FAILED,
	RUN_RESTART,
	RUN_UNSUPPORTEDVIDEO
};

class IEngineAPI : public IBaseInterface
{
public:
	virtual int Run( HINSTANCE instance, char *basedir, const char *cmdline, char *szCommand, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory ) = 0;
};

#define VENGINE_LAUNCHER_API_VERSION "VENGINE_LAUNCHER_API_VERSION002"

#endif // IENGINEAPI_H
