#include <base_feature.h>
#include <sys.h>

CBaseFeature *CBaseFeature::s_pBaseFeaturesList = NULL;

static CBaseFeature *s_pLastLoaded = NULL;
static bool s_bFeaturesLoaded = false;

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------

void LoadFeatures()
{
	if ( s_bFeaturesLoaded )
		return;

	s_bFeaturesLoaded = true;

	CBaseFeature *pCur, *pNext;
	pCur = CBaseFeature::s_pBaseFeaturesList;

	while (pCur)
	{
		pNext = pCur->m_pNext;

		if ( pCur->Load() )
		{
			pCur->SetLoaded(true);
		}
		else
		{
			pCur->SetLoaded(false);
		}

		if ( !pNext )
		{
			s_pLastLoaded = pCur;
		}

		pCur = pNext;
	}
}

void PostLoadFeatures()
{
	if ( s_bFeaturesLoaded )
	{
		CBaseFeature *pCur, *pNext;
		pCur = CBaseFeature::s_pBaseFeaturesList;

		while (pCur)
		{
			pNext = pCur->m_pNext;

			pCur->PostLoad();

			pCur = pNext;
		}
	}
}

void UnloadFeatures()
{
	if ( s_bFeaturesLoaded )
	{
		s_bFeaturesLoaded = false;

		CBaseFeature *pCur, *pPrev;
		pCur = s_pLastLoaded;

		while (pCur)
		{
			pPrev = pCur->m_pNext;

			if ( pCur->IsLoaded() )
				pCur->Unload();

			pCur->SetLoaded(false);

			pCur = pPrev;
		}
	}
}

//-----------------------------------------------------------------------------
// CBaseFeature
//-----------------------------------------------------------------------------

CBaseFeature::CBaseFeature()
{
	m_bLoaded = false;

	m_pNext = NULL;
	m_pPrev = NULL;

	m_pNext = s_pBaseFeaturesList;

	if (s_pBaseFeaturesList)
	{
		s_pBaseFeaturesList->m_pPrev = this;
	}

	s_pBaseFeaturesList = this;
}