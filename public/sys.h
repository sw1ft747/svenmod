#ifndef SYSTEM_UTILS_H	
#define SYSTEM_UTILS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "interface.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
typedef void *HMODULE;
#endif

//-----------------------------------------------------------------------------
// Purpose: Terminates the process
//-----------------------------------------------------------------------------

void Sys_TerminateProcess(int nCode);

//-----------------------------------------------------------------------------
// Purpose: Shows an error message box and terminates the process
//-----------------------------------------------------------------------------

void Sys_Error(const char *pszMessageFormat, ...);

//-----------------------------------------------------------------------------
// Purpose: Shows an error message box
//-----------------------------------------------------------------------------

void Sys_ErrorMessage(const char *pszMessageFormat, ...);

//-----------------------------------------------------------------------------
// Purpose: Gets name of the current process
//-----------------------------------------------------------------------------

bool Sys_GetExecutableName(char *pszExecName, int nSize);

//-----------------------------------------------------------------------------
// Purpose: Gets pointer to the extension of the file, otherwise returns NULL
//-----------------------------------------------------------------------------

const char *Sys_GetFileExtension(const char *pszFilePath);

//-----------------------------------------------------------------------------
// Purpose: Gets full path of the current process
//-----------------------------------------------------------------------------

char *Sys_GetLongPathName();

//-----------------------------------------------------------------------------
// Purpose: Returns a module's handle
//-----------------------------------------------------------------------------

HMODULE Sys_GetModuleHandle(const char *pszModuleName);

//-----------------------------------------------------------------------------
// Purpose: Returns a pointer to a function, given a module
//-----------------------------------------------------------------------------

void *Sys_GetProcAddress(HMODULE hModule, const char *pszProcName);

//-----------------------------------------------------------------------------
// Purpose: Loads a DLL/component from disk and returns a handle to it
//-----------------------------------------------------------------------------

HMODULE Sys_LoadModule(const char *pszModuleName);

//-----------------------------------------------------------------------------
// Purpose: Unloads a DLL/component from
//-----------------------------------------------------------------------------

void Sys_UnloadModule(HMODULE hModule);

//-----------------------------------------------------------------------------
// Purpose: Returns CreateInterface factory from the given module
//-----------------------------------------------------------------------------

CreateInterfaceFn Sys_GetFactory(HMODULE hModule);

#endif // SYSTEM_UTILS_H