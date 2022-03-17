#include "sample_plugin.h"
#include "client_hooks.h"

#include <interface.h>
#include <dbg.h>
#include <convar.h>
#include <usermessages.h>

//-----------------------------------------------------------------------------
// Export the global interface
//-----------------------------------------------------------------------------

CSamplePlugin g_SamplePlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSamplePlugin, IClientPlugin, CLIENT_PLUGIN_INTERFACE_VERSION, g_SamplePlugin);

//-----------------------------------------------------------------------------
// Example hooks
//-----------------------------------------------------------------------------

UserMsgHookFn ORIG_UserMsgHook_VoteMenu = NULL;
DetourHandle_t hUserMsgHook_VoteMenu = 0;

CommandCallbackFn ORIG_version_callback = NULL;
DetourHandle_t hversion_callback = 0;

int UserMsgHook_VoteMenu(const char *pszName, int iSize, void *pBuffer)
{
	UserMessages::BeginRead(pBuffer, iSize);

	int type = UserMessages::ReadByte();

	Msg("Called vote of type: %d\n", type);

	return ORIG_UserMsgHook_VoteMenu(pszName, iSize, pBuffer);
}

void version_callback(void)
{
	Msg("Protocol version: %d\n", SvenModAPI()->GetProtocolVersion());
	Msg("Engine build: %d\n", SvenModAPI()->GetEngineBuild());
	Msg("Client version: %s\n", SvenModAPI()->GetClientVersion()->string);
}

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
			Msg("SteamID 64: %llu\n", PlayerUtils()->GetSteamID(playerindex));
			Msg("View Angles: %.3f %.3f %.3f\n", VectorExpand(va));
		}
	}
}

//-----------------------------------------------------------------------------
// Implement plugin methods
//-----------------------------------------------------------------------------

api_version_s CSamplePlugin::GetAPIVersion()
{
	return SVENMOD_API_VER;
}

bool CSamplePlugin::Load(CreateInterfaceFn pfnSvenModFactory, ISvenModAPI *pSvenModAPI, IPluginHelpers *pPluginHelpers)
{
	BindApiToGlobals(pSvenModAPI);

	g_pHooks->RegisterClientHooks( &g_ClientHooks );
	g_pHooks->RegisterClientPostHooks( &g_ClientPostHooks );

	hUserMsgHook_VoteMenu = g_pHooks->HookUserMessage( "VoteMenu", UserMsgHook_VoteMenu, &ORIG_UserMsgHook_VoteMenu );

	if ( !hUserMsgHook_VoteMenu )
	{
		Warning("Cannot hook user's message VoteMenu\n");
		return false;
	}

	hversion_callback = g_pHooks->HookConsoleCommand( "version", version_callback, &ORIG_version_callback );

	if ( !hversion_callback )
	{
		Warning("Cannot hook user's message VoteMenu\n");
		g_pHooks->UnhookUserMessage( hUserMsgHook_VoteMenu );

		return false;
	}

	ConVar_Register();

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
	ConVar_Unregister();

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

void CSamplePlugin::GameFrame(client_state_t state, double frametime, bool bPostRunCmd)
{
	if (bPostRunCmd) // called from HUD_Frame
	{

	}
	else // called on start of function Host_Frame
	{

	}
}

PLUGIN_RESULT CSamplePlugin::Draw(void)
{
	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT CSamplePlugin::DrawHUD(float time, int intermission)
{
	return PLUGIN_CONTINUE;
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
	return "1.0";
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