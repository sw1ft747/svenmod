/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#ifndef SDK_STUDIO_EVENT_H
#define SDK_STUDIO_EVENT_H

#ifdef _WIN32
#pragma once
#endif

#define STUDIO_EVENT_SPECIFIC 0
#define STUDIO_EVENT_SCRIPTED 1000
#define STUDIO_EVENT_SHARED 2000
#define STUDIO_EVENT_CLIENT 5000

#define AE_TYPE_SERVER			( 1 << 0 )
#define AE_TYPE_SCRIPTED		( 1 << 1 )
#define AE_TYPE_SHARED			( 1 << 2 )
#define AE_TYPE_WEAPON			( 1 << 3 )
#define AE_TYPE_CLIENT			( 1 << 4 )
#define AE_TYPE_FACEPOSER		( 1 << 5 )

#define AE_NOT_AVAILABLE		-1

enum StudioAnimEvent
{
	// Uh.. are AE_ (server) events valid for GoldSrc?
	AE_INVALID = -1, // So we have something more succint to check for than '-1'
	AE_EMPTY,
	AE_NPC_LEFTFOOT, // NPC_EVENT_LEFTFOOT 2050
	AE_NPC_RIGHTFOOT, // NPC_EVENT_RIGHTFOOT 2051
	AE_NPC_BODYDROP_LIGHT, // NPC_EVENT_BODYDROP_LIGHT 2001
	AE_NPC_BODYDROP_HEAVY, // NPC_EVENT_BODYDROP_HEAVY 2002
	AE_NPC_SWISHSOUND, // NPC_EVENT_SWISHSOUND 2010
	AE_NPC_180TURN, // NPC_EVENT_180TURN 2020
	AE_NPC_ITEM_PICKUP, // NPC_EVENT_ITEM_PICKUP 2040
	AE_NPC_WEAPON_DROP, // NPC_EVENT_WEAPON_DROP 2041
	AE_NPC_WEAPON_SET_SEQUENCE_NAME, // NPC_EVENT_WEAPON_SET_SEQUENCE_NAME 2042
	AE_NPC_WEAPON_SET_SEQUENCE_NUMBER, // NPC_EVENT_WEAPON_SET_SEQUENCE_NUMBER 2043
	AE_NPC_WEAPON_SET_ACTIVITY, // NPC_EVENT_WEAPON_SET_ACTIVITY 2044
	AE_NPC_HOLSTER,
	AE_NPC_DRAW,
	AE_CL_PLAYSOUND,// CL_EVENT_SOUND 5004 // Emit a sound
	AE_CL_STOPSOUND,
	AE_SV_PLAYSOUND,
	AE_START_SCRIPTED_EFFECT,
	AE_STOP_SCRIPTED_EFFECT,
	AE_CLIENT_EFFECT_ATTACH,
	AE_MUZZLEFLASH, // Muzzle flash from weapons held by the player
	AE_NPC_MUZZLEFLASH, // Muzzle flash from weapons held by NPCs
	AE_THUMPER_THUMP, //Thumper Thump!
	AE_AMMOCRATE_PICKUP_AMMO, //Ammo crate pick up ammo!
	AE_NPC_RAGDOLL,

	// Scripted sequence events
	SCRIPT_EVENT_DEAD = 1000, // 1000 character is now dead
	SCRIPT_EVENT_NOINTERRUPT, // 1001 does not allow interrupt
	SCRIPT_EVENT_CANINTERRUPT, // 1002 will allow interrupt
	SCRIPT_EVENT_FIREEVENT, // 1003 Fires OnScriptEventXX output in the script entity, where XX is the event number from the options data.
	SCRIPT_EVENT_SOUND, // 1004 Play named wave file (on CHAN_BODY)
	SCRIPT_EVENT_SENTENCE, // 1005 Play named sentence
	SCRIPT_EVENT_INAIR, // 1006 Leave the character in air at the end of the sequence (don't find the floor)
	SCRIPT_EVENT_ENDANIMATION, // 1007 Set the animation by name after the sequence completes
	SCRIPT_EVENT_SOUND_VOICE, // 1008 Play named wave file (on CHAN_VOICE)
	SCRIPT_EVENT_SENTENCE_RND1, // 1009 Play sentence group 25% of the time
	SCRIPT_EVENT_NOT_DEAD, // 1010 Bring back to life (for life/death sequences)
	SCRIPT_EVENT_EMPHASIS, // 1011 Emphasis point for gestures
	SCRIPT_EVENT_BODYGROUPON = 1020, // 1020 Turn a bodygroup on
	SCRIPT_EVENT_BODYGROUPOFF, // Turn a bodygroup off
	SCRIPT_EVENT_BODYGROUPTEMP, // Turn a bodygroup on until this sequence ends
	SCRIPT_EVENT_FIRE_INPUT = 1100, // Fires named input on the event handler

	// NPC/Monster events
	NPC_EVENT_BODYDROP_LIGHT = 2001,
	NPC_EVENT_BODYDROP_HEAVY,
	NPC_EVENT_SWISHSOUND = 2010,
	NPC_EVENT_180TURN = 2020,
	NPC_EVENT_ITEM_PICKUP = 2040,
	NPC_EVENT_WEAPON_DROP,
	NPC_EVENT_WEAPON_SET_SEQUENCE_NAME,
	NPC_EVENT_WEAPON_SET_SEQUENCE_NUMBER,
	NPC_EVENT_WEAPON_SET_ACTIVITY,
	NPC_EVENT_LEFTFOOT = 2050,
	NPC_EVENT_RIGHTFOOT,
	NPC_EVENT_OPEN_DOOR,

	// Weapon events
	EVENT_WEAPON_MELEE_HIT = 3001,
	EVENT_WEAPON_SMG1,
	EVENT_WEAPON_MELEE_SWISH,
	EVENT_WEAPON_SHOTGUN_FIRE,
	EVENT_WEAPON_THROW,
	EVENT_WEAPON_AR1,
	EVENT_WEAPON_AR2,
	EVENT_WEAPON_HMG1,
	EVENT_WEAPON_SMG2,
	EVENT_WEAPON_MISSILE_FIRE,
	EVENT_WEAPON_SNIPER_RIFLE_FIRE,
	EVENT_WEAPON_AR2_GRENADE,
	EVENT_WEAPON_THROW2,
	EVENT_WEAPON_PISTOL_FIRE,
	EVENT_WEAPON_RELOAD,
	EVENT_WEAPON_THROW3,
	EVENT_WEAPON_RELOAD_SOUND, // Use this + EVENT_WEAPON_RELOAD_FILL_CLIP to prevent shooting during the reload animation
	EVENT_WEAPON_RELOAD_FILL_CLIP,
	EVENT_WEAPON_SMG1_BURST1 = 3101, // first round in a 3-round burst
	EVENT_WEAPON_SMG1_BURSTN, // 2, 3 rounds
	EVENT_WEAPON_AR2_ALTFIRE,
	EVENT_WEAPON_SEQUENCE_FINISHED = 3900,
	EVENT_WEAPON_LAST = 3999, //Must be the last weapon event

	// Client-side events
	CL_EVENT_MUZZLEFLASH0 = 5001, // Muzzleflash on attachment 0
	CL_EVENT_MUZZLEFLASH1 = 5011, // Muzzleflash on attachment 1
	CL_EVENT_MUZZLEFLASH2 = 5021, // Muzzleflash on attachment 2
	CL_EVENT_MUZZLEFLASH3 = 5031, // Muzzleflash on attachment 3
	CL_EVENT_SPARK0 = 5002, // Spark on attachment 0
	CL_EVENT_NPC_MUZZLEFLASH0, // Muzzleflash on attachment 0 for third person views
	CL_EVENT_NPC_MUZZLEFLASH1 = 5013, // Muzzleflash on attachment 1 for third person views
	CL_EVENT_NPC_MUZZLEFLASH2 = 5023, // Muzzleflash on attachment 2 for third person views
	CL_EVENT_NPC_MUZZLEFLASH3 = 5033, // Muzzleflash on attachment 3 for third person views
	CL_EVENT_SOUND = 5004, // Emit a sound // NOTE THIS MUST MATCH THE DEFINE AT CBaseEntity::PrecacheModel on the server!!!!!
	CL_EVENT_EJECTBRASS1 = 6001, // Eject a brass shell from attachment 1
	CL_EVENT_DISPATCHEFFECT0 = 9001, // Hook into a DispatchEffect on attachment 0
	CL_EVENT_DISPATCHEFFECT1 = 9011, // Hook into a DispatchEffect on attachment 1
	CL_EVENT_DISPATCHEFFECT2 = 9021, // Hook into a DispatchEffect on attachment 2
	CL_EVENT_DISPATCHEFFECT3 = 9031, // Hook into a DispatchEffect on attachment 3
	CL_EVENT_DISPATCHEFFECT4 = 9041, // Hook into a DispatchEffect on attachment 4
	CL_EVENT_DISPATCHEFFECT5 = 9051, // Hook into a DispatchEffect on attachment 5
	CL_EVENT_DISPATCHEFFECT6 = 9061, // Hook into a DispatchEffect on attachment 6
	CL_EVENT_DISPATCHEFFECT7 = 9071, // Hook into a DispatchEffect on attachment 7
	CL_EVENT_DISPATCHEFFECT8 = 9081, // Hook into a DispatchEffect on attachment 8
	CL_EVENT_DISPATCHEFFECT9 = 9091, // Hook into a DispatchEffect on attachment 9
	CL_EVENT_FOOTSTEP_LEFT = 6004,
	CL_EVENT_FOOTSTEP_RIGHT,
	CL_EVENT_MFOOTSTEP_LEFT, // Footstep sounds based on material underfoot.
	CL_EVENT_MFOOTSTEP_RIGHT,
	CL_EVENT_MFOOTSTEP_LEFT_LOUD, // Loud material impact sounds from feet attachments
	CL_EVENT_MFOOTSTEP_RIGHT_LOUD,
	CL_EVENT_SPRITEGROUP_CREATE = 6002, // c_env_spritegroup
	CL_EVENT_SPRITEGROUP_DESTROY
};

typedef struct mstudioevent_s
{
	int frame;
	int event;
	int type;
	char options[64];
} mstudioevent_t;

#endif