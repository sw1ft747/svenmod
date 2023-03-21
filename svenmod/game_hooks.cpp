#include "client.h"
#include "client_weapon.h"
#include "render.h"
#include "game_hooks.h"
#include "plugins_manager.h"
#include "gamedata_finder.h"
#include "svenmod.h"

#include <hl_sdk/engine/APIProxy.h> 

#include <IClientHooks.h>
#include <ISvenModAPI.h>
#include <ILoggingSystem.h>
#include <IHooks.h>

#include <messagebuffer.h>
#include <sys.h>
#include <dbg.h>

extern extra_player_info_t *g_pPlayerExtraInfo;

//-----------------------------------------------------------------------------
// Macro defenitions
//-----------------------------------------------------------------------------

#define FOR_EACH_HOOK(iterName) for (unsigned int i = 0; i < g_ClientHooks.size(); i++)
#define FOR_EACH_POST_HOOK(iterName) for (unsigned int i = 0; i < g_ClientPostHooks.size(); i++)

//-----------------------------------------------------------------------------
// Vars
//-----------------------------------------------------------------------------

static CMessageBuffer CurWeaponBuffer;

static bool s_bLoading = false;

static cl_clientfuncs_t s_ClientFuncsOriginal;
static cl_clientfuncs_t s_ClientFuncsHooked;

std::vector<IClientHooks *> g_ClientHooks;
std::vector<IClientHooks *> g_ClientPostHooks;

//-----------------------------------------------------------------------------
// PaintTraverse hook
//-----------------------------------------------------------------------------

DECLARE_CLASS_HOOK(void, PaintTraverse, vgui::IPanel *, vgui::VPANEL, bool, bool);

DECLARE_CLASS_FUNC(void, HOOKED_PaintTraverse, vgui::IPanel *thisptr, vgui::VPANEL vguiPanel, bool forceRepaint, bool allowForce)
{
	static vgui::VPANEL s_StaticPanel = 0;

	if ( !s_StaticPanel )
	{
		const char *pszPanelName = vgui::panel()->GetName(vguiPanel);

		if ( !strcmp(pszPanelName, "StaticPanel") )
			s_StaticPanel = vguiPanel;
	}
	else if ( s_StaticPanel == vguiPanel && *g_pClientState == CLS_ACTIVE )
	{
		g_PluginsManager.Draw();
	}

	ORIG_PaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);
}

//-----------------------------------------------------------------------------
// SCR_BeginLoadingPlaque hook
//-----------------------------------------------------------------------------

DECLARE_HOOK(void, __cdecl, SCR_BeginLoadingPlaque, int);

DECLARE_FUNC(void, __cdecl, HOOKED_SCR_BeginLoadingPlaque, int unk)
{
	s_bLoading = false;

	g_PluginsManager.OnBeginLoading();

	ORIG_SCR_BeginLoadingPlaque(unk);
}

//-----------------------------------------------------------------------------
// SCR_EndLoadingPlaque hook
//-----------------------------------------------------------------------------

DECLARE_HOOK(void, __cdecl, SCR_EndLoadingPlaque);

DECLARE_FUNC(void, __cdecl, HOOKED_SCR_EndLoadingPlaque)
{
	s_bLoading = true;

	g_PluginsManager.OnEndLoading();

	ORIG_SCR_EndLoadingPlaque();
}

//-----------------------------------------------------------------------------
// CL_Disconnect hook
//-----------------------------------------------------------------------------

DECLARE_HOOK(void, __cdecl, CL_Disconnect);

DECLARE_FUNC(void, __cdecl, HOOKED_CL_Disconnect)
{
	s_bLoading = false;

	g_PluginsManager.OnDisconnect();

	ORIG_CL_Disconnect();
}

//-----------------------------------------------------------------------------
// R_RenderScene hook
//-----------------------------------------------------------------------------

DECLARE_HOOK(void, __cdecl, R_RenderScene);

DECLARE_FUNC(void, __cdecl, HOOKED_R_RenderScene)
{
	ORIG_R_RenderScene();

	if ( *g_pClientState == CLS_ACTIVE )
	{
		g_Render.Draw();
	}
}

//-----------------------------------------------------------------------------
// User's message "CurrentWeapon" hook
//-----------------------------------------------------------------------------

UserMsgHookFn ORIG_UserMsgHook_CurWeapon = NULL;

int UserMsgHook_CurWeapon(const char *pszName, int iSize, void *pBuffer)
{
	CurWeaponBuffer.Init(pBuffer, iSize, true);
	CurWeaponBuffer.BeginReading();

	int iState = CurWeaponBuffer.ReadByte();
	int iId = CurWeaponBuffer.ReadShort();

	if ( iState )
		g_iCurrentWeaponID = iId;
	else if ( iId <= WEAPON_NONE )
		g_iCurrentWeaponID = WEAPON_NONE;

#ifdef SVENMOD_DEBUG
	Msg("CurWeapon : iState = %d\n", iState);
	Msg("CurWeapon : iId = %d\n", iId);
#endif

	return ORIG_UserMsgHook_CurWeapon(pszName, iSize, pBuffer);
}

//-----------------------------------------------------------------------------
// Client DLL hooks
//-----------------------------------------------------------------------------

static int HUD_VidInit(void)
{
	g_iCurrentWeaponID = -1;
	g_bCurrentWeaponCustom = false;

	g_Render.DrawClear();

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_VidInit();

		if (result == HOOK_STOP)
		{
			return 1;
		}
		else if (result == HOOK_CALL_STOP)
		{
			return s_ClientFuncsOriginal.HUD_VidInit();
		}
	}

	int func_result = s_ClientFuncsOriginal.HUD_VidInit();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_VidInit();
	}

	return func_result;
}

static int HUD_Redraw(float time, int intermission)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_Redraw(time, intermission);

		if (result == HOOK_STOP)
		{
		#ifdef SVENMOD_DEBUG
			g_Client.ShowDebugInfo();
		#endif
			return 1;
		}
		else if (result == HOOK_CALL_STOP)
		{
			int r = s_ClientFuncsOriginal.HUD_Redraw(time, intermission);

			g_PluginsManager.DrawHUD(time, intermission);
		#ifdef SVENMOD_DEBUG
			g_Client.ShowDebugInfo();
		#endif

			return r;
		}
	}

	int func_result = s_ClientFuncsOriginal.HUD_Redraw(time, intermission);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_Redraw(time, intermission);
	}

	g_PluginsManager.DrawHUD(time, intermission);
#ifdef SVENMOD_DEBUG
	g_Client.ShowDebugInfo();
#endif

	return func_result;
}

static FORCEINLINE void CheckClientData(client_data_t *pcldata, float flTime)
{
	if ( s_bLoading )
	{
		g_PluginsManager.OnFirstClientdataReceived( pcldata, flTime );
		s_bLoading = false;
	}
}

static int HOOK_RETURN_VALUE HUD_UpdateClientData(client_data_t *pcldata, float flTime)
{
	int changed = 0;

	int SavedRetVal = 0;
	bool bRetValOverridden = false;

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_UpdateClientData(&changed, pcldata, flTime);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = changed;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return changed;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_UpdateClientData(pcldata, flTime);
			CheckClientData( pcldata, flTime );
			return changed;
		}
	}

	changed = s_ClientFuncsOriginal.HUD_UpdateClientData(pcldata, flTime);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		int dummy = bRetValOverridden ? SavedRetVal : changed;
		HOOK_RESULT result = pClientPostHooks->HUD_UpdateClientData(&dummy, pcldata, flTime);

		if (result == HOOK_OVERRIDE)
		{
			if ( !bRetValOverridden )
			{
				SavedRetVal = dummy;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			CheckClientData( pcldata, flTime );
			return dummy;
		}
	}

	CheckClientData( pcldata, flTime );

	return bRetValOverridden ? SavedRetVal : changed;
}

static void HUD_PlayerMove(playermove_t *ppmove, int server)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_PlayerMove(ppmove, server);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_PlayerMove(ppmove, server);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_PlayerMove(ppmove, server);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_PlayerMove(ppmove, server);
	}
}

static void IN_ActivateMouse(void)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->IN_ActivateMouse();

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.IN_ActivateMouse();
			return;
		}
	}

	s_ClientFuncsOriginal.IN_ActivateMouse();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->IN_ActivateMouse();
	}
}

static void IN_DeactivateMouse(void)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->IN_DeactivateMouse();

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.IN_DeactivateMouse();
			return;
		}
	}

	s_ClientFuncsOriginal.IN_DeactivateMouse();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->IN_DeactivateMouse();
	}
}

static void IN_MouseEvent(int mstate)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->IN_MouseEvent(mstate);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.IN_MouseEvent(mstate);
			return;
		}
	}

	s_ClientFuncsOriginal.IN_MouseEvent(mstate);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->IN_MouseEvent(mstate);
	}
}

static void IN_ClearStates(void)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->IN_ClearStates();

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.IN_ClearStates();
			return;
		}
	}

	s_ClientFuncsOriginal.IN_ClearStates();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->IN_ClearStates();
	}
}

static void IN_Accumulate(void)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->IN_Accumulate();

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.IN_Accumulate();
			return;
		}
	}

	s_ClientFuncsOriginal.IN_Accumulate();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->IN_Accumulate();
	}
}

static void CL_CreateMove(float frametime, usercmd_t *cmd, int active)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->CL_CreateMove(frametime, cmd, active);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.CL_CreateMove(frametime, cmd, active);

			if ( g_bForceWeaponReload )
			{
				cmd->buttons |= IN_RELOAD;
				g_bForceWeaponReload = false;
			}

			return;
		}
	}

	s_ClientFuncsOriginal.CL_CreateMove(frametime, cmd, active);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->CL_CreateMove(frametime, cmd, active);
	}

	if ( g_bForceWeaponReload )
	{
		cmd->buttons |= IN_RELOAD;
		g_bForceWeaponReload = false;
	}
}

static int HOOK_RETURN_VALUE CL_IsThirdPerson(void)
{
	int thirdperson = 0;

	int SavedRetVal = 0;
	bool bRetValOverridden = false;

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->CL_IsThirdPerson(&thirdperson);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = thirdperson;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return thirdperson;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.CL_IsThirdPerson();
			return thirdperson;
		}
	}

	thirdperson = s_ClientFuncsOriginal.CL_IsThirdPerson();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		int dummy = bRetValOverridden ? SavedRetVal : thirdperson;
		HOOK_RESULT result = pClientPostHooks->CL_IsThirdPerson(&dummy);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = dummy;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return dummy;
		}
	}

	return bRetValOverridden ? SavedRetVal : thirdperson;
}

static kbutton_t *HOOK_RETURN_VALUE KB_Find(const char *name)
{
	kbutton_t *button = NULL;

	kbutton_t *SavedRetVal = NULL;
	bool bRetValOverridden = false;

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->KB_Find(&button, name);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = button;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return button;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.KB_Find(name);
			return button;
		}
	}

	button = s_ClientFuncsOriginal.KB_Find(name);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		kbutton_t *dummy = bRetValOverridden ? SavedRetVal : button;
		HOOK_RESULT result = pClientPostHooks->KB_Find(&dummy, name);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = dummy;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return dummy;
		}
	}

	return bRetValOverridden ? SavedRetVal : button;
}

static void CAM_Think(void)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->CAM_Think();

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.CAM_Think();
			return;
		}
	}

	s_ClientFuncsOriginal.CAM_Think();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->CAM_Think();
	}
}

static void V_CalcRefdef(ref_params_t *pparams)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->V_CalcRefdef(pparams);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.V_CalcRefdef(pparams);
			return;
		}
	}

	s_ClientFuncsOriginal.V_CalcRefdef(pparams);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->V_CalcRefdef(pparams);
	}

	g_Render.SetRenderOrigin( pparams->vieworg );
}

static int HOOK_RETURN_VALUE HUD_AddEntity(int type, cl_entity_t *ent, const char *modelname)
{
	int visible = 0;

	int SavedRetVal = 0;
	bool bRetValOverridden = false;

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_AddEntity(&visible, type, ent, modelname);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = visible;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return visible;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_AddEntity(type, ent, modelname);
			return visible;
		}
	}

	visible = s_ClientFuncsOriginal.HUD_AddEntity(type, ent, modelname);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		int dummy = bRetValOverridden ? SavedRetVal : visible;
		HOOK_RESULT result = pClientPostHooks->HUD_AddEntity(&visible, type, ent, modelname);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = dummy;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return dummy;
		}
	}

	return bRetValOverridden ? SavedRetVal : visible;
}

static void HUD_CreateEntities(void)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_CreateEntities();

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_CreateEntities();
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_CreateEntities();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_CreateEntities();
	}
}

static void HUD_DrawNormalTriangles(void)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_DrawNormalTriangles();

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_DrawNormalTriangles();
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_DrawNormalTriangles();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_DrawNormalTriangles();
	}
}

static void HUD_DrawTransparentTriangles(void)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_DrawTransparentTriangles();

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_DrawTransparentTriangles();
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_DrawTransparentTriangles();

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_DrawTransparentTriangles();
	}
}

static void HUD_StudioEvent(const mstudioevent_t *studio_event, const cl_entity_t *entity)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_StudioEvent(studio_event, entity);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_StudioEvent(studio_event, entity);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_StudioEvent(studio_event, entity);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_StudioEvent(studio_event, entity);
	}
}

static void HUD_PostRunCmd(local_state_t *from, local_state_t *to, usercmd_t *cmd, int runfuncs, double time, unsigned int random_seed)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);

		if (result == HOOK_STOP)
		{
			g_Client.Update(from, to, cmd, time, random_seed);
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
			g_Client.Update(from, to, cmd, time, random_seed);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
	}

	g_Client.Update(from, to, cmd, time, random_seed);
}

static void HUD_TxferLocalOverrides(entity_state_t *state, const clientdata_t *client)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_TxferLocalOverrides(state, client);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_TxferLocalOverrides(state, client);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_TxferLocalOverrides(state, client);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_TxferLocalOverrides(state, client);
	}
}

static void HUD_ProcessPlayerState(entity_state_t *dst, const entity_state_t *src)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_ProcessPlayerState(dst, src);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_ProcessPlayerState(dst, src);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_ProcessPlayerState(dst, src);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_ProcessPlayerState(dst, src);
	}
}

static void HUD_TxferPredictionData(entity_state_t *ps, const entity_state_t *pps, clientdata_t *pcd, const clientdata_t *ppcd, weapon_data_t *wd, const weapon_data_t *pwd)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
	}
}

static void Demo_ReadBuffer(int size, unsigned const char *buffer)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->Demo_ReadBuffer(size, buffer);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.Demo_ReadBuffer(size, buffer);
			return;
		}
	}

	s_ClientFuncsOriginal.Demo_ReadBuffer(size, buffer);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->Demo_ReadBuffer(size, buffer);
	}
}

static int HOOK_RETURN_VALUE HUD_ConnectionlessPacket(netadr_t *net_from, const char *args, const char *response_buffer, int *response_buffer_size)
{
	int valid_packet = 0;

	int SavedRetVal = 0;
	bool bRetValOverridden = false;

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_ConnectionlessPacket(&valid_packet, net_from, args, response_buffer, response_buffer_size);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = valid_packet;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return valid_packet;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);
			return valid_packet;
		}
	}

	valid_packet = s_ClientFuncsOriginal.HUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		int dummy = bRetValOverridden ? SavedRetVal : valid_packet;
		HOOK_RESULT result = pClientPostHooks->HUD_ConnectionlessPacket(&valid_packet, net_from, args, response_buffer, response_buffer_size);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = dummy;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return dummy;
		}
	}

	return bRetValOverridden ? SavedRetVal : valid_packet;
}

static int HOOK_RETURN_VALUE HUD_GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	int hullnumber_exist = 0;

	int SavedRetVal = 0;
	bool bRetValOverridden = false;

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_GetHullBounds(&hullnumber_exist, hullnumber, mins, maxs);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = hullnumber_exist;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return hullnumber_exist;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_GetHullBounds(hullnumber, mins, maxs);
			return hullnumber_exist;
		}
	}

	hullnumber_exist = s_ClientFuncsOriginal.HUD_GetHullBounds(hullnumber, mins, maxs);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		int dummy = bRetValOverridden ? SavedRetVal : hullnumber_exist;
		HOOK_RESULT result = pClientPostHooks->HUD_GetHullBounds(&hullnumber_exist, hullnumber, mins, maxs);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = dummy;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return dummy;
		}
	}

	return bRetValOverridden ? SavedRetVal : hullnumber_exist;
}

static void HUD_Frame(double time)
{
	extern int *g_pClientState;
	extern double *g_pFrametime;

	g_PluginsManager.Frame( *g_pClientState, *g_pFrametime, true );

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_Frame(time);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_Frame(time);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_Frame(time);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_Frame(time);
	}
}

static int HOOK_RETURN_VALUE HUD_Key_Event(int down, int keynum, const char *pszCurrentBinding)
{
	int process_key = 0;

	int SavedRetVal = 0;
	bool bRetValOverridden = false;

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_Key_Event(&process_key, down, keynum, pszCurrentBinding);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = process_key;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return process_key;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_Key_Event(down, keynum, pszCurrentBinding);
			return process_key;
		}
	}

	process_key = s_ClientFuncsOriginal.HUD_Key_Event(down, keynum, pszCurrentBinding);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		int dummy = bRetValOverridden ? SavedRetVal : process_key;
		HOOK_RESULT result = pClientPostHooks->HUD_Key_Event(&process_key, down, keynum, pszCurrentBinding);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = dummy;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return dummy;
		}
	}

	return bRetValOverridden ? SavedRetVal : process_key;
}

static void HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, TEMPENTITY **ppTempEntFree, TEMPENTITY **ppTempEntActive, int (*Callback_AddVisibleEntity)(cl_entity_t *pEntity), void (*Callback_TempEntPlaySound)(TEMPENTITY *pTemp, float damp))
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
	}
}

static cl_entity_t *HOOK_RETURN_VALUE HUD_GetUserEntity(int index)
{
	cl_entity_t *ent = NULL;

	cl_entity_t *SavedRetVal = NULL;
	bool bRetValOverridden = false;

	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_GetUserEntity(&ent, index);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = ent;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return ent;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_GetUserEntity(index);
			return ent;
		}
	}

	ent = s_ClientFuncsOriginal.HUD_GetUserEntity(index);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		cl_entity_t *dummy = bRetValOverridden ? SavedRetVal : ent;
		HOOK_RESULT result = pClientPostHooks->HUD_GetUserEntity(&ent, index);

		if (result == HOOK_OVERRIDE)
		{
			if (!bRetValOverridden)
			{
				SavedRetVal = dummy;
				bRetValOverridden = true;
			}
		}
		else if (result == HOOK_STOP)
		{
			return dummy;
		}
	}

	return bRetValOverridden ? SavedRetVal : ent;
}

static void HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_VoiceStatus(entindex, bTalking);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_VoiceStatus(entindex, bTalking);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_VoiceStatus(entindex, bTalking);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_VoiceStatus(entindex, bTalking);
	}
}

static void HUD_DirectorMessage(unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_DirectorMessage(command, firstObject, secondObject, flags);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_DirectorMessage(command, firstObject, secondObject, flags);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_DirectorMessage(command, firstObject, secondObject, flags);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_DirectorMessage(command, firstObject, secondObject, flags);
	}
}

static void HUD_ChatInputPosition(int *x, int *y)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *pClientHooks = g_ClientHooks[i];

		HOOK_RESULT result = pClientHooks->HUD_ChatInputPosition(x, y);

		if (result == HOOK_STOP)
		{
			return;
		}
		else if (result == HOOK_CALL_STOP)
		{
			s_ClientFuncsOriginal.HUD_ChatInputPosition(x, y);
			return;
		}
	}

	s_ClientFuncsOriginal.HUD_ChatInputPosition(x, y);

	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *pClientPostHooks = g_ClientPostHooks[i];

		HOOK_RESULT result = pClientPostHooks->HUD_ChatInputPosition(x, y);
	}
}

static int HUD_GetPlayerTeam(int iplayer)
{
	if (iplayer > 0 && iplayer <= MAXCLIENTS)
	{
		return g_pPlayerExtraInfo[iplayer].teamnumber;
	}

	return -1;
}

static void ClientFactory(void)
{
}

//-----------------------------------------------------------------------------
// CGameHooksHandler impl
//-----------------------------------------------------------------------------

void CGameHooksHandler::Init()
{
	s_ClientFuncsHooked =
	{
		g_pClientFuncs->Initialize,
		g_pClientFuncs->HUD_Init,
		HUD_VidInit,
		HUD_Redraw,
		HUD_UpdateClientData,
		g_pClientFuncs->HUD_Reset,
		HUD_PlayerMove,
		g_pClientFuncs->HUD_PlayerMoveInit,
		g_pClientFuncs->HUD_PlayerMoveTexture,
		IN_ActivateMouse,
		IN_DeactivateMouse,
		IN_MouseEvent,
		IN_ClearStates,
		IN_Accumulate,
		CL_CreateMove,
		CL_IsThirdPerson,
		g_pClientFuncs->CL_CameraOffset,
		KB_Find,
		CAM_Think,
		V_CalcRefdef,
		HUD_AddEntity,
		HUD_CreateEntities,
		HUD_DrawNormalTriangles,
		HUD_DrawTransparentTriangles,
		HUD_StudioEvent,
		HUD_PostRunCmd,
		g_pClientFuncs->HUD_Shutdown,
		HUD_TxferLocalOverrides,
		HUD_ProcessPlayerState,
		HUD_TxferPredictionData,
		Demo_ReadBuffer,
		HUD_ConnectionlessPacket,
		HUD_GetHullBounds,
		HUD_Frame,
		HUD_Key_Event,
		HUD_TempEntUpdate,
		HUD_GetUserEntity,
		HUD_VoiceStatus,
		HUD_DirectorMessage,
		g_pClientFuncs->HUD_GetStudioModelInterface,
		HUD_ChatInputPosition,
		HUD_GetPlayerTeam,
		ClientFactory,
		g_pClientFuncs->HUD_OnClientDisconnect
	};

	memcpy( &s_ClientFuncsOriginal, g_pClientFuncs, sizeof(cl_clientfuncs_t) );
	memcpy( g_pClientFuncs, &s_ClientFuncsHooked, sizeof(cl_clientfuncs_t) );

	void *m_pfnSCR_BeginLoadingPlaque = g_GameDataFinder.FindSCR_BeginLoadingPlaque();
	void *m_pfnSCR_EndLoadingPlaque = g_GameDataFinder.FindSCR_EndLoadingPlaque();
	void *m_pfnCL_Disconnect = g_GameDataFinder.FindCL_Disconnect();
	void *m_pfnR_RenderScene = g_GameDataFinder.FindR_RenderScene();

	m_hPaintTraverse = DetoursAPI()->DetourVirtualFunction( vgui::panel(), 41, HOOKED_PaintTraverse, GET_FUNC_PTR(ORIG_PaintTraverse) );
	m_hSCR_BeginLoadingPlaque = DetoursAPI()->DetourFunction( m_pfnSCR_BeginLoadingPlaque, HOOKED_SCR_BeginLoadingPlaque, GET_FUNC_PTR(ORIG_SCR_BeginLoadingPlaque) );
	m_hSCR_EndLoadingPlaque = DetoursAPI()->DetourFunction( m_pfnSCR_EndLoadingPlaque, HOOKED_SCR_EndLoadingPlaque, GET_FUNC_PTR(ORIG_SCR_EndLoadingPlaque) );
	m_hCL_Disconnect = DetoursAPI()->DetourFunction( m_pfnCL_Disconnect, HOOKED_CL_Disconnect, GET_FUNC_PTR(ORIG_CL_Disconnect) );
	m_hR_RenderScene = DetoursAPI()->DetourFunction( m_pfnR_RenderScene, HOOKED_R_RenderScene, GET_FUNC_PTR(ORIG_R_RenderScene) );
	m_hUserMsgHook_CurWeapon = Hooks()->HookUserMessage( "CurWeapon", UserMsgHook_CurWeapon, &ORIG_UserMsgHook_CurWeapon );

	if ( m_hPaintTraverse == DETOUR_INVALID_HANDLE )
	{
		Sys_Error("[SvenMod] Failed to hook virtual function \"PaintTraverse\"");
	}

	if ( m_hSCR_BeginLoadingPlaque == DETOUR_INVALID_HANDLE )
	{
		Sys_Error("[SvenMod] Failed to hook function \"SCR_BeginLoadingPlaque\"");
	}
	
	if ( m_hSCR_EndLoadingPlaque == DETOUR_INVALID_HANDLE )
	{
		Sys_Error("[SvenMod] Failed to hook function \"SCR_EndLoadingPlaque\"");
	}

	if ( m_hCL_Disconnect == DETOUR_INVALID_HANDLE )
	{
		Sys_Error("[SvenMod] Failed to hook function \"CL_Disconnect\"");
	}

	if ( m_hR_RenderScene == DETOUR_INVALID_HANDLE )
	{
		Sys_Error("[SvenMod] Failed to hook function \"R_RenderScene\"");
	}
}

void CGameHooksHandler::Shutdown()
{
	DetoursAPI()->RemoveDetour( m_hPaintTraverse );
	DetoursAPI()->RemoveDetour( m_hSCR_BeginLoadingPlaque );
	DetoursAPI()->RemoveDetour( m_hSCR_EndLoadingPlaque );
	DetoursAPI()->RemoveDetour( m_hCL_Disconnect );
	DetoursAPI()->RemoveDetour( m_hR_RenderScene );

	Hooks()->UnhookUserMessage( m_hUserMsgHook_CurWeapon );

	memcpy( g_pClientFuncs, &s_ClientFuncsOriginal, sizeof(cl_clientfuncs_t) );

	g_ClientHooks.clear();
	g_ClientPostHooks.clear();
}

bool CGameHooksHandler::RegisterClientHooks(IClientHooks *pClientHooks)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *clienthooks = g_ClientHooks[i];

		if (clienthooks == pClientHooks)
		{
			return false;
		}
	}

	g_ClientHooks.push_back(pClientHooks);
	return true;
}

bool CGameHooksHandler::UnregisterClientHooks(IClientHooks *pClientHooks)
{
	FOR_EACH_HOOK(i)
	{
		IClientHooks *clienthooks = g_ClientHooks[i];

		if (clienthooks == pClientHooks)
		{
			g_ClientHooks.erase(g_ClientHooks.begin() + i);
			return true;
		}
	}

	return false;
}

bool CGameHooksHandler::RegisterClientPostHooks(IClientHooks *pClientPostHooks)
{
	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *clienthooks = g_ClientPostHooks[i];

		if (clienthooks == pClientPostHooks)
		{
			return false;
		}
	}

	g_ClientPostHooks.push_back(pClientPostHooks);
	return true;
}

bool CGameHooksHandler::UnregisterClientPostHooks(IClientHooks *pClientPostHooks)
{
	FOR_EACH_POST_HOOK(i)
	{
		IClientHooks *clienthooks = g_ClientPostHooks[i];

		if (clienthooks == pClientPostHooks)
		{
			g_ClientPostHooks.erase(g_ClientPostHooks.begin() + i);
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CGameHooksHandler g_GameHooksHandler;