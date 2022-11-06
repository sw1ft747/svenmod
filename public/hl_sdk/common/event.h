/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#ifndef SDK_EVENT_H
#define SDK_EVENT_H

#ifdef _WIN32
#pragma once
#endif

#include "event_args.h"

typedef void (*EventHookFn)(event_args_t *args);

typedef struct event_s
{
	struct event_s *next;
	char *name;
	EventHookFn function;
} event_t;

#endif // SDK_EVENT_H