#ifndef ICLIENT_H
#define ICLIENT_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "math/vector.h"
#include "hl_sdk/common/entity_state.h"

//-----------------------------------------------------------------------------
// Purpose: interface to local player
//-----------------------------------------------------------------------------

abstract_class IClient
{
public:
	virtual						~IClient() {}

	//-----------------------------------------------------------------------------
	// Pointers to saved structs
	//-----------------------------------------------------------------------------

	virtual local_state_t		*GetLocalState() = 0;
	virtual entity_state_t		*GetEntityState() = 0;
	virtual clientdata_t		*GetClientData() = 0;

	//-----------------------------------------------------------------------------
	// Time
	//-----------------------------------------------------------------------------

	virtual double				Time() = 0;
	virtual float				Frametime() = 0;

	//-----------------------------------------------------------------------------
	// 
	//-----------------------------------------------------------------------------

	virtual int					GetPlayerIndex() = 0;
	virtual int					GetViewModelIndex() = 0;

	//-----------------------------------------------------------------------------
	// Client's current state
	//-----------------------------------------------------------------------------

	virtual bool				IsMultiplayer() = 0;
	virtual bool				IsConnected() = 0;
	virtual bool				IsInGame() = 0;

	//-----------------------------------------------------------------------------
	// 
	//-----------------------------------------------------------------------------

	virtual bool				IsDead() = 0;
	virtual bool				IsDying() = 0;
	virtual bool				IsSpectating() = 0;
	
	virtual bool				IsOnGround() = 0;
	virtual bool				IsOnLadder() = 0;

	virtual bool				IsDucked() = 0;
	virtual bool				IsDucking() = 0;

	virtual bool				HasWeapon() = 0;
	virtual bool				CanAttack() = 0;

	virtual float				GetHealth() = 0;
	virtual int					GetFlags() = 0;
	virtual int					GetMoveType() = 0;
	virtual int					GetEffects() = 0;
	virtual int					GetGroundEntity() = 0;
	virtual float				GetFallVelocity() = 0;
	virtual float				GetMaxSpeed() = 0;
	virtual float				GetClientMaxSpeed() = 0;

	virtual int					GetWaterLevel() = 0;
	virtual int					GetOldWaterLevel() = 0;
	virtual int					GetWaterType() = 0;

	virtual int					GetCurrentWeaponID() = 0;
	virtual int					GetWeaponAnim() = 0;
	virtual float				GetNextAttack() = 0;

	virtual int					GetObserverMode() = 0;
	virtual int					GetObserverTarget() = 0;

	virtual unsigned int		GetRandomSeed() = 0;
	virtual float				GetFOV() = 0;

	//-----------------------------------------------------------------------------
	// Various button states
	//-----------------------------------------------------------------------------

	virtual int					Buttons() = 0;
	virtual int					ButtonLast() = 0;
	virtual int					ButtonPressed() = 0;
	virtual int					ButtonReleased() = 0;

	//-----------------------------------------------------------------------------
	// Player's view angles, offset, velocity
	//-----------------------------------------------------------------------------

	virtual Vector				GetViewAngles() = 0;

	virtual Vector				GetOrigin() = 0;
	virtual const Vector		&GetOrigin() const = 0;
	
	virtual Vector				GetVelocity() = 0;
	virtual const Vector		&GetVelocity() const = 0;

	virtual Vector				GetBaseVelocity() = 0;
	virtual const Vector		&GetBaseVelocity() const = 0;

	virtual Vector				GetViewOffset() = 0;
	virtual const Vector		&GetViewOffset() const = 0;

	virtual Vector				GetAngles() = 0;
	virtual const Vector		&GetAngles() const = 0;

	virtual Vector				GetForwardVector() = 0;
	virtual const Vector		&GetForwardVector() const = 0;

	virtual Vector				GetRightVector() = 0;
	virtual const Vector		&GetRightVector() const = 0;

	virtual Vector				GetUpVector() = 0;
	virtual const Vector		&GetUpVector() const = 0;
};

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

extern IClient *g_pClient;
PLATFORM_INTERFACE IClient *Client();

#endif // ICLIENT_H
