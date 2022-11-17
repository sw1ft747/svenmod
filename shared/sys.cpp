#include <sys.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <ISvenModAPI.h>

#ifdef PLATFORM_LINUX
#include <dlfcn.h> // dlopen,dlclose, et al
#include <libgen.h>
#include <unistd.h>
#include <limits.h>

#define GetProcAddress dlsym
#endif

//-----------------------------------------------------------------------------
// Purpose: Terminates the process
//-----------------------------------------------------------------------------

void Sys_TerminateProcess(int nCode)
{
#ifdef PLATFORM_WINDOWS
	TerminateProcess(GetCurrentProcess(), nCode);
#else
	exit(nCode);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Shows an error message box and terminates the process
//-----------------------------------------------------------------------------

void Sys_Error(const char *pszMessageFormat, ...)
{
	static char buffer[4096] = { 0 };

	va_list args;

	va_start(args, pszMessageFormat);
	vsnprintf(buffer, M_ARRAYSIZE(buffer), pszMessageFormat, args);
	va_end(args);

	buffer[M_ARRAYSIZE(buffer) - 1] = 0;

#ifdef PLATFORM_WINDOWS
	MessageBoxA(NULL, buffer, "Fatal Error", MB_ICONERROR);
#else
	printf("[MSG] %s\n", buffer);
#endif

	Sys_TerminateProcess(0);
}

//-----------------------------------------------------------------------------
// Purpose: Shows an error message box
//-----------------------------------------------------------------------------

void Sys_ErrorMessage(const char *pszMessageFormat, ...)
{
	static char buffer[4096] = { 0 };

	va_list args;

	va_start(args, pszMessageFormat);
	vsnprintf(buffer, M_ARRAYSIZE(buffer), pszMessageFormat, args);
	va_end(args);

	buffer[M_ARRAYSIZE(buffer) - 1] = 0;

#ifdef PLATFORM_WINDOWS
	MessageBoxA(NULL, buffer, "Fatal Error", MB_ICONERROR);
#else
	printf("[MSG] %s\n", buffer);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Gets name of the current process
//-----------------------------------------------------------------------------

bool Sys_GetExecutableName(char *pszExecName, int nSize)
{
#ifdef PLATFORM_WINDOWS
	return GetModuleFileNameA(GetModuleHandleA(NULL), pszExecName, nSize) != 0;
#elif defined(PLATFORM_LINUX)
	if (nSize > 0)
		pszExecName[0] = 0;

	return readlink("/proc/self/exe", pszExecName, nSize) != -1;
#endif

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Gets pointer to the extension of the file, otherwise returns NULL
//-----------------------------------------------------------------------------

const char *Sys_GetFileExtension(const char *pszFilePath)
{
	const char *pszExtension = NULL;

	while (*pszFilePath)
	{
		if (*pszFilePath == '.')
		{
			pszExtension = pszFilePath;
		}

		pszFilePath++;
	}

	if (pszExtension)
	{
		pszExtension++;

		if ( !*pszExtension )
			pszExtension = NULL;
	}

	return pszExtension;
}

//-----------------------------------------------------------------------------
// Purpose: Gets full path of the current process
//-----------------------------------------------------------------------------

char *Sys_GetLongPathName()
{
#ifdef PLATFORM_WINDOWS
	char szShortPath[MAX_PATH];
	static char szLongPath[MAX_PATH];
	char *pszPath = NULL;

	szShortPath[0] = 0;
	szLongPath[0] = 0;

	if (GetModuleFileNameA(NULL, szShortPath, sizeof(szShortPath)))
	{
		GetLongPathNameA(szShortPath, szLongPath, sizeof(szLongPath));
		pszPath = strrchr(szLongPath, '\\');

		if (pszPath[0])
			pszPath[1] = 0;

		size_t len = strlen(szLongPath);

		if (len > 0)
		{
			if (szLongPath[len - 1] == '\\')
				szLongPath[len - 1] = 0;
		}
	}

	return szLongPath;
#elif defined(PLATFORM_LINUX)
	static char szLongPath[PATH_MAX];
	
	szLongPath[0] = 0;
	
	if (readlink("/proc/self/exe", szLongPath, PATH_MAX) != -1)
	{
		char *pszPath = strrchr(szLongPath, '/');
		
		if (pszPath[0])
			pszPath[1] = 0;

		size_t len = strlen(szLongPath);

		if (len > 0)
		{
			if (szLongPath[len - 1] == '/')
				szLongPath[len - 1] = 0;
		}
	}

	return szLongPath;
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Returns a module's handle
//-----------------------------------------------------------------------------

HMODULE Sys_GetModuleHandle(const char *pszModuleName)
{
#ifdef PLATFORM_WINDOWS
	return GetModuleHandleA(pszModuleName);
#elif defined(PLATFORM_LINUX)
	void *handle = NULL;

	if (pszModuleName == NULL)
		return dlopen(NULL, RTLD_NOW);

	if ((handle = dlopen(pszModuleName, RTLD_NOW)) == NULL)
		return NULL;

	dlclose(handle);
	return reinterpret_cast<HMODULE>(handle);
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Returns a pointer to a function, given a module
//-----------------------------------------------------------------------------

void *Sys_GetProcAddress(HMODULE hModule, const char *pszProcName)
{
#ifdef PLATFORM_WINDOWS
	return GetProcAddress(hModule, pszProcName);
#elif defined(PLATFORM_LINUX)
	return dlsym(hModule, pszProcName);
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Loads a DLL/component from disk and returns a handle to it
//-----------------------------------------------------------------------------

HMODULE Sys_LoadModule(const char *pszModuleName)
{
#ifdef PLATFORM_WINDOWS
	HMODULE hDLL = LoadLibraryA(pszModuleName);
#elif defined(PLATFORM_LINUX)
	HMODULE hDLL = NULL;

	char szAbsoluteModuleName[1024] = { 0 };

	if (pszModuleName[0] != '/')
	{
		char szCwd[1024];
		char szAbsoluteModuleName[1024];

		//Prevent loading from garbage paths if the path is too large for the buffer
		if (!getcwd(szCwd, sizeof(szCwd)))
		{
			exit(-1);
		}

		if (szCwd[strlen(szCwd) - 1] == '/')
			szCwd[strlen(szCwd) - 1] = 0;

		snprintf(szAbsoluteModuleName, M_ARRAYSIZE(szAbsoluteModuleName), "%s/%s", szCwd, pszModuleName);

		hDLL = dlopen(szAbsoluteModuleName, RTLD_NOW);
	}
	else
	{
		snprintf(szAbsoluteModuleName, M_ARRAYSIZE(szAbsoluteModuleName), "%s", pszModuleName);
		hDLL = dlopen(pszModuleName, RTLD_NOW);
	}
#endif

	if (!hDLL)
	{
		char str[512];
#ifdef PLATFORM_WINDOWS
		snprintf(str, M_ARRAYSIZE(str), "%s.dll", pszModuleName);
		hDLL = LoadLibraryA(str);
#elif defined(PLATFORM_LINUX)
		printf("Sys_LoadModule Error: %s\n", dlerror());
		snprintf(str, M_ARRAYSIZE(str), "%s.so", szAbsoluteModuleName);
		hDLL = dlopen(str, RTLD_NOW);
#endif
	}

	return hDLL;
}

//-----------------------------------------------------------------------------
// Purpose: Unloads a DLL/component from
//-----------------------------------------------------------------------------

void Sys_UnloadModule(HMODULE hModule)
{
	if (!hModule)
		return;

#ifdef PLATFORM_WINDOWS
	FreeLibrary(hModule);
#elif defined(PLATFORM_LINUX)
	dlclose((void *)hModule);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Returns CreateInterface factory from the given module
//-----------------------------------------------------------------------------

CreateInterfaceFn Sys_GetFactory(HMODULE hModule)
{
	if (!hModule)
		return NULL;

	return reinterpret_cast<CreateInterfaceFn>(Sys_GetProcAddress(hModule, CREATEINTERFACE_PROCNAME));
}
