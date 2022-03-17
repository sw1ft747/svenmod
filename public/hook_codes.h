#ifndef HOOK_CODES_H
#define HOOK_CODES_H

#ifdef _WIN32
#pragma once
#endif

#define HOOK_RETURN_VALUE

typedef enum
{
	HOOK_CONTINUE = 0,	// keep going
	HOOK_OVERRIDE,		// run the game dll function but use our return value instead
	HOOK_STOP,			// don't run the game dll function at all
	HOOK_CALL_STOP		// call the game dll function and stop further hooks from executing 
} HOOK_RESULT;

#endif // HOOK_CODES_H