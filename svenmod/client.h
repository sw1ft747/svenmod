#ifndef CLIENT_H
#define CLIENT_H

#ifdef _WIN32
#pragma once
#endif

#include <IClient.h>
#include <hl_sdk/common/usercmd.h>
#include <hl_sdk/common/entity_state.h>

//-----------------------------------------------------------------------------
// CClient
//-----------------------------------------------------------------------------

class CClient : public IClient
{
public:
	CClient();

public: // IClient
	virtual local_state_t		*GetLocalState();
	virtual entity_state_t		*GetEntityState();
	virtual clientdata_t		*GetClientData();

	virtual double				Time();
	virtual float				Frametime();

	virtual int					GetPlayerIndex();
	virtual int					GetViewModelIndex();

	virtual bool				IsMultiplayer();
	virtual bool				IsConnected();
	virtual bool				IsInGame();

	virtual bool				IsDead();
	virtual bool				IsDying();
	virtual bool				IsSpectating();
	
	virtual bool				IsOnGround();
	virtual bool				IsOnLadder();

	virtual bool				IsDucked();
	virtual bool				IsDucking();

	virtual bool				HasWeapon();
	virtual bool				CanAttack();

	virtual float				GetHealth();
	virtual int					GetFlags();
	virtual int					GetMoveType();
	virtual int					GetEffects();
	virtual int					GetGroundEntity();
	virtual float				GetFallVelocity();
	virtual float				GetMaxSpeed();
	virtual float				GetClientMaxSpeed();

	virtual int					GetWaterLevel();
	virtual int					GetOldWaterLevel();
	virtual int					GetWaterType();

	virtual int					GetCurrentWeaponID();
	virtual int					GetWeaponAnim();
	virtual float				GetNextAttack();

	virtual int					GetObserverMode();
	virtual int					GetObserverTarget();

	virtual unsigned int		GetRandomSeed();
	virtual float				GetFOV();

	virtual int					Buttons();
	virtual int					ButtonLast();
	virtual int					ButtonPressed();
	virtual int					ButtonReleased();

	virtual Vector				GetViewAngles();

	virtual Vector				GetOrigin();
	virtual const Vector		&GetOrigin() const;
	
	virtual Vector				GetVelocity();
	virtual const Vector		&GetVelocity() const;

	virtual Vector				GetBaseVelocity();
	virtual const Vector		&GetBaseVelocity() const;

	virtual Vector				GetViewOffset();
	virtual const Vector		&GetViewOffset() const;

	virtual Vector				GetAngles();
	virtual const Vector		&GetAngles() const;

	virtual Vector				GetForwardVector();
	virtual const Vector		&GetForwardVector() const;

	virtual Vector				GetRightVector();
	virtual const Vector		&GetRightVector() const;

	virtual Vector				GetUpVector();
	virtual const Vector		&GetUpVector() const;

public:
	void Update(local_state_t *from, local_state_t *to, usercmd_t *cmd, double time, unsigned int random_seed);
#ifdef SVENMOD_DEBUG
	void ShowDebugInfo();
#endif

	FORCEINLINE local_state_t *GetLocalStateNV() { return &m_LocalState; };

private:
#ifdef SVENMOD_DEBUG
	void ShowWeaponData();
	void ShowClientData();
	void ShowEntityState();
	void ShowPlayerMove();
#endif

private:
	local_state_t m_LocalState;
	unsigned int m_uiRandomSeed;
	double m_dbTime;

	int m_nButtons;
	int m_afButtonLast;
	int m_afButtonPressed;
	int m_afButtonReleased;
};

extern CClient g_Client;

#endif // CLIENT_H