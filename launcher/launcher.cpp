// Sven Mod Launcher

#ifdef _WIN32
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <hl_sdk/common/Platform.h>

#include <platform.h>
#include <interface.h>
#include <sys.h>

#include <ISvenMod.h>
#include <IEngineAPI.h>
#include <IFileSystem.h>
#include <ICommandLine.h>
#include <IRegistry.h>

#ifdef PLATFORM_LINUX
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <locale.h>
#endif

ISvenMod *g_pSvenMod = NULL;
IEngineAPI *g_pEngineAPI = NULL;
IFileSystem *g_pFileSystem = NULL;

#ifdef _WIN32
typedef BOOL (__cdecl *IsDebuggerPresentFn)(void);
#endif

#ifdef PLATFORM_WINDOWS
static void StartupWinSock()
{
	WSAData WSAData;
	WSAStartup(0x202, &WSAData);
}

static void CleanupWinSock()
{
	WSACleanup();
}

static bool GrabSvenCoopMutex(HANDLE *hMutex)
{
#pragma warning(push)
#pragma warning(disable : 6387)

	IsDebuggerPresentFn IsDebuggerPresent = (IsDebuggerPresentFn)GetProcAddress(GetModuleHandle("kernel32.dll"), "IsDebuggerPresent");

	if ( !IsDebuggerPresent() )
	{
		*hMutex = CreateMutex(NULL, FALSE, TEXT("svencoop_singleton_mutex"));

		DWORD dwWaitResult = WaitForSingleObject(*hMutex, 0);

		if (dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_ABANDONED)
			return true;

		return false;
	}

#pragma warning(pop)

	return true;
}
#endif

static void V_CreateCmdLine(const char *pszCmdLine)
{
	CommandLine()->CreateCmdLine( pszCmdLine );
}

static void V_CreateCmdLine(int argc, char *argv[])
{
	CommandLine()->CreateCmdLine( argc, argv );
}

static void RemoveCommandParams(const char *pszNewCommandParams)
{
	CommandLine()->RemoveParm("-sw");
	CommandLine()->RemoveParm("-startwindowed");
	CommandLine()->RemoveParm("-windowed");
	CommandLine()->RemoveParm("-window");
	CommandLine()->RemoveParm("-full");
	CommandLine()->RemoveParm("-fullscreen");
	CommandLine()->RemoveParm("-soft");
	CommandLine()->RemoveParm("-software");
	CommandLine()->RemoveParm("-gl");
	CommandLine()->RemoveParm("-d3d");
	CommandLine()->RemoveParm("-w");
	CommandLine()->RemoveParm("-width");
	CommandLine()->RemoveParm("-h");
	CommandLine()->RemoveParm("-height");
	CommandLine()->RemoveParm("-novid");

	if (strstr(pszNewCommandParams, "-game"))
		CommandLine()->RemoveParm("-game");

	if (strstr(pszNewCommandParams, "+load"))
		CommandLine()->RemoveParm("+load");
}

#ifdef PLATFORM_WINDOWS
static bool CheckVideoModeInit()
{
	if ( Registry()->ReadInt("CrashInitializingVideoMode", 0) )
	{
		Registry()->WriteInt("CrashInitializingVideoMode", 0);

		const char *pszHardware = Registry()->ReadString("EngineDLL", "hw.dll");

		if ( !stricmp(pszHardware, "hw.dll") )
		{
			Registry()->WriteInt("ScreenWidth", 640);
			Registry()->WriteInt("ScreenHeight", 480);
			Registry()->WriteInt("ScreenBPP", 32);

			if ( Registry()->ReadInt("EngineD3D", 0) )
			{
				Registry()->WriteInt("EngineD3D", 0);
				Sys_ErrorMessage("The game has detected that the previous attempt to start in D3D video mode failed.\nThe game will now run attempt to run in openGL mode.");
			}
			else
			{
				Registry()->WriteString("EngineDLL", "hw.dll");
				Sys_ErrorMessage("The game has detected that the previous attempt to start in openGL video mode failed.\nThe game will now run in software mode.");
			}
		}

		return false;
	}

	return true;
}
#endif

static bool LoadFileSystem(HMODULE *pModule, CreateInterfaceFn *pfnCreateInterface)
{
	HMODULE hModule = NULL;

#ifdef PLATFORM_WINDOWS
	hModule = Sys_LoadModule("filesystem_stdio.dll");
#elif defined(PLATFORM_LINUX)
	hModule = Sys_LoadModule("filesystem_stdio.so");
#endif

	*pModule = hModule;

	if (!hModule)
	{
	#ifdef PLATFORM_WINDOWS
		Sys_ErrorMessage("Failed to load library \"filesystem_stdio.dll\"");
	#elif defined(PLATFORM_LINUX)
		Sys_ErrorMessage("Failed to load library \"filesystem_stdio.so\"");
	#endif

		return false;
	}
	else
	{
		*pfnCreateInterface = Sys_GetFactory(hModule);
	}

	if (!*pfnCreateInterface)
	{
		Sys_ErrorMessage("Cannot find FileSystem's interface factory.");
		Sys_UnloadModule(hModule);

		return false;
	}

	return true;
}

static bool LoadEngine(HMODULE *pModule, CreateInterfaceFn *pfnCreateInterface)
{
	HMODULE hModule = NULL;

#ifdef PLATFORM_WINDOWS
	hModule = Sys_LoadModule("hw.dll");
#elif defined(PLATFORM_LINUX)
	hModule = Sys_LoadModule("hw.so");
#endif

	*pModule = hModule;

	if (!hModule)
	{
	#ifdef PLATFORM_WINDOWS
		Sys_ErrorMessage("Failed to load engine binary \"hw.dll\"");
	#elif defined(PLATFORM_LINUX)
		Sys_ErrorMessage("Failed to load engine binary \"hw.so\"");
	#endif

		return false;
	}
	else
	{
		*pfnCreateInterface = Sys_GetFactory(hModule);
	}

	if (!*pfnCreateInterface)
	{
		Sys_ErrorMessage("Cannot find Engine's interface factory.");
		Sys_UnloadModule(hModule);

		return false;
	}

	return true;
}

static bool LoadSvenMod(HMODULE *pModule, CreateInterfaceFn *pfnCreateInterface)
{
	HMODULE hModule = NULL;

#ifdef PLATFORM_WINDOWS
	hModule = Sys_LoadModule("svenmod.dll");
#elif defined(PLATFORM_LINUX)
	hModule = Sys_LoadModule("svenmod.so");
#endif

	*pModule = hModule;

	if (!hModule)
	{
	#ifdef PLATFORM_WINDOWS
		Sys_ErrorMessage("Failed to load SvenMod's binary \"svenmod.dll\"");
	#elif defined(PLATFORM_LINUX)
		Sys_ErrorMessage("Failed to load SvenMod's binary \"svenmod.so\"");
	#endif

		return false;
	}
	else
	{
		*pfnCreateInterface = Sys_GetFactory(hModule);
	}

	if (!*pfnCreateInterface)
	{
		Sys_ErrorMessage("Cannot find SvenMod's interface factory.");
		Sys_UnloadModule(hModule);

		return false;
	}

	return true;
}

static int LauncherMain(HINSTANCE hInstance, int argc, char *argv[])
{
	static char szNewCommandParams[2048];
	
#ifdef PLATFORM_LINUX
	static char szFileName[PATH_MAX];
#else
	static char szFileName[MAX_PATH];
	
	HANDLE hMutex = NULL;
#endif

	HMODULE hFileSystem = NULL;
	HMODULE hEngine = NULL;
	HMODULE hSvenMod = NULL;

	CreateInterfaceFn pfnFileSystemFactory = NULL;
	CreateInterfaceFn pfnEngineFactory = NULL;
	CreateInterfaceFn pfnSvenModFactory = NULL;

	int iEngineRunResult = RUN_FAILED;

#ifdef PLATFORM_LINUX
	const char pszLocale[] = "en_US.UTF-8";
	
	setenv("LC_ALL", pszLocale, 1);
	setlocale(6, pszLocale); // todo: what constant is 6?
	
	char *pszCurrentLocale = setlocale(6, NULL);
	
	if ( strcasecmp(pszCurrentLocale, pszLocale) != 0 )
	{
		Sys_ErrorMessage("WARNING: setlocale(\"%s\") failed, using locale: \"%s\". International characters may not work.", pszLocale, pszCurrentLocale);
	}
#endif

	// Init command line
#ifdef PLATFORM_WINDOWS
	V_CreateCmdLine( GetCommandLine() );
#else
	V_CreateCmdLine( argc, argv );
#endif

#ifdef PLATFORM_WINDOWS
	if ( !GrabSvenCoopMutex(&hMutex) && CommandLine()->CheckParm("-allowmultiple") == NULL )
	{
		Sys_ErrorMessage("Could not launch game.\nOnly one instance of this game can be run at a time.");
		return 0;
	}
#endif

	// Get executable name
	if ( !Sys_GetExecutableName(szFileName, M_ARRAYSIZE(szFileName)) )
	{
		Sys_ErrorMessage("Failed to get executable name.");
		return 0;
	}

	const char *pszExeName = strrchr(szFileName, '\\') + 1;

	if ( CommandLine()->CheckParm("-game") != NULL )
	{
		if ( strcmp(CommandLine()->CheckParm("-game"), "svencoop") )
		{
			Sys_ErrorMessage("Supported value for param -game is 'svencoop'.");
			return 0;
		}
	}
	else
	{
		// It's definitely sven cope!
		CommandLine()->AppendParm("-game", "svencoop");
	}

#ifdef PLATFORM_WINDOWS
	if ( CommandLine()->CheckParm("-low") != NULL )
	{
		SetPriorityClass( GetCurrentProcess(), IDLE_PRIORITY_CLASS );
	}
	else if ( CommandLine()->CheckParm("-high") != NULL )
	{
		SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
	}

	StartupWinSock();

	Registry()->Init();

	if ( !CheckVideoModeInit() )
		return 0;

	Registry()->WriteString("EngineDLL", "hw.dll");
#endif

	// Start main routine
	do
	{
		// Load file system and engine
		if ( !LoadFileSystem(&hFileSystem, &pfnFileSystemFactory) )
			return 0;

		if ( !LoadEngine(&hEngine, &pfnEngineFactory) )
			return 0;
		
		if ( !LoadSvenMod(&hSvenMod, &pfnSvenModFactory) )
			return 0;

		// Find interfaces
		// This shit can't find any interface on Linux, what the actual fuck
		g_pFileSystem = reinterpret_cast<IFileSystem *>(pfnFileSystemFactory(FILESYSTEM_INTERFACE_VERSION, NULL));
		g_pEngineAPI = reinterpret_cast<IEngineAPI *>(pfnEngineFactory(VENGINE_LAUNCHER_API_VERSION, NULL));
		g_pSvenMod = reinterpret_cast<ISvenMod *>(pfnSvenModFactory(SVENMOD_INTERFACE_VERSION, NULL));

		if ( !g_pFileSystem )
		{
			Sys_ErrorMessage("Cannot find IFileSystem interface.");
			goto L_EXIT;
		}

		if ( !g_pEngineAPI )
		{
			Sys_ErrorMessage("Cannot find IEngineAPI interface.");
			goto L_EXIT;
		}
		
		if ( !g_pSvenMod )
		{
			Sys_ErrorMessage("Cannot find ISvenMod interface.");
			goto L_EXIT;
		}

		*szNewCommandParams = 0;

		g_pFileSystem->Mount();
		g_pFileSystem->AddSearchPath(Sys_GetLongPathName(), "ROOT");

	#ifdef PLATFORM_WINDOWS
		if ( !g_pSvenMod->Init( CommandLine(), g_pFileSystem, Registry() ) )
	#else
		if ( !g_pSvenMod->Init( CommandLine(), g_pFileSystem, NULL ) )
	#endif
		{
			Sys_ErrorMessage("SvenMod failed initialization");
			goto L_EXIT;
		}

		iEngineRunResult = g_pEngineAPI->Run(hInstance, Sys_GetLongPathName(), CommandLine()->GetCmdLine(), szNewCommandParams, Sys_GetFactoryThis(), pfnFileSystemFactory);

		g_pSvenMod->Shutdown();

		if ( iEngineRunResult == RUN_UNSUPPORTEDVIDEO )
		{
		#ifdef PLATFORM_WINDOWS
			Registry()->WriteInt("ScreenWidth", 640);
			Registry()->WriteInt("ScreenHeight", 480);
			Registry()->WriteInt("ScreenBPP", 32);

			Registry()->WriteString("EngineDLL", "hw.dll");
		#endif

			Sys_ErrorMessage("The specified video mode is not supported.");

		L_EXIT:
			Sys_UnloadModule(hSvenMod);
			Sys_UnloadModule(hEngine);
			Sys_UnloadModule(hFileSystem);

		#ifdef PLATFORM_WINDOWS
			Registry()->Shutdown();

			CleanupWinSock();

			if (hMutex)
				CloseHandle(hMutex);
		#endif

			return 0;
		}

		RemoveCommandParams(szNewCommandParams);
		CommandLine()->AppendParm(szNewCommandParams, NULL);

		g_pFileSystem->Unmount();

		Sys_UnloadModule(hSvenMod);
		Sys_UnloadModule(hEngine);
		Sys_UnloadModule(hFileSystem);

	} while ( iEngineRunResult == RUN_RESTART );

#ifdef PLATFORM_WINDOWS
	Registry()->Shutdown();

	CleanupWinSock();

	if (hMutex)
		CloseHandle(hMutex);
#endif
	
	return 1;
}

#ifdef PLATFORM_WINDOWS

int APIENTRY WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	return LauncherMain(hInstance, 0, NULL);
}

#else

int main( int argc, char *argv[] )
{
	return LauncherMain(NULL, argc, argv);
}

#endif
