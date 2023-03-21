#ifndef IMEMORYUTILS_H
#define IMEMORYUTILS_H

#ifdef _WIN32
#pragma once
#endif

#include <future>

#include "platform.h"
#include "sys.h"

#include "memutils/patterns.h"
#include "memutils/moduleinfo.h"
#include "udis86/udis86.h"

#ifdef PLATFORM_LINUX // Linux
#include <sys/mman.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE				4096
#define PAGE_ALIGN_UP(x)		((x + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_ADDR(addr)		((long)addr & ~(PAGE_SIZE - 1))
#endif

// not actual flags as in Windows
#define MEM_RESERVE				MAP_PRIVATE
#define MEM_COMMIT				MAP_ANONYMOUS

#define PAGE_NOACCESS			0
#define PAGE_READONLY			PROT_READ
#define PAGE_READWRITE			PROT_READ | PROT_WRITE
#define PAGE_EXECUTE_READ		PROT_READ | PROT_EXEC
#define PAGE_EXECUTE_READWRITE	PROT_READ | PROT_WRITE | PROT_EXEC

// can be returned in function 'IMemoryUtils::VirtualProtect' from param @pfOldProtect
#define PAGE_SHARED				(1 << 3)
#define PAGE_PRIVATE			(1 << 4)
#endif

//-----------------------------------------------------------------------------
// Purpose: interface to memory's API
//-----------------------------------------------------------------------------

abstract_class IMemoryUtils
{
public:
	virtual ~IMemoryUtils() {}

	//-----------------------------------------------------------------------------
	// Initialize ud structure
	//-----------------------------------------------------------------------------

	virtual void InitDisasm( ud_t *instruction, void *buffer, uint8_t mode, size_t buffer_length = 128 ) = 0;

	//-----------------------------------------------------------------------------
	// Disassemble memory
	// For more abilities, use exported udis86 functions: public/udis86/include/extern.h
	//-----------------------------------------------------------------------------

	virtual int Disassemble( ud_t *instruction ) = 0;

	//-----------------------------------------------------------------------------
	// Set memory/page protection
	//-----------------------------------------------------------------------------

	virtual bool VirtualProtect( void *pAddress, size_t size, int fNewProtect, int *pfOldProtect ) = 0;

	//-----------------------------------------------------------------------------
	// Allocate virtual memory
	//-----------------------------------------------------------------------------

	virtual void *VirtualAlloc( void *pAddress, size_t size, int fAllocationType, int fProtection ) = 0;

	//-----------------------------------------------------------------------------
	// Free allocated virtual memory
	//-----------------------------------------------------------------------------

	virtual bool VirtualFree( void *pAddress, size_t size, int fFreeType = 0 ) = 0;

	//-----------------------------------------------------------------------------
	// Patch memory address with given length
	//-----------------------------------------------------------------------------

	virtual void PatchMemory( void *pAddress, unsigned char *pPatchBytes, int length ) = 0;

	//-----------------------------------------------------------------------------
	// NOP memory address with given length
	//-----------------------------------------------------------------------------

	virtual void MemoryNOP( void *pAddress, int length ) = 0;

	//-----------------------------------------------------------------------------
	// Calculate absolute function address from CALL/JMP opcode
	// Pointer @pCallOpcode MUST point to CALL/JMP opcode
	//-----------------------------------------------------------------------------

	virtual void *CalcAbsoluteAddress( void *pCallOpcode ) = 0;

	//-----------------------------------------------------------------------------
	// Calculate relative address for calling/jumping from CALL/JMP opcode
	// Pointer @pFrom MUST point to CALL/JMP opcode
	//-----------------------------------------------------------------------------

	virtual void *CalcRelativeAddress( void *pFrom, void *pTo ) = 0;

	//-----------------------------------------------------------------------------
	// Get a virtual methods table
	//-----------------------------------------------------------------------------

	virtual void *GetVTable( void *pClassInstance ) = 0;

	//-----------------------------------------------------------------------------
	// Get a virtual function from virtual methods table
	//-----------------------------------------------------------------------------

	virtual void *GetVirtualFunction( void *pClassInstance, int nFunctionIndex ) = 0;

	//-----------------------------------------------------------------------------
	// Get module info
	//-----------------------------------------------------------------------------

	virtual bool RetrieveModuleInfo( HMODULE hModule, moduleinfo_t *pModInfo ) = 0;

	//-----------------------------------------------------------------------------
	// Lookup for a symbol in Symbol Table
	// In Windows, will be called GetProcAddress function
	//-----------------------------------------------------------------------------

	virtual void *ResolveSymbol( HMODULE hModule, const char *pszSymbol ) = 0;

	//-----------------------------------------------------------------------------
	// Find signature from pattern_s structure
	//-----------------------------------------------------------------------------

	virtual void *FindPattern( HMODULE hModule, pattern_t *pPattern, unsigned int offset = 0 ) = 0;
	virtual void *FindPatternWithin( HMODULE hModule, pattern_t *pPattern, void *pSearchStart, void *pSearchEnd ) = 0;

	//-----------------------------------------------------------------------------
	// Find signature from string with given mask
	// If signature: "\xD9\x1D\x2A\x2A\x2A\x2A\x75\x0A\xA1", then mask: "xx????xxx"
	//-----------------------------------------------------------------------------

	virtual void *FindPattern( HMODULE hModule, const char *pszPattern, char *pszMask, unsigned int offset = 0 ) = 0;
	virtual void *FindPatternWithin( HMODULE hModule, const char *pszPattern, char *pszMask, void *pSearchStart, void *pSearchEnd ) = 0;

	//-----------------------------------------------------------------------------
	// Find signature from string but ignore a specific byte
	// If signature: "\xD9\x1D\x2A\x2A\x2A\x2A\x75\x0A\xA1", then ignore byte can be: '0x2A'
	//-----------------------------------------------------------------------------

	virtual void *FindPattern( HMODULE hModule, const char *pszPattern, unsigned int length, unsigned int offset = 0, char ignoreByte = '\x2A' ) = 0;
	virtual void *FindPatternWithin( HMODULE hModule, const char *pszPattern, unsigned int length, void *pSearchStart, void *pSearchEnd, char ignoreByte = '\x2A' ) = 0;

	//-----------------------------------------------------------------------------
	// Find signature from range of bytes with a specific byte to ignore
	// For example: unsigned char sig[] = { 0xD9, 0x1D, 0x2A, 0x2A, 0x2A, 0x2A, 0x75, 0x0A, 0xA1 };
	//-----------------------------------------------------------------------------

	virtual void *FindPattern( HMODULE hModule, unsigned char *pPattern, unsigned int length, unsigned int offset = 0, unsigned char ignoreByte = 0x2A ) = 0;
	virtual void *FindPatternWithin( HMODULE hModule, unsigned char *pPattern, unsigned int length, void *pSearchStart, void *pSearchEnd, unsigned char ignoreByte = 0x2A ) = 0;

	//-----------------------------------------------------------------------------
	// Lookup for a string
	//-----------------------------------------------------------------------------

	virtual void *FindString( HMODULE hModule, const char *pszString, unsigned int offset = 0 ) = 0;
	virtual void *FindStringWithin( HMODULE hModule, const char *pszString, void *pSearchStart, void *pSearchEnd ) = 0;

	//-----------------------------------------------------------------------------
	// Lookup for an address
	//-----------------------------------------------------------------------------

	virtual void *FindAddress( HMODULE hModule, void *pAddress, unsigned int offset = 0 ) = 0;
	virtual void *FindAddressWithin( HMODULE hModule, void *pAddress, void *pSearchStart, void *pSearchEnd ) = 0;

	//-----------------------------------------------------------------------------
	// Wrappers
	//-----------------------------------------------------------------------------

	inline void *FindPattern( HMODULE hModule, pattern_reg_t *patterns, int *patternIndex )
	{
		for ( int i = 0; patterns[ i ].name != NULL; i++ )
		{
			void *pAddress = NULL;
			pattern_t *pattern = patterns[ i ].pattern;

			if ( ( pAddress = FindPattern( hModule, pattern ) ) != NULL )
			{
				if ( patternIndex != NULL )
					*patternIndex = i;

				return pAddress;
			}
		}

		return NULL;
	}

	inline std::future<void *> FindPatternAsync( HMODULE hModule, pattern_t *pPattern, unsigned int offset = 0 )
	{
		IMemoryUtils *pMemoryUtils = this;
		return std::async( [pMemoryUtils, hModule, pPattern, offset] { return pMemoryUtils->FindPattern( hModule, pPattern, offset ); } );
	}

	inline void FindPatternAsync( HMODULE hModule, pattern_reg_t *patterns, std::vector<std::future<void *>> &futures )
	{
		// Maybe 4 as minimum
		futures.reserve( 4 );

		IMemoryUtils *pMemoryUtils = this;

		for ( int i = 0; patterns[ i ].name != NULL; i++ )
		{
			pattern_t *pattern = patterns[ i ].pattern;
			futures.push_back( std::async( [pMemoryUtils, hModule, pattern] { return pMemoryUtils->FindPattern( hModule, pattern ); } ) );
		}
	}

	inline void *GetPatternFutureValue( std::vector<std::future<void *>> &futures, int *patternIndex )
	{
		void *pAddress = NULL;

		for ( size_t i = 0; i < futures.size(); i++ )
		{
			void *adr = futures[ i ].get();

			if ( pAddress == NULL && adr != NULL )
			{
				if ( patternIndex != NULL )
					*patternIndex = i;

				pAddress = adr;
			}
		}

		return pAddress;
	}
};

#define MEMORYUTILS_INTERFACE_VERSION "MemoryUtils002"

extern IMemoryUtils *g_pMemoryUtils;
PLATFORM_INTERFACE IMemoryUtils *MemoryUtils();

#endif // IMEMORYUTILS_H