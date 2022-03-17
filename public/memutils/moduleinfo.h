#ifndef MODULEINFO_H
#define MODULEINFO_H

#ifdef _WIN32
#pragma once
#endif

struct moduleinfo_s
{
	void *pBaseOfDll;
	unsigned int SizeOfImage;
};

#endif // MODULEINFO_H