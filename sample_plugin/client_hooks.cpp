#include "client_hooks.h"

#include <dbg.h>

CClientHooks g_ClientHooks;
CClientPostHooks g_ClientPostHooks;

//-----------------------------------------------------------------------------
// Client DLL hooks
//-----------------------------------------------------------------------------

HOOK_RESULT CClientHooks::HUD_VidInit(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_Redraw(float time, int intermission)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientHooks::HUD_UpdateClientData(int *changed, client_data_t *pcldata, float flTime)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_PlayerMove(playermove_t *ppmove, int server)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::IN_ActivateMouse(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::IN_DeactivateMouse(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::IN_MouseEvent(int mstate)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::IN_ClearStates(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::IN_Accumulate(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::CL_CreateMove(float frametime, usercmd_t *cmd, int active)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientHooks::CL_IsThirdPerson(int *thirdperson)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientHooks::KB_Find(kbutton_t **button, const char *name)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::CAM_Think(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::V_CalcRefdef(ref_params_t *pparams)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientHooks::HUD_AddEntity(int *visible, int type, cl_entity_t *ent, const char *modelname)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_CreateEntities(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_DrawNormalTriangles(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_DrawTransparentTriangles(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_StudioEvent(const mstudioevent_t *studio_event, const cl_entity_t *entity)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_PostRunCmd(local_state_t *from, local_state_t *to, usercmd_t *cmd, int runfuncs, double time, unsigned int random_seed)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_TxferLocalOverrides(entity_state_t *state, const clientdata_t *client)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_ProcessPlayerState(entity_state_t *dst, const entity_state_t *src)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_TxferPredictionData(entity_state_t *ps, const entity_state_t *pps, clientdata_t *pcd, const clientdata_t *ppcd, weapon_data_t *wd, const weapon_data_t *pwd)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::Demo_ReadBuffer(int size, unsigned const char *buffer)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientHooks::HUD_ConnectionlessPacket(int *valid_packet, netadr_t *net_from, const char *args, const char *response_buffer, int *response_buffer_size)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientHooks::HUD_GetHullBounds(int *hullnumber_exist, int hullnumber, float *mins, float *maxs)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_Frame(double time)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientHooks::HUD_Key_Event(int *process_key, int down, int keynum, const char *pszCurrentBinding)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, TEMPENTITY **ppTempEntFree, TEMPENTITY **ppTempEntActive, int (*Callback_AddVisibleEntity)(cl_entity_t *pEntity), void (*Callback_TempEntPlaySound)(TEMPENTITY *pTemp, float damp))
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientHooks::HUD_GetUserEntity(cl_entity_t **ent, int index)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_DirectorMessage(unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientHooks::HUD_ChatInputPosition(int *x, int *y)
{
	return HOOK_CONTINUE;
}

//-----------------------------------------------------------------------------
// Client DLL post hooks
//-----------------------------------------------------------------------------

HOOK_RESULT CClientPostHooks::HUD_VidInit(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_Redraw(float time, int intermission)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientPostHooks::HUD_UpdateClientData(int *changed, client_data_t *pcldata, float flTime)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_PlayerMove(playermove_t *ppmove, int server)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::IN_ActivateMouse(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::IN_DeactivateMouse(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::IN_MouseEvent(int mstate)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::IN_ClearStates(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::IN_Accumulate(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::CL_CreateMove(float frametime, usercmd_t *cmd, int active)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientPostHooks::CL_IsThirdPerson(int *thirdperson)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientPostHooks::KB_Find(kbutton_t **button, const char *name)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::CAM_Think(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::V_CalcRefdef(ref_params_t *pparams)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientPostHooks::HUD_AddEntity(int *visible, int type, cl_entity_t *ent, const char *modelname)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_CreateEntities(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_DrawNormalTriangles(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_DrawTransparentTriangles(void)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_StudioEvent(const mstudioevent_t *studio_event, const cl_entity_t *entity)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_PostRunCmd(local_state_t *from, local_state_t *to, usercmd_t *cmd, int runfuncs, double time, unsigned int random_seed)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_TxferLocalOverrides(entity_state_t *state, const clientdata_t *client)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_ProcessPlayerState(entity_state_t *dst, const entity_state_t *src)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_TxferPredictionData(entity_state_t *ps, const entity_state_t *pps, clientdata_t *pcd, const clientdata_t *ppcd, weapon_data_t *wd, const weapon_data_t *pwd)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::Demo_ReadBuffer(int size, unsigned const char *buffer)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientPostHooks::HUD_ConnectionlessPacket(int *valid_packet, netadr_t *net_from, const char *args, const char *response_buffer, int *response_buffer_size)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientPostHooks::HUD_GetHullBounds(int *hullnumber_exist, int hullnumber, float *mins, float *maxs)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_Frame(double time)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientPostHooks::HUD_Key_Event(int *process_key, int down, int keynum, const char *pszCurrentBinding)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, TEMPENTITY **ppTempEntFree, TEMPENTITY **ppTempEntActive, int (*Callback_AddVisibleEntity)(cl_entity_t *pEntity), void (*Callback_TempEntPlaySound)(TEMPENTITY *pTemp, float damp))
{
	return HOOK_CONTINUE;
}

HOOK_RESULT HOOK_RETURN_VALUE CClientPostHooks::HUD_GetUserEntity(cl_entity_t **ent, int index)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_DirectorMessage(unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags)
{
	return HOOK_CONTINUE;
}

HOOK_RESULT CClientPostHooks::HUD_ChatInputPosition(int *x, int *y)
{
	return HOOK_CONTINUE;
}