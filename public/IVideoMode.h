#ifndef IVIDEOMODE_H
#define IVIDEOMODE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "sys.h"

#include "modes.h"

abstract_class IVideoMode
{
public:
	virtual const char *GetName() = 0;

	virtual	int Init() = 0;
	virtual void Shutdown() = 0;

	virtual void AddMode(int width, int height, int bpp) = 0;

	virtual vmode_t *GetCurrentMode() = 0;
	virtual vmode_t *GetMode(int num) = 0;

	virtual int GetModeCount() = 0;

	virtual bool IsWindowedMode() = 0;

	virtual int GetInitialized() = 0;
	virtual void SetInitialized(bool initialized) = 0;

	virtual void UpdateWindowPosition() = 0;

	// (=
	virtual void FlipScreen() = 0;

	virtual void RestoreVideo() = 0;
	virtual void ReleaseVideo() = 0;

	virtual void Unknown() = 0;

	virtual int MaxBitsPerPixel() = 0;

	virtual void ReleaseFullScreen() = 0;
	virtual void ChangeDisplaySettingsToFullscreen() = 0;
};

extern IVideoMode *g_pVideoMode;
PLATFORM_INTERFACE IVideoMode *VideoMode();

#endif // IVIDEOMODE_H
