#include "cvar.h"

#include <gameui/IGameConsole.h>
#include <gameui/gameconsole.h>

#include <dbg.h>
#include <interface.h>
#include <generichash.h>
#include <convar.h>

#include <hl_sdk/engine/APIProxy.h>

#define CONSOLE_PRINT_MESSAGE_LENGTH 8192

// Patterns
DEFINE_PATTERN(Z_Free_sig, "56 8B 74 24 08 85 F6 75 0D 68 ? ? ? ? E8 ? ? ? ? 83 C4 04 83 C6 E8");
DEFINE_PATTERN(Cvar_DirectSet_sig, "81 EC ? ? 00 00 A1 ? ? ? ? 33 C4 89 84 24 ? ? 00 00 56 8B B4 24 ? ? 00 00 57 8B BC 24 ? ? 00 00 85 FF");
DEFINE_PATTERN(Cvar_RemoveClientDLLCvars_sig, "56 8B 35 ? ? ? ? 57 33 FF 85 F6 74 2A 53 90");
DEFINE_PATTERN(Cvar_RemoveClientDLLCmds_sig, "A1 ? ? ? ? 57 33 FF 85 C0 74 24 53 8B 5C 24 0C 56 8B 30");
DEFINE_PATTERN(CGameConsoleDialog__DPrint, "55 8B EC 56 8B F1 FF B6 ? ? 00 00 8B 8E ? ? 00 00 E8 ? ? ? ? FF 75 08 8B 8E ? ? 00 00 E8 ? ? ? ? 5E 5D C2 04 00");

extern client_version_s g_ClientVersion;;

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
	if (m_bInitialized)
		return true;

	if (g_ClientVersion.version != 5025)
		LogWarning("Validate paddings of class \"CGameConsoleDialog\"");

	ud_t instruction;

	bool bFoundFirstCall = false;
	int iDisassembledBytes = 0;

	void *pCvar_RemoveClientDLLCvars = NULL;
	void *pCvar_RemoveClientDLLCmds = NULL;

	m_nNextDLLIdentifier = 0;

	// ToDo: just take it from Quake?
	if ( (Z_Free = (Z_FreeFn)MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Z_Free_sig )) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Couldn't find function Z_Free");
		return false;
	}
	
	if ( (Cvar_DirectSet = (Cvar_DirectSetFn)MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Cvar_DirectSet_sig )) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Couldn't find function Cvar_DirectSet");
		return false;
	}
	
	if ( (pCvar_RemoveClientDLLCvars = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Cvar_RemoveClientDLLCvars_sig )) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Couldn't find function Cvar_RemoveClientDLLCvars");
		return false;
	}
	
	if ( (pCvar_RemoveClientDLLCmds = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, Cvar_RemoveClientDLLCmds_sig )) == NULL )
	{
		Sys_ErrorMessage("[SvenMod] Couldn't find function Cvar_RemoveClientDLLCmds");
		return false;
	}
	
	// Get cvar and cmd list

	MemoryUtils()->InitDisasm(&instruction, pCvar_RemoveClientDLLCvars, 32, 32);

	do
	{
		if (instruction.mnemonic == UD_Imov && instruction.operand[0].type == UD_OP_REG && instruction.operand[0].base == UD_R_ESI && instruction.operand[1].type == UD_OP_MEM)
		{
			m_ppCvarList = reinterpret_cast<cvar_t **>(instruction.operand[1].lval.udword);
			break;
		}

	} while ( MemoryUtils()->Disassemble(&instruction) );
	
	MemoryUtils()->InitDisasm(&instruction, pCvar_RemoveClientDLLCmds, 32, 32);

	do
	{
		if (instruction.mnemonic == UD_Imov && instruction.operand[0].type == UD_OP_REG && instruction.operand[0].base == UD_R_EAX && instruction.operand[1].type == UD_OP_MEM)
		{
			m_ppCmdList = reinterpret_cast<cmd_t **>(instruction.operand[1].lval.udword);
			break;
		}

	} while ( MemoryUtils()->Disassemble(&instruction) );

	// Commands args

	if ( *((unsigned char *)g_pEngineFuncs->Cmd_Argc) == 0xE9 ) // JMP opcode
	{
		void *pCmd_Argc = MemoryUtils()->CalcAbsoluteAddress( g_pEngineFuncs->Cmd_Argc );

		MemoryUtils()->InitDisasm(&instruction, pCmd_Argc, 32, 16);

		do
		{
			if (instruction.mnemonic == UD_Imov && instruction.operand[0].type == UD_OP_REG && instruction.operand[0].base == UD_R_EAX && instruction.operand[1].type == UD_OP_MEM)
			{
				m_pArgC = reinterpret_cast<int *>(instruction.operand[1].lval.udword);
				break;
			}

		} while ( MemoryUtils()->Disassemble(&instruction) );
	}

	if ( !m_pArgC )
	{
		Sys_Error("[SvenMod] Failed to get g_pArgC");
	}

	if ( *((unsigned char *)g_pEngineFuncs->Cmd_Argv) == 0xE9 ) // JMP opcode
	{
		void *pCmd_Argv = MemoryUtils()->CalcAbsoluteAddress( g_pEngineFuncs->Cmd_Argv );

		MemoryUtils()->InitDisasm(&instruction, pCmd_Argv, 32, 96);

		do
		{
			if (instruction.mnemonic == UD_Imov && instruction.operand[0].type == UD_OP_REG && instruction.operand[0].base == UD_R_EAX &&
				instruction.operand[1].type == UD_OP_MEM && instruction.operand[1].index == UD_R_EAX && instruction.operand[1].scale == 4 && instruction.operand[1].offset == 32)
			{
				m_ppArgV = reinterpret_cast<const char **>(instruction.operand[1].lval.udword);
				break;
			}

		} while ( MemoryUtils()->Disassemble(&instruction) );
	}

	if ( !m_ppArgV )
	{
		Sys_Error("[SvenMod] Failed to get g_ppArgV");
	}

	// Console print

	void *pCGameConsoleDialog__DPrint = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->GameUI, CGameConsoleDialog__DPrint );

	if ( !pCGameConsoleDialog__DPrint)
	{
		Sys_Error("[SvenMod] Couldn't find CGameConsoleDialog::DPrint");
		return false;
	}

	unsigned char *p = (unsigned char *)pCGameConsoleDialog__DPrint;

	MemoryUtils()->InitDisasm(&instruction, pCGameConsoleDialog__DPrint, 32, 128);

	do
	{
		if (instruction.mnemonic == UD_Icall && instruction.operand[0].type == UD_OP_JIMM)
		{
			if ( !bFoundFirstCall )
			{
				RichText__InsertColorChange = (RichText__InsertColorChangeFn)MemoryUtils()->CalcAbsoluteAddress(p);
				bFoundFirstCall = true;
			}
			else
			{
				RichText__InsertString = (RichText__InsertStringFn)MemoryUtils()->CalcAbsoluteAddress(p);
				break;
			}
		}

		p += iDisassembledBytes;

	} while ( iDisassembledBytes = MemoryUtils()->Disassemble(&instruction) );

	if ( !RichText__InsertColorChange )
	{
		Sys_Error("[SvenMod] Failed to get vgui2::RichText::InsertColorChange");
		return false;
	}

	if ( !RichText__InsertString )
	{
		Sys_Error("[SvenMod] Failed to get vgui2::RichText::InsertString");
		return false;
	}

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

			if (pCommandBase->IsRegistered())
			{
				bool bFound = false;

				if (pCommandBase->IsCommand())
				{
					ConCommand *pCommand = dynamic_cast<ConCommand *>(pCommandBase);

					if (!pCommand)
					{
						Warning("[SvenMod] Can't cast to ConVar, invalid ConCommandBase\n");
						continue;
					}

					cmd_t *pPrev = NULL;
					cmd_t *pCmd = *m_ppCmdList;

					while (pCmd)
					{
						if (!stricmp(pCmd->name, pCommand->GetName()))
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

					if (bFound && pCmd)
						free((void *)pCmd);

					pCommand->m_pCommand = NULL;
				}
				else
				{
					ConVar *pConVar = dynamic_cast<ConVar *>(pCommandBase);

					if (!pConVar)
					{
						Warning("[SvenMod] Can't cast to ConCommand, invalid ConCommandBase\n");
						continue;
					}

					cvar_t *pPrev = NULL;
					cvar_t *pCvar = *m_ppCvarList;

					while (pCvar)
					{
						if (!stricmp(pCvar->name, pConVar->GetName()))
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

					if (bFound && pCvar)
					{
						Z_Free((void *)pCvar->string);
						Z_Free((void *)pCvar);
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
	// FIXME: use binary tree to sort the cvars

	int iCount = 0;

	ConsolePrint("----------------------------------\n");

	for (int i = 0; i < m_CommandHash.Size(); i++)
	{
		const CConCommandHash::datapool_t &bucket = m_CommandHash.m_Buckets[i];

		for (size_t j = 0; j < bucket.size(); j++)
		{
			ConCommandBase *pCommandBase = bucket[j];

			if ( pszPrefix && strncmp(pszPrefix, pCommandBase->GetName(), strlen(pszPrefix)) )
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
			ConCommand *pCommand = dynamic_cast<ConCommand *>(pCommandBase);

			if ( !pCommand )
			{
				Warning("[SvenMod] Can't cast to ConCommand, invalid ConCommandBase\n");
				m_CommandHash.Remove(pCommandBase);
				return;
			}

			g_pEngineFuncs->AddCommand( pCommand->GetName(), pCommand->m_pfnCallback );
			pCommand->m_pCommand = FindCmd( pCommand->GetName() );

			if (pCommand->m_pCommand)
			{
				pCommand->m_pCommand->flags = pCommand->GetFlags();
			}
		}
		else
		{
			ConVar *pConVar = dynamic_cast<ConVar *>(pCommandBase);

			if ( !pConVar )
			{
				Warning("[SvenMod] Can't cast to ConVar, invalid ConCommandBase\n");
				m_CommandHash.Remove(pCommandBase);
				return;
			}

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
		ConCommand *pCommand = dynamic_cast<ConCommand *>(pCommandBase);

		if ( !pCommand )
		{
			Warning("[SvenMod] Can't cast to ConVar, invalid ConCommandBase\n");
			return;
		}

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

		if (bFound && pCmd)
			free( (void *)pCmd );

		pCommand->m_pCommand = NULL;
	}
	else
	{
		ConVar *pConVar = dynamic_cast<ConVar *>(pCommandBase);

		if ( !pConVar )
		{
			Warning("[SvenMod] Can't cast to ConCommand, invalid ConCommandBase\n");
			return;
		}

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

		if (bFound && pCvar)
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
						ConCommand *pCommand = dynamic_cast<ConCommand *>(pCommandBase);

						if ( !pCommand )
						{
							Warning("[SvenMod] Can't cast to ConVar, invalid ConCommandBase\n");
							continue;
						}

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

						if (bFound && pCmd)
							free( (void *)pCmd );

						pCommand->m_pCommand = NULL;
					}
					else
					{
						ConVar *pConVar = dynamic_cast<ConVar *>(pCommandBase);

						if ( !pConVar )
						{
							Warning("[SvenMod] Can't cast to ConCommand, invalid ConCommandBase\n");
							continue;
						}

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

						if (bFound && pCvar)
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
		return dynamic_cast<ConVar *>(pCommandBase);

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
		return dynamic_cast<ConCommand *>(pCommandBase);

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
			ConVar *pConVar = dynamic_cast<ConVar *>(bucket[j]);

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
		vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszFormat, args);
		va_end(args);

		szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

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
		vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszFormat, args);
		va_end(args);

		szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

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
		vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszFormat, args);
		va_end(args);

		szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

		CGameConsoleDialog *pGameConsoleDialog = m_pGameConsole->GetGameConsoleDialog();

		RichText__InsertColorChange( pGameConsoleDialog->m_pRichText, s_ConsoleDefaultPrintColor );
		RichText__InsertString( pGameConsoleDialog->m_pRichText, szFormattedMsg );
	}
}

//-----------------------------------------------------------------------------
// Commands args
//-----------------------------------------------------------------------------

int CCvar::ArgC() const
{
	return *m_pArgC;
}

const char **CCvar::ArgV() const
{
	return m_ppArgV;
}

const char *CCvar::Arg(int nIndex) const
{
	int args = *m_pArgC;

	Assert(nIndex >= 0 && nIndex < args);

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
		snprintf(buffer, sizeof(buffer), "%f", eps);
	}
	else
	{
		snprintf(buffer, sizeof(buffer), "%d", int(value));
	}

	buffer[sizeof(buffer) - 1] = 0;

	Cvar_DirectSet(pCvar, buffer);
}

void CCvar::SetValue(cvar_t *pCvar, int value)
{
	char buffer[32];

	snprintf(buffer, sizeof(buffer), "%d", value);
	buffer[sizeof(buffer) - 1] = 0;

	Cvar_DirectSet(pCvar, buffer);
}

void CCvar::SetValue(cvar_t *pCvar, bool value)
{
	char buffer[32];

	snprintf(buffer, sizeof(buffer), "%d", int(value));
	buffer[sizeof(buffer) - 1] = 0;

	Cvar_DirectSet(pCvar, buffer);
}

void CCvar::SetValue(cvar_t *pCvar, Color value)
{
	char buffer[24];

	snprintf(buffer, sizeof(buffer), "%hhu %hhu %hhu %hhu", value.r, value.g, value.b, value.a);
	buffer[sizeof(buffer) - 1] = 0;

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

	snprintf(buffer, sizeof(buffer), "%hhu %hhu %hhu %hhu", value.r, value.g, value.b, value.a);
	buffer[sizeof(buffer) - 1] = 0;

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

	if (pCvar)
		return GetStringFromCvar(pCvar);

	return "";
}

float CCvar::GetFloatFromCvar(const char *pszName)
{
	cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszName);

	if (pCvar)
		return GetFloatFromCvar(pCvar);

	return 0.f;
}

int CCvar::GetIntFromCvar(const char *pszName)
{
	cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszName);

	if (pCvar)
		return GetIntFromCvar(pCvar);

	return 0;
}

bool CCvar::GetBoolFromCvar(const char *pszName)
{
	cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszName);

	if (pCvar)
		return GetBoolFromCvar(pCvar);

	return false;
}

Color CCvar::GetColorFromCvar(const char *pszName)
{
	cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszName);

	if (pCvar)
		return GetColorFromCvar(pCvar);

	return { 255, 255, 255, 255 };
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
	vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

	g_pCVar->ConsolePrint(szFormattedMsg);
}

void Warning(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

	g_pCVar->ConsoleColorPrint(s_WarningPrintColor, szFormattedMsg);
}

void DevMsg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	if ( g_pCVar->CanPrint() && bool(g_CVar.m_pDeveloper->value) )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszMessageFormat, args);
		va_end(args);

		szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

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
		vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszMessageFormat, args);
		va_end(args);

		szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

		g_pCVar->ConsoleColorPrint(s_WarningPrintColor, szFormattedMsg);
	}
}

void ConColorMsg(const Color &clr, const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

	g_pCVar->ConsoleColorPrint(clr, szFormattedMsg);
}

void ConMsg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	va_list args;
	va_start(args, pszMessageFormat);
	vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszMessageFormat, args);
	va_end(args);

	szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

	g_pCVar->ConsolePrint(szFormattedMsg);
}

void ConDMsg(const char *pszMessageFormat, ...)
{
	static char szFormattedMsg[CONSOLE_PRINT_MESSAGE_LENGTH] = { 0 };

	if ( g_pCVar->CanPrint() && bool(g_CVar.m_pDeveloper->value) )
	{
		va_list args;
		va_start(args, pszMessageFormat);
		vsnprintf(szFormattedMsg, (sizeof(szFormattedMsg) / sizeof(char)), pszMessageFormat, args);
		va_end(args);

		szFormattedMsg[(sizeof(szFormattedMsg) / sizeof(char)) - 1] = 0;

		g_pCVar->ConsolePrint(szFormattedMsg);
	}
}