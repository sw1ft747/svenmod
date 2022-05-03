#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#pragma once
#define PLATFORM_WINDOWS
#elif defined(__linux__)
#define PLATFORM_LINUX
#endif

#pragma warning(disable : 26812)

#ifdef PLATFORM_WINDOWS

#define DLL_EXPORT			extern "C" __declspec(dllexport)
#define DLL_IMPORT			extern "C" __declspec(dllimport)

#define DLL_CLASS_EXPORT	__declspec(dllexport)
#define DLL_CLASS_IMPORT	__declspec(dllimport)

#define DLL_GLOBAL_EXPORT	extern __declspec(dllexport)
#define DLL_GLOBAL_IMPORT	extern __declspec(dllimport)

#define FORCEINLINE			__forceinline
#define NOINLINE			__declspec(noinline)

#elif defined(PLATFORM_LINUX)

#define DLL_EXPORT			extern "C" __attribute__((visibility("default")))
#define DLL_IMPORT			extern "C"

#define DLL_CLASS_EXPORT	__attribute__((visibility("default")))
#define DLL_CLASS_IMPORT

#define DLL_GLOBAL_EXPORT	__attribute__((visibility("default")))
#define DLL_GLOBAL_IMPORT	extern

#define FORCEINLINE			inline __attribute__((always_inline))
#define NOINLINE			__attribute__((noinline))

#else
	
#define DLL_EXPORT
#define DLL_IMPORT

#define DLL_CLASS_EXPORT
#define DLL_CLASS_IMPORT

#define DLL_GLOBAL_EXPORT
#define DLL_GLOBAL_IMPORT

#define FORCEINLINE
#define NOINLINE

#endif

#define abstract_class class

#ifndef STATIC_LINKED

#ifdef PLATFORM_EXPORTS
	#define PLATFORM_INTERFACE DLL_EXPORT
#else
	#define PLATFORM_INTERFACE DLL_IMPORT
#endif

#else
	
#define PLATFORM_INTERFACE extern

#endif // STATIC_LINKED

#endif // PLATFORM_H
