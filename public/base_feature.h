#ifndef BASE_FEATURE_H
#define BASE_FEATURE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

//-----------------------------------------------------------------------------
// Load/unload all features
//-----------------------------------------------------------------------------

// Returns 'false' if at least one feature can't be loaded, otherwise 'true'
bool LoadFeatures();
void PostLoadFeatures();

void PauseFeatures();
void UnpauseFeatures();

void UnloadFeatures();

//-----------------------------------------------------------------------------
// Purpose: abstract class to create a loadable feature
//-----------------------------------------------------------------------------

abstract_class CBaseFeature
{
	friend bool LoadFeatures();
	friend void PostLoadFeatures();

	friend void PauseFeatures();
	friend void UnpauseFeatures();

	friend void UnloadFeatures();

public:
	CBaseFeature();
	virtual ~CBaseFeature() {}

	virtual bool Load() { return true; }

	// Should be called after loading all features, i.e. in IClientPlugin::PostLoad
	virtual void PostLoad() {}

	virtual void Unload() {}

	virtual void Pause() {}
	virtual void Unpause() {}

	virtual bool IsLoaded() final { return m_bLoaded; }
	virtual bool IsPaused() final { return m_bPaused; }

private:
	virtual void SetLoaded(bool state) final { m_bLoaded = state; }
	virtual void SetPaused(bool state) final { m_bPaused = state; }

private:
	bool m_bLoaded;
	bool m_bPaused;

	CBaseFeature *m_pNext;
	CBaseFeature *m_pPrev;

	static CBaseFeature *s_pBaseFeaturesList;
};

#endif // BASE_FEATURE_H