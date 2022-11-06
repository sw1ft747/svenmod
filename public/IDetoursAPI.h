#ifndef IDETOURS_API_H
#define IDETOURS_API_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "sys.h"

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define FUNC_SIGNATURE(retType, callConv, typeName, ...) typedef retType(callConv *typeName)(__VA_ARGS__)

#define DECLARE_FUNC_PTR(retType, callConv, funcName, ...) typedef retType(callConv *(funcName##Fn))(__VA_ARGS__); funcName##Fn funcName = 0
#define DECLARE_FUNC(retType, callConv, funcName, ...) retType callConv funcName(__VA_ARGS__)

#define DECLARE_CLASS_FUNC(retType, funcName, thisPtr, ...) retType __fastcall funcName(thisPtr, void *edx, __VA_ARGS__)

#define GET_FUNC_PTR(funcName) (void **)&funcName

#define DECLARE_HOOK(retType, callConv, funcName, ...) \
	typedef retType(callConv *funcName##Fn)(__VA_ARGS__); \
	static retType callConv HOOKED_##funcName(__VA_ARGS__); \
	funcName##Fn ORIG_##funcName = 0

#define DECLARE_CLASS_HOOK(retType, funcName, thisPtr, ...) \
	typedef retType(__thiscall *funcName##Fn)(thisPtr, __VA_ARGS__); \
	static retType __fastcall HOOKED_##funcName(thisPtr, void *edx, __VA_ARGS__); \
	funcName##Fn ORIG_##funcName = 0

//-----------------------------------------------------------------------------
// Detour handle
//-----------------------------------------------------------------------------

typedef int DetourHandle_t;
#define DETOUR_INVALID_HANDLE (DetourHandle_t)(-1)

//-----------------------------------------------------------------------------
// IDetoursAPI interface
//-----------------------------------------------------------------------------

abstract_class IDetoursAPI
{
public:
	virtual					~IDetoursAPI() {}

	virtual void			Init() = 0;

	//-----------------------------------------------------------------------------
	// Hook function
	// 
	// @param ppOriginalFunction - pointer to pointer to original function to call
	// @param iDisasmMinBytes - minimum bytes to steal from original function, it
	// can be helpful if detour's trampoline crashes (i.e. jump-short opcode causes it)
	// ToDo: fix it..
	//-----------------------------------------------------------------------------

	virtual DetourHandle_t	DetourFunction(void *pFunction, void *pDetourFunction, void **ppOriginalFunction, bool bPause = false, int iDisasmMinBytes = 5) = 0;

	//-----------------------------------------------------------------------------
	// Find function from import address table (using Sys_GetProcAddress) and hook it
	//-----------------------------------------------------------------------------

	virtual DetourHandle_t	DetourFunctionByName(HMODULE hModule, const char *pszFunctionName, void *pDetourFunction, void **ppOriginalFunction, bool bPause = false, int iDisasmMinBytes = 5) = 0;

	//-----------------------------------------------------------------------------
	// Hook function from virtual methods table
	//-----------------------------------------------------------------------------

	virtual DetourHandle_t	DetourVirtualFunction(void *pClassInstance, int nFunctionIndex, void *pDetourFunction, void **ppOriginalFunction, bool bPause = false) = 0;

	//-----------------------------------------------------------------------------
	// Pause function. Returns true if success
	//-----------------------------------------------------------------------------

	virtual bool			PauseDetour(DetourHandle_t hDetour) = 0;

	//-----------------------------------------------------------------------------
	// Unpause function. Returns true if success
	//-----------------------------------------------------------------------------

	virtual bool			UnpauseDetour(DetourHandle_t hDetour) = 0;

	//-----------------------------------------------------------------------------
	// Remove/unhook function
	//-----------------------------------------------------------------------------

	virtual bool			RemoveDetour(DetourHandle_t hDetour) = 0;

	//-----------------------------------------------------------------------------
	// Pause all registered detours
	//-----------------------------------------------------------------------------

	virtual bool			PauseAllDetours() = 0;

	//-----------------------------------------------------------------------------
	// Unpause all registered detours
	//-----------------------------------------------------------------------------

	virtual bool			UnpauseAllDetours() = 0;
};

#define DETOURS_API_INTERFACE_VERSION "DetoursAPI001"

extern IDetoursAPI *g_pDetoursAPI;
PLATFORM_INTERFACE IDetoursAPI *DetoursAPI();

#endif // IDETOURS_API_H