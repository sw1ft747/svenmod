#include "sample_plugin.h"
#include "client_hooks.h"

#include <interface.h>
#include <dbg.h>
#include <convar.h>
#include <messagebuffer.h>

#include <hl_sdk/common/protocol.h>

//-----------------------------------------------------------------------------
// Export the global interface
//-----------------------------------------------------------------------------

CSamplePlugin g_SamplePlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSamplePlugin, IClientPlugin, CLIENT_PLUGIN_INTERFACE_VERSION, g_SamplePlugin);

//-----------------------------------------------------------------------------
// Example hooks
//-----------------------------------------------------------------------------

CMessageBuffer SetAngleBuffer;
CMessageBuffer VoteMenuBuffer;

NetMsgHookFn ORIG_NetMsgHook_SetAngle = NULL;
DetourHandle_t hNetMsgHook_SetAngle = 0;

UserMsgHookFn ORIG_UserMsgHook_VoteMenu = NULL;
DetourHandle_t hUserMsgHook_VoteMenu = 0;

CommandCallbackFn ORIG_version_callback = NULL;
DetourHandle_t hversion_callback = 0;

void NetMsgHook_SetAngle(void)
{
	char buffer[512];

	CNetMessageParams *params = Utils()->GetNetMessageParams();
	SetAngleBuffer.Init( params->buffer, params->readcount, params->badread );

	float pitch = SetAngleBuffer.ReadHiresAngle();
	float yaw = SetAngleBuffer.ReadHiresAngle();
	float roll = SetAngleBuffer.ReadHiresAngle();

	snprintf(buffer, sizeof(buffer) / sizeof(*buffer), "Server tried to set your view angles to \"%.1f %.1f %.1f\"\n", pitch, yaw, roll);
	Utils()->PrintChatText(buffer);

	// Apply read to the buffer to do not call an original function SVC_SETANGLE
	Utils()->ApplyReadToNetMessageBuffer( &SetAngleBuffer );
}

int UserMsgHook_VoteMenu(const char *pszName, int iSize, void *pBuffer)
{
	VoteMenuBuffer.Init(pBuffer, iSize, true);

	int type = VoteMenuBuffer.ReadByte();

	Msg("Called vote of type: %d\n", type);

	return ORIG_UserMsgHook_VoteMenu(pszName, iSize, pBuffer);
}

void version_callback(void)
{
	Msg("Protocol version: %d\n", SvenModAPI()->GetProtocolVersion());
	Msg("Engine build: %d\n", SvenModAPI()->GetEngineBuild());
	Msg("Client version: %s\n", SvenModAPI()->GetClientVersion()->string);
}

void CvarChangeHook_fps_max(cvar_t *pCvar, const char *pszOldValue, float flOldValue)
{
	char buffar[512];
	snprintf(buffar, sizeof(buffar) / sizeof(char), "Cvar \"fps_max\" has been changed from %.1f to %.1f\n", flOldValue, pCvar->value);

	Utils()->PrintChatText( buffar );
}

//-----------------------------------------------------------------------------
// CVars and ConCommands
//-----------------------------------------------------------------------------

CON_COMMAND(sp_print_arg, "")
{
	if (args.ArgC() > 1)
	{
		SvenModAPI()->EngineFuncs()->Con_Printf("%s\n", args[1]);
	}
}

CON_COMMAND(sp_get_player_info, "")
{
	if (args.ArgC() > 1)
	{
		int playerindex = atoi(args[1]);

		if (playerindex > 0 && playerindex <= MAXCLIENTS)
		{
			Vector va;
			PlayerUtils()->GetViewAngles(playerindex, va);

			Msg("Health: %.1f\n", PlayerUtils()->GetHealth(playerindex));
			Msg("Armor: %.1f\n", PlayerUtils()->GetArmor(playerindex));
			Msg("Team Number: %d\n", PlayerUtils()->GetTeamNumber(playerindex));
			Msg("Steam64 ID: %llu\n", PlayerUtils()->GetSteamID(playerindex));
			Msg("View Angles: %.3f %.3f %.3f\n", VectorExpand(va));
		}
	}
}

//-----------------------------------------------------------------------------
// Implement plugin methods
//-----------------------------------------------------------------------------

api_version_t CSamplePlugin::GetAPIVersion()
{
	return SVENMOD_API_VER;
}

bool CSamplePlugin::Load(CreateInterfaceFn pfnSvenModFactory, ISvenModAPI *pSvenModAPI, IPluginHelpers *pPluginHelpers)
{
	BindApiToGlobals(pSvenModAPI);

	g_pHooks->RegisterClientHooks( &g_ClientHooks );
	g_pHooks->RegisterClientPostHooks( &g_ClientPostHooks );

	hNetMsgHook_SetAngle = g_pHooks->HookNetworkMessage( SVC_SETANGLE, NetMsgHook_SetAngle, &ORIG_NetMsgHook_SetAngle );
	hUserMsgHook_VoteMenu = g_pHooks->HookUserMessage( "VoteMenu", UserMsgHook_VoteMenu, &ORIG_UserMsgHook_VoteMenu );
	hversion_callback = g_pHooks->HookConsoleCommand( "version", version_callback, &ORIG_version_callback );

	ConVar_Register();

	g_pHooks->HookCvarChange( CVar()->FindCvar("fps_max"), CvarChangeHook_fps_max );

	Msg("CSamplePlugin::Load\n");
	return true;
}

void CSamplePlugin::PostLoad(bool bGlobalLoad)
{
	if ( bGlobalLoad )
	{
		Msg("CSamplePlugin::PostLoad (bGlobalLoad)\n");
	}
	else
	{
		Msg("CSamplePlugin::PostLoad\n");
	}
}

void CSamplePlugin::Unload(void)
{
	g_pHooks->UnhookCvarChange( CVar()->FindCvar("fps_max"), CvarChangeHook_fps_max );

	ConVar_Unregister();

	g_pHooks->UnhookNetworkMessage( hNetMsgHook_SetAngle );
	g_pHooks->UnhookUserMessage( hUserMsgHook_VoteMenu );
	g_pHooks->UnhookConsoleCommand( hversion_callback );

	g_pHooks->UnregisterClientPostHooks( &g_ClientPostHooks );
	g_pHooks->UnregisterClientHooks( &g_ClientHooks );

	Msg("CSamplePlugin::Unload\n");
}

bool CSamplePlugin::Pause(void)
{
	Msg("CSamplePlugin::Pause\n");
	return true;
}

void CSamplePlugin::Unpause(void)
{
	Msg("CSamplePlugin::Unpause\n");
}

void CSamplePlugin::OnFirstClientdataReceived(client_data_t *pcldata, float flTime)
{
	Msg("CSamplePlugin::OnFirstClientdataReceived\n");
}

void CSamplePlugin::OnBeginLoading(void)
{
	Msg("CSamplePlugin::OnBeginLoading\n");
}

void CSamplePlugin::OnEndLoading(void)
{
	Msg("CSamplePlugin::OnEndLoading\n");
}

void CSamplePlugin::OnDisconnect(void)
{
	Msg("CSamplePlugin::OnDisconnect\n");
}

void CSamplePlugin::GameFrame(client_state_t state, double frametime, bool bPostRunCmd)
{
	if (bPostRunCmd) // called from HUD_Frame
	{

	}
	else // called on start of function Host_Frame
	{

	}
}

void CSamplePlugin::Draw(void)
{
}

void CSamplePlugin::DrawHUD(float time, int intermission)
{
}

const char *CSamplePlugin::GetName(void)
{
	return "Sample Plugin";
}

const char *CSamplePlugin::GetAuthor(void)
{
	return "Sw1ft";
}

const char *CSamplePlugin::GetVersion(void)
{
	return "1.0.2";
}

const char *CSamplePlugin::GetDescription(void)
{
	return "A sample plugin for SvenMod";
}

const char *CSamplePlugin::GetURL(void)
{
	return "https://github.com/sw1ft747";
}

const char *CSamplePlugin::GetDate(void)
{
	return SVENMOD_BUILD_TIMESTAMP;
}

const char *CSamplePlugin::GetLogTag(void)
{
	return "SAMPLE_PLUGIN";
}