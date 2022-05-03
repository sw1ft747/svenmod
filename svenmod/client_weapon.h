#ifndef CLIENTWEAPON_H
#define CLIENTWEAPON_H

#ifdef _WIN32
#pragma once
#endif

#include <IClientWeapon.h>

//-----------------------------------------------------------------------------
// CClientWeapon
//-----------------------------------------------------------------------------

class CClientWeapon : public IClientWeapon
{
public:
	CClientWeapon();

public: // IClientWeapon
	virtual weapon_data_t	*GetWeaponDataList();

	virtual weapon_data_t	*GetWeaponData();
	virtual weapon_data_t	*GetWeaponData(int iWeaponID);

	virtual bool			IsCustom();

	virtual int				Clip();

	virtual int				PrimaryAmmo();
	virtual int				SecondaryAmmo();

	virtual int				PrimaryAmmoType();
	virtual int				SecondaryAmmoType();

	virtual float			GetNextPrimaryAttack();
	virtual float			GetNextSecondaryAttack();

	virtual float			GetWeaponIdle();
	
	virtual bool			CanPrimaryAttack();
	virtual bool			CanSecondaryAttack();

	virtual bool			IsReloading();
	virtual bool			IsInZoom();

	virtual void			Reload();
	virtual void			Drop();

private:
	weapon_data_t *GetWeaponDataInternal(int iWeaponID);
};

extern int g_iCurrentWeaponID;
extern bool g_bCurrentWeaponCustom;
extern bool g_bForceWeaponReload;

extern CClientWeapon g_ClientWeapon;

#endif // CLIENTWEAPON_H