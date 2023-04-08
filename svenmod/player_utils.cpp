#include <IPlayerUtils.h>
#include <ISvenModAPI.h>

//-----------------------------------------------------------------------------
// CPlayerUtils
//-----------------------------------------------------------------------------

class CPlayerUtils : public IPlayerUtils
{
public:
	virtual extra_player_info_t *GetExtraInfo(int iPlayerIndex);
	virtual extra_player_info_t *GetExtraInfo(cl_entity_t *pPlayer);

	virtual float GetHealth(int iPlayerIndex);
	virtual float GetArmor(int iPlayerIndex);
	virtual int GetTeamNumber(int iPlayerIndex);
	virtual uint64 GetSteamID(int iPlayerIndex);

	virtual void GetViewAngles(int iPlayerIndex, Vector &vViewAngles);
	virtual void GetViewAngles(cl_entity_t *pPlayer, Vector &vViewAngles);
};

//-----------------------------------------------------------------------------
// CPlayerUtils implementation
//-----------------------------------------------------------------------------

extra_player_info_t *CPlayerUtils::GetExtraInfo(int iPlayerIndex)
{
	if (iPlayerIndex > 0 && iPlayerIndex <= MAXCLIENTS)
		return &g_pPlayerExtraInfo[iPlayerIndex];

	return NULL;
}

extra_player_info_t *CPlayerUtils::GetExtraInfo(cl_entity_t *pPlayer)
{
	if (pPlayer->index > 0 && pPlayer->index <= MAXCLIENTS)
		return &g_pPlayerExtraInfo[pPlayer->index];

	return NULL;
}

float CPlayerUtils::GetHealth(int iPlayerIndex)
{
	if (iPlayerIndex > 0 && iPlayerIndex <= MAXCLIENTS)
		return g_pPlayerExtraInfo[iPlayerIndex].health;

	return -1.f;
}

float CPlayerUtils::GetArmor(int iPlayerIndex)
{
	if (iPlayerIndex > 0 && iPlayerIndex <= MAXCLIENTS)
		return g_pPlayerExtraInfo[iPlayerIndex].armor;

	return -1.f;
}

int CPlayerUtils::GetTeamNumber(int iPlayerIndex)
{
	if (iPlayerIndex > 0 && iPlayerIndex <= MAXCLIENTS)
		return g_pPlayerExtraInfo[iPlayerIndex].teamnumber;

	return -1;
}

uint64 CPlayerUtils::GetSteamID(int iPlayerIndex)
{
	if (iPlayerIndex > 0 && iPlayerIndex <= MAXCLIENTS)
	{
		player_info_s *pPlayerInfo = g_pEngineStudio->PlayerInfo(iPlayerIndex - 1);

		if ( !pPlayerInfo )
			return 0uLL;

		return pPlayerInfo->m_nSteamID;
	}
	
	return 0uLL;
}

void CPlayerUtils::GetViewAngles(int iPlayerIndex, Vector &vOut)
{
	if (iPlayerIndex > 0 && iPlayerIndex <= MAXCLIENTS)
	{
		cl_entity_t *pPlayer = g_pEngineFuncs->GetEntityByIndex(iPlayerIndex);

		if (pPlayer)
		{
			vOut.x = pPlayer->curstate.angles.x * (89.0f / 9.8876953125f);
			vOut.y = pPlayer->curstate.angles.y;
			vOut.z = pPlayer->curstate.angles.z;
			return;
		}
	}

	vOut.Zero();
}

void CPlayerUtils::GetViewAngles(cl_entity_t *pPlayer, Vector &vOut)
{
	if (pPlayer->index > 0 && pPlayer->index <= MAXCLIENTS)
	{
		vOut.x = pPlayer->curstate.angles.x * (89.0f / 9.8876953125f);
		vOut.y = pPlayer->curstate.angles.y;
		vOut.z = pPlayer->curstate.angles.z;
		return;
	}
	
	vOut.Zero();
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CPlayerUtils g_PlayerUtils;
IPlayerUtils *g_pPlayerUtils = (IPlayerUtils *)&g_PlayerUtils;

IPlayerUtils *PlayerUtils()
{
	return g_pPlayerUtils;
}
