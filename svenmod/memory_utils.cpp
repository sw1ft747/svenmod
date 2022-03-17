#include <IMemoryUtils.h>
#include <interface.h>
#include <data_struct/hashtable.h>

#ifdef PLATFORM_LINUX
#include <sys/mman.h>
#endif

class CMemoryUtils : public IMemoryUtils
{
public:
	CMemoryUtils();
	virtual ~CMemoryUtils();

	virtual void InitDisasm(ud_t *instruction, void *buffer, uint8_t mode, size_t buffer_length = 128);

	virtual int Disassemble(ud_t *instruction);

	virtual bool VirtualProtect(void *pAddress, int size, int fNewProtect, unsigned long *pfOldProtect);
	
	virtual void PatchMemory(void *pAddress, unsigned char *pPatchBytes, int length);

	virtual void MemoryNOP(void *pAddress, int length);

	virtual void *CalcAbsoluteAddress(void *pCallOpcode);

	virtual void *CalcRelativeAddress(void *pFrom, void *pTo);

	virtual void *GetVTable(void *pClassInstance);
	
	virtual void *GetVirtualFunction(void *pClassInstance, int nFunctionIndex);

	virtual bool RetrieveModuleInfo(HMODULE hModule, moduleinfo_s *pModInfo);

	virtual void *ResolveSymbol(HMODULE hModule, const char *pszSymbol);

	virtual void *FindPattern(HMODULE hModule, pattern_s *pPattern, unsigned int offset = 0);

	virtual void *FindPattern(HMODULE hModule, const char *pszPattern, char *pszMask, unsigned int offset = 0);

	virtual void *FindPattern(HMODULE hModule, const char *pszPattern, unsigned int length, unsigned int offset = 0, char ignoreByte = '\x2A');

	virtual void *FindPattern(HMODULE hModule, unsigned char *pPattern, unsigned int length, unsigned int offset = 0, unsigned char ignoreByte = 0x2A);

	virtual void *FindString(HMODULE hModule, const char *pszString, unsigned int offset = 0);

	virtual void *FindAddress(HMODULE hModule, void *pAddress, unsigned int offset = 0);

private:
	moduleinfo_s m_ModuleInfo;
	CHashTable<HMODULE, moduleinfo_s> m_ModuleInfoHash;
};

//-----------------------------------------------------------------------------
// CMemoryUtils impl
//-----------------------------------------------------------------------------

CMemoryUtils::CMemoryUtils() : m_ModuleInfo(), m_ModuleInfoHash(16)
{
}

CMemoryUtils::~CMemoryUtils()
{
	m_ModuleInfoHash.Purge();
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
//-----------------------------------------------------------------------------

bool CMemoryUtils::VirtualProtect(void *pAddress, int size, int fNewProtect, unsigned long *pfOldProtect)
{
#ifdef PLATFORM_WINDOWS
	return ::VirtualProtect(pAddress, size, fNewProtect, (PDWORD)pfOldProtect);
#elif defined(PLATFORM_LINUX)
	pAddress = (void *)((unsigned long)pAddress & ~(4095));
	size = size + 4095 & ~(4095);

	if (pfOldProtect)
	{
		// ToDo
	}

	return mprotect(pAddress, size, fNewProtect) != -1;
#endif

	return false;
}

void CMemoryUtils::PatchMemory(void *pAddress, unsigned char *pPatchBytes, int length)
{
#ifdef PLATFORM_WINDOWS
	DWORD dwProtection;

	::VirtualProtect(pAddress, length, PAGE_EXECUTE_READWRITE, &dwProtection);

	memcpy(pAddress, pPatchBytes, length);

	::VirtualProtect(pAddress, length, dwProtection, &dwProtection);
#else
#error Implement Linux equivalent
#endif
}

void CMemoryUtils::MemoryNOP(void *pAddress, int length)
{
#ifdef PLATFORM_WINDOWS
	DWORD dwProtection;

	::VirtualProtect(pAddress, length, PAGE_EXECUTE_READWRITE, &dwProtection);

	memset(pAddress, 0x90, length);

	::VirtualProtect(pAddress, length, dwProtection, &dwProtection);
#else
#error Implement Linux equivalent
#endif
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

bool CMemoryUtils::RetrieveModuleInfo(HMODULE hModule, moduleinfo_s *pModInfo)
{
	if ( !hModule )
		return false;

	moduleinfo_s *pHashEntry = NULL;

	if ( pHashEntry = m_ModuleInfoHash.Find(hModule) )
	{
		*pModInfo = *pHashEntry;
		return true;
	}

#ifdef PLATFORM_WINDOWS
	MEMORY_BASIC_INFORMATION memInfo;

	IMAGE_DOS_HEADER *dos;
	IMAGE_NT_HEADERS *pe;
	IMAGE_FILE_HEADER *file;
	IMAGE_OPTIONAL_HEADER *opt;

	if ( VirtualQuery(hModule, &memInfo, sizeof(MEMORY_BASIC_INFORMATION)) )
	{
		if ( memInfo.State != MEM_COMMIT || memInfo.Protect == PAGE_NOACCESS )
			return false;

		unsigned int dwAllocationBase = (unsigned int)memInfo.AllocationBase;
		pModInfo->pBaseOfDll = memInfo.AllocationBase;

		dos = reinterpret_cast<IMAGE_DOS_HEADER *>(dwAllocationBase);
		pe = reinterpret_cast<IMAGE_NT_HEADERS *>(dwAllocationBase + dos->e_lfanew);

		file = &pe->FileHeader;
		opt = &pe->OptionalHeader;

		if (dos->e_magic == IMAGE_DOS_SIGNATURE && pe->Signature == IMAGE_NT_SIGNATURE && opt->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC && file->Machine == IMAGE_FILE_MACHINE_I386)
		{
			pModInfo->SizeOfImage = opt->SizeOfImage;

			m_ModuleInfoHash.Insert(hModule, *pModInfo);

			return true;
		}
	}
#else
#error Implement Linux equivalent
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
#error Implement Linux equivalent
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void *CMemoryUtils::FindPattern(HMODULE hModule, pattern_s *pPattern, unsigned int offset /* = 0 */)
{
	if ( RetrieveModuleInfo(hModule, &m_ModuleInfo) )
	{
		unsigned long nLength = pPattern->length;
		unsigned char *pSignature = &pPattern->signature;

		unsigned char *pSearchStart = (unsigned char *)m_ModuleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + m_ModuleInfo.SizeOfImage - nLength;

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
	if ( RetrieveModuleInfo(hModule, &m_ModuleInfo) )
	{
		unsigned long nMaskLength = strlen(pszMask);

		unsigned char *pSearchStart = (unsigned char *)m_ModuleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + m_ModuleInfo.SizeOfImage - nMaskLength;

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
	if ( RetrieveModuleInfo(hModule, &m_ModuleInfo) )
	{
		unsigned char *pSearchStart = (unsigned char *)m_ModuleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + m_ModuleInfo.SizeOfImage - length;

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
	if ( RetrieveModuleInfo(hModule, &m_ModuleInfo) )
	{
		unsigned char *pSearchStart = (unsigned char *)m_ModuleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + m_ModuleInfo.SizeOfImage - length;

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
	if ( RetrieveModuleInfo(hModule, &m_ModuleInfo) )
	{
		unsigned long nLength = strlen(pszString);

		unsigned char *pSearchStart = (unsigned char *)m_ModuleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + m_ModuleInfo.SizeOfImage - nLength;

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
	if ( RetrieveModuleInfo(hModule, &m_ModuleInfo) )
	{
		unsigned char *pSearchStart = (unsigned char *)m_ModuleInfo.pBaseOfDll + offset;
		unsigned char *pSearchEnd = pSearchStart + m_ModuleInfo.SizeOfImage - sizeof(void *);

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
// Export Memory API
//-----------------------------------------------------------------------------

CMemoryUtils g_MemoryUtils;
IMemoryUtils *g_pMemoryUtils = &g_MemoryUtils;

IMemoryUtils *MemoryUtils()
{
	return g_pMemoryUtils;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CMemoryUtils, IMemoryUtils, MEMORYUTILS_INTERFACE_VERSION, g_MemoryUtils);