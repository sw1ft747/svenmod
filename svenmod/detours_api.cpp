#pragma warning(disable : 26812)

#include <vector>

#include <IDetoursAPI.h>
#include <IMemoryUtils.h>

#include <sys.h>

#include <udis86/udis86.h>
#include <data_struct/hashtable.h>

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <tlhelp32.h>
#else
#include <IMemoryUtils.h>
#endif

#define FOR_EACH_DETOUR(iter) for (unsigned int iter = 0; iter < m_detours.size(); iter++)

class CDetourFunction;
class CDetourContext;

typedef enum
{
	DETOUR_FUNCTION = 0,
	DETOUR_VTABLE_FUNCTION,
	DETOUR_IAT_FUNCTION
} DETOUR_TYPE;

extern bool g_bAutoPauseDetours;

//-----------------------------------------------------------------------------
// CDetoursAPI
//-----------------------------------------------------------------------------

class CDetoursAPI : public IDetoursAPI
{
	friend class CDetourContext;

public:
	CDetoursAPI();
	virtual ~CDetoursAPI();

	virtual void Init();

	virtual DetourHandle_t DetourFunction(void *pFunction, void *pDetourFunction, void **ppOriginalFunction, bool bPause = false, int iDisasmMinBytes = 5);

	virtual DetourHandle_t DetourFunctionByName(HMODULE hModule, const char *pszFunctionName, void *pDetourFunction, void **ppOriginalFunction, bool bPause = false, int iDisasmMinBytes = 5);

	virtual DetourHandle_t DetourVirtualFunction(void *pClassInstance, int nFunctionIndex, void *pDetourFunction, void **ppOriginalFunction, bool bPause = false);
	
	virtual bool PauseDetour(DetourHandle_t hDetour);

	virtual bool UnpauseDetour(DetourHandle_t hDetour);

	virtual bool RemoveDetour(DetourHandle_t hDetour);

	virtual bool PauseAllDetours();

	virtual bool UnpauseAllDetours();

private:
	DetourHandle_t CreateDetour(DETOUR_TYPE type, void *pFunction, void *pDetourFunction, void **ppOriginalFunction, bool bPause, int iDisasmMinBytes);
	DetourHandle_t AllocateDetourHandle();

	void SuspendThreads();
	void ResumeThreads();

private:
	CHashTable<DetourHandle_t, CDetourFunction *> m_DetoursTable;
	CHashTable<void *, CDetourContext *> m_ContextsTable;

#ifdef PLATFORM_WINDOWS
	std::vector<DWORD> m_SuspendedThreads;
	DWORD m_dwCurrentThreadID;
	DWORD m_dwCurrentProcessID;
#endif

	int m_iDetourHandles;
};

//-----------------------------------------------------------------------------
// CDetourFunction
//-----------------------------------------------------------------------------

class CDetourFunction
{
	friend class CDetourContext;

public:
	CDetourFunction(DetourHandle_t handle, CDetourContext *pContext, void *pDetour, void **ppTrampoline);

	bool IsPaused() const;
	void *GetDetour() const;
	CDetourContext *GetContext() const;

	void SetTrampoline(void *pTrampoline);

public:
	bool operator==(DetourHandle_t handle) const;

private:
	DetourHandle_t m_handle;
	CDetourContext *m_pContext;

	void *m_pDetour;
	void **m_ppTrampoline;

	bool m_bPaused;
};

//-----------------------------------------------------------------------------
// CDetourContext
//-----------------------------------------------------------------------------

class CDetourContext
{
public:
	CDetourContext(DETOUR_TYPE type, void *pDetourTarget);
	~CDetourContext();

	bool Init(int iDisasmMinBytes);
	
	// Pause all detours
	bool Pause();
	bool Unpause();

	void InstallDetours();
	void RemoveDetours();

public:
	DETOUR_TYPE GetType() const;

	bool HasDetours() const;
	void *GetDetourTarget() const;

public:
	CDetourFunction *FindDetourByFunction(void *pDetourFunction);

	CDetourFunction *AddDetour(DetourHandle_t hDetour, void *pDetourFunction, void **ppTrampoline, bool bPaused);
	bool RemoveDetour(DetourHandle_t hDetour);
	
	bool PauseDetour(DetourHandle_t hDetour);
	bool UnpauseDetour(DetourHandle_t hDetour);

private:
	DETOUR_TYPE m_type;
	std::vector<CDetourFunction *> m_detours;

	bool m_bDetoursAttached;

	void *m_pFunction;
	void *m_pGateway;

	int m_nStolenBytes;
	unsigned char *m_pOriginalBytes;
	unsigned char *m_pPatchedBytes;
};

//-----------------------------------------------------------------------------
// CDetoursAPI implementation
//-----------------------------------------------------------------------------

CDetoursAPI::CDetoursAPI() : m_DetoursTable(48), m_ContextsTable(48)
{
	m_iDetourHandles = 0;

	m_dwCurrentThreadID = 0;
	m_dwCurrentProcessID = 0;
}

CDetoursAPI::~CDetoursAPI()
{
	for (int i = 0; i < m_ContextsTable.Count(); i++)
	{
		HashTableIterator_t it = m_ContextsTable.First(i);

		while ( m_ContextsTable.IsValidIterator(it) )
		{
			delete m_ContextsTable.ValueAt(i, it);
			it = m_ContextsTable.Next(i, it);
		}
	}

	m_DetoursTable.Purge();
	m_ContextsTable.Purge();
}

void CDetoursAPI::Init()
{
#ifdef PLATFORM_WINDOWS
	m_dwCurrentThreadID = GetCurrentThreadId();
	m_dwCurrentProcessID = GetCurrentProcessId();
#endif
}

DetourHandle_t CDetoursAPI::DetourFunction(void *pFunction, void *pDetourFunction, void **ppOriginalFunction, bool bPause /* = false */, int iDisasmMinBytes /* = 5 */)
{
	return CreateDetour(DETOUR_FUNCTION, pFunction, pDetourFunction, ppOriginalFunction, bPause, iDisasmMinBytes);
}

DetourHandle_t CDetoursAPI::DetourFunctionByName(HMODULE hModule, const char *pszFunctionName, void *pDetourFunction, void **ppOriginalFunction, bool bPause /* = false */, int iDisasmMinBytes /* = 5 */)
{
#ifdef PLATFORM_WINDOWS
	void *pFunction = Sys_GetProcAddress(hModule, pszFunctionName);
#else
	void *pFunction = MemoryUtils()->ResolveSymbol(hModule, pszFunctionName);
#endif

	if ( !pFunction )
		return DETOUR_INVALID_HANDLE;

	return CreateDetour(DETOUR_FUNCTION, pFunction, pDetourFunction, ppOriginalFunction, bPause, iDisasmMinBytes);
}

DetourHandle_t CDetoursAPI::DetourVirtualFunction(void *pClassInstance, int nFunctionIndex, void *pDetourFunction, void **ppOriginalFunction, bool bPause /* = false */)
{
	void *pVTable = *static_cast<void **>(pClassInstance);

	if ( !pVTable )
		return DETOUR_INVALID_HANDLE;

	void *pFunction = (void *)((unsigned long *)pVTable + nFunctionIndex);

	return CreateDetour(DETOUR_VTABLE_FUNCTION, pFunction, pDetourFunction, ppOriginalFunction, bPause, 5);
}

bool CDetoursAPI::PauseDetour(DetourHandle_t hDetour)
{
	CDetourFunction **ppDetour = m_DetoursTable.Find(hDetour);

	if (ppDetour)
	{
		return (*ppDetour)->GetContext()->PauseDetour(hDetour);
	}

	return false;
}

bool CDetoursAPI::UnpauseDetour(DetourHandle_t hDetour)
{
	CDetourFunction **ppDetour = m_DetoursTable.Find(hDetour);

	if (ppDetour)
	{
		return (*ppDetour)->GetContext()->UnpauseDetour(hDetour);
	}

	return false;
}

bool CDetoursAPI::RemoveDetour(DetourHandle_t hDetour)
{
	CDetourFunction **ppDetour = m_DetoursTable.Find(hDetour);

	if (ppDetour)
	{
		CDetourContext *pContext = (*ppDetour)->GetContext();
		bool bRemoved = pContext->RemoveDetour(hDetour);

		if (bRemoved)
		{
			m_DetoursTable.Remove(hDetour);

			if ( !pContext->HasDetours() )
			{
				m_ContextsTable.Remove( pContext->GetDetourTarget() );
				delete pContext;
			}

			return true;
		}
	}

	return false;
}

bool CDetoursAPI::PauseAllDetours()
{
	bool bPaused = false;

	for (int i = 0; i < m_ContextsTable.Count(); i++)
	{
		HashTableIterator_t it = m_ContextsTable.First(i);

		while ( m_ContextsTable.IsValidIterator(it) )
		{
			bPaused = m_ContextsTable.ValueAt(i, it)->Pause() || bPaused;
			it = m_ContextsTable.Next(i, it);
		}
	}

	return bPaused;
}

bool CDetoursAPI::UnpauseAllDetours()
{
	bool bUnpaused = false;

	for (int i = 0; i < m_ContextsTable.Count(); i++)
	{
		HashTableIterator_t it = m_ContextsTable.First(i);

		while ( m_ContextsTable.IsValidIterator(it) )
		{
			bUnpaused = m_ContextsTable.ValueAt(i, it)->Unpause() || bUnpaused;
			it = m_ContextsTable.Next(i, it);
		}
	}

	return bUnpaused;
}

DetourHandle_t CDetoursAPI::CreateDetour(DETOUR_TYPE type, void *pFunction, void *pDetourFunction, void **ppOriginalFunction, bool bPause, int iDisasmMinBytes)
{
	if ( g_bAutoPauseDetours )
		bPause = false;

	if ( iDisasmMinBytes < 5 )
	{
		iDisasmMinBytes = 5;
	}

	CDetourContext *pContext = NULL;
	CDetourContext **pContextEntry = m_ContextsTable.Find(pFunction);

	if ( pContextEntry )
	{
		pContext = *pContextEntry;

		if ( !pContext->FindDetourByFunction(pDetourFunction) )
		{
			CDetourFunction *pDetour = NULL;
			DetourHandle_t hDetour = AllocateDetourHandle();

			if ( pDetour = pContext->AddDetour( hDetour, pDetourFunction, ppOriginalFunction, bPause ) )
			{
				m_DetoursTable.Insert( hDetour, pDetour );
				return hDetour;
			}
		}
	}
	else
	{
		pContext = new CDetourContext(type, pFunction);

		if ( !pContext->Init( iDisasmMinBytes ) )
		{
			delete pContext;
		}
		else
		{
			CDetourFunction *pDetour = NULL;
			DetourHandle_t hDetour = AllocateDetourHandle();

			m_ContextsTable.Insert( pFunction, pContext );
			pDetour = pContext->AddDetour( hDetour, pDetourFunction, ppOriginalFunction, bPause );

			if (pDetour)
			{
				m_DetoursTable.Insert( hDetour, pDetour );
				return hDetour;
			}
		}
	}

	return DETOUR_INVALID_HANDLE;
}

DetourHandle_t CDetoursAPI::AllocateDetourHandle()
{
	return m_iDetourHandles++;
}

void CDetoursAPI::SuspendThreads()
{
#ifdef PLATFORM_WINDOWS
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 te;
		te.dwSize = sizeof(te);

		if (Thread32First(hSnapshot, &te))
		{
			do
			{
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
				{
					if (te.th32OwnerProcessID == m_dwCurrentProcessID && te.th32ThreadID != m_dwCurrentThreadID)
					{
						HANDLE hThread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);

						if (hThread != NULL)
						{
							m_SuspendedThreads.push_back(te.th32ThreadID);

							SuspendThread(hThread);
							CloseHandle(hThread);
						}
					}
				}
				te.dwSize = sizeof(te);

			} while (Thread32Next(hSnapshot, &te));
		}

		CloseHandle(hSnapshot);
	}
#endif
}

void CDetoursAPI::ResumeThreads()
{
#ifdef PLATFORM_WINDOWS
	for (int i = m_SuspendedThreads.size() - 1; i >= 0; --i)
	{
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, m_SuspendedThreads[i]);

		if (hThread != NULL)
		{
			ResumeThread(hThread);
			CloseHandle(hThread);
		}
	}

	m_SuspendedThreads.clear();
#endif
}

//-----------------------------------------------------------------------------
// Export Detours API
//-----------------------------------------------------------------------------

CDetoursAPI g_DetoursAPI;
IDetoursAPI *g_pDetoursAPI = &g_DetoursAPI;

IDetoursAPI *DetoursAPI()
{
	return g_pDetoursAPI;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CDetoursAPI, IDetoursAPI, DETOURS_API_INTERFACE_VERSION, g_DetoursAPI);

//-----------------------------------------------------------------------------
// CDetourFunction implementation
//-----------------------------------------------------------------------------

CDetourFunction::CDetourFunction(DetourHandle_t handle, CDetourContext *pContext, void *pDetour, void **ppTrampoline)
{
	m_handle = handle;

	m_pContext = pContext;
	m_pDetour = pDetour;
	m_ppTrampoline = ppTrampoline;

	m_bPaused = false;
}

inline CDetourContext *CDetourFunction::GetContext() const
{
	return m_pContext;
}

inline void *CDetourFunction::GetDetour() const
{
	return m_pDetour;
}

inline void CDetourFunction::SetTrampoline(void *pTrampoline)
{
	*m_ppTrampoline = pTrampoline;
}

inline bool CDetourFunction::IsPaused() const
{
	return m_bPaused;
}

bool CDetourFunction::operator==(DetourHandle_t handle) const
{
	return m_handle == handle;
}

//-----------------------------------------------------------------------------
// CDetourContext implementation
//-----------------------------------------------------------------------------

CDetourContext::CDetourContext(DETOUR_TYPE type, void *pDetourTarget)
{
	m_type = type;

	m_bDetoursAttached = false;

	m_pFunction = pDetourTarget;
	m_pGateway = NULL;

	// DETOUR_FUNCTION
	m_nStolenBytes = 0;
	m_pOriginalBytes = NULL;
	m_pPatchedBytes = NULL;
}

CDetourContext::~CDetourContext()
{
	RemoveDetours();

	FOR_EACH_DETOUR(i)
	{
		CDetourFunction *pDetour = m_detours[i];

		if ( m_type == DETOUR_VTABLE_FUNCTION )
			pDetour->SetTrampoline( m_pGateway );
		else
			pDetour->SetTrampoline( m_pFunction );

		delete pDetour;
	}

	if (m_type == DETOUR_FUNCTION)
	{
		if (m_pOriginalBytes)
		{
			free((void *)m_pOriginalBytes);
			m_pOriginalBytes = m_pPatchedBytes = NULL;
		}

		if (m_pGateway)
		{
			MemoryUtils()->VirtualFree(m_pGateway, 0, MEM_RELEASE);
			m_pGateway = NULL;
		}
	}

	m_detours.clear();
}

bool CDetourContext::Init(int iDisasmMinBytes)
{
	if (m_type == DETOUR_FUNCTION)
	{
		std::vector<void *> vCalleeFunctions;
		m_nStolenBytes = 0;

		ud_t instruction;
		unsigned char *buffer = (unsigned char *)m_pFunction;

		ud_init(&instruction);
		ud_set_mode(&instruction, 32);
		ud_set_input_buffer(&instruction, (const uint8_t *)buffer, iDisasmMinBytes <= 15 ? 15 : iDisasmMinBytes); // 15 - longest x86 instruction

		// Disassemble for JMP opcode
		do
		{
			int disassembledBytes = ud_disassemble(&instruction);

			// Save callee functions, then we can calc their relative addresses in gateway
			if (instruction.mnemonic == UD_Icall || instruction.mnemonic == UD_Ijmp)
			{
				unsigned long luRelativeAddress = *(unsigned long *)(buffer + 1);
				void *pAbsoluteAddress = (void *)(luRelativeAddress + (unsigned long)buffer + sizeof(void *) + 1);

				vCalleeFunctions.push_back( pAbsoluteAddress );
			}

			buffer += disassembledBytes;
			m_nStolenBytes += disassembledBytes;

		} while (m_nStolenBytes < iDisasmMinBytes);

		int nNOPs = m_nStolenBytes - 5;

		// Allocate needed bytes once
		m_pOriginalBytes = (unsigned char *)malloc(m_nStolenBytes * 2);

		if ( !m_pOriginalBytes )
		{
			vCalleeFunctions.clear();
			return false;
		}

		m_pPatchedBytes = m_pOriginalBytes + m_nStolenBytes;

		memcpy(m_pOriginalBytes, m_pFunction, m_nStolenBytes);
		*m_pPatchedBytes = 0xE9; // JMP opcode; relative address will be calculated later

		// NOP the rest opcodes, just for safe
		if ( nNOPs )
			memset(m_pPatchedBytes + 5, 0x90, nNOPs);

		// Initializaing the gateway

		// Alloc size: m_nStolenBytes + 5 ( JMP [relative offset] )
		unsigned char callBytes[5] = { 0xE9 };
		m_pGateway = MemoryUtils()->VirtualAlloc(NULL, m_nStolenBytes + sizeof(callBytes), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if ( !m_pGateway )
		{
			vCalleeFunctions.clear();
			return false;
		}

		memcpy(m_pGateway, m_pOriginalBytes, m_nStolenBytes);

		// Write relative addresses to CALL opcodes
		if ( vCalleeFunctions.size() > 0 )
		{
			int iter = 0;
			int disassembledBytes = 0;

			buffer = (unsigned char *)m_pGateway;

			ud_init(&instruction);
			ud_set_mode(&instruction, 32);
			ud_set_input_buffer(&instruction, (const uint8_t *)buffer, m_nStolenBytes);

			do
			{
				if (instruction.mnemonic == UD_Icall || instruction.mnemonic == UD_Ijmp)
				{
					*(unsigned long *)(buffer + 1) = ((unsigned long)vCalleeFunctions[iter] - ((unsigned long)buffer + sizeof(void *) + 1));
					++iter;
				}

				buffer += disassembledBytes;

			} while ( disassembledBytes = ud_disassemble(&instruction) );
		}

		// Calc relative address to jump back to original function
		*(unsigned long *)(callBytes + 1) = ((unsigned long)m_pFunction + m_nStolenBytes) - (((unsigned long)m_pGateway + m_nStolenBytes) + sizeof(void *) + 1);

		// And copy it
		memcpy((unsigned char *)m_pGateway + m_nStolenBytes, callBytes, sizeof(callBytes));

		vCalleeFunctions.clear();
		return true;
	}
	else if (m_type == DETOUR_VTABLE_FUNCTION)
	{
		m_pGateway = *static_cast<void **>(m_pFunction);
		return true;
	}

	return false;
}

bool CDetourContext::Pause()
{
	bool bPaused = false;
	
	FOR_EACH_DETOUR(i)
	{
		CDetourFunction *pDetour = m_detours[i];

		if ( !pDetour->IsPaused() )
		{
			bPaused = true;
			pDetour->m_bPaused = true;
		}
	}
	
	if ( bPaused )
	{
		InstallDetours();
	}

	return bPaused;
}

bool CDetourContext::Unpause()
{
	bool bUnpaused = false;
	
	FOR_EACH_DETOUR(i)
	{
		CDetourFunction *pDetour = m_detours[i];

		if ( pDetour->IsPaused() )
		{
			bUnpaused = true;
			pDetour->m_bPaused = false;
		}
	}
	
	if ( bUnpaused )
	{
		InstallDetours();
	}

	return bUnpaused;
}

void CDetourContext::InstallDetours()
{
	CDetourFunction *pFirstDetour = NULL;
	CDetourFunction *pLastDetour = NULL;

	FOR_EACH_DETOUR(i)
	{
		CDetourFunction *pDetour = m_detours[i];

		if ( !pDetour->IsPaused() )
		{
			if ( !pFirstDetour )
				pFirstDetour = pDetour;

			if ( pLastDetour )
			{
				pLastDetour->SetTrampoline( pDetour->GetDetour() );
			}

			pLastDetour = pDetour;
		}
	}

	if ( !pFirstDetour )
	{
		RemoveDetours();
		return;
	}

	int dwProtection;

	if (m_type == DETOUR_FUNCTION)
	{
		g_DetoursAPI.SuspendThreads();

		// Relative address
		*(unsigned long *)(m_pPatchedBytes + 1) = (unsigned long)pFirstDetour->GetDetour() - ((unsigned long)m_pFunction + sizeof(void *) + 1);

		MemoryUtils()->VirtualProtect(m_pFunction, m_nStolenBytes, PAGE_EXECUTE_READWRITE, &dwProtection);

		memcpy(m_pFunction, m_pPatchedBytes, m_nStolenBytes);

		MemoryUtils()->VirtualProtect(m_pFunction, m_nStolenBytes, dwProtection, NULL);

		g_DetoursAPI.ResumeThreads();
	}
	else if (m_type == DETOUR_VTABLE_FUNCTION)
	{
		MemoryUtils()->VirtualProtect(m_pFunction, sizeof(void *), PAGE_EXECUTE_READWRITE, &dwProtection);

		*(void **)(m_pFunction) = pFirstDetour->GetDetour();

		MemoryUtils()->VirtualProtect(m_pFunction, sizeof(void *), dwProtection, NULL);
	}

	pLastDetour->SetTrampoline( m_pGateway );
	m_bDetoursAttached = true;
}

void CDetourContext::RemoveDetours()
{
	if ( !m_bDetoursAttached )
		return;

	int dwProtection;

	if (m_type == DETOUR_FUNCTION)
	{
		g_DetoursAPI.SuspendThreads();

		MemoryUtils()->VirtualProtect(m_pFunction, m_nStolenBytes, PAGE_EXECUTE_READWRITE, &dwProtection);

		memcpy(m_pFunction, m_pOriginalBytes, m_nStolenBytes);

		MemoryUtils()->VirtualProtect(m_pFunction, m_nStolenBytes, dwProtection, NULL);

		g_DetoursAPI.ResumeThreads();
	}
	else if (m_type == DETOUR_VTABLE_FUNCTION)
	{
		MemoryUtils()->VirtualProtect(m_pFunction, sizeof(void *), PAGE_EXECUTE_READWRITE, &dwProtection);

		*(void **)(m_pFunction) = m_pGateway;

		MemoryUtils()->VirtualProtect(m_pFunction, sizeof(void *), dwProtection, NULL);
	}

	if (m_type == DETOUR_VTABLE_FUNCTION)
	{
		FOR_EACH_DETOUR(i)
		{
			CDetourFunction *pDetour = m_detours[i];
			pDetour->SetTrampoline(m_pGateway);
		}
	}
	else
	{
		FOR_EACH_DETOUR(i)
		{
			CDetourFunction *pDetour = m_detours[i];
			pDetour->SetTrampoline(m_pFunction);
		}
	}

	m_bDetoursAttached = false;
}

inline bool CDetourContext::HasDetours() const
{
	return m_detours.size() > 0;
}

inline void *CDetourContext::GetDetourTarget() const
{
	return m_pFunction;
}

inline DETOUR_TYPE CDetourContext::GetType() const
{
	return m_type;
}

CDetourFunction *CDetourContext::FindDetourByFunction(void *pDetourFunction)
{
	FOR_EACH_DETOUR(i)
	{
		CDetourFunction *pDetour = m_detours[i];

		if ( pDetour->GetDetour() == pDetourFunction )
			return pDetour;
	}

	return NULL;
}

CDetourFunction *CDetourContext::AddDetour(DetourHandle_t hDetour, void *pDetourFunction, void **ppTrampoline, bool bPaused)
{
	CDetourFunction *pDetour = new CDetourFunction(hDetour, this, pDetourFunction, ppTrampoline);

	if ( !pDetour )
		return NULL;

	m_detours.push_back(pDetour);

	if (bPaused)
		pDetour->m_bPaused = true;
	else
		InstallDetours();

	return pDetour;
}

bool CDetourContext::RemoveDetour(DetourHandle_t hDetour)
{
	FOR_EACH_DETOUR(i)
	{
		CDetourFunction *pDetour = m_detours[i];

		if ( *pDetour == hDetour )
		{
			bool bPaused = pDetour->IsPaused();

			delete pDetour;
			m_detours.erase(m_detours.begin() + i);

			if ( !bPaused )
				InstallDetours();

			return true;
		}
	}

	return false;
}

bool CDetourContext::PauseDetour(DetourHandle_t hDetour)
{
	FOR_EACH_DETOUR(i)
	{
		CDetourFunction *pDetour = m_detours[i];

		if ( *pDetour == hDetour )
		{
			if ( pDetour->IsPaused() )
				return false;

			pDetour->m_bPaused = true;
			InstallDetours();

			return true;
		}
	}

	return false;
}

bool CDetourContext::UnpauseDetour(DetourHandle_t hDetour)
{
	FOR_EACH_DETOUR(i)
	{
		CDetourFunction *pDetour = m_detours[i];

		if ( *pDetour == hDetour )
		{
			if ( !pDetour->IsPaused() )
				return false;

			pDetour->m_bPaused = false;
			InstallDetours();

			return true;
		}
	}

	return false;
}
