#ifndef MODULEINFO_H
#define MODULEINFO_H

#ifdef _WIN32
#pragma once
#endif

typedef struct moduleinfo_s
{
	void *pBaseOfDll;
	unsigned int SizeOfImage;
} moduleinfo_t;

#endif // MODULEINFO_H