//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#include <convar.h>
#include <ICvar.h>

#include <dbg.h>

//-----------------------------------------------------------------------------
// Statically constructed list of ConCommandBases, 
// used for registering them with the ICVar interface
//-----------------------------------------------------------------------------

ConCommandBase *ConCommandBase::s_pConCommandBases = NULL;
IConCommandBaseAccessor *ConCommandBase::s_pAccessor = NULL;

static int s_nCVarFlag = 0;
static int s_nDLLIdentifier = -1;	// A unique identifier indicating which DLL this convar came from
static bool s_bRegistered = false;

class CDefaultAccessor : public IConCommandBaseAccessor
{
public:
	virtual bool RegisterConCommandBase( ConCommandBase *pVar )
	{
		CVar()->RegisterConCommand( pVar );
		return true;
	}
};

static CDefaultAccessor s_DefaultAccessor;

//-----------------------------------------------------------------------------
// Called by the framework to register ConCommandBases with the ICVar
//-----------------------------------------------------------------------------

void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor)
{
	if ( s_bRegistered )
		return;

	AssertFatal( s_nDLLIdentifier < 0 );

	s_bRegistered = true;
	s_nCVarFlag = nCVarFlag;
	s_nDLLIdentifier = CVar()->AllocateDLLIdentifier();

	ConCommandBase *pCur, *pNext;

	ConCommandBase::s_pAccessor = pAccessor ? pAccessor : &s_DefaultAccessor;
	pCur = ConCommandBase::s_pConCommandBases;

	while ( pCur )
	{
		pNext = pCur->m_pNext;

		pCur->AddFlags( s_nCVarFlag );
		pCur->Init();

		pCur = pNext;
	}

	ConCommandBase::s_pConCommandBases = NULL;
}

void ConVar_Unregister(void)
{
	if ( !s_bRegistered )
		return;

	AssertFatal( s_nDLLIdentifier >= 0 );

	CVar()->UnregisterConCommands( s_nDLLIdentifier );

	s_nDLLIdentifier = -1;
	s_bRegistered = false;
}

//-----------------------------------------------------------------------------
// Print description
//-----------------------------------------------------------------------------

void ConVar_PrintDescription(const ConCommandBase *pVar)
{
	Assert( pVar );

	if ( !pVar->IsCommand() )
	{
		float fMin, fMax;

		ConVar *var = (ConVar *)pVar;
		const char *pszValue = var->GetString();

		bool bMin = var->GetMin(fMin);
		bool bMax = var->GetMax(fMax);

		if (var)
		{
			CVar()->ConsoleColorPrintf({ 255, 100, 100, 255 }, "\"%s\" = \"%s\"", var->GetName(), pszValue);

			if ( stricmp(pszValue, var->GetDefault()) )
			{
				CVar()->ConsolePrintf(" ( def. \"%s\" )", var->GetDefault());
			}
		}

		if ( bMin )
		{
			CVar()->ConsolePrintf(" min. %f", fMin);
		}

		if ( bMax )
		{
			CVar()->ConsolePrintf(" max. %f", fMax);
		}
	}
	else
	{
		CVar()->ConsoleColorPrintf({ 255, 100, 100, 255 }, "\"%s\"", pVar->GetName());
	}

	const char *pszHelpText = pVar->GetHelpText();

	if ( pszHelpText && *pszHelpText )
	{
		CVar()->ConsolePrintf("\n - %s\n", pszHelpText);
	}
	else
	{
		CVar()->ConsolePrint("\n");
	}
}

//-----------------------------------------------------------------------------
// ConCommandBase
//-----------------------------------------------------------------------------

ConCommandBase::ConCommandBase(void)
{
	m_bRegistered = false;
	m_pszName = NULL;
	m_pszHelpString = NULL;

	m_nFlags = 0;
	m_pNext = NULL;
}

ConCommandBase::~ConCommandBase()
{
}

ConCommandBase::ConCommandBase(const char *pszName, const char *pszHelpString /* = 0 */, int flags /* = 0 */)
{
	Create(pszName, pszHelpString, flags);
}

bool ConCommandBase::IsCommand(void) const
{
	return true;
}

CVarDLLIdentifier_t ConCommandBase::GetDLLIdentifier(void) const
{
	return s_nDLLIdentifier;
}

void ConCommandBase::Create(const char *pszName, const char *pszHelpString /* = 0 */, int flags /* = 0 */)
{
	m_bRegistered = false;

	m_pszName = pszName;
	m_pszHelpString = pszHelpString ? pszHelpString : "";

	m_nFlags = flags;

	m_pNext = s_pConCommandBases;
	s_pConCommandBases = this;

	if (s_pAccessor)
	{
		Init();
	}
}

void ConCommandBase::Init(void)
{
	if ( s_pAccessor != NULL )
	{
		s_pAccessor->RegisterConCommandBase(this);
	}
}

void ConCommandBase::Shutdown(void)
{
	if ( CVar() )
	{
		CVar()->UnregisterConCommand(this);
	}
}

const char *ConCommandBase::GetName(void) const
{
	return m_pszName;
}

bool ConCommandBase::IsFlagSet(int flag) const
{
	return (flag & m_nFlags) ? true : false;
}

void ConCommandBase::AddFlags(int flags)
{
	m_nFlags |= flags;
}

void ConCommandBase::RemoveFlags(int flags)
{
	m_nFlags &= ~flags;
}

int ConCommandBase::GetFlags(void) const
{
	return m_nFlags;
}

const ConCommandBase *ConCommandBase::GetNext(void) const
{
	return m_pNext;
}

ConCommandBase *ConCommandBase::GetNext(void)
{
	return m_pNext;
}

const char *ConCommandBase::GetHelpText(void) const
{
	return m_pszHelpString;
}

bool ConCommandBase::IsRegistered(void) const
{
	return m_bRegistered;
}

//-----------------------------------------------------------------------------
// Tokenizer class
//-----------------------------------------------------------------------------

CCommand::CCommand(void)
{
	m_nArgc = 0;
	m_ppArgv = NULL;
}

CCommand::CCommand(int nArgC, const char **ppArgV)
{
	Assert( nArgC > 0 );

	m_nArgc = nArgC;
	m_ppArgv = ppArgV;
}

const char *CCommand::FindArg(const char *pszName) const
{
	int nArgC = ArgC();

	for (int i = 1; i < nArgC; i++)
	{
		if ( !stricmp(Arg(i), pszName) )
			return (i + 1) < nArgC ? Arg(i + 1) : "";
	}

	return 0;
}

int CCommand::FindArgInt(const char *pszName, int nDefaultVal) const
{
	const char *pVal = FindArg(pszName);

	if ( pVal != NULL )
		return atoi(pVal);
	else
		return nDefaultVal;
}

//-----------------------------------------------------------------------------
// ConCommand
//-----------------------------------------------------------------------------

ConCommand::ConCommand(const char *pszName, FnCommandCallback_t pfnCallback, const char *pszHelpString, int flags)
{
	m_pfnCallback = pfnCallback;

	m_pCommand = NULL;

	ConCommandBase::Create(pszName, pszHelpString, flags);
}

ConCommand::~ConCommand()
{
}

bool ConCommand::IsCommand(void) const
{
	return true;
}

bool ConCommand::IsFlagSet(int flag) const
{
	if ( IsRegistered() && m_pCommand != NULL )
	{
		return (flag & m_pCommand->flags) ? true : false;
	}

	return (flag & m_nFlags) ? true : false;
}

void ConCommand::AddFlags(int flags)
{
	if ( IsRegistered() && m_pCommand != NULL )
	{
		m_pCommand->flags |= flags;
	}
	else
	{
		m_nFlags |= flags;
	}
}

void ConCommand::RemoveFlags(int flags)
{
	if ( IsRegistered() && m_pCommand != NULL )
	{
		m_pCommand->flags &= ~flags;
	}
	else
	{
		m_nFlags &= ~flags;
	}
}

int ConCommand::GetFlags(void) const
{
	if ( IsRegistered() && m_pCommand != NULL )
	{
		return m_pCommand->flags;
	}

	return m_nFlags;
}

cmd_t *ConCommand::GetCmdPointer(void) const
{
	return m_pCommand;
}

//-----------------------------------------------------------------------------
// ConVar
//-----------------------------------------------------------------------------

ConVar::ConVar(const char *pszName, const char *pszDefaultValue, int flags /* = 0 */)
{
	Create(pszName, pszDefaultValue, flags, NULL);
}

ConVar::ConVar(const char *pszName, const char *pszDefaultValue, int flags, const char *pszHelpString)
{
	Create(pszName, pszDefaultValue, flags, pszHelpString);
}

ConVar::ConVar(const char *pszName, const char *pszDefaultValue, int flags, const char *pszHelpString, bool bMin, float fMin, bool bMax, float fMax)
{
	Create(pszName, pszDefaultValue, flags, pszHelpString, bMin, fMin, bMax, fMax);
}

ConVar::~ConVar()
{
}

void ConVar::Create(const char *pszName, const char *pszDefaultValue, int flags /* = 0 */, const char *pszHelpString /* = NULL */,
					bool bMin /* = false */, float fMin /* = 0.0 */, bool bMax /* = false */, float fMax /* = false */)
{
	static const char *empty_string = "";

	// Name should be static data
	m_pszDefaultValue = pszDefaultValue ? pszDefaultValue : empty_string;
	Assert( m_pszDefaultValue );

	m_bHasMin = bMin;
	m_fMinVal = fMin;
	m_bHasMax = bMax;
	m_fMaxVal = fMax;

	m_pCvar = NULL;

	ConCommandBase::Create( pszName, pszHelpString, flags );
}

bool ConVar::IsCommand(void) const
{
	return false;
}

bool ConVar::IsFlagSet(int flag) const
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		return (flag & m_pCvar->flags) ? true : false;
	}

	return (flag & m_nFlags) ? true : false;
}

void ConVar::AddFlags(int flags)
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		m_pCvar->flags |= flags;
	}
	else
	{
		m_nFlags |= flags;
	}
}

void ConVar::RemoveFlags(int flags)
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		m_pCvar->flags &= ~flags;
	}
	else
	{
		m_nFlags &= ~flags;
	}
}

int ConVar::GetFlags(void) const
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		return m_pCvar->flags;
	}

	return m_nFlags;
}

cvar_t *ConVar::GetCvarPointer(void) const
{
	return m_pCvar;
}

//-----------------------------------------------------------------------------
// Get value
//-----------------------------------------------------------------------------

float ConVar::GetFloat(void) const
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		return m_pCvar->value;
	}

	return 0.f;
}

int ConVar::GetInt(void) const
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		return int(m_pCvar->value);
	}

	return 0;
}

Color ConVar::GetColor(void) const
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		return CVar()->GetColorFromCvar(m_pCvar);
	}

	return { 255, 255, 255, 255 };
}

bool ConVar::GetBool(void) const
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		return !!int(m_pCvar->value);
	}

	return false;
}

const char *ConVar::GetString(void) const
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		return m_pCvar->string;
	}

	return "";
}

//-----------------------------------------------------------------------------
// Set value
//-----------------------------------------------------------------------------

void ConVar::SetValue(const char *value)
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		CVar()->SetValue(m_pCvar, value);
	}
}

void ConVar::SetValue(float value)
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		CVar()->SetValue(m_pCvar, value);
	}
}

void ConVar::SetValue(int value)
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		CVar()->SetValue(m_pCvar, value);
	}
}

void ConVar::SetValue(bool value)
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		CVar()->SetValue(m_pCvar, value);
	}
}

void ConVar::SetValue(Color value)
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		CVar()->SetValue(m_pCvar, value);
	}
}

void ConVar::Revert(void)
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		CVar()->SetValue(m_pCvar, m_pszDefaultValue);
	}
}

//-----------------------------------------------------------------------------
// Min/max
//-----------------------------------------------------------------------------

bool ConVar::HasMin(void) const
{
	return m_bHasMin;
}

bool ConVar::HasMax(void) const
{
	return m_bHasMax;
}

bool ConVar::GetMin(float &minVal) const
{
	minVal = m_fMinVal;
	return m_bHasMin;
}

bool ConVar::GetMax(float &maxVal) const
{
	maxVal = m_fMaxVal;
	return m_bHasMax;
}

float ConVar::GetMinValue(void) const
{
	return m_fMinVal;
}

float ConVar::GetMaxValue(void) const
{
	return m_fMaxVal;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

const char *ConVar::GetDefault(void) const
{
	return m_pszDefaultValue;
}

void ConVar::SetDefault(const char *pszDefault)
{
	static const char *empty_string = "";
	m_pszDefaultValue = pszDefault ? pszDefault : empty_string;

	Assert( m_pszDefaultValue );
}

bool ConVar::Clamp(void)
{
	if ( IsRegistered() && m_pCvar != NULL )
	{
		float value = m_pCvar->value;

		if ( m_bHasMin && (value < m_fMinVal) )
		{
			CVar()->SetValue(m_pCvar, m_fMinVal);
			return true;
		}

		if ( m_bHasMax && (value > m_fMaxVal) )
		{
			CVar()->SetValue(m_pCvar, m_fMaxVal);
			return true;
		}
	}

	return false;
}

bool ConVar::ClampValue(float &value)
{
	if ( m_bHasMin && (value < m_fMinVal) )
	{
		value = m_fMinVal;
		return true;
	}

	if ( m_bHasMax && (value > m_fMaxVal) )
	{
		value = m_fMaxVal;
		return true;
	}

	return false;
}