#include <IClientPlugin.h>
#include <ICvar.h>
#include <convar.h>
#include <svenmod_version.h>
#include <dbg.h>

#include "cvar.h"
#include "plugins_manager.h"

//-----------------------------------------------------------------------------
// sm options
//-----------------------------------------------------------------------------

FORCEINLINE void ConCommand_sm_plugins(const CCommand &args)
{
	const int argCount = args.ArgC();
	bool bPrintUsage = false;

	if (argCount > 2)
	{
		const char *pszCommand = args[2];

		if ( !stricmp(pszCommand, "list") )
		{
			g_PluginsManager.PrintPlugins();
		}
		else if ( !stricmp(pszCommand, "info") )
		{
			if (argCount > 3)
			{
				int nPluginIndex = atoi(args[3]);
				g_PluginsManager.PrintPluginInfo(nPluginIndex);
			}
			else
				ConMsg("Usage:  sm plugins info <index>\n");
		}
		else if ( !stricmp(pszCommand, "load") )
		{
			if (argCount > 3)
			{
				const char *pszFileName = args[3];
				g_PluginsManager.LoadPlugin(pszFileName, false);
			}
			else
				ConMsg("Usage:  sm plugins load <filename>\n");
		}
		else if ( !stricmp(pszCommand, "unload") )
		{
			if (argCount > 3)
			{
				int nPluginIndex = atoi(args[3]);
				g_PluginsManager.UnloadPlugin(nPluginIndex);
			}
			else
				ConMsg("Usage: sm plugins unload <index>\n");
		}
		else if ( !stricmp(pszCommand, "unload_all") )
		{
			g_PluginsManager.UnloadPlugins();
			ConMsg("[SvenMod] Unloaded all plugins\n");
		}
		else if ( !stricmp(pszCommand, "pause") )
		{
			if (argCount > 3)
			{
				int nPluginIndex = atoi(args[3]);
				g_PluginsManager.PausePlugin(nPluginIndex);
			}
			else
				ConMsg("Usage:  sm plugins pause <index>\n");
		}
		else if ( !stricmp(pszCommand, "unpause") )
		{
			if (argCount > 3)
			{
				int nPluginIndex = atoi(args[3]);
				g_PluginsManager.UnpausePlugin(nPluginIndex);
			}
			else
				ConMsg("Usage:  sm plugins unpause <index>\n");
		}
		else if ( !stricmp(pszCommand, "pause_all") )
		{
			if (g_PluginsManager.PausePlugins())
				ConMsg("[SvenMod] All plugins have been paused\n");
			else
				ConMsg("[SvenMod] None of plugins have been paused\n");
		}
		else if ( !stricmp(pszCommand, "unpause_all") )
		{
			if (g_PluginsManager.UnpausePlugins())
				ConMsg("[SvenMod] All plugins have been unpaused\n");
			else
				ConMsg("[SvenMod] None of plugins have been unpaused\n");
		}
		else if ( !stricmp(pszCommand, "refresh") )
		{
			g_PluginsManager.UnloadPlugins();
			g_PluginsManager.LoadPlugins();

			ConMsg("[SvenMod] Reloaded all plugins from file \"../svenmod/plugins.txt\"");
		}
		else
		{
			bPrintUsage = true;
		}
	}
	else
	{
		bPrintUsage = true;
	}

	if (bPrintUsage)
	{
		ConMsg("* sm plugins list        - Print list of indexed plugins\n");
		ConMsg("* sm plugins info        - Print detailed information about a plugin\n");
	#ifdef PLATFORM_WINDOWS
		ConMsg("* sm plugins load        - Load a plugin from directory \"../svenmod/plugins/*.dll\"\n");
	#else
		ConMsg("* sm plugins load        - Load a plugin from directory \"../svenmod/plugins/*.so\"\n");
	#endif
		ConMsg("* sm plugins unload      - Unload a loaded plugin\n");
		ConMsg("* sm plugins unload_all  - Unload all loaded plugins\n");
		ConMsg("* sm plugins pause       - Pause a loaded plugin\n");
		ConMsg("* sm plugins unpause     - Unpause a loaded plugin\n");
		ConMsg("* sm plugins pause_all   - Pause all running plugins\n");
		ConMsg("* sm plugins unpause_all - Unpause all paused plugins\n");
		ConMsg("* sm plugins refresh     - Reload all plugins from file \"../svenmod/plugins.txt\"\n");
	}
}

FORCEINLINE void ConCommand_sm_info(const CCommand &args)
{
	ConMsg("---------- SvenMod Info ----------\n");
	ConMsg("* Version:          %s\n", SVENMOD_VERSION_STRING);
	ConMsg("* API Version:      %s\n", SVENMOD_API_VERSION_STRING);
	ConMsg("* Plugin Interface: %s\n", CLIENT_PLUGIN_INTERFACE_VERSION);
	ConMsg("* URL:              https://github.com/sw1ft747/svenmod\n");
	ConMsg("* Compiled in:      %s\n", SVENMOD_BUILD_TIMESTAMP);
	ConMsg("----------------------------------\n");
}

FORCEINLINE void ConCommand_sm_printcvars(const CCommand &args)
{
	const int argCount = args.ArgC();
	bool bPrintUsage = false;

	if (argCount > 2)
	{
		const char *pszArgument = args[2];

		if ( !stricmp(pszArgument, "all") )
		{
			if (argCount > 3)
			{
				if ( !stricmp(args[3], "?") )
				{
					if (argCount > 4)
						g_CVar.PrintCvars(0, args[4]);
					else
						ConMsg("Usage:  sm printcvars all ? <prefix>\n");
				}
			}
			else
			{
				g_CVar.PrintCvars(0, NULL);
				ConMsg("For syntax:  sm printcvars all ? <prefix>\n");
			}
		}
		else if ( !stricmp(pszArgument, "cvar") )
		{
			if (argCount > 3)
			{
				if ( !stricmp(args[3], "?") )
				{
					if (argCount > 4)
						g_CVar.PrintCvars(1, args[4]);
					else
						ConMsg("Usage:  sm printcvars cvar ? <prefix>\n");
				}
			}
			else
			{
				g_CVar.PrintCvars(1, NULL);
				ConMsg("For syntax:  sm printcvars cvar ? <prefix>\n");
			}
		}
		else if ( !stricmp(pszArgument, "cmd") )
		{
			if (argCount > 3)
			{
				if ( !stricmp(args[3], "?") )
				{
					if (argCount > 4)
						g_CVar.PrintCvars(2, args[4]);
					else
						ConMsg("Usage:  sm printcvars cmd ? <prefix>\n");
				}
			}
			else
			{
				g_CVar.PrintCvars(2, NULL);
				ConMsg("For syntax:  sm printcvars cmd ? <prefix>\n");
			}
		}
		else
		{
			bPrintUsage = true;
		}
	}
	else
	{
		bPrintUsage = true;
	}

	if (bPrintUsage)
	{
		ConMsg("* sm printcvars all  - Print all registered convars/concommands\n");
		ConMsg("* sm printcvars cvar - Print only convars\n");
		ConMsg("* sm printcvars cmd  - Print only concommands\n");
	}
}

//-----------------------------------------------------------------------------
// Console commands
//-----------------------------------------------------------------------------

CON_COMMAND(sm, "Options for SvenMod")
{
	bool bPrintUsage = false;

	if (args.ArgC() > 1)
	{
		const char *pszOption = args[1];

		if ( !stricmp(pszOption, "plugins") )
			ConCommand_sm_plugins(args);
		else if ( !stricmp(pszOption, "info") )
			ConCommand_sm_info(args);
		else if ( !stricmp(pszOption, "printcvars") )
			ConCommand_sm_printcvars(args);
		else
			bPrintUsage = true;
	}
	else
	{
		bPrintUsage = true;
	}

	if (bPrintUsage)
	{
		ConMsg("Usage:  sm <command> [arg1] [arg2]..\n");
		ConMsg("* sm plugins     - Plugin commands\n");
		ConMsg("* sm info        - Query info\n");
		ConMsg("* sm printcvars  - Print registered convars/concommands\n");
	}
}

CON_COMMAND(help, "Find help about a convar/concommand registered through SvenMod.")
{
	if (args.ArgC() > 1)
	{
		const char *pszName = args[1];
		const ConCommandBase *var = CVar()->FindCommandBase( pszName );

		if (var)
			ConVar_PrintDescription(var);
		else
			ConMsg("help:  no cvar or command named \"%s\"\n", pszName);
	}
	else
	{
		ConMsg("Usage:  help <cvarname>\n");
	}
}

CON_COMMAND(toggle, "Toggle between values")
{
	const int argCount = args.ArgC();

	if (argCount > 1)
	{
		const char *pszCvar = args[1];
		cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszCvar);

		if (pCvar)
		{
			if (argCount == 2)
			{
				bool bValue = static_cast<bool>(pCvar->value);
				g_pEngineFuncs->Cvar_SetValue(pszCvar, float(!bValue));
			}
			else
			{
				int i;
				for (i = 2; i < argCount; i++)
				{
					if ( !strcmp(pCvar->string, args[i]) )
						break;
				}

				i++;

				if (i >= argCount)
					i = 2;

				g_pEngineFuncs->Cvar_Set(pszCvar, args[i]);
			}
		}
	}
	else
	{
		ConMsg("Usage:  toggle <cvarname> <value #1> <value #2> <value #3>..\n");
	}
}

CON_COMMAND(incrementvar, "Increment a cvar")
{
	if (args.ArgC() >= 5)
	{
		const char *pszCvar = args[1];
		cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszCvar);

		if (pCvar)
		{
			float currentValue = pCvar->value;
			float startValue = std::stof(args[2]);
			float endValue = std::stof(args[3]);
			float delta = std::stof(args[4]);
			float newValue = currentValue + delta;

			if (newValue > endValue)
				newValue = startValue;
			else if (newValue < startValue)
				newValue = endValue;

			g_pEngineFuncs->Cvar_SetValue(pszCvar, newValue);
		}
	}
	else
	{
		ConMsg("Usage:  incrementvar <cvarname> <minvalue> <maxvalue> <delta>\n");
	}
}

CON_COMMAND(multvar, "Multiply a cvar")
{
	if (args.ArgC() >= 5)
	{
		const char *pszCvar = args[1];
		cvar_t *pCvar = g_pEngineFuncs->GetCvarPointer(pszCvar);

		if (pCvar)
		{
			float currentValue = pCvar->value;
			float startValue = std::stof(args[2]);
			float endValue = std::stof(args[3]);
			float factor = std::stof(args[4]);
			float newValue = currentValue * factor;

			if (newValue > endValue)
				newValue = startValue;
			else if (newValue < startValue)
				newValue = endValue;

			g_pEngineFuncs->Cvar_SetValue(pszCvar, newValue);
		}
	}
	else
	{
		ConMsg("Usage:  multvar <cvarname> <minvalue> <maxvalue> <factor>\n");
	}
}
