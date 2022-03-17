#ifndef IVGUIMODULELOADER_H
#define IVGUIMODULELOADER_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "interface.h"
#include "sys.h"

//-----------------------------------------------------------------------------
// Purpose: interface to accessing all loaded modules
//-----------------------------------------------------------------------------

abstract_class IVGuiModuleLoader
{
public:
	virtual						~IVGuiModuleLoader() {}

	virtual int					GetModuleCount( void ) = 0;
	virtual const char			*GetModuleLabel( int iModuleIndex ) = 0;
	virtual CreateInterfaceFn	GetModuleFactory( int iModuleIndex ) = 0;
	virtual bool				ActivateModule( int iModuleIndex ) = 0;
	virtual bool				ActivateModule( const char *pszModuleName ) = 0;
	virtual void				SetPlatformToRestart( void ) = 0;
	virtual void				NotifyOnExitToDesktop( void ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

#define VGUIMODULELOADER_INTERFACE_VERSION "VGuiModuleLoader003"

extern IVGuiModuleLoader *g_pVModuleLoader;

#endif // IVGUIMODULELOADER_H
