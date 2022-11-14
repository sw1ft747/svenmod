#ifndef SM_CVAR_H
#define SM_CVAR_H

#ifdef _WIN32
#pragma once
#endif

#include <ICvar.h>
#include <IMemoryUtils.h>
#include <ISvenModAPI.h>

#include <vector>

//-----------------------------------------------------------------------------
// Signatures
//-----------------------------------------------------------------------------

typedef void (__cdecl *Z_FreeFn)(void *);
typedef void (__cdecl *Cvar_DirectSetFn)(cvar_t *, const char *);
typedef void (__thiscall *RichText__InsertColorChangeFn)(void *, Color);
typedef void (__thiscall *RichText__InsertStringFn)(void *, const char *);

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

class CGameConsole;

//-----------------------------------------------------------------------------
// ConCommandBase hash
//-----------------------------------------------------------------------------

class CConCommandHash
{
	friend class CCvar;

public:
	~CConCommandHash();

	void Init();

	unsigned int Hash(ConCommandBase *pCommand);
	unsigned int Hash(const char *pszCommand);

	ConCommandBase *Find(ConCommandBase *pCommand);
	ConCommandBase *Find(const char *pszName);

	bool Insert(ConCommandBase *pCommand);

	bool Remove(ConCommandBase *pCommand);
	bool Remove(const char *pszCommand);

	void RemoveAll();

	int Size();

protected:
	typedef std::vector<ConCommandBase *> datapool_t;

	enum
	{
		NUM_BUCKETS = 511,
		NUM_BUCKETS_MASK = NUM_BUCKETS
	};

	datapool_t m_Buckets[NUM_BUCKETS];
};

//-----------------------------------------------------------------------------
// CCvar
//-----------------------------------------------------------------------------

class CCvar : public ICvar
{
	friend class CHooks;

public:
	bool Init();
	void Shutdown();

	void EnablePrint();
	void DisablePrint();

	void PrintCvars(int mode, const char *pszPrefix) const;

public:
	CCvar();
	virtual						~CCvar();

	virtual CVarDLLIdentifier_t AllocateDLLIdentifier();

	virtual void				RegisterConCommand(ConCommandBase *pCommandBase);
	virtual void				UnregisterConCommand(ConCommandBase *pCommandBase);
	virtual void				UnregisterConCommands(CVarDLLIdentifier_t id);

	virtual const char			*GetCommandLineValue(const char *pszVariableName);

	virtual cvar_t				*FindCvar(const char *pszName);
	virtual const cvar_t		*FindCvar(const char *pszName) const;

	virtual cmd_t				*FindCmd(const char *pszName);
	virtual const cmd_t			*FindCmd(const char *pszName) const;

	virtual ConCommandBase		*FindCommandBase(const char *pszName);
	virtual const ConCommandBase *FindCommandBase(const char *pszName) const;

	virtual ConVar				*FindVar(const char *pszName);
	virtual const ConVar		*FindVar(const char *pszName) const;

	virtual ConCommand			*FindCommand(const char *pszName);
	virtual const ConCommand	*FindCommand(const char *pszName) const;

	virtual void				RevertFlaggedConVars(int nFlag);

	virtual bool				CanPrint(void) const;

	virtual void				ConsoleColorPrint(const Color &clr, const char *pszMessage) const;
	virtual void				ConsolePrint(const char *pszMessage) const;
	virtual void				ConsoleDPrint(const char *pszMessage) const;

	virtual void				ConsoleColorPrintf(const Color &clr, const char *pszFormat, ...) const;
	virtual void				ConsolePrintf(const char *pszFormat, ...) const;
	virtual void				ConsoleDPrintf(const char *pszFormat, ...) const;

	virtual int					ArgC(void) const;
	virtual const char			**ArgV(void) const;
	virtual const char			*Arg(int nIndex) const;

	virtual void				SetValue(cvar_t *pCvar, const char *value);
	virtual void				SetValue(cvar_t *pCvar, float value);
	virtual void				SetValue(cvar_t *pCvar, int value);
	virtual void				SetValue(cvar_t *pCvar, bool value);
	virtual void				SetValue(cvar_t *pCvar, Color value);

	virtual void				SetValue(const char *pszCvar, const char *value);
	virtual void				SetValue(const char *pszCvar, float value);
	virtual void				SetValue(const char *pszCvar, int value);
	virtual void				SetValue(const char *pszCvar, bool value);
	virtual void				SetValue(const char *pszCvar, Color value);

	virtual const char			*GetStringFromCvar(cvar_t *pCvar);
	virtual float				GetFloatFromCvar(cvar_t *pCvar);
	virtual int					GetIntFromCvar(cvar_t *pCvar);
	virtual bool				GetBoolFromCvar(cvar_t *pCvar);
	virtual Color				GetColorFromCvar(cvar_t *pCvar);

	virtual const char			*GetStringFromCvar(const char *pszName);
	virtual float				GetFloatFromCvar(const char *pszName);
	virtual int					GetIntFromCvar(const char *pszName);
	virtual bool				GetBoolFromCvar(const char *pszName);
	virtual Color				GetColorFromCvar(const char *pszName);

private:
	Cvar_DirectSetFn GetCvar_DirectSet();

private:
	// pointers to functions
	Z_FreeFn Z_Free;
	Cvar_DirectSetFn Cvar_DirectSet;

	RichText__InsertColorChangeFn RichText__InsertColorChange;
	RichText__InsertStringFn RichText__InsertString;

public:
	cvar_t *m_pDeveloper;

private:
	cvar_t **m_ppCvarList;
	cmd_t **m_ppCmdList;

	mutable CConCommandHash m_CommandHash;
	CGameConsole *m_pGameConsole;

	int *m_pArgC;
	const char **m_ppArgV;

	CVarDLLIdentifier_t m_nNextDLLIdentifier;

	bool m_bInitialized;
	bool m_bCanPrint;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

extern CCvar g_CVar;

#endif // SM_CVAR_H