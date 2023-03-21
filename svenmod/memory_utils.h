#ifndef MEMORYUTILS_H
#define MEMORYUTILS_H

#ifdef _WIN32
#pragma once
#endif

#include <IMemoryUtils.h>
#include <interface.h>
#include <data_struct/hashtable.h>
#include <data_struct/hash.h>

#ifdef PLATFORM_LINUX
#define DUPE_SYMBOLS_NAME 0
#endif

//-----------------------------------------------------------------------------
// CMemoryUtils
//-----------------------------------------------------------------------------

class CMemoryUtils : public IMemoryUtils
{
public:
	CMemoryUtils();
	virtual ~CMemoryUtils();

	virtual void InitDisasm(ud_t *instruction, void *buffer, uint8_t mode, size_t buffer_length = 128);

	virtual int Disassemble(ud_t *instruction);

	virtual bool VirtualProtect(void *pAddress, size_t size, int fNewProtect, int *pfOldProtect);
	
	virtual void *VirtualAlloc(void *pAddress, size_t size, int fAllocationType, int fProtection);
	
	virtual bool VirtualFree(void *pAddress, size_t size, int fFreeType = 0);
	
	virtual void PatchMemory(void *pAddress, unsigned char *pPatchBytes, int length);

	virtual void MemoryNOP(void *pAddress, int length);

	virtual void *CalcAbsoluteAddress(void *pCallOpcode);

	virtual void *CalcRelativeAddress(void *pFrom, void *pTo);

	virtual void *GetVTable(void *pClassInstance);
	
	virtual void *GetVirtualFunction(void *pClassInstance, int nFunctionIndex);

	virtual bool RetrieveModuleInfo(HMODULE hModule, moduleinfo_t *pModInfo);

	virtual void *ResolveSymbol(HMODULE hModule, const char *pszSymbol);

	virtual void *FindPattern(HMODULE hModule, pattern_t *pPattern, unsigned int offset = 0);
	virtual void *FindPatternWithin(HMODULE hModule, pattern_t *pPattern, void *pSearchStart, void *pSearchEnd);

	virtual void *FindPattern(HMODULE hModule, const char *pszPattern, char *pszMask, unsigned int offset = 0);
	virtual void *FindPatternWithin(HMODULE hModule, const char *pszPattern, char *pszMask, void *pSearchStart, void *pSearchEnd);

	virtual void *FindPattern(HMODULE hModule, const char *pszPattern, unsigned int length, unsigned int offset = 0, char ignoreByte = '\x2A');
	virtual void *FindPatternWithin(HMODULE hModule, const char *pszPattern, unsigned int length, void *pSearchStart, void *pSearchEnd, char ignoreByte = '\x2A');

	virtual void *FindPattern(HMODULE hModule, unsigned char *pPattern, unsigned int length, unsigned int offset = 0, unsigned char ignoreByte = 0x2A);
	virtual void *FindPatternWithin(HMODULE hModule, unsigned char *pPattern, unsigned int length, void *pSearchStart, void *pSearchEnd, unsigned char ignoreByte = 0x2A);

	virtual void *FindString(HMODULE hModule, const char *pszString, unsigned int offset = 0);
	virtual void *FindStringWithin(HMODULE hModule, const char *pszString, void *pSearchStart, void *pSearchEnd);

	virtual void *FindAddress(HMODULE hModule, void *pAddress, unsigned int offset = 0);
	virtual void *FindAddressWithin(HMODULE hModule, void *pAddress, void *pSearchStart, void *pSearchEnd);

#ifdef PLATFORM_LINUX
private:
	typedef struct symbol_s
	{
		const char *name;
		size_t length;
		void *address;
	} symbol_t;
	
	class CLookupFunctor
	{
	public:
		CLookupFunctor() {}
		
		// Compare
		bool operator()(const symbol_t &a, const symbol_t &b) const
		{
			return a.length == b.length && !strcmp(a.name, b.name);
		}
		
		// Get hash
		unsigned int operator()(const symbol_t &sym) const
		{
			return HashKey((unsigned char *)sym.name, sym.length);
		}
	};

	class CSymbolTable
	{
	public:
		CSymbolTable() : m_SymbolsTable(3, m_Functor, m_Functor)
		{
		}
		
		CSymbolTable(int tableSize) : m_SymbolsTable(tableSize, m_Functor, m_Functor)
		{
		}
		
		~CSymbolTable()
		{
		#if DUPE_SYMBOLS_NAME
			for (int i = 0; i < m_SymbolsTable.Count(); i++)
			{
				HashIterator_t it = m_SymbolsTable.First(i);
				
				while ( m_SymbolsTable.IsValidIterator(it) )
				{
					symbol_t &sym = m_SymbolsTable.At(i, it);
					
					free( (void *)sym.name );
					
					it = m_SymbolsTable.Next(i, it);
				}
			}
		#endif
		}
		
		void ResizeTable(int tableSize)
		{
			m_SymbolsTable.Resize(tableSize);
		}
		
		symbol_t *FindSymbol(const char *pszSymbol, int length)
		{
			symbol_t symbol_find =
			{
				pszSymbol,
				(size_t)length,
				NULL
			};
		
			return m_SymbolsTable.Find( symbol_find );
		}
		
		symbol_t *FindSymbol(const char *pszSymbol)
		{
			return FindSymbol( pszSymbol, strlen(pszSymbol) );
		}
		
		symbol_t *InternSymbol(const char *pszSymbol, int length, void *pAddress)
		{
		#if DUPE_SYMBOLS_NAME
			pszSymbol = strdup(pszSymbol);
		#endif
		
			symbol_t symbol =
			{
				pszSymbol,
				(size_t)length,
				pAddress
			};
			
			bool bInserted = m_SymbolsTable.Insert(symbol);
			
			if ( !bInserted )
			{
			#if DUPE_SYMBOLS_NAME
				free( (void *)pszSymbol );
			#endif
			}
			
			return m_SymbolsTable.Find(symbol);
		}
		
		symbol_t *InternSymbol(const char *pszSymbol, void *pAddress)
		{
			return InternSymbol(pszSymbol, strlen(pszSymbol), pAddress);
		}
		
	private:
		CLookupFunctor m_Functor;
		CHash<symbol_t, CLookupFunctor &, CLookupFunctor &> m_SymbolsTable;
	};
	
	class CModuleSymbolTable
	{
	public:
		CModuleSymbolTable() : table()
		{
			handle = NULL;
			last_pos = 0;
		}
	
	public:
		CSymbolTable table;
		HMODULE handle;
		size_t last_pos;
	};

private:
	int ReadMemoryProtection(void *pAddress);
#endif

private:
	CHashTable<HMODULE, moduleinfo_t> m_ModuleInfoTable;
	
#ifdef PLATFORM_LINUX
	std::vector<CModuleSymbolTable *> m_SymbolTables;
#endif
};

extern CMemoryUtils g_MemoryUtils;

#endif // MEMORYUTILS_H