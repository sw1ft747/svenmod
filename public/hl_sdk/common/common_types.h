//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef SDK_COMMON_TYPES_H
#define SDK_COMMON_TYPES_H

#ifdef _WIN32
#pragma once
#endif

/**
*	@file
*
*	Common data types
*/

struct Point
{
	int x = 0;
	int y = 0;
};

struct Rect
{
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;
};

#endif // COMMON_TYPES_H