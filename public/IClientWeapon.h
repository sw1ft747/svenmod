#ifndef ICLIENTWEAPON_H
#define ICLIENTWEAPON_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "hl_sdk/cl_dll/ammo.h"
#include "hl_sdk/common/entity_state.h"

//-----------------------------------------------------------------------------
// Purpose: interface to client's weapon, mostly for the current weapon which player holds
//-----------------------------------------------------------------------------

abstract_class IClientWeapon
{
public:
	virtual					~IClientWeapon() {}

	// Returns array of MAX_WEAPONS elements
	virtual weapon_data_t	*GetWeaponDataList() = 0;

	virtual weapon_data_t	*GetWeaponData() = 0;
	virtual weapon_data_t	*GetWeaponData(int iWeaponID) = 0;
	inline weapon_data_t	*GetWeaponData(WEAPON *pWeapon) { return GetWeaponData(pWeapon->iId); }

	virtual bool			IsCustom() = 0;

	virtual int				Clip() = 0;

	virtual int				PrimaryAmmo() = 0;
	virtual int				SecondaryAmmo() = 0;

	virtual int				PrimaryAmmoType() = 0;
	virtual int				SecondaryAmmoType() = 0;

	virtual float			GetNextPrimaryAttack() = 0;
	virtual float			GetNextSecondaryAttack() = 0;

	virtual float			GetWeaponIdle() = 0;
	
	virtual bool			CanPrimaryAttack() = 0;
	virtual bool			CanSecondaryAttack() = 0;

	virtual bool			IsReloading() = 0;
	virtual bool			IsInZoom() = 0;

	virtual void			Reload() = 0;
	virtual void			Drop() = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

extern IClientWeapon *g_pClientWeapon;
PLATFORM_INTERFACE IClientWeapon *ClientWeapon();

#endif // ICLIENTWEAPON_H
