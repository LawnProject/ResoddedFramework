#include "LawnApp.h"
#include "Resources.h"
#include "Sexy.TodLib/TodStringFile.h"
#include <cstdlib>
#include <filesystem>
#include <vector>

using namespace Sexy;

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#if defined(__ANDROID__) || defined(__IPHONEOS__)
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#endif

#ifdef __IPHONEOS__
#include <fstream>
#endif

bool (*gAppCloseRequest)();
bool (*gAppHasUsedCheatKeys)();
SexyString (*gGetCurrentLevelName)();

#ifdef _WIN32
static std::vector<std::string> gUtf8ArgsStorage;
static std::vector<char*> gUtf8Argv;

static void BuildUtf8ArgsFromWin32(int& argc, char**& argv)
{
	int aWideArgc = 0;
	LPWSTR* aWideArgv = CommandLineToArgvW(GetCommandLineW(), &aWideArgc);
	if (aWideArgv == nullptr || aWideArgc <= 0)
		return;

	gUtf8ArgsStorage.clear();
	gUtf8Argv.clear();
	gUtf8ArgsStorage.reserve(static_cast<size_t>(aWideArgc));
	gUtf8Argv.reserve(static_cast<size_t>(aWideArgc));

	for (int i = 0; i < aWideArgc; ++i)
	{
		const wchar_t* aWide = aWideArgv[i];
		int aLen = WideCharToMultiByte(CP_UTF8, 0, aWide, -1, nullptr, 0, nullptr, nullptr);
		if (aLen <= 0)
		{
			gUtf8ArgsStorage.emplace_back();
		}
		else
		{
			std::string aUtf8;
			aUtf8.resize(static_cast<size_t>(aLen - 1));
			WideCharToMultiByte(CP_UTF8, 0, aWide, -1, aUtf8.data(), aLen, nullptr, nullptr);
			gUtf8ArgsStorage.emplace_back(std::move(aUtf8));
		}
	}

	for (auto& aStr : gUtf8ArgsStorage)
		gUtf8Argv.push_back(const_cast<char*>(aStr.c_str()));

	argc = static_cast<int>(gUtf8Argv.size());
	argv = gUtf8Argv.data();

	LocalFree(aWideArgv);
}
#endif

//0x44E8F0
int main(int argc, char** argv)
{
#ifdef _WIN32
	BuildUtf8ArgsFromWin32(argc, argv);
#endif

#ifdef __IPHONEOS__
	// iOS: verify that game resources exist in the Documents folder before launching
	bool aHasGameResources = false;
	std::filesystem::path aDocsPath;
	const char* aHome = std::getenv("HOME");
	if (aHome != nullptr && aHome[0] != '\0')
	{
		aDocsPath = std::filesystem::path(aHome) / "Documents";
		aHasGameResources = std::filesystem::is_regular_file(aDocsPath / "main.pak") &&
			std::filesystem::is_directory(aDocsPath / "properties");
	}

	if (!aHasGameResources)
	{
		// Write a README to help the user know what to do
		const std::filesystem::path aReadmePath = aDocsPath / "README.txt";
		if (!aDocsPath.empty() && !std::filesystem::exists(aReadmePath))
		{
			std::ofstream(aReadmePath, std::ios::out | std::ios::trunc)
				<< "Place your `main.pak` and `properties/` folder here to play the game.\n";
		}

		SDL_Init(SDL_INIT_VIDEO);
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Resources Not Found",
			"Please place main.pak and the properties/ folder into the "
			"LawnProject folder using the Files app or Finder/iTunes file sharing.\n\n"
			"The app will now exit.",
			NULL
		);
		SDL_Quit();
		return 1;
	}
#endif

	TodStringListSetColors(gLawnStringFormats, gLawnStringFormatCount);
	gGetCurrentLevelName = LawnGetCurrentLevelName;
	gAppCloseRequest = LawnGetCloseRequest;
	gAppHasUsedCheatKeys = LawnHasUsedCheatKeys;

	gLawnApp = new LawnApp();

#ifdef _WIN32
	// On Windows, detect if resources are one level up (running from build dir)
	gLawnApp->mChangeDirTo =
		(!Sexy::FileExists("properties/resources.xml") && Sexy::FileExists("../properties/resources.xml")) ? ".." : ".";
#endif

	gLawnApp->Init();
	gLawnApp->Start();

	gLawnApp->Shutdown();
	if (gLawnApp)
		delete gLawnApp;

	return 0;
};