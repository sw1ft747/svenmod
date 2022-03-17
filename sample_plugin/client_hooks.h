#ifndef CLIENT_HOOKS_H
#define CLIENT_HOOKS_H

#ifdef _WIN32
#pragma once
#endif

#include <IClientHooks.h>

//-----------------------------------------------------------------------------
// Client DLL hooks
//-----------------------------------------------------------------------------

class CClientHooks : public IClientHooks
{
public:
	virtual HOOK_RESULT HUD_VidInit(void);

	virtual HOOK_RESULT HUD_Redraw(float time, int intermission);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_UpdateClientData(int *changed, client_data_t *pcldata, float flTime);

	virtual HOOK_RESULT HUD_PlayerMove(playermove_t *ppmove, int server);

	virtual HOOK_RESULT IN_ActivateMouse(void);

	virtual HOOK_RESULT IN_DeactivateMouse(void);

	virtual HOOK_RESULT IN_MouseEvent(int mstate);

	virtual HOOK_RESULT IN_ClearStates(void);

	virtual HOOK_RESULT IN_Accumulate(void);

	virtual HOOK_RESULT CL_CreateMove(float frametime, usercmd_t *cmd, int active);

	virtual HOOK_RESULT HOOK_RETURN_VALUE CL_IsThirdPerson(int *thirdperson);

	virtual HOOK_RESULT HOOK_RETURN_VALUE KB_Find(kbutton_t **button, const char *name);

	virtual HOOK_RESULT CAM_Think(void);

	virtual HOOK_RESULT V_CalcRefdef(ref_params_t *pparams);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_AddEntity(int *visible, int type, cl_entity_t *ent, const char *modelname);

	virtual HOOK_RESULT HUD_CreateEntities(void);

	virtual HOOK_RESULT HUD_DrawNormalTriangles(void);

	virtual HOOK_RESULT HUD_DrawTransparentTriangles(void);

	virtual HOOK_RESULT HUD_StudioEvent(const mstudioevent_t *studio_event, const cl_entity_t *entity);

	virtual HOOK_RESULT HUD_PostRunCmd(local_state_t *from, local_state_t *to, usercmd_t *cmd, int runfuncs, double time, unsigned int random_seed);

	virtual HOOK_RESULT HUD_TxferLocalOverrides(entity_state_t *state, const clientdata_t *client);

	virtual HOOK_RESULT HUD_ProcessPlayerState(entity_state_t *dst, const entity_state_t *src);

	virtual HOOK_RESULT HUD_TxferPredictionData(entity_state_t *ps, const entity_state_t *pps, clientdata_t *pcd, const clientdata_t *ppcd, weapon_data_t *wd, const weapon_data_t *pwd);

	virtual HOOK_RESULT Demo_ReadBuffer(int size, unsigned const char *buffer);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_ConnectionlessPacket(int *valid_packet, netadr_t *net_from, const char *args, const char *response_buffer, int *response_buffer_size);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_GetHullBounds(int *hullnumber_exist, int hullnumber, float *mins, float *maxs);

	virtual HOOK_RESULT HUD_Frame(double time);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_Key_Event(int *process_key, int down, int keynum, const char *pszCurrentBinding);

	virtual HOOK_RESULT HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, TEMPENTITY **ppTempEntFree, TEMPENTITY **ppTempEntActive, int (*Callback_AddVisibleEntity)(cl_entity_t *pEntity), void (*Callback_TempEntPlaySound)(TEMPENTITY *pTemp, float damp));

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_GetUserEntity(cl_entity_t **ent, int index);

	virtual HOOK_RESULT HUD_VoiceStatus(int entindex, qboolean bTalking);

	virtual HOOK_RESULT HUD_DirectorMessage(unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);

	virtual HOOK_RESULT HUD_ChatInputPosition(int *x, int *y);
};

//-----------------------------------------------------------------------------
// Client DLL post hooks
//-----------------------------------------------------------------------------

class CClientPostHooks : public IClientHooks
{
public:
	virtual HOOK_RESULT HUD_VidInit(void);

	virtual HOOK_RESULT HUD_Redraw(float time, int intermission);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_UpdateClientData(int *changed, client_data_t *pcldata, float flTime);

	virtual HOOK_RESULT HUD_PlayerMove(playermove_t *ppmove, int server);

	virtual HOOK_RESULT IN_ActivateMouse(void);

	virtual HOOK_RESULT IN_DeactivateMouse(void);

	virtual HOOK_RESULT IN_MouseEvent(int mstate);

	virtual HOOK_RESULT IN_ClearStates(void);

	virtual HOOK_RESULT IN_Accumulate(void);

	virtual HOOK_RESULT CL_CreateMove(float frametime, usercmd_t *cmd, int active);

	virtual HOOK_RESULT HOOK_RETURN_VALUE CL_IsThirdPerson(int *thirdperson);

	virtual HOOK_RESULT HOOK_RETURN_VALUE KB_Find(kbutton_t **button, const char *name);

	virtual HOOK_RESULT CAM_Think(void);

	virtual HOOK_RESULT V_CalcRefdef(ref_params_t *pparams);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_AddEntity(int *visible, int type, cl_entity_t *ent, const char *modelname);

	virtual HOOK_RESULT HUD_CreateEntities(void);

	virtual HOOK_RESULT HUD_DrawNormalTriangles(void);

	virtual HOOK_RESULT HUD_DrawTransparentTriangles(void);

	virtual HOOK_RESULT HUD_StudioEvent(const mstudioevent_t *studio_event, const cl_entity_t *entity);

	virtual HOOK_RESULT HUD_PostRunCmd(local_state_t *from, local_state_t *to, usercmd_t *cmd, int runfuncs, double time, unsigned int random_seed);

	virtual HOOK_RESULT HUD_TxferLocalOverrides(entity_state_t *state, const clientdata_t *client);

	virtual HOOK_RESULT HUD_ProcessPlayerState(entity_state_t *dst, const entity_state_t *src);

	virtual HOOK_RESULT HUD_TxferPredictionData(entity_state_t *ps, const entity_state_t *pps, clientdata_t *pcd, const clientdata_t *ppcd, weapon_data_t *wd, const weapon_data_t *pwd);

	virtual HOOK_RESULT Demo_ReadBuffer(int size, unsigned const char *buffer);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_ConnectionlessPacket(int *valid_packet, netadr_t *net_from, const char *args, const char *response_buffer, int *response_buffer_size);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_GetHullBounds(int *hullnumber_exist, int hullnumber, float *mins, float *maxs);

	virtual HOOK_RESULT HUD_Frame(double time);

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_Key_Event(int *process_key, int down, int keynum, const char *pszCurrentBinding);

	virtual HOOK_RESULT HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, TEMPENTITY **ppTempEntFree, TEMPENTITY **ppTempEntActive, int (*Callback_AddVisibleEntity)(cl_entity_t *pEntity), void (*Callback_TempEntPlaySound)(TEMPENTITY *pTemp, float damp));

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_GetUserEntity(cl_entity_t **ent, int index);

	virtual HOOK_RESULT HUD_VoiceStatus(int entindex, qboolean bTalking);

	virtual HOOK_RESULT HUD_DirectorMessage(unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);

	virtual HOOK_RESULT HUD_ChatInputPosition(int *x, int *y);
};

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

extern CClientHooks g_ClientHooks;
extern CClientPostHooks g_ClientPostHooks;

#endif // CLIENT_HOOKS_H