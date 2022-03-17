#ifndef SDK_API_PROXY_H
#define SDK_API_PROXY_H

#ifdef _WIN32
#pragma once
#endif

#include "../common/Platform.h"
#include "../common/netadr.h"
#include "../common/Sequence.h"
#include "../common/common_types.h"
#include "../common/ref_params.h"
#include "../common/cl_entity.h"
#include "../common/entity_state.h"
#include "../common/enums.h"
#include "../common/cvardef.h"
#include "../common/usermsg.h"
#include "../common/usercmd.h"
#include "../common/studio_event.h"

#include "../common/triangleapi.h"
#include "../common/r_efx.h"
#include "../common/event_api.h"
#include "../common/demo_api.h"
#include "../common/net_api.h"
#include "../common/ivoicetweak.h"

#include "../pm_shared/pm_shared.h"
#include "../cl_dll/kbutton.h"
#include "../cl_dll/cl_dll.h"

// ********************************************************
// Functions exported by the engine
// ********************************************************

typedef struct cl_enginefuncs_s
{
	// sprite handlers
	VHSPRITE				(*SPR_Load)(const char *szPicName);
	int						(*SPR_Frames)(VHSPRITE hPic);
	int						(*SPR_Height)(VHSPRITE hPic, int frame);
	int						(*SPR_Width)(VHSPRITE hPic, int frame);
	void					(*SPR_Set)(VHSPRITE hPic, int r, int g, int b);
	void					(*SPR_Draw)(int frame, int x, int y, const Rect *prc);
	void					(*SPR_DrawHoles)(int frame, int x, int y, const Rect *prc);
	void					(*SPR_DrawAdditive)(int frame, int x, int y, const Rect *prc);
	void					(*SPR_EnableScissor)(int x, int y, int width, int height);
	void					(*SPR_DisableScissor)(void);
	client_sprite_t*		(*SPR_GetList)(const char *psz, int *piCount);

	// screen handlers
	void					(*FillRGBA)(int x, int y, int width, int height, int r, int g, int b, int a);
	// before calling, do this: myScreenInfoStruct.iSize = sizeof(SCREENINFO);
	int						(*GetScreenInfo)(SCREENINFO *pscrinfo);
	void					(*SetCrosshair)(VHSPRITE hspr, Rect rc, int r, int g, int b);

	// cvar handlers
	struct cvar_s*			(*RegisterVariable)(const char *szName, const char *szValue, int flags);
	float					(*GetCvarFloat)(const char *szName);
	char*					(*GetCvarString)(const char *szName);

	// command handlers
	int						(*AddCommand)(const char *cmd_name, void (*function)(void));
	int						(*HookUserMsg)(const char *szMsgName, UserMsgHookFn pfn);

	int						(*ServerCmd)(const char *szCmdString);
	int						(*ClientCmd)(const char *szCmdString);

	void					(*GetPlayerInfo)(int ent_num, hud_player_info_t *pinfo);

	// sound handlers
	void					(*PlaySoundByName)(const char *szSound, float volume);
	void					(*PlaySoundByIndex)(int iSound, float volume);

	// vector helpers
	void					(*AngleVectors)(const float *vecAngles, float *forward, float *right, float *up);

	// text message system
	client_textmessage_t*	(*TextMessageGet)(const char *pName);
	int						(*DrawCharacter)(int x, int y, int number, int r, int g, int b);
	int						(*DrawConsoleString)(int x, int y, const char *string);
	void					(*DrawSetTextColor)(float r, float g, float b);
	void					(*DrawConsoleStringLen)(const char *string, int *length, int *height);

	void					(*ConsolePrint)(const char *string);
	void					(*CenterPrint)(const char *string);


	// Added for user input processing
	int						(*GetWindowCenterX)(void);
	int						(*GetWindowCenterY)(void);

	void					(*GetViewAngles)(float *);
	void					(*SetViewAngles)(float *);

	int						(*GetMaxClients)(void);

	void					(*Cvar_SetValue)(const char *cvar, float value);

	int						(*Cmd_Argc)(void);
	char*					(*Cmd_Argv)(int arg);
	void					(*Con_Printf)(const char *fmt, ...);
	void					(*Con_DPrintf)(const char *fmt, ...);
	void					(*Con_NPrintf)(int pos, const char *fmt, ...);
	void					(*Con_NXPrintf)(struct con_nprint_s *info, const char *fmt, ...);

	const char*				(*PhysInfo_ValueForKey)(const char *key);
	const char*				(*ServerInfo_ValueForKey)(const char *key);

	float					(*GetClientMaxspeed)(void);
	int						(*CheckParm)(const char *parm, const char **ppnext);
	void					(*Key_Event)(int key, int down);
	void					(*GetMousePosition)(int *mx, int *my);
	int						(*IsNoClipping)(void);

	struct cl_entity_s*		(*GetLocalPlayer)(void);
	struct cl_entity_s*		(*GetViewModel)(void);
	struct cl_entity_s*		(*GetEntityByIndex)(int idx);

	float					(*GetClientTime)(void);

	void					(*V_CalcShake)(void);
	void					(*V_ApplyShake)(float *origin, float *angles, float factor);

	int						(*PM_PointContents)(float *point, int *truecontents);
	int						(*PM_WaterEntity)(float *p);
	struct pmtrace_s*		(*PM_TraceLine)(float *start, float *end, int flags, int usehull, int ignore_pe);

	struct model_s*			(*LoadModel)(const char *modelname, int *index);
	int						(*CreateVisibleEntity)(int type, struct cl_entity_s *ent);

	const struct model_s*	(*GetSpritePointer)(VHSPRITE VHSPRITE);

	void					(*PlaySoundByNameAtLocation)(const char *szSound, float volume, float *origin);

	unsigned short			(*PrecacheEvent)(int type, const char *psz);
	void					(*PlaybackEvent)(int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
	
	void					(*WeaponAnim)(int iAnim, int body);

	float					(*RandomFloat)(float flLow, float flHigh);
	long					(*RandomLong)(long lLow, long lHigh);

	void					(*HookEvent)(const char *name, void (*pfnEvent)(struct event_args_s *args));
	int						(*Con_IsVisible)();
	const char*				(*GetGameDirectory)(void);
	struct cvar_s*			(*GetCvarPointer)(const char *szName);
	const char*				(*Key_NameForBinding)(const char *pBinding);
	const char*				(*GetLevelName)(void);

	void					(*GetScreenFade)(struct screenfade_s *fade);
	void					(*SetScreenFade)(struct screenfade_s *fade);

	void*					(*VGuiWrap_GetPanel)();
	void					(*VGui_ViewportPaintBackground)(int extents[4]);

	byte*					(*COM_LoadFile)(const char *path, int usehunk, int *pLength);
	char*					(*COM_ParseFile)(const char *data, const char *token);
	void					(*COM_FreeFile)(void *buffer);

	struct triangleapi_s	*pTriAPI;
	struct efx_api_s		*pEfxAPI;
	struct event_api_s		*pEventAPI;
	struct demo_api_s		*pDemoAPI;
	struct net_api_s		*pNetAPI;
	struct IVoiceTweak_s	*pVoiceTweak;

	// returns 1 if the client is a spectator only (connected to a proxy), 0 otherwise or 2 if in dev_overview mode
	int						(*IsSpectateOnly)(void);
	struct model_s*			(*LoadMapSprite)(const char *filename);

	// file search functions
	void					(*COM_AddAppDirectoryToSearchPath)(const char *pszBaseDir, const char *appName);
	int						(*COM_ExpandFilename)(const char *fileName, const char *nameOutBuffer, int nameOutBufferSize);

	// User info
	// playerNum is in the range (1, MaxClients)
	// returns NULL if player doesn't exit
	// returns "" if no value is set
	const char*				(*PlayerInfo_ValueForKey)(int playerNum, const char *key);
	void					(*PlayerInfo_SetValueForKey)(const char *key, const char *value);

	// Gets a unique ID for the specified player. This is the same even if you see the player on a different server.
	// iPlayer is an entity index, so client 0 would use iPlayer=1.
	// Returns false if there is no player on the server in the specified slot.
	qboolean				(*GetPlayerUniqueID)(int iPlayer, char playerID[16]);

	// TrackerID access
	int						(*GetTrackerIDForPlayer)(int playerSlot);
	int						(*GetPlayerForTrackerID)(int trackerID);

	// Same as pfnServerCmd, but the message goes in the unreliable stream so it can't clog the net stream
	// (but it might not get there).
	int						(*ServerCmdUnreliable)(const char *pszCmdString);

	void					(*GetMousePos)(struct Point *ppt);
	void					(*SetMousePos)(int x, int y);
	void					(*SetMouseEnable)(qboolean fEnable);

	struct cvar_s*			(*GetCvarList)(void);
	struct cmd_s*			(*GetCmdList)(void);

	char*					(*GetCvarName)(struct cvar_s *cvar);
	char*					(*GetCmdName)(struct cmd_s *cmd);

	float					(*GetServerTime)(void);
	float					(*GetServerGravity)(void);

	const struct model_s*	(*PrecacheSprite)(VHSPRITE spr);

	void					(*OverrideLightmap)(int override);
	void					(*SetLightmapColor)(float r, float g, float b);
	void					(*SetLightmapDarkness)(float dark);

	//this will always fail with the current engine
	int						(*SequenceGet)(int flags, const char *seq);

	void					(*SPR_DrawGeneric)(int frame, int x, int y, const Rect *prc, int blendsrc, int blenddst, int unknown1, int unknown2);

	//this will always fail with engine, don't call
	//it actually has paramenters but i dunno what they do
	void					(*SequencePickSentence)(void);

	//localizes hud string, uses Legacy font from skin def
	// also supports unicode strings
	int						(*VGUI2_DrawLocalizedHudString)(int x, int y, const char *str, int r, int g, int b);

	//i can't get this to work for some reason, don't use this
	int						(*VGUI2_DrawLocalizedConsoleString)(int x, int y, const char *str);

	//gets keyvalue for local player, useful for querying vgui menus or autohelp
	const char*				(*LocalPlayerInfo_ValueForKey)(const char *key);

	//another vgui2 text drawing function, i dunno how it works
	//it doesn't localize though
	void					(*VGUI2_DrawCharacter)(int x, int y, const char *text, unsigned long font);

	int						(*VGUI2_DrawUnicodeCharacter)(int x, int y, short number, int r, int g, int b, unsigned long hfont);

	//checks sound header of a sound file, determines if its a supported type
	int						(*GetApproxWavePlayLength)(const char *path);

	//for condition zero, returns interface from GameUI
	void*					(*VguiWrap2_GetCareerUI)(void);

	void					(*Cvar_Set)(const char *cvar, const char *value);

	//this actually checks for if the CareerGameInterface is found
	//and if a server is being run
	int						(*VGuiWrap2_IsInCareerMatch)(void);

	void					(*PlaySoundVoiceByName)(const char *sound, float vol, float pitch);

	void					(*PrimeMusicStream)(const char *mp3, int flags);

	//get the systems current time as a float
	float					(*Sys_FloatTime)(void);

	void					(*ProcessTutorMessageDecayBuffer)(int *array, int size);
	void					(*ConstructTutorMessageDecayBuffer)(int *array, int size);
	void					(*ResetTutorMessageDecayData)(void);

	void					(*PlaySoundByNameAtPitch)(const char *sound, float vol, float pitch);

	void					(*FillRGBABlend)(int x, int y, int width, int height, int r, int g, int b, int a);

	int						(*GetGameAppID)(void);
	struct cmdalias_s*		(*GetAliasesList)(void);

	void					(*VguiWrap2_GetMouseDelta)(int *x, int *y);

} cl_enginefuncs_t;

extern cl_enginefuncs_s *g_pEngineFuncs;

// ********************************************************
// Functions exported by the client .dll
// ********************************************************

typedef struct cl_clientfuncs_s
{
	int (*Initialize)(cl_enginefuncs_t *pEnginefuncs, int iVersion);
	void (*HUD_Init)(void);
	int (*HUD_VidInit)(void);
	int (*HUD_Redraw)(float time, int intermission);
	int (*HUD_UpdateClientData)(client_data_t *pcldata, float flTime);
	void (*HUD_Reset)(void);
	void (*HUD_PlayerMove)(struct playermove_s *ppmove, int server);
	void (*HUD_PlayerMoveInit)(struct playermove_s *ppmove);
	char (*HUD_PlayerMoveTexture)(const char *name);
	void (*IN_ActivateMouse)(void);
	void (*IN_DeactivateMouse)(void);
	void (*IN_MouseEvent)(int mstate);
	void (*IN_ClearStates)(void);
	void (*IN_Accumulate)(void);
	void (*CL_CreateMove)(float frametime, struct usercmd_s *cmd, int active);
	int (*CL_IsThirdPerson)(void);
	void (*CL_CameraOffset)(float *ofs);
	struct kbutton_s *(*KB_Find)(const char *name);
	void (*CAM_Think)(void);
	void (*V_CalcRefdef)(struct ref_params_s *pparams);
	int (*HUD_AddEntity)(int type, struct cl_entity_s *ent, const char *modelname);
	void (*HUD_CreateEntities)(void);
	void (*HUD_DrawNormalTriangles)(void);
	void (*HUD_DrawTransparentTriangles)(void);
	void (*HUD_StudioEvent)(const struct mstudioevent_s *event, const struct cl_entity_s *entity);
	void (*HUD_PostRunCmd)(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
	void (*HUD_Shutdown)(void);
	void (*HUD_TxferLocalOverrides)(struct entity_state_s *state, const struct clientdata_s *client);
	void (*HUD_ProcessPlayerState)(struct entity_state_s *dst, const struct entity_state_s *src);
	void (*HUD_TxferPredictionData)(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
	void (*Demo_ReadBuffer)(int size, unsigned const char *buffer);
	int (*HUD_ConnectionlessPacket)(struct netadr_s *net_from, const char *args, const char *response_buffer, int *response_buffer_size);
	int (*HUD_GetHullBounds)(int hullnumber, float *mins, float *maxs);
	void (*HUD_Frame)(double time);
	int (*HUD_Key_Event)(int down, int keynum, const char *pszCurrentBinding);
	void (*HUD_TempEntUpdate)(double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s *pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s *pTemp, float damp));
	struct cl_entity_s *(*HUD_GetUserEntity)(int index);
	void (*HUD_VoiceStatus)(int entindex, qboolean bTalking);
	void (*HUD_DirectorMessage)(unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);
	int (*HUD_GetStudioModelInterface)(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio);
	void (*HUD_ChatInputPosition)(int *x, int *y);
	int (*HUD_GetPlayerTeam)(int iplayer);
	void (*ClientFactory)(void);

	// From Sven Cope, never called
	void (*HUD_OnClientDisconnect)(void);

} cl_clientfuncs_t;

extern cl_clientfuncs_s *g_pClientFuncs;

#endif