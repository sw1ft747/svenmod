// Debugging interface

#ifndef DBG_H
#define DBG_H

#ifdef _WIN32
#pragma once
#endif

#include "ILoggingSystem.h"

#include "platform.h"
#include "sys.h"
#include "color.h"

#if defined(DBG_ASSERT) || defined(DBG_ASSERT_FATAL)
#define __AssertMsg(exp, msg, bFatal) \
	if ( !static_cast<bool>(exp) ) \
	{ \
		LogAssert("%s (%d) : %s.\n", __FILE__, __LINE__, msg); \
		ConColorMsg({255, 20, 20, 255}, "[SvenMod] Assertion Failed: %s\n", msg); \
		if ( bFatal ) \
		{ \
			Sys_Error("[SvenMod] Assertion Failed\n%s (%d) : %s", __FILE__, __LINE__, msg); \
		} \
	}
#endif

#ifdef DBG_ASSERT

#define Assert(exp) __AssertMsg( exp, #exp, false )
#define AssertMsg(exp, msg) __AssertMsg( exp, msg, false )

#else

#define Assert(exp) ( (void)0 )
#define AssertMsg(exp, msg) ( (void)0 )

#endif

#ifdef DBG_ASSERT_FATAL

#define AssertFatal(exp) __AssertMsg( exp, #exp, true )
#define AssertFatalMsg(exp, msg) __AssertMsg( exp, msg, true )

#else

#define AssertFatal(exp) ( (void)0 )
#define AssertFatalMsg(exp, msg) ( (void)0 )

#endif

#define COMPILE_TIME_ASSERT(exp) typedef char compile_time_assert_type[(exp) ? 1 : -1];

//-----------------------------------------------------------------------------
// Console debugging
//-----------------------------------------------------------------------------

PLATFORM_INTERFACE void Msg(const char *pszMessageFormat, ...);
PLATFORM_INTERFACE void Warning(const char *pszMessageFormat, ...);
PLATFORM_INTERFACE void DevMsg(const char *pszMessageFormat, ...);
PLATFORM_INTERFACE void DevWarning(const char *pszMessageFormat, ...);
PLATFORM_INTERFACE void ConColorMsg(const Color &clr, const char *pszMessageFormat, ...);
PLATFORM_INTERFACE void ConMsg(const char *pszMessageFormat, ...); // Same as Msg
PLATFORM_INTERFACE void ConDMsg(const char *pszMessageFormat, ...); // Same as DevMsg

#endif // DBG_H