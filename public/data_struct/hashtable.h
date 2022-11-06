// Hash Table

#ifndef HASHTABLE_H
#define HASHTABLE_H

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

typedef int HashTableIterator_t;

//-----------------------------------------------------------------------------
// Hash Table: key (only integer type) - value (any data)
//-----------------------------------------------------------------------------

template <typename KeyT, class ValueT>
class CHashTable
{
public:
	typedef struct HashPair_s
	{
		KeyT key;
		ValueT value;
	} HashPair_t;

public:
	CHashTable(int iBucketsCount);
	~CHashTable();

	ValueT *Find( const KeyT key ) const;
	bool Insert( const KeyT key, const ValueT &value, void (*pfnOnInsertFailed)(ValueT *pFoundValue, ValueT *pInsertValue) = NULL );
	bool Remove( const KeyT key, bool (*pfnOnRemove)(ValueT *pRemoveValue, ValueT *pUserValue) = NULL, ValueT *pUserValue = NULL );

	void RemoveAll();
	void Clear();

	void Purge();

	void IterateEntries( void (*pfnCallback)(const KeyT key, ValueT &value) );

	HashTableIterator_t First( int ndxBucket );
	HashTableIterator_t Next( int ndxBucket, HashTableIterator_t iterator );

	KeyT &KeyAt( int ndxBucket, HashTableIterator_t iterator );
	ValueT &ValueAt( int ndxBucket, HashTableIterator_t iterator );

	bool IsValidIterator( HashTableIterator_t iterator );

	int Count() const { return m_Size; }
	int Size() const { return m_Size; }

private:
	unsigned int HashKey(const KeyT key) const;

protected:
	typedef std::vector<HashPair_t> HashTableBucketList_t;
	std::vector<HashTableBucketList_t> m_Buckets;

	int m_Size = 0;
};

template <typename KeyT, class ValueT>
CHashTable<KeyT, ValueT>::CHashTable(int iBucketsCount)
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

template <typename KeyT, class ValueT>
CHashTable<KeyT, ValueT>::~CHashTable()
{
	Purge();
}

template <typename KeyT, class ValueT>
inline ValueT *CHashTable<KeyT, ValueT>::Find(const KeyT key) const
{
	unsigned int hash = HashKey(key);
	int index = hash % m_Size;
	
	for (size_t i = 0; i < m_Buckets[index].size(); i++)
	{
		if ( m_Buckets[index][i].key == key )
		{
			return const_cast<ValueT *>(&m_Buckets[index][i].value);
		}
	}

	return NULL;
}

template <typename KeyT, class ValueT>
inline bool CHashTable<KeyT, ValueT>::Insert(const KeyT key, const ValueT &value, void (*pfnOnInsertFailed)(ValueT *pFoundValue, ValueT *pInsertValue) /* = NULL */)
{
	unsigned int hash = HashKey(key);
	int index = hash % m_Size;
	
	for (size_t i = 0; i < m_Buckets[index].size(); i++)
	{
		if ( m_Buckets[index][i].key == key )
		{
			if ( pfnOnInsertFailed != NULL )
				pfnOnInsertFailed(const_cast<ValueT *>(&m_Buckets[index][i].value), const_cast<ValueT *>(&value));

			return false;
		}
	}

	m_Buckets[index].push_back({ key, value });
	return true;
}

template <typename KeyT, class ValueT>
inline bool CHashTable<KeyT, ValueT>::Remove(const KeyT key, bool (*pfnOnRemove)(ValueT *pRemoveValue, ValueT *pUserValue) /* = NULL */, ValueT *pUserValue /* = NULL */)
{
	unsigned int hash = HashKey(key);
	int index = hash % m_Size;
	
	for (size_t i = 0; i < m_Buckets[index].size(); i++)
	{
		if ( m_Buckets[index][i].key == key )
		{
			if ( pfnOnRemove && !pfnOnRemove(&m_Buckets[index][i].value, pUserValue) )
				return false;

			m_Buckets[index].erase( m_Buckets[index].begin() + i );
			return true;
		}
	}

	return false;
}

template <typename KeyT, class ValueT>
inline void CHashTable<KeyT, ValueT>::RemoveAll()
{
	Clear();
}

template <typename KeyT, class ValueT>
inline void CHashTable<KeyT, ValueT>::Purge()
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

template <typename KeyT, class ValueT>
inline void CHashTable<KeyT, ValueT>::Clear()
{
	if ( !m_Buckets.empty() )
	{
		for (int i = 0; i < m_Size; i++)
		{
			m_Buckets[i].clear();
		}
	}
}

template <typename KeyT, class ValueT>
inline void CHashTable<KeyT, ValueT>::IterateEntries(void (*pfnCallback)(const KeyT key, ValueT &value))
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

template <typename KeyT, class ValueT>
HashTableIterator_t CHashTable<KeyT, ValueT>::First(int ndxBucket)
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

template <typename KeyT, class ValueT>
HashTableIterator_t CHashTable<KeyT, ValueT>::Next(int ndxBucket, HashTableIterator_t iterator)
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

template <typename KeyT, class ValueT>
KeyT &CHashTable<KeyT, ValueT>::KeyAt(int ndxBucket, HashTableIterator_t iterator)
{
	AssertFatalMsg( ndxBucket >= 0 && ndxBucket < m_Size && iterator >= 0 && iterator < (int)m_Buckets[ndxBucket].size(), "out of range" );

	return m_Buckets[ndxBucket][iterator].key;
}

template <typename KeyT, class ValueT>
ValueT &CHashTable<KeyT, ValueT>::ValueAt(int ndxBucket, HashTableIterator_t iterator)
{
	AssertFatalMsg( ndxBucket >= 0 && ndxBucket < m_Size && iterator >= 0 && iterator < (int)m_Buckets[ndxBucket].size(), "out of range" );

	return m_Buckets[ndxBucket][iterator].value;
}

template <typename KeyT, class ValueT>
bool CHashTable<KeyT, ValueT>::IsValidIterator(HashTableIterator_t iterator)
{
	return (iterator != (HashTableIterator_t)-1);
}

template <typename KeyT, class ValueT>
FORCEINLINE unsigned int CHashTable<KeyT, ValueT>::HashKey(const KeyT key) const
{
	return ::HashKey((unsigned char *)&key, sizeof(KeyT));
}

#endif // HASHTABLE_H
