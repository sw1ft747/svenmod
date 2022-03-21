#ifndef INVENTORY_H
#define INVENTORY_H

#ifdef _WIN32
#pragma once
#endif

#include <IInventory.h>

//-----------------------------------------------------------------------------
// WeaponsResource wrapper
//-----------------------------------------------------------------------------

class CWeaponsResource
{
public:
	WEAPON *rgSlots[MAX_WEAPON_SLOTS + 1][MAX_WEAPON_POSITIONS + 1];
	int	riAmmo[MAX_AMMO_TYPES];
};

//-----------------------------------------------------------------------------
// CInventory
//-----------------------------------------------------------------------------

class CInventory : public IInventory
{
public:
	CInventory();

	void					Init( void );

	virtual WeaponsResource *GetWeaponsResource( void );

	virtual int				GetMaxWeaponSlots( void );
	virtual int				GetMaxWeaponPositions( void );

	virtual WEAPON			*GetWeapon( int iWeaponID );
	virtual WEAPON			*GetWeapon( int iSlot, int iPos );
	virtual WEAPON			*GetWeapon( const char *pszWeaponName );

	virtual WEAPON			*GetFirstPos( int iSlot );
	virtual WEAPON			*GetNextActivePos( int iSlot, int iSlotPos );

	virtual bool			HasAmmo( WEAPON *pWeapon );
	virtual int				CountAmmo( int iAmmoType );

	virtual int				GetPrimaryAmmoCount( WEAPON *pWeapon );
	virtual int				GetSecondaryAmmoCount( WEAPON *pWeapon );

	virtual void			SelectWeapon( WEAPON *pWeapon );
	virtual void			SelectWeapon( const char *pszWeaponName );
	
	virtual void			DropWeapon( WEAPON *pWeapon );
	virtual void			DropWeapon( const char *pszWeaponName );

private:
	CWeaponsResource *m_pWR;
};

extern CInventory g_Inventory;

#endif // INVENTORY_H