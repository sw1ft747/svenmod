#include "inventory.h"

//-----------------------------------------------------------------------------
// CInventory implementation
//-----------------------------------------------------------------------------

CInventory::CInventory()
{
	m_pWR = NULL;
}

void CInventory::Init()
{
	m_pWR = reinterpret_cast<CWeaponsResource *>(g_pWeaponsResource);
}

WeaponsResource *CInventory::GetWeaponsResource()
{
	return reinterpret_cast<WeaponsResource *>(m_pWR);
}

int CInventory::GetMaxWeaponSlots()
{
	return MAX_WEAPON_SLOTS;
}

int CInventory::GetMaxWeaponPositions()
{
	return MAX_WEAPON_POSITIONS;
}

WEAPON *CInventory::GetWeapon(int iWeaponID)
{
	for (int i = 0; i < MAX_WEAPON_SLOTS; i++)
	{
		for (int j = 0; j < MAX_WEAPON_POSITIONS; j++)
		{
			WEAPON *pWeapon = m_pWR->rgSlots[i][j];

			if (pWeapon && pWeapon->iId == iWeaponID)
			{
				return pWeapon;
			}
		}
	}

	return NULL;
}

WEAPON *CInventory::GetWeapon(int iSlot, int iPos)
{
	return m_pWR->rgSlots[iSlot][iPos];
}

WEAPON *CInventory::GetWeapon(const char *pszWeaponName)
{
	for (int i = 0; i < MAX_WEAPON_SLOTS; i++)
	{
		for (int j = 0; j < MAX_WEAPON_POSITIONS; j++)
		{
			WEAPON *pWeapon = m_pWR->rgSlots[i][j];

			if (pWeapon && !stricmp(pWeapon->szName, pszWeaponName))
			{
				return pWeapon;
			}
		}
	}

	return NULL;
}

WEAPON *CInventory::GetFirstPos(int iSlot)
{
	WEAPON *pret = NULL;

	for (int i = 0; i < MAX_WEAPON_POSITIONS; i++)
	{
		if (m_pWR->rgSlots[iSlot][i] && HasAmmo(m_pWR->rgSlots[iSlot][i]))
		{
			pret = m_pWR->rgSlots[iSlot][i];
			break;
		}
	}

	return pret;
}

WEAPON *CInventory::GetNextActivePos(int iSlot, int iSlotPos)
{
	if (iSlotPos >= MAX_WEAPON_POSITIONS || iSlot >= MAX_WEAPON_SLOTS)
		return NULL;

	WEAPON *pWeapon = m_pWR->rgSlots[iSlot][iSlotPos + 1];

	if ( !pWeapon || !HasAmmo(pWeapon) )
		return GetNextActivePos(iSlot, iSlotPos + 1);

	return pWeapon;
}

bool CInventory::HasAmmo(WEAPON *pWeapon)
{
	// weapons with no max ammo can always be selected
	if (pWeapon->iMax1 == -1)
		return true;

	return (pWeapon->iAmmoType == -1 || pWeapon->iClip > 0 || CountAmmo(pWeapon->iAmmoType) != 0 || CountAmmo(pWeapon->iAmmo2Type) != 0 || pWeapon->iFlags & WEAPON_FLAGS_SELECTONEMPTY);
}

int CInventory::CountAmmo(int iAmmoType)
{
	if (iAmmoType < 0)
		return 0;

	return m_pWR->riAmmo[iAmmoType];
}

int CInventory::GetPrimaryAmmoCount(WEAPON *pWeapon)
{
	if ( pWeapon->iAmmoType == -1 )
		return 0;

	int ammo = CountAmmo(pWeapon->iAmmoType);

	if ( !ammo )
	{
		int clip = pWeapon->iClip;

		if (clip != -1)
			return -clip;

		return 0;
	}

	return ammo;
}

int CInventory::GetSecondaryAmmoCount(WEAPON *pWeapon)
{
	if ( pWeapon->iAmmo2Type == -1 )
		return 0;

	int ammo = CountAmmo(pWeapon->iAmmo2Type);

	if ( !ammo )
	{
		int count = pWeapon->iCount;

		if (count != -1)
			return -count;

		return 0;
	}

	return ammo;
}

void CInventory::SelectWeapon(WEAPON *pWeapon)
{
	g_pEngineFuncs->ClientCmd(pWeapon->szName);
}

void CInventory::SelectWeapon(const char *pszWeaponName)
{
	g_pEngineFuncs->ClientCmd(pszWeaponName);
}

#define CMD_BUFFER_LENGTH 140

void CInventory::DropWeapon(WEAPON *pWeapon)
{
	static char cmd[CMD_BUFFER_LENGTH] = { 0 };

	snprintf(cmd, CMD_BUFFER_LENGTH, "drop %s\n", pWeapon->szName);

	cmd[CMD_BUFFER_LENGTH - 1] = 0;

	g_pEngineFuncs->ClientCmd(cmd);
}

void CInventory::DropWeapon(const char *pszWeaponName)
{
	static char cmd[CMD_BUFFER_LENGTH] = { 0 };

	snprintf(cmd, CMD_BUFFER_LENGTH, "drop %s\n", pszWeaponName);

	cmd[CMD_BUFFER_LENGTH - 1] = 0;

	g_pEngineFuncs->ClientCmd(cmd);
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CInventory g_Inventory;
IInventory *g_pInventory = (IInventory *)&g_Inventory;

IInventory *Inventory()
{
	return g_pInventory;
}