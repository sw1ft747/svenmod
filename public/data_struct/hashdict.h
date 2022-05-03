// Hash Dictionary

#ifndef HASHDICT_H
#define HASHDICT_H

#ifdef _WIN32
#pragma once
#endif

#include "dbg.h"
#include "generichash.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <vector>

typedef int HashDictIterator_t;

//-----------------------------------------------------------------------------
// Hash Dictionary: key (string) - value (any data)
//-----------------------------------------------------------------------------

template <class T, bool bCaseInsensitive = true, bool bDupeStrings = true>
class CHashDict
{
public:
	typedef struct HashPair_s
	{
		const char *key;
		T value;
	} HashPair_t;

public:
	CHashDict(int iBucketsCount);
	~CHashDict();

	T *Find( const char *pszKey ) const;
	bool Insert( const char *pszKey, const T &value, void (*pfnOnInsertFailed)(T *pFoundValue, T *pInsertValue) = NULL );
	bool Remove( const char *pszKey, bool (*pfnOnRemove)(T *pRemoveValue, T *pUserValue) = NULL, T *pUserValue = NULL );

	void RemoveAll();
	void Clear();

	void Purge();

	void IterateEntries( void (*pfnCallback)(const char *pszKey, T &value) );

	HashDictIterator_t First( int ndxBucket );
	HashDictIterator_t Next( int ndxBucket, HashDictIterator_t iterator );

	const char *KeyAt( int ndxBucket, HashDictIterator_t iterator );
	T &ValueAt( int ndxBucket, HashDictIterator_t iterator );

	bool IsValidIterator( HashDictIterator_t iterator );

	int Count() const { return m_Size; }
	int Size() const { return m_Size; }

private:
	unsigned int HashKey(const char *pszKey) const;

protected:
	typedef std::vector<HashPair_t> HashDictBucketList_t;
	std::vector<HashDictBucketList_t> m_Buckets;

	int m_Size = 0;
};

template <class T, bool bCaseInsensitive, bool bDupeStrings>
CHashDict<T, bCaseInsensitive, bDupeStrings>::CHashDict(int iBucketsCount)
{
	if ((iBucketsCount & (iBucketsCount - 1)) == 0) // power of two
		m_Size = iBucketsCount - 1;
	else
		m_Size = iBucketsCount;

	m_Buckets.reserve(m_Size);
	m_Buckets.resize(m_Size);

	for (int i = 0; i < m_Size; i++)
	{
		m_Buckets[i].clear();
	}
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
CHashDict<T, bCaseInsensitive, bDupeStrings>::~CHashDict()
{
	Purge();
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
inline T *CHashDict<T, bCaseInsensitive, bDupeStrings>::Find(const char *pszKey) const
{
	unsigned int hash = HashKey(pszKey);
	int index = hash % m_Size;
	
	for (size_t i = 0; i < m_Buckets[index].size(); i++)
	{
		if ( bCaseInsensitive ? !stricmp(m_Buckets[index][i].key, pszKey) : !strcmp(m_Buckets[index][i].key, pszKey) )
		{
			return const_cast<T *>(&m_Buckets[index][i].value);
		}
	}

	return NULL;
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
inline bool CHashDict<T, bCaseInsensitive, bDupeStrings>::Insert(const char *pszKey, const T &value, void (*pfnOnInsertFailed)(T *pFoundValue, T *pInsertValue) /* = NULL */)
{
	unsigned int hash = HashKey(pszKey);
	int index = hash % m_Size;
	
	for (size_t i = 0; i < m_Buckets[index].size(); i++)
	{
		if ( bCaseInsensitive ? !stricmp(m_Buckets[index][i].key, pszKey) : !strcmp(m_Buckets[index][i].key, pszKey) )
		{
			if (pfnOnInsertFailed)
				pfnOnInsertFailed(const_cast<T *>(&m_Buckets[index][i].value), const_cast<T *>(&value));

			return false;
		}
	}

	if (bDupeStrings)
	{
		m_Buckets[index].push_back({ strdup(pszKey), value });
	}
	else
	{
		m_Buckets[index].push_back({ pszKey, value });
	}

	return true;
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
inline bool CHashDict<T, bCaseInsensitive, bDupeStrings>::Remove(const char *pszKey, bool (*pfnOnRemove)(T *pRemoveValue, T *pUserValue) /* = NULL */, T *pUserValue /* = NULL */)
{
	unsigned int hash = HashKey(pszKey);
	int index = hash % m_Size;
	
	for (size_t i = 0; i < m_Buckets[index].size(); i++)
	{
		if ( bCaseInsensitive ? !stricmp(m_Buckets[index][i].key, pszKey) : !strcmp(m_Buckets[index][i].key, pszKey) )
		{
			if ( pfnOnRemove && !pfnOnRemove(&m_Buckets[index][i].value, pUserValue) )
				return false;

			if (bDupeStrings)
			{
				free((void *)m_Buckets[index][i].key);
			}

			m_Buckets[index].erase( m_Buckets[index].begin() + i );
			return true;
		}
	}
	
	return false;
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
inline void CHashDict<T, bCaseInsensitive, bDupeStrings>::RemoveAll()
{
	Clear();
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
inline void CHashDict<T, bCaseInsensitive, bDupeStrings>::Purge()
{
	if ( !m_Buckets.empty() )
	{
		for (int i = 0; i < m_Size; i++)
		{
			if (bDupeStrings)
			{
				for (size_t j = 0; j < m_Buckets[i].size(); j++)
				{
					free((void *)m_Buckets[i][j].key);
				}
			}

			m_Buckets[i].clear();
		}

		m_Buckets.clear();
	}
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
inline void CHashDict<T, bCaseInsensitive, bDupeStrings>::Clear()
{
	if ( !m_Buckets.empty() )
	{
		for (int i = 0; i < m_Size; i++)
		{
			if (bDupeStrings)
			{
				for (size_t j = 0; j < m_Buckets[i].size(); j++)
				{
					free((void *)m_Buckets[i][j].key);
				}
			}

			m_Buckets[i].clear();
		}
	}
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
inline void CHashDict<T, bCaseInsensitive, bDupeStrings>::IterateEntries(void (*pfnCallback)(const char *pszKey, T &value))
{
	if ( !m_Buckets.empty() )
	{
		for (int i = 0; i < m_Size; i++)
		{
			for (size_t j = 0; j < m_Buckets[i].size(); j++)
			{
				pfnCallback( m_Buckets[i][j].key, m_Buckets[i][j].value );
			}
		}
	}
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
HashDictIterator_t CHashDict<T, bCaseInsensitive, bDupeStrings>::First(int ndxBucket)
{
	if (ndxBucket >= 0 && ndxBucket < m_Size)
	{
		if ( !m_Buckets[ndxBucket].empty() )
		{
			return 0;
		}
	}

	return -1;
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
HashDictIterator_t CHashDict<T, bCaseInsensitive, bDupeStrings>::Next(int ndxBucket, HashDictIterator_t iterator)
{
	if (ndxBucket >= 0 && ndxBucket < m_Size)
	{
		if ( !m_Buckets[ndxBucket].empty() && ++iterator < (int)m_Buckets[ndxBucket].size() )
		{
			return iterator;
		}
	}

	return -1;
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
const char *CHashDict<T, bCaseInsensitive, bDupeStrings>::KeyAt(int ndxBucket, HashDictIterator_t iterator)
{
	AssertFatalMsg( ndxBucket >= 0 && ndxBucket < m_Size && iterator >= 0 && iterator < (int)m_Buckets[ndxBucket].size(), "out of range" );

	return m_Buckets[ndxBucket][iterator].key;
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
T &CHashDict<T, bCaseInsensitive, bDupeStrings>::ValueAt(int ndxBucket, HashDictIterator_t iterator)
{
	AssertFatalMsg( ndxBucket >= 0 && ndxBucket < m_Size && iterator >= 0 && iterator < (int)m_Buckets[ndxBucket].size(), "out of range" );

	return m_Buckets[ndxBucket][iterator].value;
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
bool CHashDict<T, bCaseInsensitive, bDupeStrings>::IsValidIterator(HashDictIterator_t iterator)
{
	return (iterator != (HashDictIterator_t)-1);
}

template <class T, bool bCaseInsensitive, bool bDupeStrings>
FORCEINLINE unsigned int CHashDict<T, bCaseInsensitive, bDupeStrings>::HashKey(const char *pszKey) const
{
	unsigned int hash;

	if (bCaseInsensitive)
	{
		hash = HashStringCaseless(pszKey);
	}
	else
	{
		hash = HashString(pszKey);
	}

	return hash;
}

#endif // HASHDICT_H
