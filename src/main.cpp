#include "LawnApp.h"
#include "Resources.h"
#include "Sexy.TodLib/TodStringFile.h"
#ifdef _DEBUG
#include <zlib.h>
#endif

using namespace Sexy;

bool (*gAppCloseRequest)();		//[0x69E6A0]
bool (*gAppHasUsedCheatKeys)(); //[0x69E6A4]
SexyString (*gGetCurrentLevelName)();

//0x44E8F0
int main()
{
	//gHInstance = hInstance;
#ifdef _DEBUG
	printf("Using zlib version: %s\n", zlibVersion());
#endif

	TodStringListSetColors(gLawnStringFormats, gLawnStringFormatCount);
	gGetCurrentLevelName = LawnGetCurrentLevelName;
	gAppCloseRequest = LawnGetCloseRequest;
	gAppHasUsedCheatKeys = LawnHasUsedCheatKeys;

	gLawnApp = new LawnApp();
	gLawnApp->mChangeDirTo =
		(!Sexy::FileExists("properties/resources.xml") && Sexy::FileExists("../properties/resources.xml")) ? ".." : ".";

	gLawnApp->Init();
	gLawnApp->Start();

	gLawnApp->Shutdown();
	if (gLawnApp)
		delete gLawnApp;

	return 0;
};