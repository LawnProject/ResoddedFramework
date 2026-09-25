#include <clocale>

// SDL3's Android loader resolves the native entry point by looking up the
// "SDL_main" symbol. Including SDL_main.h redefines main() to SDL_main so
// the shared library actually exports the name SDL looks for.
#ifdef __ANDROID__
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_system.h>
#include <unistd.h>
#endif

#include "LawnApp.h"
#include "Sexy.TodLib/TodStringFile.h"

bool (*gAppCloseRequest)();
bool (*gAppHasUsedCheatKeys)();
SexyString (*gGetCurrentLevelName)();

int LawnMain()
{
#ifdef __ANDROID__
	// Android starts the process with the working directory at "/", which is
	// read-only. The game opens its pak, saves and logs through relative
	// paths, so point them at the app's external files dir instead, which is
	// also where PvZPortableActivity stages main.pak and properties/.
	{
		const char *aDataDir = SDL_GetAndroidExternalStoragePath();
		if (aDataDir)
		{
			if (chdir(aDataDir) != 0)
				SDL_Log("Could not chdir to '%s': %s", aDataDir, SDL_GetError());
			else
				SDL_Log("Working directory set to '%s'", aDataDir);
		}
		else
		{
			SDL_Log("Could not resolve the Android external storage path");
		}
	}
#endif

	// make locale UTF-8 so console output displays correctly
	std::setlocale(LC_ALL, "en_us.UTF-8");

	TodStringListSetColors(gLawnStringFormats, gLawnStringFormatCount);
	gGetCurrentLevelName = LawnGetCurrentLevelName;
	gAppCloseRequest = LawnGetCloseRequest;
	gAppHasUsedCheatKeys = LawnHasUsedCheatKeys;

	gLawnApp = new LawnApp();
	gLawnApp->mChangeDirTo =
	    (!Sexy::FileExists("properties/resources.xml") && Sexy::FileExists("../properties/resources.xml")) ? ".." : ".";

	gLawnApp->Init();
	if (gLawnApp->mRunInCompileMode)
	{
		gLawnApp->Shutdown();
		delete gLawnApp;
		return 0;
	}
	gLawnApp->Start();

	gLawnApp->Shutdown();
	delete gLawnApp;

	return 0;
}

// Windows subsystem entrypoint (doesn't show console when double-clicked)
#ifdef WIN32
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Attach to parent console when launched from a terminal
	if (AttachConsole(ATTACH_PARENT_PROCESS))
	{
		FILE *dummy;
		freopen_s(&dummy, "CONOUT$", "w", stdout);
		freopen_s(&dummy, "CONOUT$", "w", stderr);
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
	}
	return LawnMain();
}
#endif

// Console/GNU/UNIX entrypoint
int main(int argc, char **argv)
{
	return LawnMain();
};
