//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef SDK_KBUTTON_H
#define SDK_KBUTTON_H

#ifdef _WIN32
#pragma once
#endif

typedef struct kbutton_s
{
	int down[2]; // key nums holding it down
	int state;	 // low bit is down state
} kbutton_t;

#endif // SDK_KBUTTON_H