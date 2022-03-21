#ifndef ICLIENTINVENTORY_H
#define ICLIENTINVENTORY_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

#include "hl_sdk/cl_dll/ammohistory.h"

//-----------------------------------------------------------------------------
// Purpose: interface to client's inventory (weapons, items, etc..), not the Sven's inventory
//-----------------------------------------------------------------------------

abstract_class IInventory
{
public:
	virtual					~IInventory() {}

	virtual WeaponsResource *GetWeaponsResource( void ) = 0;

	virtual int				GetMaxWeaponSlots( void ) = 0;
	virtual int				GetMaxWeaponPositions( void ) = 0;

	// Only for reading
	virtual WEAPON			*GetWeapon( int iWeaponID ) = 0;
	virtual WEAPON			*GetWeapon( int iSlot, int iPos ) = 0;
	virtual WEAPON			*GetWeapon( const char *pszWeaponName ) = 0;

	virtual WEAPON			*GetFirstPos( int iSlot ) = 0;
	virtual WEAPON			*GetNextActivePos( int iSlot, int iSlotPos ) = 0;

	virtual bool			HasAmmo( WEAPON *pWeapon ) = 0;
	virtual int				CountAmmo( int iAmmoType ) = 0;

	virtual int				GetPrimaryAmmoCount( WEAPON *pWeapon ) = 0;
	virtual int				GetSecondaryAmmoCount( WEAPON *pWeapon ) = 0;

	virtual void			SelectWeapon( WEAPON *pWeapon ) = 0;
	virtual void			SelectWeapon( const char *pszWeaponName ) = 0;
	
	virtual void			DropWeapon( WEAPON *pWeapon ) = 0;
	virtual void			DropWeapon( const char *pszWeaponName ) = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

extern IInventory *g_pInventory;
PLATFORM_INTERFACE IInventory *Inventory();

#endif // ICLIENTINVENTORY_H
