#ifndef SDK_SIZEBUF_H
#define SDK_SIZEBUF_H

#ifdef _WIN32
#pragma once
#endif

#include "Platform.h"

typedef struct sizebuf_s
{
	qboolean	allowoverflow;
	qboolean	overflowed;
	byte		*data;
	int			maxsize;
	int			cursize;
} sizebuf_t;

#endif // SDK_SIZEBUF_H