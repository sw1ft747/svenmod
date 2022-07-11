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

#ifndef SDK_NETMSG_H
#define SDK_NETMSG_H

#ifdef _WIN32
#pragma once
#endif

typedef void (*NetMsgHookFn)(void);

typedef struct netmsg_s
{
	char *name;
	NetMsgHookFn function;
	int index;
} netmsg_t;

#endif // SDK_NETMSG_H