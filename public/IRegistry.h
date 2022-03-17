//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//
#ifndef IREGISTRY_H
#define IREGISTRY_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

//-----------------------------------------------------------------------------
// Purpose: Interface to registry
//-----------------------------------------------------------------------------

abstract_class IRegistry
{
public:
	virtual ~IRegistry() {}

	// Init/shutdown
	virtual void			Init( void ) = 0;
	virtual void			Shutdown( void ) = 0;

	// Read/write integers
	virtual int				ReadInt( const char *key, int defaultValue = 0 ) = 0;
	virtual void			WriteInt( const char *key, int value ) = 0;

	// Read/write strings
	virtual const char		*ReadString( const char *key, const char *defaultValue = 0 ) = 0;
	virtual void			WriteString( const char *key, const char *value ) = 0;
};

extern IRegistry *g_pRegistry;
PLATFORM_INTERFACE IRegistry *Registry();

#endif // IREGISTRY_H