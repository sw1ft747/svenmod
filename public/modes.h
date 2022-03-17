//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef MODES_H
#define MODES_H

#ifdef _WIN32
#pragma once
#endif

typedef struct vmode_s
{
    int width;
    int height;
    int bpp; // bits per pixel
} vmode_t;

#endif // MODES_H