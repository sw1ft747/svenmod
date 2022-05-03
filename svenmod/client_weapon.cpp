#include "client.h"
#include "client_weapon.h"

#include <IInventory.h>
#include <hl_sdk/cl_dll/cl_dll.h>

int g_iCurrentWeaponID = 0;
bool g_bCurrentWeaponCustom = false;
bool g_bForceWeaponReload = false;

//-----------------------------------------------------------------------------
// CClientWeapon implementation
//-----------------------------------------------------------------------------

CClientWeapon::CClientWeapon()
{
}

weapon_data_t *CClientWeapon::GetWeaponDataList(void)
{
	return g_Client.GetLocalStateNV()->weapondata;
}

weapon_data_t *CClientWeapon::GetWeaponData(void)
{
	return GetWeaponDataInternal(g_iCurrentWeaponID);
}

weapon_data_t *CClientWeapon::GetWeaponData(int iWeaponID)
{
	return GetWeaponDataInternal(iWeaponID);
}

bool CClientWeapon::IsCustom(void)
{
	return g_bCurrentWeaponCustom;
}

int CClientWeapon::Clip()
{
	if ( g_bCurrentWeaponCustom )
	{
		WEAPON *pWeapon = g_pInventory->GetWeapon(g_iCurrentWeaponID);

		if ( !pWeapon )
			return -1;

		return pWeapon->iClip;
	}

	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return -1;

	return pWeaponData->m_iClip;
}

int CClientWeapon::PrimaryAmmo()
{
	if ( g_bCurrentWeaponCustom )
	{
		WEAPON *pWeapon = g_pInventory->GetWeapon(g_iCurrentWeaponID);

		if ( !pWeapon )
			return -1;

		return g_pInventory->GetPrimaryAmmoCount(pWeapon);
	}

	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return -1;

	switch (g_iCurrentWeaponID)
	{
	case WEAPON_RPG:
	case WEAPON_M249:
	case WEAPON_M16:
	{
		int iAmmoType = pWeaponData->iuser1;
		int ammo = g_pInventory->CountAmmo(iAmmoType);

		if ( !ammo )
		{
			int clip = pWeaponData->m_iClip;

			if (clip != -1)
				return -clip;

			return 0;
		}

		return ammo;
	}
	}

	return pWeaponData->iuser2;
}

int CClientWeapon::SecondaryAmmo()
{
	if ( g_bCurrentWeaponCustom )
	{
		WEAPON *pWeapon = g_pInventory->GetWeapon(g_iCurrentWeaponID);

		if ( !pWeapon )
			return -1;

		return g_pInventory->GetSecondaryAmmoCount(pWeapon);
	}

	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return -1;

	switch (g_iCurrentWeaponID)
	{
	case WEAPON_MP5:
	case WEAPON_M16:
		return pWeaponData->iuser3;
	}

	return 0;
}

int CClientWeapon::PrimaryAmmoType()
{
	if ( g_bCurrentWeaponCustom )
	{
		WEAPON *pWeapon = g_pInventory->GetWeapon(g_iCurrentWeaponID);

		if ( !pWeapon )
			return -1;

		return pWeapon->iAmmoType;
	}

	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return -1;

	int iAmmoType = pWeaponData->iuser1;

	if ( iAmmoType == 255 )
		return -1;

	return iAmmoType;
}

int CClientWeapon::SecondaryAmmoType()
{
	WEAPON *pWeapon = g_pInventory->GetWeapon(g_iCurrentWeaponID);

	if ( !pWeapon )
		return -1;

	return pWeapon->iAmmo2Type;
}

float CClientWeapon::GetNextPrimaryAttack()
{
	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return -1.f;

	if ( g_bCurrentWeaponCustom )
		return pWeaponData->m_flNextPrimaryAttack;
	
	return static_cast<float>(g_Client.Time()) + pWeaponData->m_flNextPrimaryAttack;
}

float CClientWeapon::GetNextSecondaryAttack()
{
	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return -1.f;

	if ( g_bCurrentWeaponCustom )
		return pWeaponData->m_flNextSecondaryAttack;

	return static_cast<float>(g_Client.Time()) + pWeaponData->m_flNextSecondaryAttack;
}

float CClientWeapon::GetWeaponIdle()
{
	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return -1.f;

	float flCurrentTime = static_cast<float>(g_Client.Time());

	if ( g_bCurrentWeaponCustom )
	{
		if ( pWeaponData->m_fInReload )
			return flCurrentTime + g_Client.GetLocalStateNV()->client.m_flNextAttack;

		return pWeaponData->m_flTimeWeaponIdle;
	}

	if ( pWeaponData->m_fInReload )
		return flCurrentTime + g_Client.GetLocalStateNV()->client.m_flNextAttack;

	return flCurrentTime + pWeaponData->m_flTimeWeaponIdle;
}

bool CClientWeapon::CanPrimaryAttack()
{
	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return false;

	float flCurrentTime = static_cast<float>(g_Client.Time());

	if ( g_bCurrentWeaponCustom )
		return flCurrentTime >= pWeaponData->m_flNextPrimaryAttack;

	switch ( g_iCurrentWeaponID )
	{
	case WEAPON_GAUSS:
		return pWeaponData->fuser4 == 0.f && flCurrentTime >= (flCurrentTime + pWeaponData->m_flNextPrimaryAttack);
		
	case WEAPON_RPG:
		return pWeaponData->fuser1 == 0.f && flCurrentTime >= (flCurrentTime + pWeaponData->m_flNextPrimaryAttack);

	case WEAPON_HANDGRENADE:
		return pWeaponData->fuser1 >= 0.f && pWeaponData->fuser2 >= 0.f && flCurrentTime >= (flCurrentTime + pWeaponData->m_flNextPrimaryAttack);

	case WEAPON_DISPLACER:
		return pWeaponData->fuser1 == 0.f;
	}

	return flCurrentTime >= (flCurrentTime + pWeaponData->m_flNextPrimaryAttack);
}

bool CClientWeapon::CanSecondaryAttack()
{
	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return false;

	float flCurrentTime = static_cast<float>(g_Client.Time());

	if ( g_bCurrentWeaponCustom )
		return flCurrentTime >= pWeaponData->m_flNextSecondaryAttack;

	switch ( g_iCurrentWeaponID )
	{
	case WEAPON_M16:
		return pWeaponData->fuser1 == 0.f && flCurrentTime >= (flCurrentTime + pWeaponData->m_flNextSecondaryAttack);

	case WEAPON_HANDGRENADE:
		return pWeaponData->fuser1 < 0.f && flCurrentTime >= (flCurrentTime + pWeaponData->m_flNextSecondaryAttack);

	case WEAPON_SATCHEL:
		return pWeaponData->iuser2 != 0 && flCurrentTime >= (flCurrentTime + pWeaponData->m_flNextSecondaryAttack);

	case WEAPON_DISPLACER:
		return pWeaponData->fuser1 == 0.f;
	}

	return flCurrentTime >= (flCurrentTime + pWeaponData->m_flNextSecondaryAttack);
}

bool CClientWeapon::IsReloading()
{
	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return true;

	if ( g_bCurrentWeaponCustom )
		return pWeaponData->m_fInReload;

	switch ( g_iCurrentWeaponID )
	{
	case WEAPON_SHOTGUN:
		return pWeaponData->fuser4 != 0.f;

	case WEAPON_SPORE_LAUNCHER:
		return pWeaponData->iuser3 != 0;
	}

	return pWeaponData->m_fInReload;
}

bool CClientWeapon::IsInZoom()
{
	weapon_data_t *pWeaponData = GetWeaponDataInternal(g_iCurrentWeaponID);

	if ( !pWeaponData )
		return false;

	if ( g_iCurrentWeaponID == WEAPON_DESERT_EAGLE || g_iCurrentWeaponID == WEAPON_SHOTGUN )
		return false;

	return pWeaponData->m_fInZoom;
}

void CClientWeapon::Reload()
{
	g_bForceWeaponReload = true;
}

void CClientWeapon::Drop()
{
	WEAPON *pWeapon = g_pInventory->GetWeapon(g_iCurrentWeaponID);
	
	if (pWeapon)
		g_pInventory->DropWeapon( pWeapon );
}

weapon_data_t *CClientWeapon::GetWeaponDataInternal(int iWeaponID)
{
	if ( iWeaponID <= WEAPON_NONE )
		return NULL;

	return &g_Client.GetLocalStateNV()->weapondata[iWeaponID];
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CClientWeapon g_ClientWeapon;
IClientWeapon *g_pClientWeapon = (IClientWeapon *)&g_ClientWeapon;

IClientWeapon *ClientWeapon()
{
	return g_pClientWeapon;
}