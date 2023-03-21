#include "memory_utils.h"

#ifdef PLATFORM_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <link.h>
#include <hl_sdk/common/Platform.h>
#endif

//-----------------------------------------------------------------------------
// CMemoryUtils implementation
//-----------------------------------------------------------------------------

CMemoryUtils::CMemoryUtils() : m_ModuleInfoTable(15)
{
}

CMemoryUtils::~CMemoryUtils()
{
#ifdef PLATFORM_LINUX
	for (size_t i = 0; i < m_SymbolTables.size(); i++)
	{
		delete m_SymbolTables[i];
	}

	m_SymbolTables.clear();
#endif
	
	m_ModuleInfoTable.Purge();
}

//-----------------------------------------------------------------------------
// Disassembler
//-----------------------------------------------------------------------------

void CMemoryUtils::InitDisasm(ud_t *instruction, void *buffer, uint8_t mode, size_t buffer_length /* = 128 */)
{
	ud_init(instruction);
	ud_set_mode(instruction, mode);
	ud_set_input_buffer(instruction, (const uint8_t *)buffer, buffer_length);
}

int CMemoryUtils::Disassemble(ud_t *instruction)
{
	return ud_disassemble(instruction);
}

//-----------------------------------------------------------------------------
// Virtual memory
//-----------------------------------------------------------------------------

bool CMemoryUtils::VirtualProtect(void *pAddress, size_t size, int fNewProtect, int *pfOldProtect)
{
#ifdef PLATFORM_WINDOWS
	return !!(::VirtualProtect(pAddress, size, fNewProtect, (PDWORD)pfOldProtect));
#else
	pAddress = (void *)(ALIGN_ADDR(pAddress));
	size = PAGE_ALIGN_UP(size);

	if (pfOldProtect)
	{
		*pfOldProtect = ReadMemoryProtection(pAddress);
	}

	return mprotect(pAddress, size, fNewProtect) != -1;
#endif

	return false;
}

void *CMemoryUtils::VirtualAlloc(void *pAddress, size_t size, int fAllocationType, int fProtection)
{
#ifdef PLATFORM_WINDOWS
	return ::VirtualAlloc(pAddress, size, fAllocationType, fProtection);
#else
	pAddress = (void *)(ALIGN_ADDR(pAddress));
	size = PAGE_ALIGN_UP(size);

	void *addr = mmap(pAddress, size, fProtection, fAllocationType, -1, 0);
	
	if (addr == MAP_FAILED)
		return NULL;
	
	return addr;
#endif

	return NULL;
}
	
bool CMemoryUtils::VirtualFree(void *pAddress, size_t size, int fFreeType /* = 0 */)
{
#ifdef PLATFORM_WINDOWS
#pragma warning(push)
#pragma warning(disable : 28160)

	return !!(::VirtualFree(pAddress, size, fFreeType));

#pragma warning(pop)
#else
	pAddress = (void *)(ALIGN_ADDR(pAddress));
	size = PAGE_ALIGN_UP(size);

	return (void *)munmap(pAddress, size) != MAP_FAILED;
#endif

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void CMemoryUtils::PatchMemory(void *pAddress, unsigned char *pPatchBytes, int length)
{
	int dwProtection;

	this->VirtualProtect(pAddress, length, PAGE_EXECUTE_READWRITE, &dwProtection);

	memcpy(pAddress, pPatchBytes, length);

	this->VirtualProtect(pAddress, length, dwProtection, NULL);
}

void CMemoryUtils::MemoryNOP(void *pAddress, int length)
{
	int dwProtection;

	this->VirtualProtect(pAddress, length, PAGE_EXECUTE_READWRITE, &dwProtection);

	memset(pAddress, 0x90, length);

	this->VirtualProtect(pAddress, length, dwProtection, NULL);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void *CMemoryUtils::CalcAbsoluteAddress(void *pCallAddress)
{
	unsigned long luRelativeAddress = *(unsigned long *)((unsigned char *)pCallAddress + 1);
	return (void *)(luRelativeAddress + (unsigned long)pCallAddress + sizeof(void *) + 1);
}

void *CMemoryUtils::CalcRelativeAddress(void *pFrom, void *pTo)
{
	return (void *)((unsigned long)pTo - ((unsigned long)pFrom + sizeof(void *) + 1));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void *CMemoryUtils::GetVTable(void *pClassInstance)
{
	void *pVTable = *static_cast<void **>(pClassInstance);
	return pVTable;
}

void *CMemoryUtils::GetVirtualFunction(void *pClassInstance, int nFunctionIndex)
{
	void **pVTable = *static_cast<void ***>(pClassInstance);
	return reinterpret_cast<void *>(pVTable[nFunctionIndex]);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool CMemoryUtils::RetrieveModuleInfo(HMODULE hModule, moduleinfo_t *pModInfo)
{
	if ( !hModule )
		return false;

	moduleinfo_t *pHashEntry = NULL;

	if ( pHashEntry = m_ModuleInfoTable.Find(hModule) )
	{
		*pModInfo = *pHashEntry;
		return true;
	}

#ifdef PLATFORM_WINDOWS
	MEMORY_BASIC_INFORMATION memInfo;

	if ( VirtualQuery(hModule, &memInfo, sizeof(MEMORY_BASIC_INFORMATION)) )
	{
		if ( memInfo.State != MEM_COMMIT || memInfo.Protect == PAGE_NOACCESS )
			return false;

		unsigned int dwAllocationBase = (unsigned int)memInfo.AllocationBase;
		pModInfo->pBaseOfDll = memInfo.AllocationBase;

		IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)dwAllocationBase;
		IMAGE_NT_HEADERS *pe = (IMAGE_NT_HEADERS *)(dwAllocationBase + dos->e_lfanew);

		IMAGE_FILE_HEADER *file = &pe->FileHeader;
		IMAGE_OPTIONAL_HEADER *opt = &pe->OptionalHeader;

		if (dos->e_magic == IMAGE_DOS_SIGNATURE && pe->Signature == IMAGE_NT_SIGNATURE && opt->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC && file->Machine == IMAGE_FILE_MACHINE_I386)
		{
			/*
			if ( (file->Characteristics & IMAGE_FILE_DLL) == 0 )
				return false;
			*/
		
			pModInfo->SizeOfImage = opt->SizeOfImage;

			m_ModuleInfoTable.Insert(hModule, *pModInfo);

			return true;
		}
	}
#else
#if 0
	Dl_info info;
	void *pAddress = NULL;
	
	// Try to find CreateInterface symbol to don't check memory mappings
	// pAddress = dlsym(hModule, "CreateInterface");
	
	// We need to waste our time. Is there really no other way?
	if ( !pAddress )
	{
		struct link_map *dlmap;
		
		if ( dlinfo(hModule, RTLD_DI_LINKMAP, &dlmap) != 0 )
			return false;

		static char szBuffer[1024];
		const char *pszTargetModulePath = dlmap->l_name;
		
		FILE *file = fopen("/proc/self/maps", "r");
		
		while ( fgets(szBuffer, sizeof(szBuffer), file) )
		{
			const char *pszModulePath = strchr(szBuffer, '/');
			
			if (pszModulePath)
			{
				size_t count = strlen(pszTargetModulePath);
				
				if ( memcmp(pszTargetModulePath, pszModulePath, count) == 0 )
				{
					pAddress = (void *)strtoul(szBuffer, NULL, 16);
					break;
				}
			}
		}
		
		fclose(file);
	}
	
	if ( dladdr(pAddress, &info) && info.dli_fbase && info.dli_fname )
	{
		unsigned int dwAllocationBase = (unsigned int)info.dli_fbase;
		pModInfo->pBaseOfDll = info.dli_fbase;
#else
	struct link_map *dlmap;
	
	if ( dlinfo(hModule, RTLD_DI_LINKMAP, &dlmap) == 0 && dlmap->l_addr && dlmap->l_name )
	{
		unsigned int dwAllocationBase = (unsigned int)dlmap->l_addr;
		pModInfo->pBaseOfDll = (void *)dlmap->l_addr;
#endif	
		Elf32_Ehdr *file = (Elf32_Ehdr *)dwAllocationBase;
		
		if ( !memcmp(ELFMAG, file->e_ident, SELFMAG) && file->e_ident[EI_VERSION] == EV_CURRENT && file->e_ident[EI_CLASS] == ELFCLASS32 && file->e_machine == EM_386 && file->e_ident[EI_DATA] == ELFDATA2LSB )
		{
			/*
			if ( file->e_type != ET_DYN )
				return false;
			*/
		
			Elf32_Phdr *phdr = (Elf32_Phdr *)(dwAllocationBase + file->e_phoff);
			
			for (unsigned short i = 0; i < file->e_phnum; i++)
			{
				Elf32_Phdr *hdr = &phdr[i];
				
				if ( hdr->p_type == PT_LOAD && hdr->p_flags == (PF_X | PF_R) )
				{
					pModInfo->SizeOfImage = PAGE_ALIGN_UP(hdr->p_filesz);
					
					m_ModuleInfoTable.Insert(hModule, *pModInfo);
					
					return true;
				}
			}
		}
	}
#endif

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void *CMemoryUtils::ResolveSymbol(HMODULE hModule, const char *pszSymbol)
{
#ifdef PLATFORM_WINDOWS
	return GetProcAddress(hModule, pszSymbol);
#else
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		bool new_table = false;
	
		CModuleSymbolTable *dltable = NULL;
		symbol_t *symbol = NULL;
		CSymbolTable *table = NULL;

		for (size_t i = 0; i < m_SymbolTables.size(); i++)
		{
			dltable = m_SymbolTables[i];
			
			if (dltable->handle == hModule)
			{
				table = &dltable->table;
				break;
			}
		}

		if ( table == NULL )
		{
			new_table = true;
			
			dltable = new CModuleSymbolTable();
			table = &dltable->table;
			
			m_SymbolTables.push_back(dltable);
		}
		
		if ( !new_table && (symbol = table->FindSymbol( pszSymbol, strlen(pszSymbol) )) != NULL )
		{
			return symbol->address;
		}
		
		int dlfile;
		
		struct link_map *dlmap;
		struct stat dlstat;
	
		if ( dlinfo(hModule, RTLD_DI_LINKMAP, &dlmap) != 0 || !dlmap->l_addr || !dlmap->l_name )
		{
			return NULL;
		}
		
		dlfile = open(dlmap->l_name, O_RDONLY);
		
		if ( dlfile == -1 || fstat(dlfile, &dlstat) == -1 )
		{
			close(dlfile);
			return NULL;
		}
		
		Elf32_Ehdr *file_hdr = (Elf32_Ehdr *)mmap(NULL, dlstat.st_size, PROT_READ, MAP_PRIVATE, dlfile, 0);
		unsigned int map_base = (unsigned int)file_hdr;
		
		if ( file_hdr == MAP_FAILED )
		{
			close(dlfile);
			return NULL;
		}
		
		close(dlfile);
		
		if ( file_hdr->e_shoff == 0 || file_hdr->e_shstrndx == SHN_UNDEF )
		{
			munmap(file_hdr, dlstat.st_size);
			return NULL;
		}
		
		Elf32_Shdr *symtab_hdr = NULL;
		Elf32_Shdr *strtab_hdr = NULL;
		
		Elf32_Shdr *sections = (Elf32_Shdr *)(map_base + file_hdr->e_shoff);	
		unsigned short sections_count = file_hdr->e_shnum;
		
		Elf32_Shdr *shstrtab_hdr = &sections[file_hdr->e_shstrndx];
		const char *shstrtab = (const char *)(map_base + shstrtab_hdr->sh_offset);

		for (unsigned short i = 0; i < sections_count; i++)
		{
			Elf32_Shdr *hdr = &sections[i];
			
		#if 0
			const char *section_name = shstrtab + hdr->sh_name;
			
			if ( !strcmp(section_name, ".symtab") )
			{
				symtab_hdr = hdr;
			}
			else if ( !strcmp(section_name, ".strtab") )
			{
				strtab_hdr = hdr;
			}
		#else
			unsigned short section_type = hdr->sh_type;
			
			if ( section_type == SHT_SYMTAB )
			{
				symtab_hdr = hdr;
			}
			else if ( section_type == SHT_STRTAB )
			{
				const char *section_name = shstrtab + hdr->sh_name;
				
				if ( !strcmp(section_name, ".strtab") )
				{
					strtab_hdr = hdr;
				}
			}
		#endif
		}
		
		if ( symtab_hdr == NULL || strtab_hdr == NULL )
		{
			munmap(file_hdr, dlstat.st_size);
			return NULL;
		}

		Elf32_Sym *symtab = (Elf32_Sym *)(map_base + symtab_hdr->sh_offset);
		const char *strtab = (const char *)(map_base + strtab_hdr->sh_offset);
		size_t symbol_count = symtab_hdr->sh_size / symtab_hdr->sh_entsize;
		
		if (new_table)
		{
			int tableSize = static_cast<int>((float)symbol_count * (7.f / 11.f));
			
			if (tableSize < 3)
				tableSize = 3;
			else if (tableSize > 65535)
				tableSize = 65535;
		
			table->ResizeTable( tableSize );
		}

		for (size_t i = dltable->last_pos; i < symbol_count; i++)
		{
			Elf32_Sym &sym = symtab[i];
			
			unsigned char sym_type = ELF32_ST_TYPE(sym.st_info);
			const char *sym_name = strtab + sym.st_name;
			
			if (sym.st_shndx == SHN_UNDEF || (sym_type != STT_FUNC && sym_type != STT_OBJECT))
				continue;
			
			symbol_t *current_symbol = table->InternSymbol(sym_name, strlen(sym_name), moduleInfo.pBaseOfDll + sym.st_value);
			
			if ( !strcmp(pszSymbol, sym_name) )
			{
				symbol = current_symbol;
				dltable->last_pos = ++i;
				break;
			}
		}
		
		munmap(file_hdr, dlstat.st_size);
		
		return symbol ? symbol->address : NULL;
	}
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Sig scanner
//-----------------------------------------------------------------------------

void *CMemoryUtils::FindPattern(HMODULE hModule, pattern_t *pPattern, unsigned int offset /* = 0 */)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned long nLength = pPattern->length;
		unsigned char *pSignature = &pPattern->signature;

		unsigned char *pSearchStart = (unsigned char *)moduleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + moduleInfo.SizeOfImage - nLength;

		while (pSearchStart < pSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < nLength; i++)
			{
				if (pSignature[i] != pPattern->ignorebyte && pSignature[i] != pSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pSearchStart;

			pSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindPattern(HMODULE hModule, const char *pszPattern, char *pszMask, unsigned int offset /* = 0 */)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned long nMaskLength = strlen(pszMask);

		unsigned char *pSearchStart = (unsigned char *)moduleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + moduleInfo.SizeOfImage - nMaskLength;

		while (pSearchStart < pSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < nMaskLength; i++)
			{
				if (pszMask[i] != '?' && pszPattern[i] != pSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pSearchStart;

			pSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindPattern(HMODULE hModule, const char *pszPattern, unsigned int length, unsigned int offset /* = 0 */, char ignoreByte /* = '0x2A' */)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned char *pSearchStart = (unsigned char *)moduleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + moduleInfo.SizeOfImage - length;

		while (pSearchStart < pSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < length; i++)
			{
				if (pszPattern[i] != ignoreByte && pszPattern[i] != pSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pSearchStart;

			pSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindPattern(HMODULE hModule, unsigned char *pPattern, unsigned int length, unsigned int offset /* = 0 */, unsigned char ignoreByte /* = 0x2A */)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned char *pSearchStart = (unsigned char *)moduleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + moduleInfo.SizeOfImage - length;

		while (pSearchStart < pSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < length; i++)
			{
				if (pPattern[i] != ignoreByte && pPattern[i] != pSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pSearchStart;

			pSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindString(HMODULE hModule, const char *pszString, unsigned int offset /* = 0 */)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned long nLength = strlen(pszString);

		unsigned char *pSearchStart = (unsigned char *)moduleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + moduleInfo.SizeOfImage - nLength;

		while (pSearchStart < pSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < nLength; i++)
			{
				if (pszString[i] != pSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pSearchStart;

			pSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindAddress(HMODULE hModule, void *pAddress, unsigned int offset /* = 0 */)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned char *pSearchStart = (unsigned char *)moduleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + moduleInfo.SizeOfImage - sizeof(void *);

		while (pSearchStart < pSearchEnd)
		{
			if (*(unsigned long *)pSearchStart == (unsigned long)pAddress)
				return (void *)pSearchStart;

			pSearchStart++;
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Find a pattern within a range
//-----------------------------------------------------------------------------

void *CMemoryUtils::FindPatternWithin(HMODULE hModule, pattern_t *pPattern, void *pSearchStart, void *pSearchEnd)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned long nLength = pPattern->length;
		unsigned char *pSignature = &pPattern->signature;

		unsigned char *pModuleSearchStart = (unsigned char *)moduleInfo.pBaseOfDll;
		unsigned char *pModuleSearchEnd = pModuleSearchStart + moduleInfo.SizeOfImage - nLength;

		if (pModuleSearchStart > (unsigned char *)pSearchStart || pModuleSearchEnd < (unsigned char *)pSearchEnd)
			return NULL;

		pModuleSearchStart = (unsigned char *)pSearchStart;
		pModuleSearchEnd = (unsigned char *)pSearchEnd;

		while (pModuleSearchStart < pModuleSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < nLength; i++)
			{
				if (pSignature[i] != pPattern->ignorebyte && pSignature[i] != pModuleSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pModuleSearchStart;

			pModuleSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindPatternWithin(HMODULE hModule, const char *pszPattern, char *pszMask, void *pSearchStart, void *pSearchEnd)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned long nMaskLength = strlen(pszMask);

		unsigned char *pModuleSearchStart = (unsigned char *)moduleInfo.pBaseOfDll;
		unsigned char *pModuleSearchEnd = pModuleSearchStart + moduleInfo.SizeOfImage - nMaskLength;

		if (pModuleSearchStart > (unsigned char *)pSearchStart || pModuleSearchEnd < (unsigned char *)pSearchEnd)
			return NULL;

		pModuleSearchStart = (unsigned char *)pSearchStart;
		pModuleSearchEnd = (unsigned char *)pSearchEnd;

		while (pModuleSearchStart < pModuleSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < nMaskLength; i++)
			{
				if (pszMask[i] != '?' && pszPattern[i] != pModuleSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pModuleSearchStart;

			pModuleSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindPatternWithin(HMODULE hModule, const char *pszPattern, unsigned int length, void *pSearchStart, void *pSearchEnd, char ignoreByte /* = '0x2A' */)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned char *pModuleSearchStart = (unsigned char *)moduleInfo.pBaseOfDll;
		unsigned char *pModuleSearchEnd = pModuleSearchStart + moduleInfo.SizeOfImage - length;

		if (pModuleSearchStart > (unsigned char *)pSearchStart || pModuleSearchEnd < (unsigned char *)pSearchEnd)
			return NULL;

		pModuleSearchStart = (unsigned char *)pSearchStart;
		pModuleSearchEnd = (unsigned char *)pSearchEnd;

		while (pModuleSearchStart < pModuleSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < length; i++)
			{
				if (pszPattern[i] != ignoreByte && pszPattern[i] != pModuleSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pModuleSearchStart;

			pModuleSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindPatternWithin(HMODULE hModule, unsigned char *pPattern, unsigned int length, void *pSearchStart, void *pSearchEnd, unsigned char ignoreByte /* = 0x2A */)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned char *pModuleSearchStart = (unsigned char *)moduleInfo.pBaseOfDll;
		unsigned char *pModuleSearchEnd = pModuleSearchStart + moduleInfo.SizeOfImage - length;

		if (pModuleSearchStart > (unsigned char *)pSearchStart || pModuleSearchEnd < (unsigned char *)pSearchEnd)
			return NULL;

		pModuleSearchStart = (unsigned char *)pSearchStart;
		pModuleSearchEnd = (unsigned char *)pSearchEnd;

		while (pModuleSearchStart < pModuleSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < length; i++)
			{
				if (pPattern[i] != ignoreByte && pPattern[i] != pModuleSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pModuleSearchStart;

			pModuleSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindStringWithin(HMODULE hModule, const char *pszString, void *pSearchStart, void *pSearchEnd)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned long nLength = strlen(pszString);

		unsigned char *pModuleSearchStart = (unsigned char *)moduleInfo.pBaseOfDll;
		unsigned char *pModuleSearchEnd = pModuleSearchStart + moduleInfo.SizeOfImage - nLength;

		if (pModuleSearchStart > (unsigned char *)pSearchStart || pModuleSearchEnd < (unsigned char *)pSearchEnd)
			return NULL;

		pModuleSearchStart = (unsigned char *)pSearchStart;
		pModuleSearchEnd = (unsigned char *)pSearchEnd;

		while (pModuleSearchStart < pModuleSearchEnd)
		{
			bool bFound = true;

			for (register unsigned long i = 0; i < nLength; i++)
			{
				if (pszString[i] != pModuleSearchStart[i])
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return (void *)pModuleSearchStart;

			pModuleSearchStart++;
		}
	}

	return NULL;
}

void *CMemoryUtils::FindAddressWithin(HMODULE hModule, void *pSearchStart, void *pSearchEnd, void *pAddress)
{
	moduleinfo_t moduleInfo;

	if ( RetrieveModuleInfo(hModule, &moduleInfo) )
	{
		unsigned char *pModuleSearchStart = (unsigned char *)moduleInfo.pBaseOfDll;
		unsigned char *pModuleSearchEnd = pModuleSearchStart + moduleInfo.SizeOfImage - sizeof(void *);

		if (pModuleSearchStart > (unsigned char *)pSearchStart || pModuleSearchEnd < (unsigned char *)pSearchEnd)
			return NULL;

		pModuleSearchStart = (unsigned char *)pSearchStart;
		pModuleSearchEnd = (unsigned char *)pSearchEnd;

		while (pModuleSearchStart < pModuleSearchEnd)
		{
			if (*(unsigned long *)pModuleSearchStart == (unsigned long)pAddress)
				return (void *)pModuleSearchStart;

			pModuleSearchStart++;
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Read memory's protection in Linux
//-----------------------------------------------------------------------------

#ifdef PLATFORM_LINUX

int CMemoryUtils::ReadMemoryProtection(void *pAddress)
{
	static char szBuffer[1024];
	int iProtection = 0;
	
	FILE *file = fopen("/proc/self/maps", "r");
	
	while ( fgets(szBuffer, M_ARRAYSIZE(szBuffer), file) )
	{
		// Tokenize format: [START_ADDR]-[END_ADDR] [PAGE_PROT] ....
	
		char *pszEndPtr1 = NULL;
		char *pszEndPtr2 = NULL;
		
		unsigned long luStartAddress = strtoul(szBuffer, &pszEndPtr1, 16);
		unsigned long luEndAddress = strtoul(pszEndPtr1 + 1, &pszEndPtr2, 16);
		
		const char *pszProtection = pszEndPtr2 + 1;
		
		if (luStartAddress <= (unsigned long)pAddress && (unsigned long)pAddress < luEndAddress)
		{
			if ( pszProtection[0] == 'r' )
			{
				iProtection |= PROT_READ;
			}
			
			if ( pszProtection[1] == 'w' )
			{
				iProtection |= PROT_WRITE;
			}
			
			if ( pszProtection[2] == 'x' )
			{
				iProtection |= PROT_EXEC;
			}
		
			if ( pszProtection[3] == 'p' )
			{
				iProtection |= PAGE_PRIVATE;
			}
			else if ( pszProtection[3] == 's' )
			{
				iProtection |= PAGE_SHARED;
			}
			
			break;
		}
	}
	
	fclose(file);
	
	return iProtection;
}

#endif

//-----------------------------------------------------------------------------
// Export Memory API
//-----------------------------------------------------------------------------

CMemoryUtils g_MemoryUtils;
IMemoryUtils *g_pMemoryUtils = &g_MemoryUtils;

IMemoryUtils *MemoryUtils()
{
	return g_pMemoryUtils;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CMemoryUtils, IMemoryUtils, MEMORYUTILS_INTERFACE_VERSION, g_MemoryUtils);