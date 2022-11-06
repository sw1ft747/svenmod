/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
//
//  hud.h
//
// class CHud declaration
//
// CHud handles the message, calculation, and drawing the HUD
//

#ifndef SDK_HUD_H
#define SDK_HUD_H

#ifdef _WIN32
#pragma once
#endif

#define RGB_YELLOWISH 0x00FFA000 //255,160,0
#define RGB_REDISH 0x00FF1010	 //255,160,0
#define RGB_GREENISH 0x0000A000	 //0,160,0

#include "../common/common_types.h"
#include "../cl_dll/cl_dll.h"

#include "ammo.h"

#define DHN_DRAWZERO 1
#define DHN_2DIGITS 2
#define DHN_3DIGITS 4
#define MIN_ALPHA 100

#define HUDELEM_ACTIVE 1

typedef struct
{
	int x, y;
} POSITION;

#include "global_consts.h"

typedef struct
{
	unsigned char r, g, b, a;
} RGBA;

#define HUD_ACTIVE 1
#define HUD_INTERMISSION 2

#define MAX_PLAYER_NAME_LENGTH 32

#define MAX_MOTD_LENGTH 1536

//
//-----------------------------------------------------
//
class CHudBase
{
public:
	virtual ~CHudBase() { }
	virtual bool Init() { return false; }
	virtual bool VidInit() { return false; }
	virtual bool Draw(float flTime) { return false; }
	virtual void Think() { }
	virtual void Reset() { }
	virtual void InitHUDData() { } // called every time a server is connected to

public:
	POSITION m_pos;
	int m_type;
	int m_iFlags; // active, moving,
};

struct HUDLIST
{
	CHudBase *p;
	HUDLIST *pNext;
};



//
//-----------------------------------------------------
//

#ifndef INSET_OFF
#define INSET_OFF 0
#define INSET_CHASE_FREE 1
#define INSET_IN_EYE 2
#define INSET_MAP_FREE 3
#define INSET_MAP_CHASE 4
#endif

#define MAX_SPEC_HUD_MESSAGES 8

#define OVERVIEW_TILE_SIZE 128 // don't change this
#define OVERVIEW_MAX_LAYERS 1

extern void VectorAngles(const float *forward, float *angles);
void NormalizeAngles(float *angles);

//-----------------------------------------------------------------------------
// Purpose: Handles the drawing of the spectator stuff (camera & top-down map and all the things on it )
//-----------------------------------------------------------------------------

#include "interpolation.h"

typedef struct overviewInfo_s
{
	char map[64];  // cl.levelname or empty
	Vector origin; // center of map
	float zoom;	   // zoom of map images
	int layers;	   // how may layers do we have
	float layersHeights[OVERVIEW_MAX_LAYERS];
	char layersImages[OVERVIEW_MAX_LAYERS][255];
	bool rotated; // are map images rotated (90 degrees) ?

	int insetWindowX;
	int insetWindowY;
	int insetWindowHeight;
	int insetWindowWidth;
} overviewInfo_t;

typedef struct overviewEntity_s
{
	VHSPRITE hSprite;
	struct cl_entity_s *entity;
	double killTime;
} overviewEntity_t;

typedef struct cameraWayPoint_s
{
	float time;
	Vector position;
	Vector angle;
	float fov;
	int flags;
} cameraWayPoint_t;

#define MAX_OVERVIEW_ENTITIES 128
#define MAX_CAM_WAYPOINTS 32

class CHudSpectator : public CHudBase
{
public:
	void Reset() override;
	int ToggleInset(bool allowOff);
	void CheckSettings();
	void InitHUDData() override;
	bool AddOverviewEntityToList(VHSPRITE sprite, cl_entity_t *ent, double killTime);
	void DeathMessage(int victim);
	bool AddOverviewEntity(int type, struct cl_entity_s *ent, const char *modelname);
	void CheckOverviewEntities();
	void DrawOverview();
	void DrawOverviewEntities();
	void GetMapPosition(float *returnvec);
	void DrawOverviewLayer();
	void LoadMapSprites();
	bool ParseOverviewFile();
	bool IsActivePlayer(cl_entity_t *ent);
	void SetModes(int iMainMode, int iInsetMode);
	void HandleButtonsDown(int ButtonPressed);
	void HandleButtonsUp(int ButtonPressed);
	void FindNextPlayer(bool bReverse);
	void FindPlayer(const char *name);
	void DirectorMessage(int iSize, void *pbuf);
	void SetSpectatorStartPosition();
	bool Init() override;
	bool VidInit() override;

	bool Draw(float flTime) override;

	void AddWaypoint(float time, Vector pos, Vector angle, float fov, int flags);
	void SetCameraView(Vector pos, Vector angle, float fov);
	float GetFOV();
	bool GetDirectorCamera(Vector &position, Vector &angle);
	void SetWayInterpolation(cameraWayPoint_t *prev, cameraWayPoint_t *start, cameraWayPoint_t *end, cameraWayPoint_t *next);

public:
	// FIXME: validate offsets
	int m_iDrawCycle;
	client_textmessage_t m_HUDMessages[MAX_SPEC_HUD_MESSAGES];
	char m_HUDMessageText[MAX_SPEC_HUD_MESSAGES][128];
	int m_lastHudMessage;
	overviewInfo_t m_OverviewData;
	overviewEntity_t m_OverviewEntities[MAX_OVERVIEW_ENTITIES];
	int m_iObserverFlags;
	int m_iSpectatorNumber;

	float m_mapZoom;	// zoom the user currently uses
	Vector m_mapOrigin; // origin where user rotates around
	cvar_t *m_drawnames;
	cvar_t *m_drawcone;
	cvar_t *m_drawstatus;
	cvar_t *m_autoDirector;
	cvar_t *m_pip;
	bool m_chatEnabled;

	bool m_IsInterpolating;
	int m_ChaseEntity;	   // if != 0, follow this entity with viewangles
	int m_WayPoint;		   // current waypoint 1
	int m_NumWayPoints;	   // current number of waypoints
	Vector m_cameraOrigin; // a help camera
	Vector m_cameraAngles; // and it's angles
	CInterpolation m_WayInterpolation;

private:
	// FIXME: validate offsets
	Vector m_vPlayerPos[MAX_PLAYERS_HUD];
	VHSPRITE m_hsprPlayerBlue;
	VHSPRITE m_hsprPlayerRed;
	VHSPRITE m_hsprPlayer;
	VHSPRITE m_hsprCamera;
	VHSPRITE m_hsprPlayerDead;
	VHSPRITE m_hsprViewcone;
	VHSPRITE m_hsprUnkownMap;
	VHSPRITE m_hsprBeam;
	VHSPRITE m_hCrosshair;

	Rect m_crosshairRect;

	struct model_s *m_MapSprite; // each layer image is saved in one sprite, where each tile is a sprite frame
	float m_flNextObserverInput;
	float m_FOV;
	float m_zoomDelta;
	float m_moveDelta;
	int m_lastPrimaryObject;
	int m_lastSecondaryObject;

	cameraWayPoint_t m_CamPath[MAX_CAM_WAYPOINTS];
};

//
//-----------------------------------------------------
//
class CHudAmmo : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float flTime) override;
	void Think() override;
	void Reset() override;
	bool DrawWList(float flTime);
	bool MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_WeaponList(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_AmmoX(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_AmmoPickup(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_WeapPickup(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_ItemPickup(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_HideWeapon(const char *pszName, int iSize, void *pbuf);

	void SlotInput(int iSlot);
	void UserCmd_Slot1();
	void UserCmd_Slot2();
	void UserCmd_Slot3();
	void UserCmd_Slot4();
	void UserCmd_Slot5();
	void UserCmd_Slot6();
	void UserCmd_Slot7();
	void UserCmd_Slot8();
	void UserCmd_Slot9();
	void UserCmd_Slot10();
	void UserCmd_Close();
	void UserCmd_NextWeapon();
	void UserCmd_PrevWeapon();

private:
	// FIXME: validate offsets
	float m_fFade;
	RGBA m_rgba;
	WEAPON *m_pWeapon;
	int m_HUD_bucket0;
	int m_HUD_selection;
};

//
//-----------------------------------------------------
//

class CHudAmmoSecondary : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	void Reset() override;
	bool Draw(float flTime) override;

	bool MsgFunc_SecAmmoVal(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_SecAmmoIcon(const char *pszName, int iSize, void *pbuf);

private:
	enum
	{
		MAX_SEC_AMMO_VALUES = 4
	};

	// FIXME: validate offsets
	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];
	float m_fFade;
};

//
//-----------------------------------------------------
//

#define DMG_IMAGE_LIFE 2 // seconds that image is up

#define DMG_IMAGE_POISON 0
#define DMG_IMAGE_ACID 1
#define DMG_IMAGE_COLD 2
#define DMG_IMAGE_DROWN 3
#define DMG_IMAGE_BURN 4
#define DMG_IMAGE_NERVE 5
#define DMG_IMAGE_RAD 6
#define DMG_IMAGE_SHOCK 7
//tf defines
#define DMG_IMAGE_CALTROP 8
#define DMG_IMAGE_TRANQ 9
#define DMG_IMAGE_CONCUSS 10
#define DMG_IMAGE_HALLUC 11
#define NUM_DMG_TYPES 12
// instant damage

typedef struct
{
	float fExpire;
	float fBaseline;
	int x, y;
} DAMAGE_IMAGE;

// =========================

class CHudHealth : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float fTime) override;
	void Reset() override;
	bool MsgFunc_Health(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_Damage(const char *pszName, int iSize, void *pbuf);
	int m_iHealth;
	int m_HUD_dmg_bio;
	int m_HUD_cross;
	float m_fAttackFront, m_fAttackRear, m_fAttackLeft, m_fAttackRight;
	void GetPainColor(int &r, int &g, int &b);
	float m_fFade;

private:
	// FIXME: validate offsets
	VHSPRITE m_hSprite;
	VHSPRITE m_hDamage;

	DAMAGE_IMAGE m_dmg[NUM_DMG_TYPES];
	int m_bitsDamage;
	bool DrawPain(float fTime);
	bool DrawDamage(float fTime);
	void CalcDamageDirection(Vector vecFrom);
	void UpdateTiles(float fTime, long bits);
};


#define FADE_TIME 100


//
//-----------------------------------------------------
//
class CHudGeiger : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float flTime) override;
	bool MsgFunc_Geiger(const char *pszName, int iSize, void *pbuf);

private:
	// FIXME: validate offsets
	int m_iGeigerRange;
};

//
//-----------------------------------------------------
//
class CHudTrain : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float flTime) override;
	bool MsgFunc_Train(const char *pszName, int iSize, void *pbuf);

private:
	// FIXME: validate offsets
	VHSPRITE m_hSprite;
	int m_iPos;
};

//
//-----------------------------------------------------
//
class CHudStatusBar : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float flTime) override;
	void Reset() override;
	void ParseStatusString(int line_num);

	bool MsgFunc_StatusText(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_StatusValue(const char *pszName, int iSize, void *pbuf);

protected:
	enum
	{
		MAX_STATUSTEXT_LENGTH = 128,
		MAX_STATUSBAR_VALUES = 8,
		MAX_STATUSBAR_LINES = 3,
	};

	// FIXME: validate offsets
	char m_szStatusText[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH]; // a text string describing how the status bar is to be drawn
	char m_szStatusBar[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];	 // the constructed bar that is drawn
	int m_iStatusValues[MAX_STATUSBAR_VALUES];						 // an array of values for use in the status bar

	bool m_bReparseString; // set to true whenever the m_szStatusBar needs to be recalculated

	// an array of colors...one color for each line
	float *m_pflNameColors[MAX_STATUSBAR_LINES];
};

struct extra_player_info_t // sizeof 88 * 33 = 2904
{
	float frags;
	int deaths;
	short playerclass;

	//short health; // UNUSED currently, spectator UI would like this
	//bool dead; // UNUSED currently, spectator UI would like this

	short teamnumber;
	char teamname[64];

	float health;
	float armor;

	char pad_1[4];
};

struct team_info_t
{
	char name[MAX_TEAM_NAME];

	// Sven Co-op specific
	char pad_1[60];

	short frags;
	short deaths;
	short ping;
	short packetloss;
	bool ownteam;
	short players;
	bool already_drawn;
	bool scores_overriden;
	int teamnumber;

	// Sven Co-op specific
	char pad_2[8];
};

#include "player_info.h"

//
//-----------------------------------------------------
//
class CHudDeathNotice : public CHudBase
{
public:
	bool Init() override;
	void InitHUDData() override;
	bool VidInit() override;
	bool Draw(float flTime) override;
	bool MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf);

private:
	// FIXME: validate offsets
	int m_HUD_d_skull; // sprite index of skull icon
};

//
//-----------------------------------------------------
//
class CHudMenu : public CHudBase
{
public:
	bool Init() override;
	void InitHUDData() override;
	bool VidInit() override;
	void Reset() override;
	bool Draw(float flTime) override;
	bool MsgFunc_ShowMenu(const char *pszName, int iSize, void *pbuf);

	void SelectMenuItem(int menu_item);

public:
	// FIXME: validate offsets
	bool m_fMenuDisplayed;
	int m_bitsValidSlots;
	float m_flShutoffTime;
	bool m_fWaitingForMore;
};

//
//-----------------------------------------------------
//

class CHudBaseTextBlock : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float flTime) override;
	// CHudBase::Think - not overriden
	// CHudBase::Reset - not overriden
	void InitHUDData() override;
	virtual void FlushText();
};

class CHudSayText : public CHudBaseTextBlock
{
public:
	bool Init() override;
	// CHudBaseTextBlock::VidInit() - not overriden
	bool Draw(float flTime) override;
	// CHudBase::Think - not overriden
	// CHudBase::Reset - not overriden
	// CHudBaseTextBlock::InitHUDData - not overriden
	// CHudBaseTextBlock::FlushText - not overriden
	virtual float GetScrollTime();
	// @messageType: 0 - client, 1 - server. Should be more? I didn't go further lol
	virtual void PrintText(int messageType, const char* pszBuf, int iBufSize, int clientIndex);
	virtual void UpdatePos();
	
	bool MsgFunc_SayText(const char *pszName, int iSize, void *pbuf);
	void SayTextPrint(const char *pszBuf, int iBufSize, int clientIndex = -1);
	void EnsureTextFitsInOneLineAndWrapIfHaveTo(int line);
	friend class CHudSpectator;

private:
	// FIXME: validate offsets
	struct cvar_s *m_HUD_saytext;
	struct cvar_s *m_HUD_saytext_time;
};

//
//-----------------------------------------------------
//
class CHudBattery : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float flTime) override;
	bool MsgFunc_Battery(const char *pszName, int iSize, void *pbuf);

private:
	// FIXME: validate offsets
	VHSPRITE m_hSprite1;
	VHSPRITE m_hSprite2;
	Rect *m_prc1;
	Rect *m_prc2;
	int m_iBat;
	int m_iBatMax;
	float m_fFade;
	int m_iHeight; // width of the battery innards
};


//
//-----------------------------------------------------
//
class CHudFlashlight : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float flTime) override;
	void Reset() override;
	bool MsgFunc_Flashlight(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_FlashBat(const char *pszName, int iSize, void *pbuf);

private:
	// FIXME: validate offsets
	VHSPRITE m_hSprite1;
	VHSPRITE m_hSprite2;
	VHSPRITE m_hBeam;
	Rect *m_prc1;
	Rect *m_prc2;
	Rect *m_prcBeam;
	float m_flBat;
	int m_iBat;
	bool m_fOn;
	float m_fFade;
	int m_iWidth; // width of the battery innards
};

//
//-----------------------------------------------------
//
const int maxHUDMessages = 16;
struct message_parms_t
{
	client_textmessage_t *pMessage;
	float time;
	int x, y;
	int totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

//
//-----------------------------------------------------
//

class CHudTextMessage : public CHudBase
{
public:
	bool Init() override;
	static char *LocaliseTextString(const char *msg, char *dst_buffer, int buffer_size);
	static char *BufferedLocaliseTextString(const char *msg);
	const char *LookupString(const char *msg_name, int *msg_dest = NULL);
	bool MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf);
};

//
//-----------------------------------------------------
//

class CHudMessage : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float flTime) override;
	bool MsgFunc_HudText(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_HudTextPro(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_GameTitle(const char *pszName, int iSize, void *pbuf);

	float FadeBlend(float fadein, float fadeout, float hold, float localTime);
	int XPosition(float x, int width, int lineWidth);
	int YPosition(float y, int height);

	void MessageAdd(const char *pName, float time);
	void MessageAdd(client_textmessage_t *newMessage);
	void MessageDrawScan(client_textmessage_t *pMessage, float time);
	void MessageScanStart();
	void MessageScanNextChar();
	void Reset() override;

private:
	// FIXME: validate offsets
	client_textmessage_t *m_pMessages[maxHUDMessages];
	float m_startTime[maxHUDMessages];
	message_parms_t m_parms;
	float m_gameTitleTime;
	client_textmessage_t *m_pGameTitle;

	int m_HUD_title_life;
	int m_HUD_title_half;
};

//
//-----------------------------------------------------
//
#define MAX_SPRITE_NAME_LENGTH 24

class CHudStatusIcons : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	void Reset() override;
	bool Draw(float flTime) override;
	bool MsgFunc_StatusIcon(const char *pszName, int iSize, void *pbuf);

	enum
	{
		MAX_ICONSPRITENAME_LENGTH = MAX_SPRITE_NAME_LENGTH,
		MAX_ICONSPRITES = 4,
	};


	//had to make these public so CHud could access them (to enable concussion icon)
	//could use a friend declaration instead...
	void EnableIcon(const char *pszIconName, unsigned char red, unsigned char green, unsigned char blue);
	void DisableIcon(const char *pszIconName);

private:
	typedef struct
	{
		char szSpriteName[MAX_ICONSPRITENAME_LENGTH];
		VHSPRITE spr;
		Rect rc;
		unsigned char r, g, b;
	} icon_sprite_t;

	// FIXME: validate offsets
	icon_sprite_t m_IconList[MAX_ICONSPRITES];
};

//
//-----------------------------------------------------
//


class CHud
{
private:
	HUDLIST *m_pHudList;

	// FIXME: validate offsets
	VHSPRITE m_hsprLogo;
	int m_iLogo;
	client_sprite_t *m_pSpriteList;
	int m_iSpriteCount;
	int m_iSpriteCountAllRes;
	float m_flMouseSensitivity; // (as_byte)this + 0x16
	int m_iConcussionEffect;

public:
	// FIXME: validate offsets
	VHSPRITE m_hsprCursor;
	float m_flTime;		  // the current client time
	float m_fOldTime;	  // the time at which the HUD was last redrawn
	double m_flTimeDelta; // the difference between flTime and fOldTime
	Vector m_vecOrigin;
	Vector m_vecAngles;
	int m_iKeyBits;
	int m_iHideHUDDisplay;
	int m_iFOV; // (as_byte)this + 0x44
	bool m_Teamplay;
	int m_iRes;
	cvar_t *m_pCvarStealMouse;
	cvar_t *m_pCvarDraw;

	int m_iFontHeight;
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b);
	int DrawHudString(int x, int y, int iMaxX, const char *szString, int r, int g, int b);
	int DrawHudStringReverse(int xpos, int ypos, int iMinX, const char *szString, int r, int g, int b);
	int DrawHudNumberString(int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b);
	int GetNumWidth(int iNumber, int iFlags);

	int GetHudNumberWidth(int number, int width, int flags);
	int DrawHudNumberReverse(int x, int y, int number, int flags, int r, int g, int b);

private:
	// the memory for these arrays are allocated in the first call to CHud::VidInit(), when the hud.txt and associated sprites are loaded.
	// freed in ~CHud()

	// FIXME: validate offsets
	VHSPRITE *m_rghSprites; /*[HUD_SPRITE_COUNT]*/ // the sprites loaded from hud.txt
	Rect *m_rgrcRects;							  /*[HUD_SPRITE_COUNT]*/
	char *m_rgszSpriteNames;					  /*[HUD_SPRITE_COUNT][MAX_SPRITE_NAME_LENGTH]*/

	struct cvar_s *default_fov;

public:
	VHSPRITE GetSprite(int index)
	{
		return (index < 0) ? 0 : m_rghSprites[index];
	}

	Rect &GetSpriteRect(int index)
	{
		return m_rgrcRects[index];
	}


	int GetSpriteIndex(const char *SpriteName); // gets a sprite index, for use in the m_rghSprites[] array

	// FIXME: validate offsets
	CHudAmmo m_Ammo;
	CHudHealth m_Health;
	CHudSpectator m_Spectator;
	CHudGeiger m_Geiger;
	CHudBattery m_Battery;
	CHudTrain m_Train;
	CHudFlashlight m_Flash;
	CHudMessage m_Message;
	CHudStatusBar m_StatusBar;
	CHudDeathNotice m_DeathNotice;
	CHudSayText m_SayText;
	CHudMenu m_Menu;
	CHudAmmoSecondary m_AmmoSecondary;
	CHudTextMessage m_TextMessage;
	CHudStatusIcons m_StatusIcons;

	void Init();
	void VidInit();
	void Think();
	bool Redraw(float flTime, bool intermission);
	bool UpdateClientData(client_data_t *cdata, float time);

	CHud() : m_iSpriteCount(0), m_pHudList(NULL)
	{
	}
	~CHud(); // destructor, frees allocated memory

	// user messages
	bool MsgFunc_Damage(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_Logo(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf);
	void MsgFunc_InitHUD(const char *pszName, int iSize, void *pbuf);
	void MsgFunc_ViewMode(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_SetFOV(const char *pszName, int iSize, void *pbuf);
	bool MsgFunc_Concuss(const char *pszName, int iSize, void *pbuf);

	// Screen information
	// Not presented in the CHud class
	SCREENINFO m_scrinfo;

	// FIXME: validate offsets
	int m_iWeaponBits;
	bool m_fPlayerDead;
	bool m_iIntermission;

	// sprite indexes
	int m_HUD_number_0;


	void AddHudElem(CHudBase *p);

	float GetSensitivity();
};

extern CHud gHUD;

extern int g_iPlayerClass;
extern int g_iTeamNumber;
extern int g_iUser1;
extern int g_iUser2;
extern int g_iUser3;

#endif