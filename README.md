# SvenMod
SvenMod is a C++ plugin environment for Sven Co-op that provides powerful Source-like API for creating and supporting client-side plugins.

Includes two main files: library `svenmod.dll` and launcher `svenmod_launcher.exe`. It is neccesary to launch the game using SvenMod's launcher.

# For developers
See the [wiki](https://github.com/sw1ft747/SvenMod/wiki "wiki") page

The project is written in Visual Studio 2022

# VAC
There's no VAC in the game but for safe you can use `-insecure` launch parameter, still you will be able to join any secure VAC server.

# Installation
1. Download the [release](https://github.com/sw1ft747/SvenMod/releases "release") version.

2. Place all files `(svenmod_launcher.exe, svenmod.dll, folder "svenmod")` from the downloaded archive in the root directory of Sven Co-op.

Also, you can rename the launcher file `svenmod_launcher.exe` to `svencoop.exe`.

3. Launch the file `svenmod_launcher.exe` to start the game with SvenMod.

# Adding plugins
1. Open the root directory of the game.

2. Open the following folder: `svenmod/plugins/`. Place the plugin you want to load in that folder. It will look like this: `Sven Co-op/svenmod/plugins/name_of_plugin_to_load.dll`.

3. Now, go back to folder `Sven Co-op/svenmod/` and open file `plugins.txt`.

4. You need to add your plugin in `"Plugins"` section between the brackets `{ }`.
The format is `"PLUGIN_NAME" "STATE"`, where `PLUGIN_NAME` - name of the plugin and `STATE` - 0 or 1 (to wit disabled/enabled)

For example, how it should look:
```
"Plugins" // Hey, I'm a comment!
{
	"name_of_the_plugin.dll"	"1" // the plugin is enabled
	"name_of_another_plugin"	"0" // the plugin is disabled
}
```

Note, everything that goes after these symbols `//` is a comment

# Console Commands
SvenMod provides several important console commands:
```
- sm <command> <arg1> <arg2>... - type "sm" in the console to get very detailed information
- help <cvarname> - will print a description about the given convar/concommand that was registered through SvenMod
- toggle <cvarname> <value #1> <value #2> <value #3>.. - Toggle between cvar values
- incrementvar <cvarname> <minvalue> <maxvalue> <delta> - Increment a cvar
- multvar <cvarname> <minvalue> <maxvalue> <factor> - Multiply a cvar
```

# Plugins
- [Improved Mute Manager](https://github.com/sw1ft747/ImprovedMuteManager "Improved Mute Manager")
