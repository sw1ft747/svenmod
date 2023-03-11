#include "cvar.h"

#include <string>
#include <algorithm>

#include <gameui/IGameConsole.h>
#include <gameui/gameconsole.h>

#include <dbg.h>
#include <interface.h>
#include <generichash.h>
#include <convar.h>

#include <hl_sdk/engine/APIProxy.h>

#include "gamedata_finder.h"

#define CONSOLE_PRINT_MESSAGE_LENGTH 8192
#define CVAR_USE_DYNAMIC_CAST 0

extern client_version_t g_ClientVersion;

//-----------------------------------------------------------------------------
// Colors
//-----------------------------------------------------------------------------

static const Color s_ConsoleDefaultPrintColor = { 240, 240, 240, 250 };
static const Color s_MsgPrintColor = { 232, 232, 232, 255 };
static const Color s_WarningPrintColor = { 255, 90, 90, 255 };

//-----------------------------------------------------------------------------
// CCvar implementation
//-----------------------------------------------------------------------------

CCvar::CCvar()
{
	m_nNextDLLIdentifier = 0;

	Z_Free = NULL;
	Cvar_DirectSet = NULL;

	RichText__InsertColorChange = NULL;
	RichText__InsertString = NULL;

	m_pGameConsole = NULL;

	m_ppCvarList = NULL;
	m_ppCmdList = NULL;

	m_pDeveloper = NULL;

	m_pArgC = NULL;
	m_ppArgV = NULL;

	m_bInitialized = false;
	m_bCanPrint = false;
}

CCvar::~CCvar()
{
	Shutdown();
}

bool CCvar::Init()
{
	if ( m_bInitialized )
		return true;

	if ( g_ClientVersion.version > 525 )
		LogWarning("Developer: Validate paddings of class \"CGameConsoleDialog\"");

	void *pfnRichText__InsertColorChange = NULL;
	void *pfnRichText__InsertString = NULL;

	m_nNextDLLIdentifier = 0;

	Z_Free = (Z_FreeFn)g_GameDataFinder.FindZ_Free(); // ToDo: just take it from Quake?
	Cvar_DirectSet = (Cvar_DirectSetFn)g_GameDataFinder.FindCvar_DirectSet();

	g_GameDataFinder.FindCvarList( &m_ppCvarList );
	g_GameDataFinder.FindCmdList( &m_ppCmdList );
	g_GameDataFinder.FindCommandArgs( &m_pArgC, &m_ppArgV );

	g_GameDataFinder.FindConsolePrint( &pfnRichText__InsertColorChange, &pfnRichText__InsertString );

	RichText__InsertColorChange = (RichText__InsertColorChangeFn)pfnRichText__InsertColorChange;
	RichText__InsertString = (RichText__InsertStringFn)pfnRichText__InsertString;

	m_pDeveloper = g_pEngineFuncs->GetCvarPointer("developer");

	if ( !m_pDeveloper )
	{
		Sys_Error("[SvenMod] Can't get \"developer\" cvar..");
		return false;
	}

	m_pGameConsole = (CGameConsole *)g_pGameConsole;

	m_bInitialized = true;
	m_bCanPrint = true;

	return true;
}

void CCvar::Shutdown()
{
	if ( !m_bInitialized )
		return;

	for (int i = 0; i < m_CommandHash.Size(); i++)
	{
		CConCommandHash::datapool_t &bucket = m_CommandHash.m_Buckets[i];

		for (size_t j = 0; j < bucket.size(); j++)
		{
			ConCommandBase *pCommandBase = bucket[j];

			if ( pCommandBase->IsRegistered() )
			{
				bool bFound = false;

				if ( pCommandBase->IsCommand() )
				{
				#if CVAR_USE_DYNAMIC_CAST
					ConCommand *pCommand = dynamic_cast<ConCommand *>(pCommandBase);

					if ( !pCommand )
					{
						Warning("[SvenMod] Can't cast to ConVar, invalid ConCommandBase\n");
						continue;
					}
				#else
					ConCommand *pCommand = reinterpret_cast<ConCommand *>(pCommandBase);
				#endif

					cmd_t *pPrev = NULL;
					cmd_t *pCmd = *m_ppCmdList;

					while (pCmd)
					{
						if ( !stricmp(pCmd->name, pCommand->GetName()) )
						{
							if (pPrev)
							{
								pPrev->next = pCmd->next;
							}
							else
							{
							#pragma warning(push)
							#pragma warning(disable : 6001)

								*m_ppCmdList = pCmd->next;

							#pragma warning(pop)
							}

							bFound = true;
							break;
						}

						pPrev = pCmd;
						pCmd = pCmd->next;
					}

					if ( bFound && pCmd )
						free( (void *)pCmd );

					pCommand->m_pCommand = NULL;
				}
				else
				{
				#if CVAR_USE_DYNAMIC_CAST
					ConVar *pConVar = dynamic_cast<ConVar *>(pCommandBase);

					if ( !pConVar )
					{
						Warning("[SvenMod] Can't cast to ConCommand, invalid ConCommandBase\n");
						continue;
					}
				#else
					ConVar *pConVar = reinterpret_cast<ConVar *>(pCommandBase);
				#endif

					cvar_t *pPrev = NULL;
					cvar_t *pCvar = *m_ppCvarList;

					while (pCvar)
					{
						if ( !stricmp(pCvar->name, pConVar->GetName()) )
						{
							if (pPrev)
							{
								pPrev->next = pCvar->next;
							}
							else
							{
								*m_ppCvarList = pCvar->next;
							}

							bFound = true;
							break;
						}

						pPrev = pCvar;
						pCvar = pCvar->next;
					}

					if ( bFound && pCvar )
					{
						Z_Free( (void *)pCvar->string );
						Z_Free( (void *)pCvar );
					}

					pConVar->m_pCvar = NULL;
				}
			}

			pCommandBase->m_bRegistered = false;
		}
	}

	m_nNextDLLIdentifier = 0;
	m_CommandHash.RemoveAll();

	m_bInitialized = false;
}

void CCvar::EnablePrint()
{
	m_bCanPrint = true;
}

void CCvar::DisablePrint()
{
	m_bCanPrint = false;
}

void CCvar::PrintCvars(int mode, const char *pszPrefix) const
{
	int iCount = 0;
	int iPrefixLength = 0;

	if ( pszPrefix )
		iPrefixLength = strlen(pszPrefix);

	ConsolePrint("----------------------------------\n");

#if CVAR_USE_DYNAMIC_CAST
	// FIXME: use binary tree to sort the cvars

	for (int i = 0; i < m_CommandHash.Size(); i++)
	{
		const CConCommandHash::datapool_t &bucket = m_CommandHash.m_Buckets[i];

		for (size_t j = 0; j < bucket.size(); j++)
		{
			ConCommandBase *pCommandBase = bucket[j];

			if ( pszPrefix && strncmp(pszPrefix, pCommandBase->GetName(), iPrefixLength) )
				continue;

			switch (mode)
			{
			case 0:
				ConsolePrintf("%s\n", pCommandBase->GetName());
				iCount++;
				break;

			case 1:
				if ( !pCommandBase->IsCommand() )
				{
					ConsolePrintf("%s\n", pCommandBase->GetName());
					iCount++;
				}
				break;

			case 2:
				if ( pCommandBase->IsCommand() )
				{
					ConsolePrintf("%s\n", pCommandBase->GetName());
					iCount++;
				}
				break;
			}
		}
	}
#else
	std::vector<std::string> vCommands;

	for (int i = 0; i < m_CommandHash.Size(); i++)
	{
		const CConCommandHash::datapool_t &bucket = m_CommandHash.m_Buckets[i];

		for (size_t j = 0; j < bucket.size(); j++)
		{
			ConCommandBase *pCommandBase = bucket[j];

			if ( pszPrefix && strncmp(pszPrefix, pCommandBase->GetName(), iPrefixLength) )
				continue;

			switch (mode)
			{
			case 0:
				vCommands.push_back( pCommandBase->GetName() );
				iCount++;
				break;

			case 1:
				if ( !pCommandBase->IsCommand() )
				{
					vCommands.push_back( pCommandBase->GetName() );
					iCount++;
				}
				break;

			case 2:
				if ( pCommandBase->IsCommand() )
				{
					vCommands.push_back( pCommandBase->GetName() );
					iCount++;
				}
				break;
			}
		}
	}

	std::sort( vCommands.begin(), vCommands.end() );

	for (std::string &command : vCommands)
	{
		ConsolePrintf("%s\n", command.c_str());
	}

	vCommands.clear();
#endif

	ConsolePrint("----------------------------------\n");

	switch (mode)
	{
	case 0:
		ConsolePrintf("%d Total CVar%s/Command%s\n", iCount, iCount == 1 ? "" : "s", iCount == 1 ? "" : "s");
		break;

	case 1:
		ConsolePrintf("%d Total CVar%s\n", iCount, iCount == 1 ? "" : "s");
		break;

	case 2:
		ConsolePrintf("%d Total Command%s\n", iCount, iCount == 1 ? "" : "s");
		break;
	}
}

CVarDLLIdentifier_t CCvar::AllocateDLLIdentifier()
{
	return m_nNextDLLIdentifier++;
}

//-----------------------------------------------------------------------------
// Register/unregister
//-----------------------------------------------------------------------------

void CCvar::RegisterConCommand(ConCommandBase *pCommandBase)
{
	if ( pCommandBase->IsRegistered() )
		return;

	if ( m_CommandHash.Insert(pCommandBase) )
	{
		if ( pCommandBase->IsCommand() )
		{
		#if CVAR_USE_DYNAMIC_CAST
			ConCommand *pCommand = dynamic_cast<ConCommand *>(pCommandBase);

			if ( !pCommand )
			{
				Warning("[SvenMod] Can't cast to ConCommand, invalid ConCommandBase\n");
				m_CommandHash.Remove(pCommandBase);
				return;
			}
		#else
			ConCommand *pCommand = reinterpret_cast<ConCommand *>(pCommandBase);
		#endif

			g_pEngineFuncs->AddCommand( pCommand->GetName(), pCommand->m_pfnCallback );
			pCommand->m_pCommand = FindCmd( pCommand->GetName() );

			if ( pCommand->m_pCommand )
			{
				pCommand->m_pCommand->flags = pCommand->GetFlags();
			}
		}
		else
		{
		#if CVAR_USE_DYNAMIC_CAST
			ConVar *pConVar = dynamic_cast<ConVar *>(pCommandBase);

			if ( !pConVar )
			{
				Warning("[SvenMod] Can't cast to ConVar, invalid ConCommandBase\n");
				m_CommandHash.Remove(pCommandBase);
				return;
			}
		#else
			ConVar *pConVar = reinterpret_cast<ConVar *>(pCommandBase);
		#endif

			pConVar->m_pCvar = g_pEngineFuncs->RegisterVariable( pConVar->GetName(), pConVar->GetDefault(), pConVar->GetFlags() );
		}

		pCommandBase->m_bRegistered = true;
	}
	else
	{
		Warning("[SvenMod] Console %s \"%s\" is already registered!\n", pCommandBase->IsCommand() ? "command" : "variable", pCommandBase->GetName());
	}
}

void CCvar::UnregisterConCommand(ConCommandBase *pCommandBase)
{
	if ( !pCommandBase->IsRegistered() )
		return;

	bool bFound = false;

	if ( pCommandBase->IsCommand() )
	{
	#if CVAR_USE_DYNAMIC_CAST
		ConCommand *pCommand = dynamic_cast<ConCommand *>(pCommandBase);

		if ( !pCommand )
		{
			Warning("[SvenMod] Can't cast to ConVar, invalid ConCommandBase\n");
			return;
		}
	#else
		ConCommand *pCommand = reinterpret_cast<ConCommand *>(pCommandBase);
	#endif

		cmd_t *pPrev = NULL;
		cmd_t *pCmd = *m_ppCmdList;

		while (pCmd)
		{
			if ( !stricmp(pCmd->name, pCommand->GetName()) )
			{
				if ( pPrev )
				{
					pPrev->next = pCmd->next;
				}
				else
				{
					*m_ppCmdList = pCmd->next;
				}

				bFound = true;
				break;
			}

			pPrev = pCmd;
			pCmd = pCmd->next;
		}

		if ( bFound && pCmd )
			free( (void *)pCmd );

		pCommand->m_pCommand = NULL;
	}
	else
	{
	#if CVAR_USE_DYNAMIC_CAST
		ConVar *pConVar = dynamic_cast<ConVar *>(pCommandBase);

		if ( !pConVar )
		{
			Warning("[SvenMod] Can't cast to ConCommand, invalid ConCommandBase\n");
			return;
		}
	#else
		ConVar *pConVar = reinterpret_cast<ConVar *>(pCommandBase);
	#endif

		cvar_t *pPrev = NULL;
		cvar_t *pCvar = *m_ppCvarList;

		while (pCvar)
		{
			if ( !stricmp(pCvar->name, pConVar->GetName()) )
			{
				if ( pPrev )
				{
					pPrev->next = pCvar->next;
				}
				else
				{
					*m_ppCvarList = pCvar->next;
				}

				bFound = true;
				break;
			}

			pPrev = pCvar;
			pCvar = pCvar->next;
		}

		if ( bFound && pCvar )
		{
			Z_Free( (void *)pCvar->string );
			Z_Free( (void *)pCvar );
		}

		pConVar->m_pCvar = NULL;
	}

	m_CommandHash.Remove(pCommandBase);
	pCommandBase->m_bRegistered = false;
}

void CCvar::UnregisterConCommands(CVarDLLIdentifier_t id)
{
	for (int i = 0; i < m_CommandHash.Size(); i++)
	{
		CConCommandHash::datapool_t &bucket = m_CommandHash.m_Buckets[i];

		for (size_t j = 0; j < bucket.size(); j++)
		{
			ConCommandBase *pCommandBase = bucket[j];

			if ( pCommandBase->GetDLLIdentifier() == id )
			{
				if ( pCommandBase->IsRegistered() )
				{
					bool bFound = false;

					if ( pCommandBase->IsCommand() )
					{
					#if CVAR_USE_DYNAMIC_CAST
						ConCommand *pCommand = dynamic_cast<ConCommand *>(pCommandBase);

						if ( !pCommand )
						{
							Warning("[SvenMod] Can't cast to ConVar, invalid ConCommandBase\n");
							continue;
						}
					#else
						ConCommand *pCommand = reinterpret_cast<ConCommand *>(pCommandBase);
					#endif

						cmd_t *pPrev = NULL;
						cmd_t *pCmd = *m_ppCmdList;

						while (pCmd)
						{
							if ( !stricmp(pCmd->name, pCommand->GetName()) )
							{
								if ( pPrev )
								{
									pPrev->next = pCmd->next;
								}
								else
								{
								#pragma warning(push)
								#pragma warning(disable : 6001)

									*m_ppCmdList = pCmd->next;

								#pragma warning(pop)
								}

								bFound = true;
								break;
							}

							pPrev = pCmd;
							pCmd = pCmd->next;
						}

						if ( bFound && pCmd )
							free( (void *)pCmd );

						pCommand->m_pCommand = NULL;
					}
					else
					{
					#if CVAR_USE_DYNAMIC_CAST
						ConVar *pConVar = dynamic_cast<ConVar *>(pCommandBase);

						if ( !pConVar )
						{
							Warning("[SvenMod] Can't cast to ConCommand, invalid ConCommandBase\n");
							continue;
						}
					#else
						ConVar *pConVar = reinterpret_cast<ConVar *>(pCommandBase);
					#endif

						cvar_t *pPrev = NULL;
						cvar_t *pCvar = *m_ppCvarList;

						while (pCvar)
						{
							if ( !stricmp(pCvar->name, pConVar->GetName()) )
							{
								if ( pPrev )
								{
									pPrev->next = pCvar->next;
								}
								else
								{
									*m_ppCvarList = pCvar->next;
								}

								bFound = true;
								break;
							}

							pPrev = pCvar;
							pCvar = pCvar->next;
						}

						if ( bFound && pCvar )
						{
							Z_Free( (void *)pCvar->string );
							Z_Free( (void *)pCvar );
						}

						pConVar->m_pCvar = NULL;
					}
				}

				pCommandBase->m_bRegistered = false;

				bucket.erase(bucket.begin() + j);
				j--;
			}
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

const char *CCvar::GetCommandLineValue(const char *pszVariableName)
{
	int nLen = strlen(pszVariableName);
	char *pSearch = (char *)calloc(nLen + 2, sizeof(char));

	if ( !pSearch )
		return NULL;

	pSearch[0] = '+';
	memcpy(&pSearch[1], pszVariableName, nLen + 1);

	const char *pszValue = CommandLine()->ParmValue(pSearch);

	free( (void *)pSearch );

	return pszValue;
}

//-----------------------------------------------------------------------------
// Find cvar
//-----------------------------------------------------------------------------

cvar_t *CCvar::FindCvar(const char *pszName)
{
	return g_pEngineFuncs->GetCvarPointer(pszName);
}

const cvar_t *CCvar::FindCvar(const char *pszName) const
{
	return const_cast<const cvar_t *>(g_pEngineFuncs->GetCvarPointer(pszName));
}

cmd_t *CCvar::FindCmd(const char *pszName)
{
	cmd_t *pCmd = *m_ppCmdList;

	while (pCmd)
	{
		if ( !stricmp(pCmd->name, pszName) )
		{
			return pCmd;
		}

		pCmd = pCmd->next;
	}

	return NULL;
}

const cmd_t *CCvar::FindCmd(const char *pszName) const
{
	cmd_t *pCmd = *m_ppCmdList;

	while (pCmd)
	{
		if ( !stricmp(pCmd->name, pszName) )
		{
			return const_cast<const cmd_t *>(pCmd);
		}

		pCmd = pCmd->next;
	}

	return NULL;
}

ConCommandBase *CCvar::FindCommandBase(const char *pszName)
{
	return m_CommandHash.Find(pszName);
}

const ConCommandBase *CCvar::FindCommandBase(const char *pszName) const
{
	return const_cast<const ConCommandBase *>(FindCommandBase(pszName));
}

ConVar *CCvar::FindVar(const char *pszName)
{
	ConCommandBase *pCommandBase = m_CommandHash.Find(pszName);

	if ( !pCommandBase->IsCommand() )
	#if CVAR_USE_DYNAMIC_CAST
		return dynamic_cast<ConVar *>(pCommandBase);
	#else
		return reinterpret_cast<ConVar *>(pCommandBase);
	#endif

	return NULL;
}

const ConVar *CCvar::FindVar(const char *pszName) const
{
	return const_cast<const ConVar *>(FindVar(pszName));
}

ConCommand *CCvar::FindCommand(const char *pszName)
{
	ConCommandBase *pCommandBase = m_CommandHash.Find(pszName);

	if ( pCommandBase->IsCommand() )
	#if CVAR_USE_DYNAMIC_CAST
		return dynamic_cast<ConCommand *>(pCommandBase);
	#else
		return reinterpret_cast<ConCommand *>(pCommandBase);
	#endif

	return NULL;
}

const ConCommand *CCvar::FindCommand(const char *pszName) const
{
	return const_cast<const ConCommand *>(FindCommand(pszName));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void CCvar::RevertFlaggedConVars(int nFlag)
{
	for (int i = 0; i < m_CommandHash.Size(); i++)
	{
		CConCommandHash::datapool_t &bucket = m_CommandHash.m_Buckets[i];

		for (size_t j = 0; j < bucket.size(); j++)
		{
		#if CVAR_USE_DYNAMIC_CAST
			ConVar *pConVar = dynamic_cast<ConVar *>(bucket[j]);
		#else
			ConVar *pConVar = reinterpret_cast<ConVar *>(bucket[j]);
		#endif

			if ( pConVar && pConVar->IsFlagSet(nFlag) )
			{
				pConVar->Revert();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Console print
//-----------------------------------------------------------------------------

bool CCvar::CanPrint() const
{
	return m_bCanPrint;
}

void CCvar::ConsoleColorPrint(const Color &clr, const char *pszMessage) const
{
	if ( m_bCanPrint && m_pGameConsole->IsInitialized() )
	{
		CGameConsoleDialog *pGameConsoleDialog = m_pGameConsole->GetGameConsoleDialog();

		RichText__InsertColorChange( pGameConsoleDialog->m_pRichText, clr );
		RichText__InsertString( pGameConsoleDialog->m_pRichText, pszMessage );
	}
}

void CCvar::ConsolePrint(const char *pszMessage) const
{
	if ( m_bCanPrint && m_pGameConsole->IsInitialized() )
	{
		CGameConsoleDialog *pGameConsoleDialog = m_pGameConsole->GetGameConsoleDialog();

		RichText__InsertColorChange( pGameConsoleDialog->m_pRichText, s_ConsoleDefaultPrintColor );
		RichText__InsertString( pGameConsoleDialog->m_pRichText, pszMessage );
	}
}

void CCvar::ConsoleDPrint(const char *pszMessage) const
{
	if ( m_bCanPrint && bool(m_pDeveloper->value) && m_pGameConsole->IsInitialized())
	{
		CGameConsoleDialog *pGameConsoleDialog = m_pGameConsole->GetGameConsoleDialog();

		RichText__InsertColorChange( pGameConsoleDialog->m_pRichText, s_ConsoleDefaultPrintColor );
		RichText__InsertString( pGameConsoleDialog->m_pRichText, pszMessage );
	}
}

void CCvar::ConsoleColorPrintf(const Color &clr, const char *pszFormat, ...) const
{
	if ( m_bCanPrint && m_pGameConsole->IsInitialized() )
	{
		static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

		va_list args;
		va_start(args, pszFormat);
		vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszFormat, args);
		va_end(args);

		szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

		CGameConsoleDialog *pGameConsoleDialog = m_pGameConsole->GetGameConsoleDialog();

		RichText__InsertColorChange( pGameConsoleDialog->m_pRichText, clr );
		RichText__InsertString( pGameConsoleDialog->m_pRichText, szFormattedMsg );
	}
}

void CCvar::ConsolePrintf(const char *pszFormat, ...) const
{
	if ( m_bCanPrint && m_pGameConsole->IsInitialized() )
	{
		static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

		va_list args;
		va_start(args, pszFormat);
		vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszFormat, args);
		va_end(args);

		szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

		CGameConsoleDialog *pGameConsoleDialog = m_pGameConsole->GetGameConsoleDialog();

		RichText__InsertColorChange( pGameConsoleDialog->m_pRichText, s_ConsoleDefaultPrintColor );
		RichText__InsertString( pGameConsoleDialog->m_pRichText, szFormattedMsg );
	}
}

void CCvar::ConsoleDPrintf(const char *pszFormat, ...) const
{
	if ( m_bCanPrint && bool(m_pDeveloper->value) && m_pGameConsole->IsInitialized() )
	{
		static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

		va_list args;
		va_start(args, pszFormat);
		vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszFormat, args);
		va_end(args);

		szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

		CGameConsoleDialog *pGameConsoleDialog = m_pGameConsole->GetGameConsoleDialog();

		RichText__InsertColorChange( pGameConsoleDialog->m_pRichText, s_ConsoleDefaultPrintColor );
		RichText__InsertString( pGameConsoleDialog->m_pRichText, szFormattedMsg );
	}
}

//-----------------------------------------------------------------------------
// Commands args
//-----------------------------------------------------------------------------

int CCvar::ArgC(void) const
{
	return *m_pArgC;
}

const char **CCvar::ArgV(void) const
{
	return m_ppArgV;
}

const char *CCvar::Arg(int nIndex) const
{
	int args = *m_pArgC;

	Assert( nIndex >= 0 && nIndex < args );

	return m_ppArgV[nIndex];
}

//-----------------------------------------------------------------------------
// Set value
//-----------------------------------------------------------------------------

void CCvar::SetValue(cvar_t *pCvar, const char *value)
{
	Cvar_DirectSet(pCvar, value);
}

void CCvar::SetValue(cvar_t *pCvar, float value)
{
	char buffer[32];

	float eps = fabs(value - int(value));

	if (eps >= 0.000001)
	{
		snprintf(buffer, M_ARRAYSIZE(buffer), "%f", eps);
	}
	else
	{
		snprintf(buffer, M_ARRAYSIZE(buffer), "%d", int(value));
	}

	buffer[M_ARRAYSIZE(buffer) - 1] = 0;

	Cvar_DirectSet(pCvar, buffer);
}

void CCvar::SetValue(cvar_t *pCvar, int value)
{
	char buffer[32];

	snprintf(buffer, M_ARRAYSIZE(buffer), "%d", value);
	buffer[M_ARRAYSIZE(buffer) - 1] = 0;

	Cvar_DirectSet(pCvar, buffer);
}

void CCvar::SetValue(cvar_t *pCvar, bool value)
{
	char buffer[32];

	snprintf(buffer, M_ARRAYSIZE(buffer), "%d", int(value));
	buffer[M_ARRAYSIZE(buffer) - 1] = 0;

	Cvar_DirectSet(pCvar, buffer);
}

void CCvar::SetValue(cvar_t *pCvar, Color value)
{
	char buffer[24];

	snprintf(buffer, M_ARRAYSIZE(buffer), "%hhu %hhu %hhu %hhu", value.r, value.g, value.b, value.a);
	buffer[M_ARRAYSIZE(buffer) - 1] = 0;

	SetValue(pCvar, buffer);
}

void CCvar::SetValue(const char *pszCvar, const char *value)
{
	g_pEngineFuncs->Cvar_Set(pszCvar, value);
}

void CCvar::SetValue(const char *pszCvar, float value)
{
	g_pEngineFuncs->Cvar_SetValue(pszCvar, value);
}

void CCvar::SetValue(const char *pszCvar, int value)
{
	g_pEngineFuncs->Cvar_SetValue(pszCvar, float(value));
}

void CCvar::SetValue(const char *pszCvar, bool value)
{
	g_pEngineFuncs->Cvar_SetValue(pszCvar, !!int(value));
}

void CCvar::SetValue(const char *pszCvar, Color value)
{
	char buffer[24];

	snprintf(buffer, M_ARRAYSIZE(buffer), "%hhu %hhu %hhu %hhu", value.r, value.g, value.b, value.a);
	buffer[M_ARRAYSIZE(buffer) - 1] = 0;

	g_pEngineFuncs->Cvar_Set(pszCvar, buffer);
}

//-----------------------------------------------------------------------------
// Get value
//-----------------------------------------------------------------------------

const char *CCvar::GetStringFromCvar(cvar_t *pCvar)
{
	return pCvar->string;
}

float CCvar::GetFloatFromCvar(cvar_t *pCvar)
{
	return pCvar->value;
}

int CCvar::GetIntFromCvar(cvar_t *pCvar)
{
	return int(pCvar->value);
}

bool CCvar::GetBoolFromCvar(cvar_t *pCvar)
{
	return !!int(pCvar->value);
}

Color CCvar::GetColorFromCvar(cvar_t *pCvar)
{
	const char *pszColor = pCvar->string;

	int nRGBA[4];
	int nParamsRead = sscanf(pszColor, "%i %i %i %i", &(nRGBA[0]), &(nRGBA[1]), &(nRGBA[2]), &(nRGBA[3]));

	if (nParamsRead >= 3)
	{
		return {
			clamp(nRGBA[0], 0, 255), // red
			clamp(nRGBA[1], 0, 255), // green
			clamp(nRGBA[2], 0, 255), // blue
			(nParamsRead == 3) ? 255 : clamp(nRGBA[3], 0, 255)  // alpha
		};
	}
	else if (nParamsRead == 2)
	{
		return {
			clamp(nRGBA[0], 0, 255),
			clamp(nRGBA[1], 0, 255),
			255,
			255
		};
	}
	else if (nParamsRead == 1)
	{
		return {
			clamp(nRGBA[0], 0, 255),
			255,
			255,
			255
		};
	}

	return { 255, 255, 255, 255 };
}

const char *CCvar::GetStringFromCvar(const char *pszName)
{
	cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszName);

	if ( pCvar )
		return GetStringFromCvar(pCvar);

	return "";
}

float CCvar::GetFloatFromCvar(const char *pszName)
{
	cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszName);

	if ( pCvar )
		return GetFloatFromCvar(pCvar);

	return 0.f;
}

int CCvar::GetIntFromCvar(const char *pszName)
{
	cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszName);

	if ( pCvar )
		return GetIntFromCvar(pCvar);

	return 0;
}

bool CCvar::GetBoolFromCvar(const char *pszName)
{
	cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszName);

	if ( pCvar )
		return GetBoolFromCvar(pCvar);

	return false;
}

Color CCvar::GetColorFromCvar(const char *pszName)
{
	cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszName);

	if ( pCvar )
		return GetColorFromCvar(pCvar);

	return { 255, 255, 255, 255 };
}

//-----------------------------------------------------------------------------
// Get Cvar_DirectSet
//-----------------------------------------------------------------------------

Cvar_DirectSetFn CCvar::GetCvar_DirectSet()
{
	return Cvar_DirectSet;
}

//-----------------------------------------------------------------------------
// CConCommandHash
//-----------------------------------------------------------------------------

CConCommandHash::~CConCommandHash()
{
	RemoveAll();
}

void CConCommandHash::Init()
{
	for (size_t i = 0; i < NUM_BUCKETS; i++)
	{
		m_Buckets[i].clear();
	}
}

unsigned int CConCommandHash::Hash(const char *pszCommand)
{
	return HashStringCaseless( pszCommand );
}

unsigned int CConCommandHash::Hash(ConCommandBase *pCommand)
{
	return HashStringCaseless( pCommand->GetName() );
}

ConCommandBase *CConCommandHash::Find(ConCommandBase *pCommand)
{
	return Find( pCommand->GetName() );
}

ConCommandBase *CConCommandHash::Find(const char *pszName)
{
	int index = Hash(pszName) % NUM_BUCKETS_MASK;

	const datapool_t &bucket = m_Buckets[index];

	for (size_t i = 0; i < bucket.size(); i++)
	{
		if ( !stricmp( bucket[i]->GetName(), pszName ) )
		{
			return bucket[i];
		}
	}

	return NULL;
}

bool CConCommandHash::Insert(ConCommandBase *pCommand)
{
	int index = Hash(pCommand) % NUM_BUCKETS_MASK;
	datapool_t &bucket = m_Buckets[index];

	for (size_t i = 0; i < bucket.size(); i++)
	{
		if ( !stricmp( bucket[i]->GetName(), pCommand->GetName() ) )
		{
			return false;
		}
	}

	bucket.push_back(pCommand);
	return true;
}

bool CConCommandHash::Remove(ConCommandBase *pCommand)
{
	return Remove( pCommand->GetName() );
}

bool CConCommandHash::Remove(const char *pszCommand)
{
	int index = Hash(pszCommand) % NUM_BUCKETS_MASK;
	datapool_t &bucket = m_Buckets[index];

	for (size_t i = 0; i < bucket.size(); i++)
	{
		if ( !stricmp( bucket[i]->GetName(), pszCommand ) )
		{
			bucket.erase(bucket.begin() + i);
			return true;
		}
	}

	return false;
}

void CConCommandHash::RemoveAll()
{
	for (int i = 0; i < NUM_BUCKETS; i++)
	{
		if ( !m_Buckets[i].empty() )
		{
			m_Buckets[i].clear();
		}
	}
}

int CConCommandHash::Size()
{
	return NUM_BUCKETS;
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CCvar g_CVar;
ICvar *g_pCVar = &g_CVar;

ICvar *CVar()
{
	return g_pCVar;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CCvar, ICvar, CVAR_INTERFACE_VERSION, g_CVar);

//-----------------------------------------------------------------------------
// Debug functions
//-----------------------------------------------------------------------------

void Msg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

	g_pCVar->ConsolePrint(szFormattedMsg);
}

void Warning(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

	g_pCVar->ConsoleColorPrint(s_WarningPrintColor, szFormattedMsg);
}

void DevMsg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	if ( g_pCVar->CanPrint() && bool(g_CVar.m_pDeveloper->value) )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszMessageFormat, args);
		va_end(args);

		szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

		g_pCVar->ConsoleDPrint(szFormattedMsg);
	}
}

void DevWarning(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	if ( g_pCVar->CanPrint() && bool(g_CVar.m_pDeveloper->value) )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszMessageFormat, args);
		va_end(args);

		szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

		g_pCVar->ConsoleColorPrint(s_WarningPrintColor, szFormattedMsg);
	}
}

void ConColorMsg(const Color &clr, const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

	g_pCVar->ConsoleColorPrint(clr, szFormattedMsg);
}

void ConMsg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

	g_pCVar->ConsolePrint(szFormattedMsg);
}

void ConDMsg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	if ( g_pCVar->CanPrint() && bool(g_CVar.m_pDeveloper->value) )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedMsg, M_ARRAYSIZE(szFormattedMsg), pszMessageFormat, args);
		va_end(args);

		szFormattedMsg[M_ARRAYSIZE(szFormattedMsg) - 1] = 0;

		g_pCVar->ConsolePrint(szFormattedMsg);
	}
}