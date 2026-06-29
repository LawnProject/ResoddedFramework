#include <clocale>

#include "LawnApp.h"
#include "Sexy.TodLib/TodStringFile.h"

bool (*gAppCloseRequest)();
bool (*gAppHasUsedCheatKeys)();
SexyString (*gGetCurrentLevelName)();

int LawnMain()
{
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
