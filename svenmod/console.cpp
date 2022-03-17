#include <IClientPlugin.h>
#include <ICvar.h>
#include <convar.h>
#include <svenmod_version.h>
#include <dbg.h>

#include "cvar_sm.h"
#include "plugins_manager.h"

CON_COMMAND(sm, "Options for SvenMod")
{
	bool bPrintUsage = false;

	if (args.ArgC() > 1)
	{
		const char *pszOption = args[1];

		if ( !stricmp(pszOption, "plugins") )
		{
			if (args.ArgC() > 2)
			{
				const char *pszCommand = args[2];

				if ( !stricmp(pszCommand, "list") )
				{
					g_PluginsManager.PrintPlugins();
				}
				else if ( !stricmp(pszCommand, "info") )
				{
					if (args.ArgC() > 3)
					{
						int nPluginIndex = atoi(args[3]);
						g_PluginsManager.PrintPluginInfo( nPluginIndex );
					}
					else
					{
						ConMsg("Usage:  sm plugins info <index>\n");
					}
				}
				else if ( !stricmp(pszCommand, "load") )
				{
					if (args.ArgC() > 3)
					{
						const char *pszFileName = args[3];

						g_PluginsManager.LoadPlugin( pszFileName, false );
					}
					else
					{
						ConMsg("Usage:  sm plugins load <filename>\n");
					}
				}
				else if ( !stricmp(pszCommand, "unload") )
				{
					if (args.ArgC() > 3)
					{
						int nPluginIndex = atoi(args[3]);

						g_PluginsManager.UnloadPlugin( nPluginIndex );
					}
					else
					{
						ConMsg("Usage: sm plugins unload <index>\n");
					}
				}
				else if ( !stricmp(pszCommand, "unload_all") )
				{
					g_PluginsManager.UnloadPlugins();
					ConMsg("[SvenMod] Unloaded all plugins\n");
				}
				else if ( !stricmp(pszCommand, "pause") )
				{
					if (args.ArgC() > 3)
					{
						int nPluginIndex = atoi(args[3]);

						g_PluginsManager.PausePlugin( nPluginIndex );
					}
					else
					{
						ConMsg("Usage:  sm plugins pause <index>\n");
					}
				}
				else if ( !stricmp(pszCommand, "unpause") )
				{
					if (args.ArgC() > 3)
					{
						int nPluginIndex = atoi(args[3]);

						g_PluginsManager.UnpausePlugin( nPluginIndex );
					}
					else
					{
						ConMsg("Usage:  sm plugins unpause <index>\n");
					}
				}
				else if ( !stricmp(pszCommand, "pause_all") )
				{
					if ( g_PluginsManager.PausePlugins() )
						ConMsg("[SvenMod] All plugins have been paused\n");
					else
						ConMsg("[SvenMod] None of plugins have been paused\n");
				}
				else if ( !stricmp(pszCommand, "unpause_all") )
				{
					if ( g_PluginsManager.UnpausePlugins() )
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
					goto L_SHOW_PL_HELP;
				}
			}
			else
			{
			L_SHOW_PL_HELP:
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
		else if ( !stricmp(pszOption, "info") )
		{
			ConMsg("[SvenMod] Information:\n");
			ConMsg("----------------------------------\n");
			ConMsg("* Version:          %s\n", SVENMOD_VERSION_STRING);
			ConMsg("* API Version:      %s\n", SVENMOD_API_VERSION_STRING);
			ConMsg("* Plugin Interface: %s\n", CLIENT_PLUGIN_INTERFACE_VERSION);
			ConMsg("* URL:              https://github.com/sw1ft747/SvenMod\n");
			ConMsg("* Compiled in:      %s\n", SVENMOD_BUILD_TIMESTAMP);
			ConMsg("----------------------------------\n");
		}
		else if ( !stricmp(pszOption, "printcvars") )
		{
			if (args.ArgC() > 2)
			{
				const char *pszArgument = args[2];

				if ( !stricmp(pszArgument, "all") )
				{
					PrintAllCvars(0);
				}
				else if ( !stricmp(pszArgument, "cvar") )
				{
					PrintAllCvars(1);
				}
				else if ( !stricmp(pszArgument, "cmd") )
				{
					PrintAllCvars(2);
				}
				else
				{
					goto L_SHOW_PRINTCVARS_HELP;
				}
			}
			else
			{
			L_SHOW_PRINTCVARS_HELP:
				ConMsg("* sm printcvars all  - Print all registered convars/concommands\n");
				ConMsg("* sm printcvars cvar - Print only convars\n");
				ConMsg("* sm printcvars cmd  - Print only concommands\n");
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
		{
			ConVar_PrintDescription(var);
		}
		else
		{
			ConMsg("help:  no cvar or command named \"%s\"\n", pszName);
		}
	}
	else
	{
		ConMsg("Usage:  help <cvarname>\n");
	}
}