#include "client.h"
#include "client_weapon.h"

#include "svenmod.h"

#include <hl_sdk/engine/APIProxy.h>
#include <hl_sdk/pm_shared/pm_shared.h>

#include <client_state.h>
#include <convar.h>
#include <IUtils.h>

#ifdef SVENMOD_DEBUG
ConVar sm_debug_new_line_height("sm_debug_new_line_height", "20", FCVAR_CLIENTDLL, "Height for each line when print anything");
ConVar sm_debug_show_weapondata("sm_debug_show_weapondata", "0", FCVAR_CLIENTDLL, "Shows on the screen current weapon's data vars");
ConVar sm_debug_show_clientdata("sm_debug_show_clientdata", "0", FCVAR_CLIENTDLL, "Shows on the screen client's data vars");
ConVar sm_debug_show_entitystate("sm_debug_show_entitystate", "0", FCVAR_CLIENTDLL, "Shows on the screen client's entity state vars");
ConVar sm_debug_show_playermove("sm_debug_show_playermove", "0", FCVAR_CLIENTDLL, "Shows on the screen player move vars");
#endif

//-----------------------------------------------------------------------------
// CClient implementation
//-----------------------------------------------------------------------------

local_state_t *CClient::GetLocalState()
{
	return &m_LocalState;
}

entity_state_t *CClient::GetEntityState()
{
	return &m_LocalState.playerstate;
}

clientdata_t *CClient::GetClientData()
{
	return &m_LocalState.client;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

double CClient::Time()
{
	return *g_pClientTime;
}

float CClient::Frametime()
{
	return static_cast<float>(*g_pFrametime);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int CClient::GetPlayerIndex()
{
	return g_pPlayerMove->player_index + 1;
}

int CClient::GetViewModelIndex()
{
	return m_LocalState.client.viewmodel;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool CClient::IsMultiplayer()
{
	return bool(g_pPlayerMove->multiplayer);
}

bool CClient::IsConnected()
{
	return *g_pClientState >= CLS_CONNECTED;
}

bool CClient::IsInGame()
{
	return *g_pClientState == CLS_ACTIVE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool CClient::IsDead()
{
	return g_pPlayerMove->iuser1 != 0 || g_pPlayerMove->dead;
}

bool CClient::IsDying()
{
	return g_pPlayerMove->dead;
}

bool CClient::IsSpectating()
{
	return g_pPlayerMove->iuser1 != 0;
}

bool CClient::IsOnGround()
{
	return g_pPlayerMove->onground != -1;
}

bool CClient::IsOnLadder()
{
	return g_pPlayerMove->movetype == MOVETYPE_FLY;
}

bool CClient::IsDucked()
{
	return g_pPlayerMove->flags & FL_DUCKING;
}

bool CClient::IsDucking()
{
	return g_pPlayerMove->bInDuck;
}

bool CClient::HasWeapon()
{
	return g_iCurrentWeaponID != WEAPON_NONE;
}

bool CClient::CanAttack()
{
	return static_cast<float>(*g_pClientTime) + m_LocalState.client.m_flNextAttack <= static_cast<float>(*g_pClientTime);
}

float CClient::GetHealth()
{
	return m_LocalState.client.health;
}

int CClient::GetFlags()
{
	return g_pPlayerMove->flags;
}

int CClient::GetMoveType()
{
	return g_pPlayerMove->movetype;
}

int CClient::GetEffects()
{
	return g_pPlayerMove->effects;
}

int CClient::GetGroundEntity()
{
	int physent = g_pPlayerMove->onground;

	if ( physent == 0 )
		return 0;

	if ( physent > 0 && physent < MAX_PHYSENTS )
	{
		return g_pPlayerMove->physents[physent].info;
	}

	return -1;
}

float CClient::GetFallVelocity()
{
	return g_pPlayerMove->flFallVelocity;
}

float CClient::GetMaxSpeed()
{
	return g_pPlayerMove->maxspeed;
}

float CClient::GetClientMaxSpeed()
{
	return g_pPlayerMove->clientmaxspeed;
}

int CClient::GetWaterLevel()
{
	return g_pPlayerMove->waterlevel;
}

int CClient::GetOldWaterLevel()
{
	return g_pPlayerMove->oldwaterlevel;
}

int CClient::GetWaterType()
{
	return g_pPlayerMove->watertype;
}

int CClient::GetCurrentWeaponID()
{
	return g_iCurrentWeaponID;
}

int CClient::GetWeaponAnim()
{
	return m_LocalState.client.weaponanim;
}

float CClient::GetNextAttack()
{
	return static_cast<float>(*g_pClientTime) + m_LocalState.client.m_flNextAttack;
}

int CClient::GetObserverMode()
{
	return g_pPlayerMove->iuser1;
}

int CClient::GetObserverTarget()
{
	return g_pPlayerMove->iuser2;
}

unsigned int CClient::GetRandomSeed()
{
	return m_uiRandomSeed;
}

float CClient::GetFOV()
{
	return m_LocalState.client.fov;
}

//-----------------------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------------------

int CClient::Buttons()
{
	return m_nButtons;
}

int CClient::ButtonLast()
{
	return m_afButtonLast;
}

int CClient::ButtonPressed()
{
	return m_afButtonPressed;
}

int CClient::ButtonReleased()
{
	return m_afButtonReleased;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Vector CClient::GetViewAngles()
{
	Vector va;
	g_pEngineFuncs->GetViewAngles(va);

	return va;
}

Vector CClient::GetOrigin()
{
	return g_pPlayerMove->origin;
}

const Vector &CClient::GetOrigin() const
{
	return g_pPlayerMove->origin;
}

Vector CClient::GetVelocity()
{
	return g_pPlayerMove->velocity;
}

const Vector &CClient::GetVelocity() const
{
	return g_pPlayerMove->velocity;
}

Vector CClient::GetBaseVelocity()
{
	return g_pPlayerMove->basevelocity;
}

const Vector &CClient::GetBaseVelocity() const
{
	return g_pPlayerMove->basevelocity;
}

Vector CClient::GetViewOffset()
{
	return g_pPlayerMove->view_ofs;
}

const Vector &CClient::GetViewOffset() const
{
	return g_pPlayerMove->view_ofs;
}

Vector CClient::GetAngles()
{
	return g_pPlayerMove->angles;
}

const Vector &CClient::GetAngles() const
{
	return g_pPlayerMove->angles;
}

Vector CClient::GetForwardVector()
{
	return g_pPlayerMove->forward;
}

const Vector &CClient::GetForwardVector() const
{
	return g_pPlayerMove->forward;
}

Vector CClient::GetRightVector()
{
	return g_pPlayerMove->right;
}

const Vector &CClient::GetRightVector() const
{
	return g_pPlayerMove->right;
}

Vector CClient::GetUpVector()
{
	return g_pPlayerMove->up;
}

const Vector &CClient::GetUpVector() const
{
	return g_pPlayerMove->up;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CClient::CClient()
{
	memset(&m_LocalState, 0, sizeof(local_state_s));
	m_uiRandomSeed = 0;

	m_nButtons = 0;
	m_afButtonLast = 0;
	m_afButtonPressed = 0;
	m_afButtonReleased = 0;
}

#ifdef SVENMOD_DEBUG
void CClient::ShowDebugInfo()
{
	ShowWeaponData();
	ShowClientData();
	ShowEntityState();
	ShowPlayerMove();
}
#endif

void CClient::Update(local_state_t *from, local_state_t *to, usercmd_t *cmd, double time, unsigned int random_seed)
{
	int buttonsChanged;

	m_nButtons = cmd->buttons;
	m_afButtonLast = from->playerstate.oldbuttons;

	buttonsChanged = m_afButtonLast ^ m_nButtons;

	m_afButtonPressed = buttonsChanged & m_nButtons;
	m_afButtonReleased = buttonsChanged & (~m_nButtons);

	m_uiRandomSeed = random_seed;
	m_dbTime = time;

	memcpy( &m_LocalState, to, sizeof(local_state_s) );

	g_bCurrentWeaponCustom = ( to->client.m_iId == 0 );
}

#ifdef SVENMOD_DEBUG
void CClient::ShowWeaponData()
{
	if ( sm_debug_show_weapondata.GetBool() )
	{
		int x = int(float(g_pUtils->GetScreenWidth()) * 0.015625f);
		int y = int(float(g_pUtils->GetScreenHeight()) * 0.0185185f);

		int offset = sm_debug_new_line_height.GetInt();

		int id = g_iCurrentWeaponID;

		g_pUtils->DrawSetTextColor(180.f / 255.f, 220.f / 255.f, 255.f / 255.f);

		g_pUtils->DrawConsoleString(x, y, "Weapon ID: %d", g_iCurrentWeaponID);
		y += offset; g_pUtils->DrawConsoleString(x, y, "Custom: %d", g_bCurrentWeaponCustom);

		y += offset * 2;
		g_pUtils->DrawConsoleString(x, y, "Clip: %d", g_pClientWeapon->Clip());
		y += offset; g_pUtils->DrawConsoleString(x, y, "PrimaryAmmo: %d", g_pClientWeapon->PrimaryAmmo());
		y += offset; g_pUtils->DrawConsoleString(x, y, "SecondaryAmmo: %d", g_pClientWeapon->SecondaryAmmo());
		y += offset; g_pUtils->DrawConsoleString(x, y, "PrimaryAmmoType: %d", g_pClientWeapon->PrimaryAmmoType());
		y += offset; g_pUtils->DrawConsoleString(x, y, "SecondaryAmmoType: %d", g_pClientWeapon->SecondaryAmmoType());

		y += offset * 2;
		g_pUtils->DrawConsoleString(x, y, "CanAttack: %d", CanAttack());
		y += offset; g_pUtils->DrawConsoleString(x, y, "CanPrimaryAttack: %d", g_pClientWeapon->CanPrimaryAttack());
		y += offset; g_pUtils->DrawConsoleString(x, y, "CanSecondaryAttack: %d", g_pClientWeapon->CanSecondaryAttack());
		y += offset; g_pUtils->DrawConsoleString(x, y, "IsReloading: %d", g_pClientWeapon->IsReloading());
		y += offset; g_pUtils->DrawConsoleString(x, y, "IsInZoom: %d", g_pClientWeapon->IsInZoom());

		y += offset * 2;
		g_pUtils->DrawConsoleString(x, y, "Current time: %.6lf", Time());
		y += offset; g_pUtils->DrawConsoleString(x, y, "NextPrimaryAttack: %.6f", g_pClientWeapon->GetNextPrimaryAttack());
		y += offset; g_pUtils->DrawConsoleString(x, y, "NextSecondaryAttack: %.6f", g_pClientWeapon->GetNextSecondaryAttack());
		y += offset; g_pUtils->DrawConsoleString(x, y, "WeaponIdle: %.6f", g_pClientWeapon->GetWeaponIdle());

		y += offset * 2;
		g_pUtils->DrawConsoleString(x, y, "weapondata.m_iClip: %d", m_LocalState.weapondata[g_iCurrentWeaponID].m_iClip);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_flNextPrimaryAttack: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].m_flNextPrimaryAttack);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_flNextSecondaryAttack: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].m_flNextSecondaryAttack);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_flTimeWeaponIdle: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].m_flTimeWeaponIdle);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_fInReload: %d", m_LocalState.weapondata[g_iCurrentWeaponID].m_fInReload);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_fInSpecialReload: %d", m_LocalState.weapondata[g_iCurrentWeaponID].m_fInSpecialReload);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_flNextReload: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].m_flNextReload);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_flPumpTime: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].m_flPumpTime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_fReloadTime: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].m_fReloadTime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_fAimedDamage: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].m_fAimedDamage);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_fNextAimBonus: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].m_fNextAimBonus);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_fInZoom: %d", m_LocalState.weapondata[g_iCurrentWeaponID].m_fInZoom);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.m_iWeaponState: %d", m_LocalState.weapondata[g_iCurrentWeaponID].m_iWeaponState);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.iuser1: %d", m_LocalState.weapondata[g_iCurrentWeaponID].iuser1);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.iuser2: %d", m_LocalState.weapondata[g_iCurrentWeaponID].iuser2);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.iuser3: %d", m_LocalState.weapondata[g_iCurrentWeaponID].iuser3);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.iuser4: %d", m_LocalState.weapondata[g_iCurrentWeaponID].iuser4);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.fuser1: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].fuser1);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.fuser2: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].fuser2);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.fuser3: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].fuser3);
		y += offset; g_pUtils->DrawConsoleString(x, y, "weapondata.fuser4: %.6f", m_LocalState.weapondata[g_iCurrentWeaponID].fuser4);
	}
}

void CClient::ShowClientData()
{
	if ( sm_debug_show_clientdata.GetBool() )
	{
		int x = int(float(g_pUtils->GetScreenWidth()) * 0.73f);
		int y = int(float(g_pUtils->GetScreenHeight()) * 0.0185185f);

		int offset = sm_debug_new_line_height.GetInt();

		g_pUtils->DrawSetTextColor(180.f / 255.f, 220.f / 255.f, 255.f / 255.f);

		g_pUtils->DrawConsoleString(x, y, "client.origin: %.6f %.6f %.6f", VectorExpand(m_LocalState.client.origin));
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.velocity: %.6f %.6f %.6f", VectorExpand(m_LocalState.client.velocity));
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.viewmodel: %d", m_LocalState.client.viewmodel);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.punchangle: %.6f %.6f %.6f", VectorExpand(m_LocalState.client.punchangle));
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.flags: %d", m_LocalState.client.flags);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.waterlevel: %d", m_LocalState.client.waterlevel);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.watertype: %d", m_LocalState.client.watertype);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.view_ofs: %.6f %.6f %.6f", VectorExpand(m_LocalState.client.view_ofs));
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.health: %.6f", m_LocalState.client.health);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.bInDuck: %d", m_LocalState.client.bInDuck);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.weapons: %d", m_LocalState.client.weapons);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.flTimeStepSound: %d", m_LocalState.client.flTimeStepSound);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.flDuckTime: %d", m_LocalState.client.flDuckTime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.flSwimTime: %d", m_LocalState.client.flSwimTime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.waterjumptime: %d", m_LocalState.client.waterjumptime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.maxspeed: %.6f", m_LocalState.client.maxspeed);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.fov: %.6f", m_LocalState.client.fov);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.weaponanim: %d", m_LocalState.client.weaponanim);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.m_iId: %d", m_LocalState.client.m_iId);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.ammo_shells: %d", m_LocalState.client.ammo_shells);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.ammo_nails: %d", m_LocalState.client.ammo_nails);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.ammo_cells: %d", m_LocalState.client.ammo_cells);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.ammo_rockets: %d", m_LocalState.client.ammo_rockets);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.m_flNextAttack: %.6f", m_LocalState.client.m_flNextAttack);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.tfstate: %d", m_LocalState.client.tfstate);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.pushmsec: %d", m_LocalState.client.pushmsec);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.deadflag: %d", m_LocalState.client.deadflag);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.iuser1: %d", m_LocalState.client.iuser1);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.iuser2: %d", m_LocalState.client.iuser2);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.iuser3: %d", m_LocalState.client.iuser3);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.iuser4: %d", m_LocalState.client.iuser4);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.fuser1: %.6f", m_LocalState.client.fuser1);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.fuser2: %.6f", m_LocalState.client.fuser2);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.fuser3: %.6f", m_LocalState.client.fuser3);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.fuser4: %.6f", m_LocalState.client.fuser4);
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.vuser1: %.6f %.6f %.6f", VectorExpand(m_LocalState.client.vuser1));
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.vuser2: %.6f %.6f %.6f", VectorExpand(m_LocalState.client.vuser2));
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.vuser3: %.6f %.6f %.6f", VectorExpand(m_LocalState.client.vuser3));
		y += offset; g_pUtils->DrawConsoleString(x, y, "client.vuser4: %.6f %.6f %.6f", VectorExpand(m_LocalState.client.vuser4));
	}
}

void CClient::ShowEntityState()
{
	if ( sm_debug_show_entitystate.GetBool() )
	{
		int x = int(float(g_pUtils->GetScreenWidth()) * 0.73f);
		int y = int(float(g_pUtils->GetScreenHeight()) * 0.0185185f);

		int offset = sm_debug_new_line_height.GetInt();

		g_pUtils->DrawSetTextColor(180.f / 255.f, 220.f / 255.f, 255.f / 255.f);

		g_pUtils->DrawConsoleString(x, y, "playerstate.entityType: %d", m_LocalState.playerstate.entityType);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.number: %d", m_LocalState.playerstate.number);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.msg_time: %.6f", m_LocalState.playerstate.msg_time);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.messagenum: %d", m_LocalState.playerstate.messagenum);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.origin: %.6f %.6f %.6f", VectorExpand(m_LocalState.playerstate.origin));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.angles: %.6f %.6f %.6f", VectorExpand(m_LocalState.playerstate.angles));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.modelindex: %d", m_LocalState.playerstate.modelindex);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.sequence: %d", m_LocalState.playerstate.sequence);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.frame: %.6f", m_LocalState.playerstate.frame);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.colormap: %d", m_LocalState.playerstate.colormap);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.skin: %d", m_LocalState.playerstate.skin);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.solid: %d", m_LocalState.playerstate.solid);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.effects: %d", m_LocalState.playerstate.effects);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.scale: %.6f", m_LocalState.playerstate.scale);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.eflags: %d", m_LocalState.playerstate.eflags);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.rendermode: %d", m_LocalState.playerstate.rendermode);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.renderamt: %d", m_LocalState.playerstate.renderamt);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.rendercolor: %d %d %d", m_LocalState.playerstate.rendercolor.r, m_LocalState.playerstate.rendercolor.g, m_LocalState.playerstate.rendercolor.b);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.renderfx: %d", m_LocalState.playerstate.renderfx);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.movetype: %d", m_LocalState.playerstate.movetype);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.animtime: %.6f", m_LocalState.playerstate.animtime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.framerate: %.6f", m_LocalState.playerstate.framerate);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.body: %d", m_LocalState.playerstate.body);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.controller: %d %d %d %d", m_LocalState.playerstate.controller[0], m_LocalState.playerstate.controller[1], m_LocalState.playerstate.controller[2], m_LocalState.playerstate.controller[3]);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.blending: %d %d %d %d", m_LocalState.playerstate.blending[0], m_LocalState.playerstate.blending[1], m_LocalState.playerstate.blending[2], m_LocalState.playerstate.blending[3]);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.velocity: %.6f %.6f %.6f", VectorExpand(m_LocalState.playerstate.velocity));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.mins: %.6f %.6f %.6f", VectorExpand(m_LocalState.playerstate.mins));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.maxs: %d", VectorExpand(m_LocalState.playerstate.maxs));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.aiment: %d", m_LocalState.playerstate.aiment);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.owner: %d", m_LocalState.playerstate.owner);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.friction: %.6f", m_LocalState.playerstate.friction);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.gravity: %.6f", m_LocalState.playerstate.gravity);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.team: %d", m_LocalState.playerstate.team);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.playerclass: %d", m_LocalState.playerstate.playerclass);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.health: %d", m_LocalState.playerstate.health);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.spectator: %d", m_LocalState.playerstate.spectator);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.weaponmodel: %d", m_LocalState.playerstate.weaponmodel);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.gaitsequence: %d", m_LocalState.playerstate.gaitsequence);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.basevelocity: %.6f %.6f %.6f", VectorExpand(m_LocalState.playerstate.basevelocity));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.usehull: %d", m_LocalState.playerstate.usehull);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.oldbuttons: %d", m_LocalState.playerstate.oldbuttons);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.onground: %d", m_LocalState.playerstate.onground);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.iStepLeft: %d", m_LocalState.playerstate.iStepLeft);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.flFallVelocity: %.6f", m_LocalState.playerstate.flFallVelocity);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.fov: %.6f", m_LocalState.playerstate.fov);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.weaponanim: %d", m_LocalState.playerstate.weaponanim);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.startpos: %.6f %.6f %.6f", VectorExpand(m_LocalState.playerstate.startpos));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.basevelocity: %.6f %.6f %.6f", VectorExpand(m_LocalState.playerstate.endpos));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.impacttime: %.6f", m_LocalState.playerstate.impacttime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playerstate.starttime: %.6f", m_LocalState.playerstate.starttime);
	}
}

void CClient::ShowPlayerMove()
{
	if ( sm_debug_show_playermove.GetBool() )
	{
		int x = int(float(g_pUtils->GetScreenWidth()) * 0.015625f);
		int y = int(float(g_pUtils->GetScreenHeight()) * 0.0185185f);

		int offset = sm_debug_new_line_height.GetInt();

		g_pUtils->DrawSetTextColor(180.f / 255.f, 220.f / 255.f, 255.f / 255.f);

		g_pUtils->DrawConsoleString(x, y, "playermove.player_index: %d", g_pPlayerMove->player_index);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.multiplayer: %d", g_pPlayerMove->multiplayer);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.time: %.6f", g_pPlayerMove->time);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.frametime: %.6f", g_pPlayerMove->frametime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.forward: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->forward));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.right: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->right));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.up: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->up));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.origin: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->origin));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.angles: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->angles));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.oldangles: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->oldangles));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.velocity: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->velocity));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.movedir: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->movedir));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.basevelocity: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->basevelocity));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.view_ofs: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->view_ofs));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.flDuckTime: %.6f", g_pPlayerMove->flDuckTime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.bInDuck: %d", g_pPlayerMove->bInDuck);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.flTimeStepSound: %d", g_pPlayerMove->flTimeStepSound);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.iStepLeft: %d", g_pPlayerMove->iStepLeft);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.flFallVelocity: %.6f", g_pPlayerMove->flFallVelocity);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.punchangle: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->punchangle));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.flSwimTime: %.6f", g_pPlayerMove->flSwimTime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.flNextPrimaryAttack: %.6f", g_pPlayerMove->flNextPrimaryAttack);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.effects: %d", g_pPlayerMove->effects);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.flags: %d", g_pPlayerMove->flags);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.usehull: %d", g_pPlayerMove->usehull);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.gravity: %.6f", g_pPlayerMove->gravity);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.friction: %.6f", g_pPlayerMove->friction);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.oldbuttons: %d", g_pPlayerMove->oldbuttons);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.waterjumptime: %.6f", g_pPlayerMove->waterjumptime);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.dead: %d", g_pPlayerMove->dead);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.deadflag: %d", g_pPlayerMove->deadflag);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.spectator: %d", g_pPlayerMove->spectator);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.movetype: %d", g_pPlayerMove->movetype);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.onground: %d", g_pPlayerMove->onground);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.waterlevel: %d", g_pPlayerMove->waterlevel);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.watertype: %d", g_pPlayerMove->watertype);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.oldwaterlevel: %d", g_pPlayerMove->oldwaterlevel);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.maxspeed: %.6f", g_pPlayerMove->maxspeed);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.clientmaxspeed: %.6f", g_pPlayerMove->clientmaxspeed);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.iuser1: %d", g_pPlayerMove->iuser1);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.iuser2: %d", g_pPlayerMove->iuser2);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.iuser3: %d", g_pPlayerMove->iuser3);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.iuser4: %d", g_pPlayerMove->iuser4);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.fuser1: %.6f", g_pPlayerMove->fuser1);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.fuser2: %.6f", g_pPlayerMove->fuser2);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.fuser3: %.6f", g_pPlayerMove->fuser3);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.fuser4: %.6f", g_pPlayerMove->fuser4);
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.vuser1: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->vuser1));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.vuser2: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->vuser2));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.vuser3: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->vuser3));
		y += offset; g_pUtils->DrawConsoleString(x, y, "playermove.vuser4: %.6f %.6f %.6f", VectorExpand(g_pPlayerMove->vuser4));
	}
}
#endif

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CClient g_Client;
IClient *g_pClient = (IClient *)&g_Client;

IClient *Client()
{
	return g_pClient;
}