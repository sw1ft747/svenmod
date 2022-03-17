#ifndef ISVENMOD_H
#define ISVENMOD_H

#ifdef _WIN32
#pragma once
#endif

#include <ICommandLine.h>
#include <IFileSystem.h>
#include <IRegistry.h>

class ISvenMod
{
public:
	virtual ~ISvenMod() {}

	virtual bool Init(ICommandLine *pCommandLine, IFileSystem *pFileSystem, IRegistry *pRegistry) = 0;
	virtual void Shutdown() = 0;
};

#define SVENMOD_INTERFACE_VERSION "SvenMod001"

#endif // ISVENMOD_H