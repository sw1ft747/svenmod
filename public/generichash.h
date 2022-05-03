#ifndef GENERIC_HASH_H
#define GENERIC_HASH_H

#ifdef _WIN32
#pragma once
#endif

#include <ctype.h>
#include <string.h>

inline unsigned int HashKey(unsigned char *pKey, size_t length)
{
	// Jenkins hash function
	unsigned int i = 0;
	unsigned int hash = 0;

	while (i != length)
	{
		hash += pKey[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}

	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return hash;
}

inline unsigned int HashString(const char *pszKey)
{
	// Jenkins hash function
	unsigned int hash = 0;

	while (*pszKey)
	{
		hash += *pszKey;
		hash += hash << 10;
		hash ^= hash >> 6;

		++pszKey;
	}

	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return hash;
}

inline unsigned int HashStringCaseless(const char *pszKey)
{
	// Jenkins hash function
	unsigned int hash = 0;

	while (*pszKey)
	{
		hash += tolower(*pszKey);
		hash += hash << 10;
		hash ^= hash >> 6;

		++pszKey;
	}

	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return hash;
}

#endif // GENERIC_HASH_H
