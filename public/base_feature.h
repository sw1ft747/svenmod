#ifndef BASE_FEATURE_H
#define BASE_FEATURE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

//-----------------------------------------------------------------------------
// Load/unload all features
//-----------------------------------------------------------------------------

void LoadFeatures();
void PostLoadFeatures();

void UnloadFeatures();

//-----------------------------------------------------------------------------
// Purpose: abstract class to create a loadable feature
//-----------------------------------------------------------------------------

abstract_class CBaseFeature
{
	friend void LoadFeatures();
	friend void PostLoadFeatures();
	friend void UnloadFeatures();

public:
	CBaseFeature();
	virtual ~CBaseFeature() {}

	virtual bool Load() { return true; }

	// Should be called after loading all features, i.e. in IClientPlugin::PostLoad
	virtual void PostLoad() {}

	virtual void Unload() {}

	virtual bool IsLoaded() final { return m_bLoaded; }

private:
	virtual void SetLoaded(bool state) final { m_bLoaded = state; }

private:
	bool m_bLoaded;

	CBaseFeature *m_pNext;
	CBaseFeature *m_pPrev;

	static CBaseFeature *s_pBaseFeaturesList;
};

#endif // BASE_FEATURE_H