#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <interface.h>

// ------------------------------------------------------------------------------------ //
// InterfaceReg.
// ------------------------------------------------------------------------------------ //

InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;

InterfaceReg::InterfaceReg(InstantiateInterfaceFn fn, const char *pName) : m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

// ------------------------------------------------------------------------------------ //
// CreateInterface.
// ------------------------------------------------------------------------------------ //

IBaseInterface *CreateInterface(const char *pName, int *pReturnCode)
{
	InterfaceReg *pCur;

	for (pCur = InterfaceReg::s_pInterfaceRegs; pCur; pCur = pCur->m_pNext)
	{
		if ( !strcmp(pCur->m_pName, pName) )
		{
			if (pReturnCode)
				*pReturnCode = IFACE_OK;

			return pCur->m_CreateFn();
		}
	}

	if (pReturnCode)
		*pReturnCode = IFACE_FAILED;

	return NULL;
}

//Local version of CreateInterface, marked static so that it is never merged with the version in other libraries
static IBaseInterface *CreateInterfaceLocal(const char *pName, int *pReturnCode)
{
	InterfaceReg *pCur;

	for (pCur = InterfaceReg::s_pInterfaceRegs; pCur; pCur = pCur->m_pNext)
	{
		if ( !strcmp(pCur->m_pName, pName) )
		{
			if (pReturnCode)
				*pReturnCode = IFACE_OK;

			return pCur->m_CreateFn();
		}
	}

	if (pReturnCode)
		*pReturnCode = IFACE_FAILED;

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: returns the instance of this module
// Output : interface_instance_t
//-----------------------------------------------------------------------------

CreateInterfaceFn Sys_GetFactoryThis()
{
	return CreateInterfaceLocal;
}