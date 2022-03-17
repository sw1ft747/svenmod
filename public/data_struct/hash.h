// Hash Map

#ifndef HASHMAP_H
#define HASHMAP_H

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

typedef int HashIterator_t;

//-----------------------------------------------------------------------------
// Hash Map: stores any data
//-----------------------------------------------------------------------------

template <class Data, typename C = bool (*)(const Data &, const Data &), typename H = unsigned int (*)(const Data &)>
class CHash
{
public:
	typedef C CompareFunc_t;
	typedef H HashFunc_t;

public:
	explicit CHash(int iBucketsCount = 32, CompareFunc_t compareFunc = 0, HashFunc_t hashFunc = 0);
	~CHash();

	void Resize( int iBucketsCount );

	Data *Find( const Data &data ) const;
	bool Insert( const Data &data, void (*pfnOnInsertFailed)(Data *pFoundData, Data *pInsertData) = NULL );
	bool Remove( const Data &data, bool (*pfnOnRemove)(Data *pRemoveData, Data *pUserData) = NULL, Data *pUserData = NULL );

	void Clear();
	void RemoveAll();

	void Purge();

	void IterateEntries( void (*pfnCallback)(Data &data) );

	HashIterator_t First( int ndxBucket );
	HashIterator_t Next( int ndxBucket, HashIterator_t iterator );

	Data &At( int ndxBucket, HashIterator_t iterator );

	bool IsValidIterator( HashIterator_t iterator );

	int Count() const { return m_Size; }
	int Size() const { return m_Size; }

protected:
	typedef std::vector<Data> HashBucketList_t;
	std::vector<HashBucketList_t> m_Buckets;

	int m_Size = 0;

	CompareFunc_t m_CompareFunc;
	HashFunc_t m_HashFunc;
};

template <class Data, typename C, typename H>
CHash<Data, C, H>::CHash(int iBucketsCount, CompareFunc_t compareFunc, HashFunc_t hashFunc) : m_CompareFunc(compareFunc), m_HashFunc(hashFunc)
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

template <class Data, typename C, typename H>
CHash<Data, C, H>::~CHash()
{
	Purge();
}

template <class Data, typename C, typename H>
void CHash<Data, C, H>::Resize(int iBucketsCount)
{
	Purge();

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

template <class Data, typename C, typename H>
inline Data *CHash<Data, C, H>::Find(const Data &data) const
{
	unsigned int hash = m_HashFunc(data);
	int index = hash % m_Size;
	
	for (size_t i = 0; i < m_Buckets[index].size(); i++)
	{
		if ( m_CompareFunc(m_Buckets[index][i], data) )
		{
			return const_cast<Data *>(&m_Buckets[index][i]);
		}
	}

	return NULL;
}

template <class Data, typename C, typename H>
inline bool CHash<Data, C, H>::Insert(const Data &data, void (*pfnOnInsertFailed)(Data *pFoundData, Data *pInsertData) /* = NULL */)
{
	unsigned int hash = m_HashFunc(data);
	int index = hash % m_Size;
	
	for (size_t i = 0; i < m_Buckets[index].size(); i++)
	{
		if ( m_CompareFunc(m_Buckets[index][i], data) )
		{
			if (pfnOnInsertFailed)
				pfnOnInsertFailed(const_cast<Data *>(&m_Buckets[index][i]), const_cast<Data *>(&data));

			return false;
		}
	}

	m_Buckets[index].push_back(data);
	return true;
}

template <class Data, typename C, typename H>
inline bool CHash<Data, C, H>::Remove(const Data &data, bool (*pfnOnRemove)(Data *pRemoveData, Data *pUserData) /* = NULL */, Data *pUserData /* = NULL */)
{
	unsigned int hash = m_HashFunc(data);
	int index = hash % m_Size;

	for (size_t i = 0; i < m_Buckets[index].size(); i++)
	{
		if ( m_CompareFunc(m_Buckets[index][i], data) )
		{
			if ( pfnOnRemove && !pfnOnRemove(&m_Buckets[index][i], pUserData) )
				return false;

			m_Buckets[index].erase( m_Buckets[index].begin() + i );
			return true;
		}
	}
	
	return false;
}

template <class Data, typename C, typename H>
inline void CHash<Data, C, H>::RemoveAll()
{
	Clear();
}

template <class Data, typename C, typename H>
inline void CHash<Data, C, H>::Purge()
{
	if ( !m_Buckets.empty() )
	{
		for (int i = 0; i < m_Size; i++)
		{
			m_Buckets[i].clear();
		}

		m_Buckets.clear();
	}
}

template <class Data, typename C, typename H>
inline void CHash<Data, C, H>::Clear()
{
	if ( !m_Buckets.empty() )
	{
		for (int i = 0; i < m_Size; i++)
		{
			m_Buckets[i].clear();
		}
	}
}

template <class Data, typename C, typename H>
inline void CHash<Data, C, H>::IterateEntries(void (*pfnCallback)(Data &data))
{
	if ( !m_Buckets.empty() )
	{
		for (int i = 0; i < m_Size; i++)
		{
			for (size_t j = 0; j < m_Buckets[i].size(); j++)
			{
				pfnCallback( m_Buckets[i][j] );
			}
		}
	}
}

template <class Data, typename C, typename H>
HashIterator_t CHash<Data, C, H>::First(int ndxBucket)
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

template <class Data, typename C, typename H>
HashIterator_t CHash<Data, C, H>::Next(int ndxBucket, HashIterator_t iterator)
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

template <class Data, typename C, typename H>
Data &CHash<Data, C, H>::At(int ndxBucket, HashIterator_t iterator)
{
	AssertFatalMsg( ndxBucket >= 0 && ndxBucket < m_Size && iterator >= 0 && iterator < (int)m_Buckets[ndxBucket].size(), "out of range" );

	return m_Buckets[ndxBucket][iterator];
}

template <class Data, typename C, typename H>
bool CHash<Data, C, H>::IsValidIterator(HashIterator_t iterator)
{
	return (iterator != (HashIterator_t)-1);
}

#endif // HASHMAP_H