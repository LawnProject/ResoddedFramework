//#define SEXY_TRACING_ENABLED
//#define SEXY_PERF_ENABLED
//#define SEXY_MEMTRACE

#include "SexyAppBase.h"
#include "resource.h"
#include "SEHCatcher.h"
#include "WidgetManager.h"
#include "Widget.h"
#include "Debug.h"
#include "KeyCodes.h"
#include "Renderer.h"
#if SEXY_USE_OPENGL
#include "OpenGL/OpenGLRenderer.h"
#endif
#if SEXY_USE_SDL3_RENDERER
#include "SDL3Renderer/SDL3Renderer.h"
#endif
#if SEXY_USE_IMGUI
#include "ImGui/ImGuiManager.h"
#endif
#include "GPUImage.h"
#include "Window.h"
#include "MemoryImage.h"
#include "HTTPTransfer.h"
#include "Dialog.h"
#include "..\ImageLib\ImageLib.h"
#include "OpenALSoundManager.h"
#include "OpenALSoundInstance.h"
#include "Rect.h"
#include "PropertiesParser.h"
#include "PerfTimer.h"
#include "MTRand.h"
#include "BuildInfo.h"

#include <fstream>

#include "SysFont.h"
#include "ResourceManager.h"
#include "BassMusicInterface.h"
#include "AutoCrit.h"
#include "../PakLib/PakInterface.h"
#include <string>

#if WIN32

#include <math.h>
#include <regstr.h>
#include <shlobj.h>
#include <direct.h>
#include <process.h>

#endif
#include "memmgr.h"

#ifdef _DEBUG
#include <zlib.h>
#include <bass.h>
#include <SDL3/SDL.h>
#endif

#include <json.hpp>

using namespace Sexy;


const int DEMO_FILE_ID = 0x42BEEF78;
const int DEMO_VERSION = 2;

SexyAppBase *Sexy::gSexyAppBase = NULL;


#if SEXY_CRASH_HANDLER

SEHCatcher Sexy::gSEHCatcher;

#endif

#if WIN32

HMODULE gVersionDLL = NULL;

#endif

static bool gScreenSaverActive = false;

#ifndef SPI_GETSCREENSAVERRUNNING
#define SPI_GETSCREENSAVERRUNNING 114
#endif

#if _WIN64
#define GWL_USERDATA GWLP_USERDATA
#endif

//HotSpot: 11 4
//Size: 32 32
unsigned char gFingerCursorData[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff,
	0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc0,
	0xff, 0xff, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xfc, 0x40, 0x01, 0xff, 0xfc,
	0x00, 0x01, 0xff, 0xfc, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01, 0xff,
	0xff, 0x80, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xe0, 0x07,
	0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00,
	0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x1b, 0x00,
	0x00, 0x00, 0x1b, 0x60, 0x00, 0x00, 0x1b, 0x68, 0x00, 0x00, 0x1b, 0x6c, 0x00, 0x01, 0x9f, 0xec, 0x00, 0x01, 0xdf,
	0xfc, 0x00, 0x00, 0xdf, 0xfc, 0x00, 0x00, 0x5f, 0xfc, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
	0x3f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//HotSpot: 15 10
//Size: 32 32
unsigned char gDraggingCursorData[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f,
	0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xf0, 0x07, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xe0,
	0x00, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xfe, 0x60, 0x00, 0xff, 0xfc, 0x20, 0x00, 0xff, 0xfc,
	0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0xff,
	0xff, 0x80, 0x01, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xf8, 0x03,
	0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
	0x00, 0x01, 0xb0, 0x00, 0x00, 0x0d, 0xb0, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6,
	0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x01, 0x8d, 0xb6, 0x00, 0x01, 0xcf, 0xfe, 0x00, 0x00, 0xef,
	0xfe, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
	0x1f, 0xfc, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x03, 0xf8, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static GPUImage *gFPSImage = NULL;

static SysFont* gDebugFont = nullptr;

Version SexyAppBase::gVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, BUILD_NUMBER);

//////////////////////////////////////////////////////////////////////////

SexyAppBase::SexyAppBase()
{
	gSexyAppBase = this;

#if WIN32
	gVersionDLL = LoadLibraryA("version.dll");
#endif

#ifdef _DEBUG
	mOnlyAllowOneCopyToRun = false;
#else
	mOnlyAllowOneCopyToRun = true;
#endif

	// Extract product version
	std::string aPath = std::filesystem::current_path().string();
	mProductVersion = GetProductVersionDLL(aPath);
	mChangeDirTo = GetFileDir(aPath);

	mNoDefer = false;
	mFullScreenPageFlip = true; // should we page flip in fullscreen?
	mTimeLoaded = SDL_GetTicks();
	mSEHOccured = false;
	mProdName = "Product";
	mTitle = "SexyApp";
	mShutdown = false;
	mExitToTop = false;
	mWidth = 640;
	mHeight = 480;
	mFullscreenBits = 16;
	mIsWindowed = true;
	mIsPhysWindowed = true;
	mFullScreenWindow = false;
	mPreferredSize = Rect(-1, -1, -1, -1);
	mSavedWindowedSize = Rect(-1, -1, -1, -1);
	mIsScreenSaver = false;
	mAllowMonitorPowersave = true;
	mWindow = nullptr;
	mMusicInterface = NULL;
#if SEXY_USE_IMGUI
	mImGuiManager = nullptr;
#endif
	mRenderer = nullptr;
	mFrameTime = 10;
	mNonDrawCount = 0;
	mDrawCount = 0;
	mSleepCount = 0;
	mUpdateCount = 0;
	mUpdateAppState = 0;
	mUpdateAppDepth = 0;
	mPendingUpdatesAcc = 0.0;
	mUpdateFTimeAcc = 0.0;
	mHasPendingDraw = true;
	mIsDrawing = false;
	mLastDrawWasEmpty = false;
	mLastTimeCheck = 0;
	mUpdateMultiplier = 1;
	mPaused = false;
	mFastForwardToUpdateNum = 0;
	mFastForwardToMarker = false;
	mFastForwardStep = false;
	mSoundManager = NULL;
	mCursorNum = CURSOR_POINTER;
	mMouseIn = false;
	mRunning = false;
	mActive = true;
	mProcessInTimer = false;
	mMinimized = false;
	mPhysMinimized = false;
	mIsDisabled = false;
	mLoaded = false;
	mYieldMainThread = false;
	mLoadingFailed = false;
	mLoadingThreadStarted = false;
#if SEXY_USE_CONTROLLER
	mUsingGamepad = false;
#endif
	mAutoStartLoadingThread = true;
	mLoadingThreadCompleted = false;
	mCursorThreadRunning = false;
	mNumLoadingThreadTasks = 0;
	mCompletedLoadingThreadTasks = 0;
	mLastDrawTick = timeGetTime();
	mNextDrawTick = timeGetTime();
	mSysCursor = true;
	mForceFullscreen = false;
	mForceWindowed = false;
	mHasFocus = true;
	mCustomCursorsEnabled = false;
	for (int i = 0; i < NUM_CURSORS; i++)
		mCachedCursors[i] = nullptr;
	mIsOpeningURL = false;
	mInitialized = false;
	mLastShutdownWasGraceful = true;
	mReadFromRegistry = false;
	mCmdLineParsed = false;
	mSkipSignatureChecks = false;
	mCtrlDown = false;
	mAltDown = false;
	mStepMode = 0;
	mCleanupSharedImages = false;
	mEnableMaximizeButton = false;

	mMusicVolume = 0.85;
	mSfxVolume = 0.85;
	mDemoMusicVolume = mDemoSfxVolume = 0.0;
	mMuteCount = 0;
	mAutoMuteCount = 0;
	mDemoMute = false;
	mMuteOnLostFocus = true;
	mCurHandleNum = 0;
	mFPSTime = 0;
	mFPSStartTick = SDL_GetTicks();
	mFPSFlipCount = 0;
	mFPSCount = 0;
	mFPSDirtyCount = 0;
	mShowFPS = false;
	mShowFPSMode = FPS_ShowFPS;
	mDrawTime = 0;
	mScreenBltTime = 0;
	mAlphaDisabled = false;
	mDebugKeysEnabled = false;
	mNoSoundNeeded = false;
	mWantFMod = false;

	mDesiredBackend = RenderingBackend::BACKEND_NONE;
	mSyncRefreshRate = 100;
	mVSyncUpdates = false;
	mVSyncBroken = false;
	mVSyncBrokenCount = 0;
	mVSyncBrokenTestStartTick = 0;
	mVSyncBrokenTestUpdates = 0;
	mWaitForVSync = true;
	mUserChanged3DSetting = false;
	mAutoEnable3D = false;
	mIs3D = true;
	mMinVidMemory3D = 6;
	mRecommendedVidMemory3D = 14;
	mRelaxUpdateBacklogCount = 0;
	mEnableWindowAspect = false;
	mWindowAspect.Set(4, 3);
	mIsWideWindow = false;

	int i;

	for (i = 0; i < NUM_CURSORS; i++)
		mCursorImages[i] = nullptr;

	for (i = 0; i < 256; i++)
		mAdd8BitMaxTable[i] = i;

	for (i = 256; i < 512; i++)
		mAdd8BitMaxTable[i] = 255;

	// Set default strings.  Init could read in overrides from partner.xml
	SetString("DIALOG_BUTTON_OK", "OK");
	SetString("DIALOG_BUTTON_CANCEL", "CANCEL");

	SetString("UPDATE_CHECK_TITLE", "Update Check");
	SetString("UPDATE_CHECK_BODY", "Checking if there are any updates available for this product ...");

	SetString("UP_TO_DATE_TITLE", "Up to Date");
	SetString("UP_TO_DATE_BODY", "There are no updates available for this product at this time.");
	SetString("NEW_VERSION_TITLE", "New Version");
	SetString("NEW_VERSION_BODY",
			  "There is an update available for this product.  Would you like to visit the web site to download it?");

	mDemoPrefix = "sexyapp";
	mDemoFileName = mDemoPrefix + ".dmo";
	mPlayingDemoBuffer = false;
	mManualShutdown = false;
	mRecordingDemoBuffer = false;
	mLastDemoMouseX = 0;
	mLastDemoMouseY = 0;
	mLastDemoUpdateCnt = 0;
	mDemoNeedsCommand = true;
	mDemoLoadingComplete = false;
	mDemoLength = 0;
	mDemoCmdNum = 0;
	mDemoCmdOrder = -1; // Means we haven't processed any demo commands yet
	mDemoCmdBitPos = 0;

	mWidgetManager = new WidgetManager(this);
	mResourceManager = new ResourceManager(this);

	#if WIN32
	mCopyMutex = NULL;
	#endif
	
#if SEXY_USE_CONTROLLER
	for (int i = 0; i < MAX_GAMEPADS; i++)
	{
		mGamepads[i] = nullptr;
	}
#endif

	mPrimaryThreadId = std::this_thread::get_id();

#if SEXY_CRASH_HANDLER
	gSEHCatcher.mApp = this;
#endif
}

SexyAppBase::~SexyAppBase()
{
	Shutdown();

	// Check if we should write the current 3d setting
	bool showedMsgBox = false;
	if (mUserChanged3DSetting)
	{
		bool writeToRegistry = true;
		bool is3D = false;
		bool is3DOptionSet = RegistryReadBoolean("Is3D", &is3D);
		if (!is3DOptionSet) // should we write the option?
		{
			if (!Is3DAccelerationRecommended()) // may need to prompt user if he wants to keep 3d acceleration on
			{
				if (Is3DAccelerated())
				{
					showedMsgBox = true;

					SDL_MessageBoxButtonData buttons[] = {{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Yes"},
														  {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No"}};
					SDL_MessageBoxData msgBoxData;
					msgBoxData.flags = SDL_MESSAGEBOX_WARNING;
					msgBoxData.title = GetString("HARDWARE_ACCEL_SWITCHED_ON",
												  "Hardware Acceleration was switched on during this session.\nIf this "
												  "resulted in slower performance,it should be switched off.\nWould "
												  "you like to keep Hardware Acceleration switched on?").c_str();
					std::string aMessage =
						(mCompanyName + " " +
						 GetString("HARDWARE_ACCEL_CONFIRMATION", "Hardware Acceleration Confirmation"));
					msgBoxData.message = aMessage.c_str();
					msgBoxData.buttons = buttons;
					msgBoxData.numbuttons = 2;
					int aResult;
					SDL_ShowMessageBox(&msgBoxData, &aResult);

					//mRenderer->mIs3D = aResult == IDYES ? true : false;
					if (aResult)
						writeToRegistry = false;
				}
				else
					writeToRegistry = false;
			}
		}

		//if (writeToRegistry)
			//RegistryWriteBoolean("Is3D", mDDInterface->mIs3D);
	}

	if (!showedMsgBox && Renderer::gRenderingPreDrawError && !IsScreenSaver())
	{

		SDL_MessageBoxButtonData buttons[] = {{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Yes"},
											  {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No"}};
		SDL_MessageBoxData msgBoxData;
		msgBoxData.flags = SDL_MESSAGEBOX_WARNING;
		std::string anAntiDangle1 =
			GetString("HARDWARE_ACCEL_NOT_WORKING",
					  "Hardware Acceleration may not have been working correctly during this session.\n"
					  "If you noticed graphics problems, you may want to turn off Hardware Acceleration.\n"
					  "Would you like to keep Hardware Acceleration switched on?");
		msgBoxData.title = anAntiDangle1.c_str();
		std::string anAntiDangle2 = mCompanyName + " " + GetString("HARDWARE_ACCEL_CONFIRMATION", "Hardware Acceleration Confirmation");
		msgBoxData.message = anAntiDangle2.c_str();
			
		msgBoxData.buttons = buttons;
		msgBoxData.numbuttons = 2;
		int aResult;
		SDL_ShowMessageBox(&msgBoxData, &aResult);

		if (aResult == 1)
			RegistryWriteBoolean("Is3D", false);
	}

	DialogMap::iterator aDialogItr = mDialogMap.begin();
	while (aDialogItr != mDialogMap.end())
	{
		mWidgetManager->RemoveWidget(aDialogItr->second);
		delete aDialogItr->second;
		++aDialogItr;
	}
	mDialogMap.clear();
	mDialogList.clear();

#if SEXY_USE_CONTROLLER
	for (int i = 0; i < MAX_GAMEPADS; i++)
	{
		if (mGamepads[i] != nullptr)
			delete mGamepads[i];
	}
#endif

	delete mWidgetManager;
	delete mResourceManager;
	delete gFPSImage;
	gFPSImage = NULL;

	SharedImageMap::iterator aSharedImageItr = mSharedImageMap.begin();
	while (aSharedImageItr != mSharedImageMap.end())
	{
		SharedImage *aSharedImage = &aSharedImageItr->second;
		DBG_ASSERTE(aSharedImage->mRefCount == 0);
		delete aSharedImage->mImage;
		mSharedImageMap.erase(aSharedImageItr++);
	}

	if (gDebugFont)
		delete gDebugFont;

	curl_global_cleanup();

	FT_Done_FreeType(mFreeTypeLib);

#if SEXY_USE_IMGUI
	delete mImGuiManager;
#endif
	delete mRenderer;
	delete mMusicInterface;
	delete mSoundManager;

	if (mWindow != NULL)
	{
		SDL_PropertiesID props = SDL_GetWindowProperties(mWindow->mInternalWindow);
		SDL_SetPointerProperty(props, "sexyappframework.userdata", nullptr);
		delete mWindow;
	}

	WaitForLoadingThread();

	gSexyAppBase = NULL;

	WriteDemoBuffer();

	for (int i = 0; i < SDL_SYSTEM_CURSOR_COUNT; i++)
	{
		SDL_DestroyCursor(mSystemCursors[i]);
	}
	for (int i = 0; i < NUM_CURSORS; i++)
		if (mCachedCursors[i] != nullptr)
			SDL_DestroyCursor(mCachedCursors[i]);
	SDL_Quit();

#if WIN32
	if (mCopyMutex != NULL)
		::CloseHandle(mCopyMutex);
	FreeLibrary(gVersionDLL);
#endif
}

static BOOL CALLBACK ChangeDisplayWindowEnumProc(HWND hwnd, LPARAM lParam)
{
	typedef std::map<HWND, RECT> WindowMap;
	static WindowMap aMap;

	if (lParam == 0 && aMap.find(hwnd) == aMap.end()) // record
	{
		RECT aRect;
		if (!IsIconic(hwnd) && IsWindowVisible(hwnd))
		{
			if (GetWindowRect(hwnd, &aRect))
			{
				//				char aBuf[4096];
				//				GetWindowText(hwnd,aBuf,4000);
				//				DWORD aProcessId = 0;
				//				GetWindowThreadProcessId(hwnd,&aProcessId);
				//				SEXY_TRACE(StrFormat("%s %d - %d %d %d %d",aBuf,aProcessId,aRect.left,aRect.top,aRect.right,aRect.bottom).c_str());
				aMap[hwnd] = aRect;
			}
		}
	}
	else
	{
		WindowMap::iterator anItr = aMap.find(hwnd);
		if (anItr != aMap.end())
		{
			RECT &r = anItr->second;
			MoveWindow(hwnd, r.left, r.top, abs(r.right - r.left), abs(r.bottom - r.top), TRUE);
		}
	}
	return TRUE;
}

void SexyAppBase::ClearUpdateBacklog(bool relaxForASecond)
{
	mLastTimeCheck = timeGetTime();
	mUpdateFTimeAcc = 0.0;

	if (relaxForASecond)
		mRelaxUpdateBacklogCount = 1000;
}

bool SexyAppBase::IsScreenSaver()
{
	return mIsScreenSaver;
}

bool SexyAppBase::AppCanRestore()
{
	return !mIsDisabled;
}

bool SexyAppBase::ReadDemoBuffer(std::string &theError)
{
	FILE *aFP = fopen(mDemoFileName.c_str(), "rb");

	if (aFP == NULL)
	{
		theError = "Demo file not found: " + mDemoFileName;
		return false;
	}

	struct AutoFile
	{
		FILE *f;
		AutoFile(FILE *file) : f(file)
		{
		}
		~AutoFile()
		{
			fclose(f);
		}
	};
	AutoFile aCloseFile(aFP);

	uint32_t aFileID;
	fread(&aFileID, 4, 1, aFP);

	DBG_ASSERTE(aFileID == DEMO_FILE_ID);
	if (aFileID != DEMO_FILE_ID)
	{
		theError = "Invalid demo file.";
		return false;
	}

	uint32_t aVersion;
	fread(&aVersion, 4, 1, aFP);

	fread(&mRandSeed, 4, 1, aFP);
	SRand(mRandSeed);

	unsigned short aStrLen = 4;
	fread(&aStrLen, 2, 1, aFP);
	if (aStrLen > 255)
		aStrLen = 255;
	char aStr[256];
	fread(aStr, 1, aStrLen, aFP);
	aStr[aStrLen] = '\0';

	DBG_ASSERTE(mProductVersion == aStr);
	if (mProductVersion != aStr)
	{
		theError = "This demo file appears to be for '" + std::string(aStr) + "'";
		return false;
	}

	int aFilePos = ftell(aFP);
	fseek(aFP, 0, SEEK_END);
	int aBytesLeft = ftell(aFP) - aFilePos;
	fseek(aFP, aFilePos, SEEK_SET);

	uint8_t *aBuffer;
	// read marker list
	if (aVersion >= 2)
	{
		int aSize;
		fread(&aSize, 4, 1, aFP);
		aBytesLeft -= 4;

		if (aSize >= aBytesLeft)
		{
			theError = "Invalid demo file.";
			return false;
		}

		Buffer aMarkerBuffer;

		aBuffer = new uint8_t[aSize];
		fread(aBuffer, 1, aSize, aFP);
		aMarkerBuffer.WriteBytes(aBuffer, aSize);
		aMarkerBuffer.SeekFront();

		int aNumItems = aMarkerBuffer.ReadLong();
		int i;
		for (i = 0; i < aNumItems && !aMarkerBuffer.AtEnd(); i++)
		{
			mDemoMarkerList.push_back(DemoMarker());
			DemoMarker &aMarker = mDemoMarkerList.back();
			aMarker.first = aMarkerBuffer.ReadString();
			aMarker.second = aMarkerBuffer.ReadLong();
		}

		if (i != aNumItems)
		{
			theError = "Invalid demo file.";
			return false;
		}

		aBytesLeft -= aSize;

		delete[] aBuffer;
	}

	// Read demo commands
	fread(&mDemoLength, 4, 1, aFP);
	aBytesLeft -= 4;

	if (aBytesLeft <= 0)
	{
		theError = "Invalid demo file.";
		return false;
	}

	aBuffer = new uint8_t[aBytesLeft];
	fread(aBuffer, 1, aBytesLeft, aFP);

	mDemoBuffer.WriteBytes(aBuffer, aBytesLeft);
	mDemoBuffer.SeekFront();

	delete[] aBuffer;
	return true;
}

void SexyAppBase::WriteDemoBuffer()
{
	if (mRecordingDemoBuffer)
	{
		FILE *aFP = fopen(mDemoFileName.c_str(), "w+b");

		if (aFP != NULL)
		{
			uint32_t aFileID = DEMO_FILE_ID;
			fwrite(&aFileID, 4, 1, aFP);

			uint32_t aVersion = DEMO_VERSION;
			fwrite(&aVersion, 4, 1, aFP);

			fwrite(&mRandSeed, 4, 1, aFP);

			unsigned short aStrLen = mProductVersion.length();
			fwrite(&aStrLen, 2, 1, aFP);
			fwrite(mProductVersion.c_str(), 1, mProductVersion.length(), aFP);

			Buffer aMarkerBuffer;
			aMarkerBuffer.WriteLong(mDemoMarkerList.size());
			for (DemoMarkerList::iterator aMarkerItr = mDemoMarkerList.begin(); aMarkerItr != mDemoMarkerList.end();
				 ++aMarkerItr)
			{
				aMarkerBuffer.WriteString(aMarkerItr->first);
				aMarkerBuffer.WriteLong(aMarkerItr->second);
			}
			int aMarkerBufferSize = aMarkerBuffer.GetDataLen();
			fwrite(&aMarkerBufferSize, 4, 1, aFP);
			fwrite(aMarkerBuffer.GetDataPtr(), aMarkerBufferSize, 1, aFP);

			uint32_t aDemoLength = mUpdateCount;
			fwrite(&aDemoLength, 4, 1, aFP);

			fwrite(mDemoBuffer.GetDataPtr(), 1, mDemoBuffer.GetDataLen(), aFP);
			fclose(aFP);
		}
	}
}

void SexyAppBase::DemoSyncBuffer(Buffer *theBuffer)
{
	if (mPlayingDemoBuffer)
	{
		if (mManualShutdown)
			return;

		PrepareDemoCommand(true);
		mDemoNeedsCommand = true;

		DBG_ASSERTE(!mDemoIsShortCmd);
		DBG_ASSERTE(mDemoCmdNum == DEMO_SYNC);

		uint32_t aLen = mDemoBuffer.ReadLong();

		theBuffer->Clear();
		for (int i = 0; i < (int)aLen; i++)
			theBuffer->WriteByte(mDemoBuffer.ReadByte());
	}
	else if (mRecordingDemoBuffer)
	{
		WriteDemoTimingBlock();
		mDemoBuffer.WriteNumBits(0, 1);
		mDemoBuffer.WriteNumBits(DEMO_SYNC, 5);
		mDemoBuffer.WriteLong(theBuffer->GetDataLen());
		mDemoBuffer.WriteBytes((uint8_t *)theBuffer->GetDataPtr(), theBuffer->GetDataLen());
	}
}

void SexyAppBase::DemoSyncString(std::string *theString)
{
	Buffer aBuffer;
	aBuffer.WriteString(*theString);
	DemoSyncBuffer(&aBuffer);
	*theString = aBuffer.ReadString();
}

void SexyAppBase::DemoSyncInt(int *theInt)
{
	Buffer aBuffer;
	aBuffer.WriteLong(*theInt);
	DemoSyncBuffer(&aBuffer);
	*theInt = aBuffer.ReadLong();
}

void SexyAppBase::DemoSyncBool(bool *theBool)
{
	Buffer aBuffer;
	aBuffer.WriteBoolean(*theBool);
	DemoSyncBuffer(&aBuffer);
	*theBool = aBuffer.ReadBoolean();
}

void SexyAppBase::DemoAssertStringEqual(const std::string &theString)
{
	if (mPlayingDemoBuffer)
	{
		if (mManualShutdown)
			return;

		PrepareDemoCommand(true);
		mDemoNeedsCommand = true;

		DBG_ASSERTE(!mDemoIsShortCmd);
		DBG_ASSERTE(mDemoCmdNum == DEMO_ASSERT_STRING_EQUAL);

		std::string aString = mDemoBuffer.ReadString();
		DBG_ASSERTE(aString == theString);
	}
	else if (mRecordingDemoBuffer)
	{
		WriteDemoTimingBlock();
		mDemoBuffer.WriteNumBits(0, 1);
		mDemoBuffer.WriteNumBits(DEMO_ASSERT_STRING_EQUAL, 5);
		mDemoBuffer.WriteString(theString);
	}
}

void SexyAppBase::DemoAddMarker(const std::string &theString)
{
	if (mPlayingDemoBuffer)
	{
		mFastForwardToMarker = false;
	}
	else if (mRecordingDemoBuffer)
	{
		mDemoMarkerList.push_back(DemoMarker(theString, mUpdateCount));
	}
}

//fuck uhh, todo: recode the entire demo system.
void SexyAppBase::DemoRegisterHandle(void* theHandle)
{
	if ((mRecordingDemoBuffer) || (mPlayingDemoBuffer))
	{
		// Insert the handle into a map with an auto-incrementing number so
		//  we can match up the auto-incrementing numbers with the handle
		//  later on, as handles may not be the same between executions
	//	std::pair<HandleToIntMap::iterator, bool> aPair =
		//	mHandleToIntMap.insert(HandleToIntMap::value_type(theHandle, mCurHandleNum));
		//DBG_ASSERT(aPair.second);
		//mCurHandleNum++;
	}
}

void SexyAppBase::DemoWaitForHandle(void* theHandle)
{
	/*
	WaitForSingleObject(theHandle, INFINITE);

	if ((mRecordingDemoBuffer) || (mPlayingDemoBuffer))
	{
		// Remove the handle from our waiting map
		HandleToIntMap::iterator anItr = mHandleToIntMap.find(theHandle);
		DBG_ASSERT(anItr != mHandleToIntMap.end());
		mHandleToIntMap.erase(anItr);
	}
	*/
}

bool SexyAppBase::DemoCheckHandle(void* theHandle)
{
	/*
	if (mPlayingDemoBuffer)
	{
		// We only need to try to get the result if we think we are waiting for one
		if (gSexyAppBase->PrepareDemoCommand(false))
		{
			if ((!gSexyAppBase->mDemoIsShortCmd) && (gSexyAppBase->mDemoCmdNum == DEMO_HANDLE_COMPLETE))
			{
				// Find auto-incrementing handle num from handle
				HandleToIntMap::iterator anItr = mHandleToIntMap.find(theHandle);
				DBG_ASSERT(anItr != mHandleToIntMap.end());

				int anOldBufferPos = gSexyAppBase->mDemoBuffer.mReadBitPos;

				// Since we don't require a demo result entry to be here, we must verify
				//  that this is referring to us
				int aDemoHandleNum = gSexyAppBase->mDemoBuffer.ReadLong();

				if (aDemoHandleNum == anItr->second)
				{
					// Alright, this was the handle we were waiting for!
					gSexyAppBase->mDemoNeedsCommand = true;

					// Actually wait for our local buddy to complete
					WaitForSingleObject(theHandle, INFINITE);
					mHandleToIntMap.erase(anItr);

					return true;
				}
				else
				{
					// Not us, go back
					gSexyAppBase->mDemoBuffer.mReadBitPos = anOldBufferPos;
				}
			}
		}

		return false;
	}
	else
	{
		if (WaitForSingleObject(theHandle, 0) == WAIT_OBJECT_0)
		{
			if (mRecordingDemoBuffer)
			{
				// Find auto-incrementing handle num from handle
				HandleToIntMap::iterator anItr = mHandleToIntMap.find(theHandle);
				DBG_ASSERT(anItr != mHandleToIntMap.end());

				gSexyAppBase->WriteDemoTimingBlock();
				gSexyAppBase->mDemoBuffer.WriteNumBits(0, 1);
				gSexyAppBase->mDemoBuffer.WriteNumBits(DEMO_HANDLE_COMPLETE, 5);
				gSexyAppBase->mDemoBuffer.WriteLong(anItr->second);

				mHandleToIntMap.erase(anItr);
			}

			return true;
		}

		return false;
	}*/
	return false;
}

void SexyAppBase::DemoAssertIntEqual(int theInt)
{
	if (mPlayingDemoBuffer)
	{
		if (mManualShutdown)
			return;

		PrepareDemoCommand(true);
		mDemoNeedsCommand = true;

		DBG_ASSERTE(!mDemoIsShortCmd);
		DBG_ASSERTE(mDemoCmdNum == DEMO_ASSERT_INT_EQUAL);

		int anInt = mDemoBuffer.ReadLong();
		DBG_ASSERTE(anInt == theInt);
	}
	else if (mRecordingDemoBuffer)
	{
		WriteDemoTimingBlock();
		mDemoBuffer.WriteNumBits(0, 1);
		mDemoBuffer.WriteNumBits(DEMO_ASSERT_INT_EQUAL, 5);
		mDemoBuffer.WriteLong(theInt);
	}
}

Dialog *SexyAppBase::NewDialog(int theDialogId,
							   bool isModal,
							   const SexyString &theDialogHeader,
							   const SexyString &theDialogLines,
							   const SexyString &theDialogFooter,
							   int theButtonMode)
{
	Dialog *aDialog =
		new Dialog(NULL, NULL, theDialogId, isModal, theDialogHeader, theDialogLines, theDialogFooter, theButtonMode);
	return aDialog;
}

Dialog *SexyAppBase::DoDialog(int theDialogId,
							  bool isModal,
							  const SexyString &theDialogHeader,
							  const SexyString &theDialogLines,
							  const SexyString &theDialogFooter,
							  int theButtonMode)
{
	KillDialog(theDialogId);

	Dialog *aDialog = NewDialog(theDialogId, isModal, theDialogHeader, theDialogLines, theDialogFooter, theButtonMode);

	AddDialog(theDialogId, aDialog);

	return aDialog;
}

Dialog *SexyAppBase::GetDialog(int theDialogId)
{
	DialogMap::iterator anItr = mDialogMap.find(theDialogId);

	if (anItr != mDialogMap.end())
		return anItr->second;

	return NULL;
}

bool SexyAppBase::KillDialog(int theDialogId, bool removeWidget, bool deleteWidget)
{
	DialogMap::iterator anItr = mDialogMap.find(theDialogId);

	if (anItr != mDialogMap.end())
	{
		Dialog *aDialog = anItr->second;

		// set the result to something else so DoMainLoop knows that the dialog is gone
		// in case nobody else sets mResult
		if (aDialog->mResult == -1)
			aDialog->mResult = 0;

		DialogList::iterator aListItr = std::find(mDialogList.begin(), mDialogList.end(), aDialog);
		if (aListItr != mDialogList.end())
			mDialogList.erase(aListItr);

		mDialogMap.erase(anItr);

		if (removeWidget || deleteWidget)
			mWidgetManager->RemoveWidget(aDialog);

		if (aDialog->IsModal())
		{
			ModalClose();
			mWidgetManager->RemoveBaseModal(aDialog);
		}

		if (deleteWidget)
			SafeDeleteWidget(aDialog);

		return true;
	}

	return false;
}

bool SexyAppBase::KillDialog(int theDialogId)
{
	return KillDialog(theDialogId, true, true);
}

bool SexyAppBase::KillDialog(Dialog *theDialog)
{
	return KillDialog(theDialog->mId);
}

int SexyAppBase::GetDialogCount()
{
	return mDialogMap.size();
}

void SexyAppBase::AddDialog(int theDialogId, Dialog *theDialog)
{
	KillDialog(theDialogId);

	if (theDialog->mWidth == 0)
	{
		// Set the dialog position ourselves
		int aWidth = mWidth / 2;
		theDialog->Resize((mWidth - aWidth) / 2, mHeight / 5, aWidth, theDialog->GetPreferredHeight(aWidth));
	}

	mDialogMap.insert(DialogMap::value_type(theDialogId, theDialog));
	mDialogList.push_back(theDialog);

	mWidgetManager->AddWidget(theDialog);
	if (theDialog->IsModal())
	{
		mWidgetManager->AddBaseModal(theDialog);
		ModalOpen();
	}
}

void SexyAppBase::AddDialog(Dialog *theDialog)
{
	AddDialog(theDialog->mId, theDialog);
}

void SexyAppBase::ModalOpen()
{
}

void SexyAppBase::ModalClose()
{
}

void SexyAppBase::DialogButtonPress(int theDialogId, int theButtonId)
{
	if (theButtonId == Dialog::ID_YES)
		ButtonPress(2000 + theDialogId);
	else if (theButtonId == Dialog::ID_NO)
		ButtonPress(3000 + theDialogId);
}

void SexyAppBase::DialogButtonDepress(int theDialogId, int theButtonId)
{
	if (theButtonId == Dialog::ID_YES)
		ButtonDepress(2000 + theDialogId);
	else if (theButtonId == Dialog::ID_NO)
		ButtonDepress(3000 + theDialogId);
}

void SexyAppBase::GotFocus()
{
}

void SexyAppBase::LostFocus()
{
}

void SexyAppBase::URLOpenFailed(const std::string &theURL)
{
	mIsOpeningURL = false;
}

void SexyAppBase::URLOpenSucceeded(const std::string &theURL)
{
	mIsOpeningURL = false;

	if (mShutdownOnURLOpen)
		Shutdown();
}

bool SexyAppBase::OpenURL(const std::string &theURL, bool shutdownOnOpen)
{
	if ((!mIsOpeningURL) || (theURL != mOpeningURL))
	{
		mShutdownOnURLOpen = shutdownOnOpen;
		mIsOpeningURL = true;
		mOpeningURL = theURL;
		mOpeningURLTime = SDL_GetTicks();

		if (SDL_OpenURL(theURL.c_str()))
		{
			return true;
		}
		else
		{
			URLOpenFailed(theURL);
			return false;
		}
	}

	return true;
}

std::string SexyAppBase::GetProductVersionDLL(const std::string &thePath)
{
	#if WIN32
	// Dynamically Load Version.dll
	typedef DWORD(APIENTRY * GetFileVersionInfoSizeFunc)(LPSTR lptstrFilename, LPDWORD lpdwHandle);
	typedef BOOL(APIENTRY * GetFileVersionInfoFunc)(LPSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
	typedef BOOL(APIENTRY * VerQueryValueFunc)(const LPVOID pBlock, LPSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);

	static GetFileVersionInfoSizeFunc aGetFileVersionInfoSizeFunc = NULL;
	static GetFileVersionInfoFunc aGetFileVersionInfoFunc = NULL;
	static VerQueryValueFunc aVerQueryValueFunc = NULL;

	if (aGetFileVersionInfoSizeFunc == NULL)
	{
		aGetFileVersionInfoSizeFunc =
			(GetFileVersionInfoSizeFunc)GetProcAddress(gVersionDLL, "GetFileVersionInfoSizeA");
		aGetFileVersionInfoFunc = (GetFileVersionInfoFunc)GetProcAddress(gVersionDLL, "GetFileVersionInfoA");
		aVerQueryValueFunc = (VerQueryValueFunc)GetProcAddress(gVersionDLL, "VerQueryValueA");
	}

	// Get Product Version
	std::string aProductVersion;

	unsigned int aSize = aGetFileVersionInfoSizeFunc((char *)thePath.c_str(), 0);
	if (aSize > 0)
	{
		uint8_t *aVersionBuffer = new uint8_t[aSize];
		aGetFileVersionInfoFunc((char *)thePath.c_str(), 0, aSize, aVersionBuffer);
		char *aBuffer;
		if (aVerQueryValueFunc(aVersionBuffer, "\\StringFileInfo\\040904B0\\ProductVersion", (void **)&aBuffer, &aSize))
		{
			aProductVersion = aBuffer;
		}
		else if (aVerQueryValueFunc(
					 aVersionBuffer, "\\StringFileInfo\\040904E4\\ProductVersion", (void **)&aBuffer, &aSize))
		{
			aProductVersion = aBuffer;
		}

		delete aVersionBuffer;
	}

	return aProductVersion;
	#else
	return "";
	#endif
	
}

void SexyAppBase::WaitForLoadingThread()
{
	while ((mLoadingThreadStarted) && (!mLoadingThreadCompleted))
		Sleep(20);
}

void SexyAppBase::SetCursorImage(int theCursorNum, Image *theImage)
{
	if ((theCursorNum >= 0) && (theCursorNum < NUM_CURSORS))
	{
		mCursorImages[theCursorNum] = theImage;
		EnforceCursor();
	}
}

void SexyAppBase::TakeScreenshot()
{
	ClearUpdateBacklog();
	if (mRenderer == nullptr)
		return;
	
	// Get free image name
	std::string anImageDir = "screenshots";
	MkDir(anImageDir);
	anImageDir += "/";

	int aMaxId = 0;
	std::string anImagePrefix = "image";

	std::string anImageName = anImageDir + anImagePrefix + StrFormat("%d", aMaxId);
	
	while (true)
	{
		anImageName = anImageDir + anImagePrefix + StrFormat("%d", aMaxId);

		if (!std::filesystem::exists(anImageName + ".png"))
			break;

		++aMaxId;
	}
	
	// Write image
	ImageLib::Image aSaveImage;
	aSaveImage.mBits = mRenderer->CaptureFrameBuffer();
	aSaveImage.mWidth = mRenderer->mWidth;
	aSaveImage.mHeight = mRenderer->mHeight;
	aSaveImage.mNumChannels = 4;
	ImageLib::WriteImage(anImageName, &aSaveImage, ".png");

}

void SexyAppBase::DumpProgramInfo()
{
	Deltree(GetAppDataFolder() + "_dump");

	MkDir(GetAppDataFolder() + "_dump");

	std::fstream aDumpStream((GetAppDataFolder() + "_dump\\imagelist.html").c_str(), std::ios::out);

	time_t aTime;
	time(&aTime);
	tm *aTM = localtime(&aTime);

	aDumpStream << "<HTML><BODY BGCOLOR=EEEEFF><CENTER><FONT SIZE=+2><B>" << asctime(aTM) << "</B></FONT><BR>"
				<< std::endl;

	int anImgNum = 0;

	int aThumbWidth = 64;
	int aThumbHeight = 64;

	ImageLib::Image anImageLibImage;
	anImageLibImage.mWidth = aThumbWidth;
	anImageLibImage.mHeight = aThumbHeight;
	anImageLibImage.mBits = new uint32_t[aThumbWidth * aThumbHeight];

	typedef std::multimap<int, MemoryImage *, std::greater<int>> SortedImageMap;

	int aTotalMemory = 0;

	SortedImageMap aSortedImageMap;
	MemoryImageSet::iterator anItr = mMemoryImageSet.begin();
	while (anItr != mMemoryImageSet.end())
	{
		MemoryImage *aMemoryImage = *anItr;

		int aNumPixels = aMemoryImage->mWidth * aMemoryImage->mHeight;

		GPUImage *aGPUImage = dynamic_cast<GPUImage *>(aMemoryImage);

		int aBitsMemory = 0;
		int aSurfaceMemory = 0;
		int aPalletizedMemory = 0;
		int aNativeAlphaMemory = 0;
		int aRLAlphaMemory = 0;
		int aRLAdditiveMemory = 0;
		int aTextureMemory = 0;

		int aMemorySize = 0;
		if (aMemoryImage->mBits != NULL)
			aBitsMemory = aNumPixels * 4;
		if ((aGPUImage != NULL) && (aGPUImage->mSurface != NULL))
			aSurfaceMemory = aNumPixels * 4; // Assume 32bit screen...
		if (aMemoryImage->mColorTable != NULL)
			aPalletizedMemory = aNumPixels + 256 * 4;
		if (aMemoryImage->mNativeAlphaData != NULL)
		{
			if (aMemoryImage->mColorTable != NULL)
				aNativeAlphaMemory = 256 * 4;
			else
				aNativeAlphaMemory = aNumPixels * 4;
		}
		if (aMemoryImage->mRLAlphaData != NULL)
			aRLAlphaMemory = aNumPixels;
		if (aMemoryImage->mRLAdditiveData != NULL)
			aRLAdditiveMemory = aNumPixels;
		if (aMemoryImage->mGPUData != NULL)
			aTextureMemory += ((TextureData *)aMemoryImage->mGPUData)->mTexMemSize;

		aMemorySize = aBitsMemory + aSurfaceMemory + aPalletizedMemory + aNativeAlphaMemory + aRLAlphaMemory +
					  aRLAdditiveMemory + aTextureMemory;
		aTotalMemory += aMemorySize;

		aSortedImageMap.insert(SortedImageMap::value_type(aMemorySize, aMemoryImage));

		++anItr;
	}

	aDumpStream << "Total Image Allocation: " << CommaSeperate(aTotalMemory).c_str() << " bytes<BR>";
	aDumpStream << "<TABLE BORDER=1 CELLSPACING=0 CELLPADDING=4>";

	int aTotalMemorySize = 0;
	int aTotalBitsMemory = 0;
	int aTotalSurfaceMemory = 0;
	int aTotalPalletizedMemory = 0;
	int aTotalNativeAlphaMemory = 0;
	int aTotalRLAlphaMemory = 0;
	int aTotalRLAdditiveMemory = 0;
	int aTotalTextureMemory = 0;

	SortedImageMap::iterator aSortedItr = aSortedImageMap.begin();
	while (aSortedItr != aSortedImageMap.end())
	{
		MemoryImage *aMemoryImage = aSortedItr->second;

		char anImageName[256];
		sprintf(anImageName, "img%04d.png", anImgNum);

		char aThumbName[256];
		sprintf(aThumbName, "thumb%04d.jpg", anImgNum);

		aDumpStream << "<TR>" << std::endl;

		aDumpStream << "<TD><A HREF=" << anImageName << "><IMG SRC=" << aThumbName << " WIDTH=" << aThumbWidth
					<< " HEIGHT=" << aThumbHeight << "></A></TD>" << std::endl;

		int aNumPixels = aMemoryImage->mWidth * aMemoryImage->mHeight;

		GPUImage *aGPUImage = dynamic_cast<GPUImage *>(aMemoryImage);

		int aMemorySize = aSortedItr->first;

		int aBitsMemory = 0;
		int aSurfaceMemory = 0;
		int aPalletizedMemory = 0;
		int aNativeAlphaMemory = 0;
		int aRLAlphaMemory = 0;
		int aRLAdditiveMemory = 0;
		int aTextureMemory = 0;
		std::string aTextureFormatName;

		if (aMemoryImage->mBits != NULL)
			aBitsMemory = aNumPixels * 4;
		if ((aGPUImage != NULL) && (aGPUImage->mSurface != NULL))
			aSurfaceMemory = aNumPixels * 4; // Assume 32bit screen...
		if (aMemoryImage->mColorTable != NULL)
			aPalletizedMemory = aNumPixels + 256 * 4;
		if (aMemoryImage->mNativeAlphaData != NULL)
		{
			if (aMemoryImage->mColorTable != NULL)
				aNativeAlphaMemory = 256 * 4;
			else
				aNativeAlphaMemory = aNumPixels * 4;
		}
		if (aMemoryImage->mRLAlphaData != NULL)
			aRLAlphaMemory = aNumPixels;
		if (aMemoryImage->mRLAdditiveData != NULL)
			aRLAdditiveMemory = aNumPixels;
		if (aMemoryImage->mGPUData != NULL)
		{
			aTextureMemory += ((TextureData *)aMemoryImage->mGPUData)->mTexMemSize;

			switch (((TextureData *)aMemoryImage->mGPUData)->mPixelFormat)
			{
			case PixelFormat_A8R8G8B8:
				aTextureFormatName = "A8R8G8B8";
				break;
			case PixelFormat_A4R4G4B4:
				aTextureFormatName = "A4R4G4B4";
				break;
			case PixelFormat_R5G6B5:
				aTextureFormatName = "R5G6B5";
				break;
			case PixelFormat_Palette8:
				aTextureFormatName = "Palette8";
				break;
			}
		}

		aTotalMemorySize += aMemorySize;
		aTotalBitsMemory += aBitsMemory;
		aTotalTextureMemory += aTextureMemory;
		aTotalSurfaceMemory += aSurfaceMemory;
		aTotalPalletizedMemory += aPalletizedMemory;
		aTotalNativeAlphaMemory += aNativeAlphaMemory;
		aTotalRLAlphaMemory += aRLAlphaMemory;
		aTotalRLAdditiveMemory += aRLAdditiveMemory;

		char aStr[256];
		sprintf(aStr,
				"%d x %d<BR>%s bytes",
				aMemoryImage->mWidth,
				aMemoryImage->mHeight,
				CommaSeperate(aMemorySize).c_str());
		aDumpStream << "<TD ALIGN=RIGHT>" << aStr << "</TD>" << std::endl;

		aDumpStream << "<TD>"
					<< SexyStringToString(
						   ((aBitsMemory != 0) ? "mBits<BR>" + CommaSeperate(aBitsMemory) : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>"
					<< SexyStringToString(((aPalletizedMemory != 0)
											   ? "Palletized<BR>" + CommaSeperate(aPalletizedMemory)
											   : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>"
					<< SexyStringToString(
						   ((aSurfaceMemory != 0) ? "DDSurface<BR>" + CommaSeperate(aSurfaceMemory) : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>"
					<< SexyStringToString(((aMemoryImage->mGPUData != NULL)
											   ? "Texture<BR>" + StringToSexyString(aTextureFormatName) +
													 "<BR>" + CommaSeperate(aTextureMemory)
											   : "&nbsp;"))
					<< "</TD>" << std::endl;

		aDumpStream << "<TD>" << SexyStringToString(((aMemoryImage->mIsVolatile) ? "Volatile" : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>" << SexyStringToString(((aMemoryImage->mForcedMode) ? "Forced" : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>" << SexyStringToString(((aMemoryImage->mHasAlpha) ? "HasAlpha" : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>" << SexyStringToString(((aMemoryImage->mHasTrans) ? "HasTrans" : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>"
					<< SexyStringToString(((aNativeAlphaMemory != 0)
											   ? "NativeAlpha<BR>" + CommaSeperate(aNativeAlphaMemory)
											   : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>"
					<< SexyStringToString(
						   ((aRLAlphaMemory != 0) ? "RLAlpha<BR>" + CommaSeperate(aRLAlphaMemory) : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>"
					<< SexyStringToString(((aRLAdditiveMemory != 0)
											   ? "RLAdditive<BR>" + CommaSeperate(aRLAdditiveMemory)
											   : "&nbsp;"))
					<< "</TD>" << std::endl;
		aDumpStream << "<TD>" << (aMemoryImage->mFilePath.empty() ? "&nbsp;" : aMemoryImage->mFilePath) << "</TD>"
					<< std::endl;

		aDumpStream << "</TR>" << std::endl;

		// Write thumb

		MemoryImage aCopiedImage(*aMemoryImage);

		uint32_t *aBits = aCopiedImage.GetBits();

		uint32_t *aThumbBitsPtr = anImageLibImage.mBits;

		for (int aThumbY = 0; aThumbY < aThumbHeight; aThumbY++)
			for (int aThumbX = 0; aThumbX < aThumbWidth; aThumbX++)
			{
				int aSrcX = (int)(aCopiedImage.mWidth * (aThumbX + 0.5)) / aThumbWidth;
				int aSrcY = (int)(aCopiedImage.mHeight * (aThumbY + 0.5)) / aThumbHeight;

				*(aThumbBitsPtr++) = aBits[aSrcX + (aSrcY * aCopiedImage.mWidth)];
			}

		ImageLib::WriteImage(
			(GetAppDataFolder() + std::string("_dump\\") + aThumbName).c_str(), &anImageLibImage, ".jpeg");

		// Write high resolution image

		ImageLib::Image anFullImage;
		anFullImage.mBits = aCopiedImage.GetBits();
		anFullImage.mWidth = aCopiedImage.GetWidth();
		anFullImage.mHeight = aCopiedImage.GetHeight();

		ImageLib::WriteImage((GetAppDataFolder() + std::string("_dump\\") + anImageName).c_str(), &anFullImage, ".png");

		anFullImage.mBits = NULL;

		anImgNum++;

		aSortedItr++;
	}

	aDumpStream << "<TD>Totals</TD>" << std::endl;
	aDumpStream << "<TD>" << SexyStringToString(CommaSeperate(aTotalMemorySize)) << "</TD>" << std::endl;
	aDumpStream << "<TD>" << SexyStringToString(CommaSeperate(aTotalBitsMemory)) << "</TD>" << std::endl;
	aDumpStream << "<TD>" << SexyStringToString(CommaSeperate(aTotalPalletizedMemory)) << "</TD>" << std::endl;
	aDumpStream << "<TD>" << SexyStringToString(CommaSeperate(aTotalSurfaceMemory)) << "</TD>" << std::endl;
	aDumpStream << "<TD>" << SexyStringToString(CommaSeperate(aTotalTextureMemory)) << "</TD>" << std::endl;
	aDumpStream << "<TD>&nbsp;</TD>" << std::endl;
	aDumpStream << "<TD>&nbsp;</TD>" << std::endl;
	aDumpStream << "<TD>&nbsp;</TD>" << std::endl;
	aDumpStream << "<TD>&nbsp;</TD>" << std::endl;
	aDumpStream << "<TD>" << SexyStringToString(CommaSeperate(aTotalNativeAlphaMemory)) << "</TD>" << std::endl;
	aDumpStream << "<TD>" << SexyStringToString(CommaSeperate(aTotalRLAlphaMemory)) << "</TD>" << std::endl;
	aDumpStream << "<TD>" << SexyStringToString(CommaSeperate(aTotalRLAdditiveMemory)) << "</TD>" << std::endl;
	aDumpStream << "<TD>&nbsp;</TD>" << std::endl;

	aDumpStream << "</TABLE></CENTER></BODY></HTML>" << std::endl;
}

double SexyAppBase::GetLoadingThreadProgress()
{
	if (mLoaded)
		return 1.0;
	if (!mLoadingThreadStarted)
		return 0.0;
	if (mNumLoadingThreadTasks == 0)
		return 0.0;
	return std::min(mCompletedLoadingThreadTasks / (double)mNumLoadingThreadTasks, 1.0);
}

bool SexyAppBase::RegistryWrite(const std::string &theValueName,
								JSONRegistryType theType,
								const uint8_t *theValue,
								uint32_t theLength)
{
	if (mPlayingDemoBuffer)
	{
		if (mManualShutdown)
			return true;

		PrepareDemoCommand(true);
		mDemoNeedsCommand = true;

		DBG_ASSERTE(!mDemoIsShortCmd);
		DBG_ASSERTE(mDemoCmdNum == DEMO_REGISTRY_WRITE);

		return mDemoBuffer.ReadNumBits(1, false) != 0;
	}

	std::filesystem::path config = GetAppDataFolder() + "/registry.json"; // always registry.json
	std::filesystem::create_directories(config.parent_path());

	nlohmann::json j;
	std::ifstream inFile(config);
	if (inFile)
	{
		try
		{
			inFile >> j;
		}
		catch (...)
		{
		}
	}

	switch (theType)
	{
	case JSONRegistryType::TYPE_STRING:
		j[theValueName] = std::string(reinterpret_cast<const char *>(theValue), theLength);
		break;
	case JSONRegistryType::TYPE_INTEGER:
		if (theLength == sizeof(int))
			j[theValueName] = *reinterpret_cast<const int *>(theValue);
		break;
	case JSONRegistryType::TYPE_BOOL:
		if (theLength == sizeof(int))
			j[theValueName] = (*reinterpret_cast<const int *>(theValue)) != 0;
		break;
	case JSONRegistryType::TYPE_DATA: {
		std::vector<uint8_t> bin(theValue, theValue + theLength);
		j[theValueName] = bin;
		break;
	}
	default:
		return false;
	}

	std::ofstream outFile(config);
	if (outFile)
		outFile << j.dump(4);

	return true;
}

bool SexyAppBase::RegistryWriteString(const std::string &theValueName, const std::string &theString)
{
	return RegistryWrite(theValueName, JSONRegistryType::TYPE_STRING, (uint8_t *)theString.c_str(), theString.length());
}

bool SexyAppBase::RegistryWriteInteger(const std::string &theValueName, int theValue)
{
	return RegistryWrite(theValueName, JSONRegistryType::TYPE_INTEGER, (uint8_t *)&theValue, sizeof(int));
}

bool SexyAppBase::RegistryWriteBoolean(const std::string &theValueName, bool theValue)
{
	int aValue = theValue ? 1 : 0;
	return RegistryWrite(theValueName, TYPE_BOOL, (uint8_t *)&aValue, sizeof(int));
}

bool SexyAppBase::RegistryWriteData(const std::string &theValueName, const uint8_t *theValue, uint32_t theLength)
{
	return RegistryWrite(theValueName, TYPE_DATA, (uint8_t *)theValue, theLength);
}

void SexyAppBase::WriteToRegistry()
{
	RegistryWriteInteger("MusicVolume", (int)(mMusicVolume * 100));
	RegistryWriteInteger("SfxVolume", (int)(mSfxVolume * 100));
	RegistryWriteInteger("Muted", (mMuteCount - mAutoMuteCount > 0) ? 1 : 0);
	RegistryWriteInteger("ScreenMode", mIsWindowed ? 0 : 1);
	RegistryWriteInteger("PreferredX", mPreferredSize.mX);
	RegistryWriteInteger("PreferredY", mPreferredSize.mY);
	RegistryWriteInteger("PreferredWidth", mPreferredSize.mWidth);
	RegistryWriteInteger("PreferredHeight", mPreferredSize.mHeight);
	RegistryWriteInteger("CustomCursors", mCustomCursorsEnabled ? 1 : 0);
	RegistryWriteInteger("InProgress", 0);
	RegistryWriteBoolean("WaitForVSync", mWaitForVSync);
	RegistryWriteBoolean("Is3D", mIs3D);
	RegistryWriteInteger("DesiredBackend", mDesiredBackend);
}

bool SexyAppBase::RegistryEraseKey(const SexyString &_theKeyName)
{ 
	if (mPlayingDemoBuffer)
	{
		if (mManualShutdown)
			return true;

		PrepareDemoCommand(true);
		mDemoNeedsCommand = true;

		DBG_ASSERTE(!mDemoIsShortCmd);
		DBG_ASSERTE(mDemoCmdNum == DEMO_REGISTRY_ERASE);

		return mDemoBuffer.ReadNumBits(1, false) != 0;
	}

	std::filesystem::path keyPath = GetAppDataFolder() + _theKeyName + "/registry.json";

	if (std::filesystem::exists(keyPath))
	{
		std::error_code anError;
		std::filesystem::remove(keyPath, anError);
		if (anError)
		{
			if (mRecordingDemoBuffer)
			{
				WriteDemoTimingBlock();
				mDemoBuffer.WriteNumBits(0, 1);
				mDemoBuffer.WriteNumBits(DEMO_REGISTRY_ERASE, 5);
				mDemoBuffer.WriteNumBits(0, 1); // failure
			}

			return false;
		}

		if (mRecordingDemoBuffer)
		{
			WriteDemoTimingBlock();
			mDemoBuffer.WriteNumBits(0, 1);
			mDemoBuffer.WriteNumBits(DEMO_REGISTRY_ERASE, 5);
			mDemoBuffer.WriteNumBits(1, 1); // success
		}
		return true;
	}

	return false;
}

void SexyAppBase::RegistryEraseValue(const SexyString &_theValueName)
{
	std::filesystem::path configPath = GetAppDataFolder() + "/registry.json";
	std::string keyName = _theValueName;

	if (!std::filesystem::exists(configPath))
		return;

	nlohmann::json j;
	std::ifstream inFile(configPath);
	if (inFile)
	{
		try
		{
			inFile >> j;
		}
		catch (...)
		{
			return;
		}
	}

	if (j.contains(keyName))
	{
		j.erase(keyName);

		std::ofstream outFile(configPath);
		if (outFile)
			outFile << j.dump(4);
	}

	return;
}

bool SexyAppBase::RegistryGetSubKeys(const std::string &theKeyName, StringVector *theSubKeys)
{
	return false;
}

bool SexyAppBase::RegistryRead(const std::string &theValueName,
							   JSONRegistryType *theType,
							   uint8_t *theValue,
							   uint32_t *theLength)
{
	return RegistryReadKey(theValueName, theType, theValue, theLength, 0);
}

bool SexyAppBase::RegistryReadKey(
	const std::string &theValueName, JSONRegistryType *theType, uint8_t *theValue, uint32_t *theLength, int theKey)
{
	std::filesystem::path configPath = GetAppDataFolder() + "/registry.json";
	if (!std::filesystem::exists(configPath) || !theType || !theValue || !theLength)
		return false;

	nlohmann::json j;
	std::ifstream inFile(configPath);
	if (!inFile)
		return false;
	try
	{
		inFile >> j;
	}
	catch (...)
	{
		return false;
	}

	if (!j.contains(theValueName))
		return false;

	auto &entry = j[theValueName];
	// JSONRegistryType::String
	if (entry.is_string())
	{
		std::string s = entry.get<std::string>();
		if (s.size() > *theLength)
			return false;
		std::memcpy(theValue, s.data(), s.size());
		*theLength = static_cast<unsigned long>(s.size());
		*theType = JSONRegistryType::TYPE_STRING;
		return true;
	}
	// JSONRegistryType::Integer
	else if (entry.is_number_integer())
	{
		if (*theLength < sizeof(int))
			return false;
		int v = entry.get<int>();
		std::memcpy(theValue, &v, sizeof(int));
		*theLength = sizeof(int);
		*theType = JSONRegistryType::TYPE_INTEGER;
		return true;
	}
	// JSONRegistryType::Bool
	else if (entry.is_boolean())
	{
		if (*theLength < sizeof(int))
			return false;
		int b = entry.get<bool>() ? 1 : 0;
		std::memcpy(theValue, &b, sizeof(int));
		*theLength = sizeof(int);
		*theType = JSONRegistryType::TYPE_BOOL;
		return true;
	}
	// JSONRegistryType::Data
	else if (entry.is_array())
	{
		size_t size = entry.size();
		if (size > *theLength)
			return false;
		for (size_t i = 0; i < size; ++i)
			theValue[i] = static_cast<uint8_t>(entry[i].get<int>());
		*theLength = static_cast<uint32_t>(size);
		*theType = JSONRegistryType::TYPE_DATA;
		return true;
	}

	return false;
}

bool SexyAppBase::RegistryReadString(const std::string &theKey, std::string *theString)
{
	std::filesystem::path configPath = GetAppDataFolder() + "/registry.json";
	if (!std::filesystem::exists(configPath) || !theString)
		return false;

	nlohmann::json j;
	std::ifstream inFile(configPath);
	if (!inFile)
		return false;
	try
	{
		inFile >> j;
	}
	catch (...)
	{
		return false;
	}

	if (j.contains(theKey) && j[theKey].is_string())
	{
		*theString = j[theKey].get<std::string>();
		return true;
	}
	return false;
}

bool SexyAppBase::RegistryReadInteger(const std::string &theKey, int *theValue)
{
	if (!theValue)
		return false;

	std::string s;

	nlohmann::json j;
	std::filesystem::path configPath = GetAppDataFolder() + "/registry.json";
	std::ifstream inFile(configPath);
	if (!inFile)
		return false;
	try
	{
		inFile >> j;
	}
	catch (...)
	{
		return false;
	}

	if (j.contains(theKey) && j[theKey].is_number_integer())
	{
		*theValue = j[theKey].get<int>();
		return true;
	}
	return false;
}

bool SexyAppBase::RegistryReadBoolean(const std::string &theKey, bool *theValue)
{
	if (!theValue)
		return false;

	nlohmann::json j;
	std::filesystem::path configPath = GetAppDataFolder() + "/registry.json";
	std::ifstream inFile(configPath);
	if (!inFile)
		return false;
	try
	{
		inFile >> j;
	}
	catch (...)
	{
		return false;
	}

	if (j.contains(theKey) && j[theKey].is_boolean())
	{
		*theValue = j[theKey].get<bool>();
		return true;
	}
	return false;
}

bool SexyAppBase::RegistryReadData(const std::string &theKey, uint8_t *theValue, uint32_t *theLength)
{
	if (!theValue || !theLength)
		return false;

	nlohmann::json j;
	std::filesystem::path configPath = GetAppDataFolder() + "/registry.json";
	std::ifstream inFile(configPath);
	if (!inFile)
		return false;
	try
	{
		inFile >> j;
	}
	catch (...)
	{
		return false;
	}

	if (j.contains(theKey) && j[theKey].is_array())
	{
		size_t size = j[theKey].size();
		if (size > *theLength)
			return false;
		for (size_t i = 0; i < size; ++i)
			theValue[i] = static_cast<uint8_t>(j[theKey][i].get<int>());
		*theLength = static_cast<uint32_t>(size);
		return true;
	}
	return false;
}

void SexyAppBase::ReadFromRegistry()
{
	mReadFromRegistry = true;

	int anInt;
	if (RegistryReadInteger("MusicVolume", &anInt))
		mMusicVolume = anInt / 100.0;

	if (RegistryReadInteger("SfxVolume", &anInt))
		mSfxVolume = anInt / 100.0;

	if (RegistryReadInteger("Muted", &anInt))
		mMuteCount = anInt;

	if (RegistryReadInteger("ScreenMode", &anInt))
		mIsWindowed = anInt == 0;

	RegistryReadInteger("PreferredX", &mPreferredSize.mX);
	RegistryReadInteger("PreferredY", &mPreferredSize.mY);
	RegistryReadInteger("PreferredWidth", &mPreferredSize.mWidth);
	RegistryReadInteger("PreferredHeight", &mPreferredSize.mHeight);

	if (RegistryReadInteger("CustomCursors", &anInt))
		EnableCustomCursors(anInt != 0);

	RegistryReadBoolean("WaitForVSync", &mWaitForVSync);
	RegistryReadBoolean("Is3D", &mIs3D);
	int aBackendInt = 0;
	RegistryReadInteger("DesiredBackend", &aBackendInt);
	mDesiredBackend = (RenderingBackend)aBackendInt;

	if (RegistryReadInteger("InProgress", &anInt))
		mLastShutdownWasGraceful = anInt == 0;

	if (!IsScreenSaver())
		RegistryWriteInteger("InProgress", 1);
}

bool SexyAppBase::WriteBytesToFile(const std::string &theFileName, const void *theData, unsigned long theDataLen)
{
	if (mPlayingDemoBuffer)
	{
		if (mManualShutdown)
			return true;

		PrepareDemoCommand(true);
		mDemoNeedsCommand = true;

		DBG_ASSERTE(!mDemoIsShortCmd);
		DBG_ASSERTE(mDemoCmdNum == DEMO_FILE_WRITE);

		bool success = mDemoBuffer.ReadNumBits(1, false) != 0;
		if (!success)
			return false;

		return true;
	}

	MkDir(GetFileDir(theFileName));
	FILE *aFP = fopen(theFileName.c_str(), "w+b");

	if (aFP == NULL)
	{
		if (mRecordingDemoBuffer)
		{
			WriteDemoTimingBlock();
			mDemoBuffer.WriteNumBits(0, 1);
			mDemoBuffer.WriteNumBits(DEMO_FILE_WRITE, 5);
			mDemoBuffer.WriteNumBits(0, 1); // failure
		}

		return false;
	}

	fwrite(theData, 1, theDataLen, aFP);
	fclose(aFP);

	if (mRecordingDemoBuffer)
	{
		WriteDemoTimingBlock();
		mDemoBuffer.WriteNumBits(0, 1);
		mDemoBuffer.WriteNumBits(DEMO_FILE_WRITE, 5);
		mDemoBuffer.WriteNumBits(1, 1); // success
	}

	return true;
}

bool SexyAppBase::WriteBufferToFile(const std::string &theFileName, const Buffer *theBuffer)
{
	return WriteBytesToFile(theFileName, theBuffer->GetDataPtr(), theBuffer->GetDataLen());
}

bool SexyAppBase::ReadBufferFromFile(const std::string &theFileName, Buffer *theBuffer, bool dontWriteToDemo)
{
	if ((mPlayingDemoBuffer) && (!dontWriteToDemo))
	{
		if (mManualShutdown)
			return false;

		PrepareDemoCommand(true);
		mDemoNeedsCommand = true;

		DBG_ASSERTE(!mDemoIsShortCmd);
		DBG_ASSERTE(mDemoCmdNum == DEMO_FILE_READ);

		bool success = mDemoBuffer.ReadNumBits(1, false) != 0;
		if (!success)
			return false;

		uint32_t aLen = mDemoBuffer.ReadLong();

		theBuffer->Clear();
		for (int i = 0; i < (int)aLen; i++)
			theBuffer->WriteByte(mDemoBuffer.ReadByte());

		return true;
	}
	else
	{
		PFILE *aFP = p_fopen(theFileName.c_str(), "rb");

		if (aFP == NULL)
		{
			if ((mRecordingDemoBuffer) && (!dontWriteToDemo))
			{
				WriteDemoTimingBlock();
				mDemoBuffer.WriteNumBits(0, 1);
				mDemoBuffer.WriteNumBits(DEMO_FILE_READ, 5);
				mDemoBuffer.WriteNumBits(0, 1); // failure
			}

			return false;
		}

		p_fseek(aFP, 0, SEEK_END);
		int aFileSize = p_ftell(aFP);
		p_fseek(aFP, 0, SEEK_SET);

		uint8_t *aData = new uint8_t[aFileSize];

		p_fread(aData, 1, aFileSize, aFP);
		p_fclose(aFP);

		theBuffer->Clear();
		theBuffer->SetData(aData, aFileSize);

		if ((mRecordingDemoBuffer) && (!dontWriteToDemo))
		{
			WriteDemoTimingBlock();
			mDemoBuffer.WriteNumBits(0, 1);
			mDemoBuffer.WriteNumBits(DEMO_FILE_READ, 5);
			mDemoBuffer.WriteNumBits(1, 1); // success
			mDemoBuffer.WriteLong(aFileSize);
			mDemoBuffer.WriteBytes(aData, aFileSize);
		}

		delete[] aData;

		return true;
	}
}

bool SexyAppBase::FileExists(const std::string &theFileName)
{
	if (mPlayingDemoBuffer)
	{
		if (mManualShutdown)
			return true;

		PrepareDemoCommand(true);
		mDemoNeedsCommand = true;

		DBG_ASSERTE(!mDemoIsShortCmd);
		DBG_ASSERTE(mDemoCmdNum == DEMO_FILE_EXISTS);

		bool success = mDemoBuffer.ReadNumBits(1, false) != 0;
		return success;
	}
	else
	{
		PFILE *aFP = p_fopen(theFileName.c_str(), "rb");

		if (mRecordingDemoBuffer)
		{
			WriteDemoTimingBlock();
			mDemoBuffer.WriteNumBits(0, 1);
			mDemoBuffer.WriteNumBits(DEMO_FILE_EXISTS, 5);
			mDemoBuffer.WriteNumBits((aFP != NULL) ? 1 : 0, 1);
		}

		if (aFP == NULL)
			return false;

		p_fclose(aFP);
		return true;
	}
}

bool SexyAppBase::EraseFile(const std::string &theFileName)
{
	if (mPlayingDemoBuffer)
		return true;

	return DeleteFileA(theFileName.c_str()) != 0;
}

void SexyAppBase::SEHOccured()
{
	SetMusicVolume(0);
	mSEHOccured = true;
	EnforceCursor();
}

std::string SexyAppBase::GetGameSEHInfo()
{
	int aSecLoaded = (SDL_GetTicks() - mTimeLoaded) / 1000;

	char aTimeStr[16];
	sprintf(aTimeStr, "%02d:%02d:%02d", (aSecLoaded / 60 / 60), (aSecLoaded / 60) % 60, aSecLoaded % 60);

	char aThreadIdStr[16];
	sprintf(aThreadIdStr, "%X", mPrimaryThreadId);

	std::string anInfoString = "Product: " + mProdName + "\r\n" + "Version: " + mProductVersion + "\r\n";

	anInfoString += "Time Loaded: " + std::string(aTimeStr) +
					"\r\n"
					"Fullscreen: " +
					(mIsWindowed ? std::string("No") : std::string("Yes")) +
					"\r\n"
					"Primary ThreadId: " +
					aThreadIdStr + "\r\n";

	return anInfoString;
}

void SexyAppBase::GetSEHWebParams(DefinesMap *theDefinesMap)
{
}

void SexyAppBase::ShutdownHook()
{
}

void SexyAppBase::Shutdown()
{
	if (std::this_thread::get_id() != mPrimaryThreadId)
	{
		mLoadingFailed = true;
	}
	else if (!mShutdown)
	{
		mExitToTop = true;
		mShutdown = true;
		ShutdownHook();

		if (mPlayingDemoBuffer)
		{
			//if the music/sfx volume is 0, then it means that in playback
			//someone pressed the "S" key to mute sounds (or that the
			//sound volume was set to 0 in the first place). Out of politeness,
			//return the system sound volume to what it last was in the game.
			SetMusicVolume(mDemoMusicVolume);
			SetSfxVolume(mDemoSfxVolume);
		}

		// Blah
		while (mCursorThreadRunning)
		{
			Sleep(10);
		}

		if (mMusicInterface != NULL)
			mMusicInterface->StopAllMusic();

		RestoreScreenResolution();

		if (mReadFromRegistry)
			WriteToRegistry();
	}
}

void SexyAppBase::RestoreScreenResolution()
{

}

void SexyAppBase::DoExit(int theCode)
{
	RestoreScreenResolution();
	exit(theCode);
}

void SexyAppBase::UpdateFrames()
{
	mUpdateCount++;

	if (!mMinimized)
	{
		if (mWidgetManager->UpdateFrame())
			++mFPSDirtyCount;
	}

	mMusicInterface->Update();
	CleanSharedImages();

#if SEXY_USE_CONTROLLER
	for (int i = 0; i < MAX_GAMEPADS; i++)
	{
		if (mGamepads[i] != nullptr)
			mGamepads[i]->UpdateStates(1.0f);
	}
#endif
}

void SexyAppBase::DoUpdateFramesF(float theFrac)
{
	if ((mVSyncUpdates) && (!mMinimized))
		mWidgetManager->UpdateFrameF(theFrac);
}

bool SexyAppBase::DoUpdateFrames()
{
	SEXY_AUTO_PERF("SexyAppBase::DoUpdateFrames");

	if (gScreenSaverActive)
		return false;

	if (mPlayingDemoBuffer)
	{
		if ((mLoadingThreadCompleted) && (!mLoaded) && (mDemoLoadingComplete))
		{
			mLoaded = true;
			::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_NORMAL);
			mYieldMainThread = false;
			LoadingThreadCompleted();
		}

		// Hrrm not sure why we check (mUpdateCount != mLastDemoUpdateCnt) here
		if ((mLoaded == mDemoLoadingComplete) && (mUpdateCount != mLastDemoUpdateCnt))
		{
			UpdateFrames();
			return true;
		}

		return false;
	}
	else
	{
		if ((mLoadingThreadCompleted) && (!mLoaded))
		{
			::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_NORMAL);
			mLoaded = true;
			mYieldMainThread = false;
			LoadingThreadCompleted();

			if (mRecordingDemoBuffer)
			{
				WriteDemoTimingBlock();
				mDemoBuffer.WriteNumBits(0, 1);
				mDemoBuffer.WriteNumBits(DEMO_LOADING_COMPLETE, 5);
			}
		}

		UpdateFrames();
		return true;
	}
}

bool gIsFailing = false;

void SexyAppBase::Redraw(Rect *theClipRect)
{
	SEXY_AUTO_PERF("SexyAppBase::Redraw");

	// Do mIsDrawing check because we could enter here at a bad time if any windows messages
	//  are processed during WidgetManager->Draw
	if ((mIsDrawing) || (mShutdown))
		return;

	if (gScreenSaverActive)
		return;

#if SEXY_USE_IMGUI
	mImGuiManager->Frame();
#endif

	static uint32_t aRetryTick = 0;
	if (!mRenderer->Redraw(theClipRect))
	{
		Renderer::gRenderingPreDrawError = false; // something wrong happened!!!
		if (!gIsFailing)
		{
			gIsFailing = true;
		}

		uint32_t aTick = SDL_GetTicks();
		if ((mActive || (aTick - aRetryTick > 1000 && mIsPhysWindowed)) && (!mMinimized))
		{
			aRetryTick = aTick;

			mWidgetManager->mImage = NULL;

			SDL_DisplayID aPrimaryDisplay = SDL_GetPrimaryDisplay();
			const SDL_DisplayMode *aMode = SDL_GetCurrentDisplayMode(aPrimaryDisplay);

			// Re-check resolution at this point, because we hit here when you change your resolution.
			if ((mWidth >= aMode->w || mHeight >= aMode->h) && mIsWindowed)
			{
				if (mForceWindowed)
				{
					Popup(GetString("PLEASE_SET_COLOR_DEPTH", "Please set your desktop color depth to 16 bit."));
					Shutdown();
					return;
				}
				mForceFullscreen = true;

				SwitchScreenMode(false);
				return;
			}

			int aResult = InitRenderer();

			//gDebugStream << SDL_GetTicks() << " ReInit..." << std::endl;
			/*
			if ((mIsWindowed) && (aResult == DDInterface::RESULT_INVALID_COLORDEPTH))
			{
				//gDebugStream << SDL_GetTicks() << "ReInit Invalid Colordepth" << std::endl;
				if (!mActive) // don't switch to full screen if not active app
					return;

				SwitchScreenMode(false);
				mForceFullscreen = true;
				return;
			}
			else if (aResult == DDInterface::RESULT_3D_FAIL)
			{
				Set3DAcclerated(false);
				return;
			}
			else if (aResult != DDInterface::RESULT_OK)
			{
				//gDebugStream << SDL_GetTicks() << " ReInit Failed" << std::endl;
				//Fail("Failed to initialize DirectDraw");
				//Sleep(1000);

				return;
			}*/

			ReInitImages();

			mWidgetManager->mImage = mRenderer->GetScreenImage();
			mWidgetManager->MarkAllDirty();

			mLastTime = timeGetTime();
		}
	}
	else
	{
		if (gIsFailing)
		{
			//gDebugStream << SDL_GetTicks() << " Redraw succeeded" << std::endl;
			gIsFailing = false;
			aRetryTick = 0;
		}
	}

	mFPSFlipCount++;
}

///////////////////////////// FPS Stuff
static PerfTimer gFPSTimer;
static int gFrameCount;
static int gFPSDisplay;
static bool gForceDisplay = false;
static void CalculateFPS()
{
	gFrameCount++;
	if (gDebugFont == nullptr)
		gDebugFont = new SysFont(gSexyAppBase, "Tahoma", 10);
	if (gFPSImage == NULL)
	{
		gFPSImage = gSexyAppBase->mRenderer->NewGPUImage();
		gFPSImage->Create(50, gDebugFont->GetHeight() + 4);
		gFPSImage->SetImageMode(false, false);
		gFPSImage->SetVolatile(true);
		gFPSImage->mPurgeBits = false;
		gFPSImage->mWantSurface = true;
		gFPSImage->PurgeBits();
	}

	if (gFPSTimer.GetDuration() >= 1000 || gForceDisplay)
	{
		gFPSTimer.Stop();
		int aPrevFPS = gFPSDisplay;
		if (!gForceDisplay)
			gFPSDisplay = (int)(gFrameCount * 1000 / gFPSTimer.GetDuration() + 0.5f);
		else
		{
			gForceDisplay = false;
			gFPSDisplay = 0;
		}

		gFPSTimer.Start();
		gFrameCount = 0;

		Graphics aDrawG(gFPSImage);
		aDrawG.SetFont(gDebugFont);
		SexyString aFPS = StrFormat("FPS: %d", gFPSDisplay);
		aDrawG.SetColor(0x000000);
		aDrawG.FillRect(0, 0, gFPSImage->GetWidth(), gFPSImage->GetHeight());
		aDrawG.SetColor(0xFFFFFF);
		aDrawG.SetColor(aPrevFPS <= gFPSDisplay ? Color(0, 255, 0) : Color(255, 0, 0));
		aDrawG.FillRect(gFPSImage->GetWidth() - 2, 0, 2, gFPSImage->GetHeight());
		aDrawG.SetColor(0xFFFFFF);
		aDrawG.DrawString(aFPS, 2, gDebugFont->GetAscent());
		gFPSImage->mBitsChangedCount++;
	}
}

///////////////////////////// FPS Stuff to draw mouse coords
static void FPSDrawCoords(int theX, int theY)
{
	if (gDebugFont == nullptr)
		gDebugFont = new SysFont(gSexyAppBase, "Tahoma", 8);
	if (gFPSImage == NULL)
	{
		gFPSImage = gSexyAppBase->mRenderer->NewGPUImage();
		gFPSImage->Create(50, gDebugFont->GetHeight() + 4);
		gFPSImage->SetImageMode(false, false);
		gFPSImage->SetVolatile(true);
		gFPSImage->mPurgeBits = false;
		gFPSImage->mWantSurface = true;
		gFPSImage->PurgeBits();
	}

	Graphics aDrawG(gFPSImage);
	aDrawG.SetFont(gDebugFont);
	SexyString aFPS = StrFormat("%d,%d", theX, theY);
	aDrawG.SetColor(0x000000);
	aDrawG.FillRect(0, 0, gFPSImage->GetWidth(), gFPSImage->GetHeight());
	aDrawG.SetColor(0xFFFFFF);
	aDrawG.DrawString(aFPS, 2, gDebugFont->GetAscent());
	gFPSImage->mBitsChangedCount++;
}

///////////////////////////// Demo TimeLeft Stuff
static GPUImage *gDemoTimeLeftImage = NULL;
static void CalculateDemoTimeLeft()
{
	if (gDebugFont == nullptr)
		gDebugFont = new SysFont(gSexyAppBase, "Tahoma", 8);
	static uint32_t aLastTick = 0;

	if (gDemoTimeLeftImage == NULL)
	{
		gDemoTimeLeftImage = gSexyAppBase->mRenderer->NewGPUImage();
		gDemoTimeLeftImage->Create(50, gDebugFont->GetHeight() + 4);
		gDemoTimeLeftImage->SetImageMode(false, false);
		gDemoTimeLeftImage->SetVolatile(true);
		gDemoTimeLeftImage->mPurgeBits = false;
		gDemoTimeLeftImage->mWantSurface = true;
		gDemoTimeLeftImage->PurgeBits();
	}

	uint32_t aTick = SDL_GetTicks();
	if (aTick - aLastTick < 1000 / gSexyAppBase->mUpdateMultiplier)
		return;

	aLastTick = aTick;

	int aNumUpdatesLeft = gSexyAppBase->mDemoLength - gSexyAppBase->mUpdateCount;
	Graphics aDrawG(gDemoTimeLeftImage);
	aDrawG.SetFont(gDebugFont);

	int aTotalSeconds = aNumUpdatesLeft * gSexyAppBase->mFrameTime / 1000;
	int aSeconds = aTotalSeconds % 60;
	int aMinutes = (aTotalSeconds / 60) % 60;
	int anHours = (aTotalSeconds / 3600);

	SexyString aFPS = StrFormat("%02d:%02d:%02d", anHours, aMinutes, aSeconds);
	aDrawG.SetColor(0x000000);
	aDrawG.FillRect(0, 0, gDemoTimeLeftImage->GetWidth(), gDemoTimeLeftImage->GetHeight());
	aDrawG.SetColor(0xFFFFFF);
	aDrawG.DrawString(aFPS, 2, gDebugFont->GetAscent());
	gDemoTimeLeftImage->mBitsChangedCount++;
}

static void UpdateScreenSaverInfo(uint32_t theTick)
{
	if (gSexyAppBase->IsScreenSaver() || !gSexyAppBase->mIsPhysWindowed)
		return;
	/*
	// Get screen saver timeout
	static uint32_t aPeriodicTick = 0;
	static uint32_t aScreenSaverTimeout = 60000;
	static bool aScreenSaverEnabled = true;

	if (theTick - aPeriodicTick > 10000)
	{
		aPeriodicTick = theTick;

		int aTimeout = 0;

		SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &aTimeout, 0);
		SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &aScreenSaverEnabled, 0);
		aTimeout -= 2;

		if (aTimeout < 1)
			aTimeout = 1;

		aScreenSaverTimeout = aTimeout * 1000;

		if (!aScreenSaverEnabled)
			gScreenSaverActive = false;
	}

	// Get more accurate last user input time
	if (gGetLastInputInfoFunc)
	{
		LASTINPUTINFO anInfo;
		anInfo.cbSize = sizeof(anInfo);
		if (gGetLastInputInfoFunc(&anInfo))
		{
			if (anInfo.dwTime > theTick)
				anInfo.dwTime = theTick;

			gSexyAppBase->mLastUserInputTick = anInfo.dwTime;
		}
	}

	if (!aScreenSaverEnabled)
		return;

	uint32_t anIdleTime = theTick - gSexyAppBase->mLastUserInputTick;
	if (gScreenSaverActive)
	{
		BOOL aBool = FALSE;
		if (SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &aBool, 0))
		{
			if (aBool) // screen saver not off yet
				return;
		}

		if (anIdleTime < aScreenSaverTimeout)
		{
			gScreenSaverActive = false;
			gSexyAppBase->mWidgetManager->MarkAllDirty();
		}
	}
	else if (anIdleTime > aScreenSaverTimeout)
		gScreenSaverActive = true;*/
}

bool SexyAppBase::DrawDirtyStuff()
{
	SEXY_AUTO_PERF("SexyAppBase::DrawDirtyStuff");
	MTAutoDisallowRand aDisallowRand;

	if (gIsFailing) // just try to reinit
	{
		Redraw(NULL);
		mHasPendingDraw = false;
		mLastDrawWasEmpty = true;
		return false;
	}

	if (mShowFPS)
	{
		switch (mShowFPSMode)
		{
		case FPS_ShowFPS:
			CalculateFPS();
			break;
		case FPS_ShowCoords:
			if (mWidgetManager != NULL)
				FPSDrawCoords(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY);
			break;
		}

		if (mPlayingDemoBuffer)
			CalculateDemoTimeLeft();
	}

	uint32_t aStartTime = timeGetTime();

	// Update user input and screen saver info
	static uint32_t aPeriodicTick = 0;
	if (aStartTime - aPeriodicTick > 1000)
	{
		aPeriodicTick = aStartTime;
		UpdateScreenSaverInfo(aStartTime);
	}

	if (gScreenSaverActive)
	{
		mHasPendingDraw = false;
		mLastDrawWasEmpty = true;
		return false;
	}

	mIsDrawing = true;
	bool drewScreen = mWidgetManager->DrawScreen();
	mIsDrawing = false;

#if SEXY_USE_CONTROLLER
	if (mUsingGamepad)
	{
		SDL_HideCursor();
	}
#endif

	if ((drewScreen || (aStartTime - mLastDrawTick >= 1000)) &&
		((int)(aStartTime - mNextDrawTick) >= 0))
	{
		mLastDrawWasEmpty = false;

		mDrawCount++;

		uint32_t aMidTime = timeGetTime();

		mFPSCount++;
		mFPSTime += aMidTime - aStartTime;

		mDrawTime += aMidTime - aStartTime;

		if (mShowFPS)
		{
			Graphics g(mRenderer->GetScreenImage());
			g.DrawImage(gFPSImage, mWidth - gFPSImage->GetWidth() - 10, mHeight - gFPSImage->GetHeight() - 10);

			if (mPlayingDemoBuffer)
				g.DrawImage(gDemoTimeLeftImage,
							mWidth - gDemoTimeLeftImage->GetWidth() - 10,
							mHeight - gFPSImage->GetHeight() - gDemoTimeLeftImage->GetHeight() - 15);
		}

		uint32_t aPreScreenBltTime = timeGetTime();
		mLastDrawTick = aPreScreenBltTime;

		Redraw(NULL);

		// This is our one UpdateFTimeAcc if we are vsynched
		UpdateFTimeAcc();

		uint32_t aEndTime = timeGetTime();

		mScreenBltTime = aEndTime - aPreScreenBltTime;

#ifdef _DEBUG
		/*if (mFPSTime >= 5000) // Show FPS about every 5 seconds
		{
			uint32_t aTickNow = SDL_GetTicks();

			OutputDebugString(StrFormat("Theoretical FPS: %d\r\n", (int) (mFPSCount * 1000 / mFPSTime)).c_str());
			OutputDebugString(StrFormat("Actual      FPS: %d\r\n", (mFPSFlipCount * 1000) / max((aTickNow - mFPSStartTick), 1)).c_str());
			OutputDebugString(StrFormat("Dirty Rate     : %d\r\n", (mFPSDirtyCount * 1000) / max((aTickNow - mFPSStartTick), 1)).c_str());

			mFPSTime = 0;
			mFPSCount = 0;
			mFPSFlipCount = 0;
			mFPSStartTick = aTickNow;
			mFPSDirtyCount = 0;
		}*/
#endif

		if ((mLoadingThreadStarted) && (!mLoadingThreadCompleted))
		{
			int aTotalTime = aEndTime - aStartTime;

			mNextDrawTick += 35 + std::max(aTotalTime, 15);

			if ((int)(aEndTime - mNextDrawTick) >= 0)
				mNextDrawTick = aEndTime;

			/*char aStr[256];
			sprintf(aStr, "Next Draw Time: %d\r\n", mNextDrawTick);
			OutputDebugString(aStr);*/
		}
		else
			mNextDrawTick = aEndTime;

		mHasPendingDraw = false;

		return true;
	}
	else
	{
		mHasPendingDraw = false;
		mLastDrawWasEmpty = true;
		return false;
	}
}

void SexyAppBase::LogScreenSaverError(const std::string &theError)
{
	static bool firstTime = true;
	char aBuf[512];

	const char *aFlag = firstTime ? "w" : "a+";
	firstTime = false;

	FILE *aFile = fopen("ScrError.txt", aFlag);
	if (aFile != NULL)
	{
		fprintf(aFile, "%s %s %u\n", theError.c_str(), _strtime(aBuf), SDL_GetTicks());
		fclose(aFile);
	}
}

void SexyAppBase::BeginPopup()
{

}

void SexyAppBase::EndPopup()
{
	if (!mIsPhysWindowed)
		mNoDefer = false;

	ClearUpdateBacklog();
	ClearKeysDown();

	if (mWidgetManager->mDownButtons)
	{
		mWidgetManager->DoMouseUps();
		ReleaseCapture();
	}
}

int SexyAppBase::MsgBox(const std::string &theText, const std::string &theTitle, int theFlags)
{
	if (IsScreenSaver())
	{
		LogScreenSaverError(theText);
		return 0;
	}//todo flags for bttns

	BeginPopup();

	std::vector<SDL_MessageBoxButtonData> aButtonVec;

	uint32_t aConvertedFlags = 0;
	if (theFlags & SexyMessageBoxFlags::MESSAGEBOX_ERROR)
		aConvertedFlags |= SDL_MESSAGEBOX_ERROR;                    
	else if (theFlags & SexyMessageBoxFlags::MESSAGEBOX_WARNING)
		aConvertedFlags |= SDL_MESSAGEBOX_WARNING;                    
	else
		aConvertedFlags |= MESSAGEBOX_INFORMATION;      

	if (theFlags & SexyMessageBoxFlags::MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT) 
		aConvertedFlags |= SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT;  
	else
		aConvertedFlags |= SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;      

	if (theFlags & SexyMessageBoxFlags::MESSAGEBOX_BTN_OK)
		aButtonVec.push_back({SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Ok"});
	if (theFlags & SexyMessageBoxFlags::MESSAGEBOX_BTN_CANCEL) 
		aButtonVec.push_back({SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Cancel"});
	if (theFlags & SexyMessageBoxFlags::MESSAGEBOX_BTN_YES) 
		aButtonVec.push_back({SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Yes"});
	if (theFlags & SexyMessageBoxFlags::MESSAGEBOX_BTN_NO) 
		aButtonVec.push_back({SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No"});


	SDL_MessageBoxData msgBoxData;
	msgBoxData.window = nullptr;
	msgBoxData.flags = aConvertedFlags;
	msgBoxData.title = theTitle.c_str();
	msgBoxData.message = theText.c_str();
	msgBoxData.buttons = aButtonVec.data();
	msgBoxData.numbuttons = aButtonVec.size();
	int aResult;
	SDL_ShowMessageBox(&msgBoxData, &aResult);

	EndPopup();

	return aResult;
}

void SexyAppBase::Popup(const std::string &theString)
{
	if (IsScreenSaver())
	{
		LogScreenSaverError(theString);
		return;
	}

	SDL_Window* aInternalWindow = nullptr;
	if (mWindow != nullptr)
		aInternalWindow = mWindow->mInternalWindow;

	BeginPopup();
	if (!mShutdown)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
								 GetString("FATAL_ERROR", "FATAL ERROR").c_str(),
								 theString.c_str(), aInternalWindow);

	EndPopup();
}

void SexyAppBase::SafeDeleteWidget(Widget *theWidget)
{
	WidgetSafeDeleteInfo aWidgetSafeDeleteInfo;
	aWidgetSafeDeleteInfo.mUpdateAppDepth = mUpdateAppDepth;
	aWidgetSafeDeleteInfo.mWidget = theWidget;
	mSafeDeleteList.push_back(aWidgetSafeDeleteInfo);
}

static intptr_t CALLBACK MarkerListDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG: {
		HWND aListBox = GetDlgItem(hwnd, 100);

		DWORD dwExtent = 0;
		HDC hDCListBox;
		HFONT hFontOld, hFontNew;
		TEXTMETRIC tm;
		RECT aRect;
		SIZE aSize;

		hDCListBox = GetDC(aListBox);
		hFontNew = (HFONT)SendMessage(aListBox, WM_GETFONT, NULL, NULL);
		hFontOld = (HFONT)SelectObject(hDCListBox, hFontNew);
		GetTextMetrics(hDCListBox, (LPTEXTMETRIC)&tm);
		GetClientRect(hwnd, &aRect);
		MoveWindow(aListBox, 10, 10, aRect.right - aRect.left - 20, aRect.bottom - aRect.top - 20, FALSE);
		for (SexyAppBase::DemoMarkerList::iterator anItr = gSexyAppBase->mDemoMarkerList.begin();
			 anItr != gSexyAppBase->mDemoMarkerList.end();
			 ++anItr)
		{
			if (anItr->second <= gSexyAppBase->mUpdateCount)
				continue;

			int aTotalSeconds = (gSexyAppBase->mDemoLength - anItr->second) * gSexyAppBase->mFrameTime / 1000;
			int aSeconds = aTotalSeconds % 60;
			int aMinutes = (aTotalSeconds / 60) % 60;
			int anHours = (aTotalSeconds / 3600);

			SexyString aStr = StrFormat("%s (%02d:%02d:%02d)", anItr->first.c_str(), anHours, aMinutes, aSeconds);
			GetTextExtentPoint32(hDCListBox, aStr.c_str(), aStr.length(), &aSize);
			dwExtent = aSize.cx + tm.tmAveCharWidth > (int)dwExtent ? aSize.cx + tm.tmAveCharWidth : (int)dwExtent;
			
			SendMessage(aListBox, LB_SETHORIZONTALEXTENT, dwExtent, 0);
			LRESULT anIndex = SendMessage(aListBox, LB_ADDSTRING, 0, (LPARAM)aStr.c_str());
			SendMessage(aListBox, LB_SETITEMDATA, anIndex, anItr->second);
		}

		SelectObject(hDCListBox, hFontOld);
		ReleaseDC(aListBox, hDCListBox);

		return TRUE;
	}

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;

	case WM_COMMAND:
		if (HIWORD(wParam) == LBN_DBLCLK)
		{
			HWND aListBox = GetDlgItem(hwnd, 100);

			int anIndex = SendMessage(aListBox, LB_GETCURSEL, 0, 0);
			if (anIndex >= 0)
			{
				int anUpdateTime = SendMessage(aListBox, LB_GETITEMDATA, anIndex, 0);
				if (anUpdateTime > gSexyAppBase->mUpdateCount)
				{
					gSexyAppBase->mFastForwardToUpdateNum = anUpdateTime;
					EndDialog(hwnd, 0);
				}
			}
			return TRUE;
		}
		break;
	}

	return FALSE;
}

static uint16_t *lpdwAlign(uint16_t *lpIn)
{
	/*
	uint32_t ul;

	ul = (uint32_t)lpIn;
	ul += 3;
	ul >>= 2;
	ul <<= 2;
	return (LPWORD)ul;*/
	return 0;
}

static int ListDemoMarkers()
{
	/*
	HGLOBAL hgbl;
	LPDLGTEMPLATE lpdt;
	LPDLGITEMTEMPLATE lpdit;
	LPWORD lpw;
	LPWSTR lpwsz;
	LRESULT ret;
	int nchar;

	hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
	if (!hgbl)
		return -1;

	lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

	// Define a dialog box.
	lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION | DS_SETFONT;
	lpdt->cdit = 1; // number of controls
	lpdt->x = 10;
	lpdt->y = 10;
	lpdt->cx = 200;
	lpdt->cy = 200;

	lpw = (LPWORD)(lpdt + 1);
	*lpw++ = 0; // no menu
	*lpw++ = 0; // predefined dialog box class (by default)

	lpwsz = (LPWSTR)lpw;
	nchar = MultiByteToWideChar(CP_ACP, 0, "Marker List", -1, lpwsz, 50);
	lpw += nchar;
	*lpw++ = 8;
	lpwsz = (LPWSTR)lpw;
	nchar = MultiByteToWideChar(CP_ACP, 0, "Tahoma", -1, lpwsz, 50);
	lpw += nchar;

	// Define Listbox
	lpw = lpdwAlign(lpw); // align DLGITEMTEMPLATE on DWORD boundary
	lpdit = (LPDLGITEMTEMPLATE)lpw;
	lpdit->x = 5;
	lpdit->y = 5;
	lpdit->cx = 190;
	lpdit->cy = 195;
	lpdit->id = 100;
	lpdit->style = WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY;
	lpdit->dwExtendedStyle = WS_EX_CLIENTEDGE;
	lpw = (LPWORD)(lpdit + 1);
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0083; // listbox class
	*lpw++ = 0;		 // no window text
	*lpw++ = 0;		 // no creation data

	GlobalUnlock(hgbl);
	ret = DialogBoxIndirect(gHInstance, (LPDLGTEMPLATE)hgbl, gSexyAppBase->mHWnd, (DLGPROC)MarkerListDialogProc);
	GlobalFree(hgbl);

	gSexyAppBase->mLastTime = timeGetTime();

	return ret;*/
	return 0;
}

static intptr_t CALLBACK JumpToTimeDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*
	switch (msg)
	{
	case WM_INITDIALOG: {
		HWND anEdit = GetDlgItem(hwnd, 100);
		HKEY aGameKey;
		std::string aKeyName = RemoveTrailingSlash("SOFTWARE\\" + gSexyAppBase->mRegKey);
		if (RegOpenKeyExA(HKEY_CURRENT_USER, aKeyName.c_str(), 0, KEY_READ | KEY_WRITE, &aGameKey) == ERROR_SUCCESS)
		{
			char aBuf[1024];
			DWORD aLength = 1000;
			DWORD aType = REG_SZ;
			if (RegQueryValueExA(aGameKey, "DemoJumpTime", 0, &aType, (uint8_t *)aBuf, &aLength) == ERROR_SUCCESS)
			{
				aBuf[aLength] = 0;
				SetWindowTextA(anEdit, aBuf);
				SendMessage(anEdit, EM_SETSEL, 0, -1);
			}
			RegCloseKey(aGameKey);
		}
		return TRUE;
	}
	break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;

	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			if (LOWORD(wParam) == IDOK)
			{
				char aBuf[512];
				HWND anEdit = GetDlgItem(hwnd, 100);
				GetWindowTextA(anEdit, aBuf, 500);

				HKEY aGameKey;
				std::string aKeyName = RemoveTrailingSlash("SOFTWARE\\" + gSexyAppBase->mRegKey);
				if (RegOpenKeyExA(HKEY_CURRENT_USER, aKeyName.c_str(), 0, KEY_READ | KEY_WRITE, &aGameKey) ==
					ERROR_SUCCESS)
				{
					RegSetValueExA(aGameKey, "DemoJumpTime", 0, REG_SZ, (const uint8_t *)aBuf, strlen(aBuf) + 1);
					RegCloseKey(aGameKey);
				}

				int aTime = 0;
				char *aPtr = strtok(aBuf, ":");
				while (aPtr != NULL)
				{
					aTime *= 60;
					aTime += atoi(aPtr);
					aPtr = strtok(NULL, ":");
				}
				aTime++;

				int aNumFrames = aTime * 1000 / gSexyAppBase->mFrameTime;
				gSexyAppBase->mFastForwardToUpdateNum = gSexyAppBase->mDemoLength - aNumFrames;
			}

			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;*/
	return false; // ????
}

static int DemoJumpToTime()
{
	/*
	HGLOBAL hgbl;
	LPDLGTEMPLATE lpdt;
	LPDLGITEMTEMPLATE lpdit;
	LPWORD lpw;
	LPWSTR lpwsz;
	LRESULT ret;
	int nchar;

	hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
	if (!hgbl)
		return -1;

	lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

	// Define a dialog box.
	lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION | DS_SETFONT;
	lpdt->cdit = 3; // number of controls
	lpdt->x = 10;
	lpdt->y = 10;
	lpdt->cx = 200;
	lpdt->cy = 50;

	lpw = (LPWORD)(lpdt + 1);
	*lpw++ = 0; // no menu
	*lpw++ = 0; // predefined dialog box class (by default)

	lpwsz = (LPWSTR)lpw;
	nchar = MultiByteToWideChar(CP_ACP, 0, "Jump To Time", -1, lpwsz, 50);
	lpw += nchar;
	*lpw++ = 8;
	lpwsz = (LPWSTR)lpw;
	nchar = MultiByteToWideChar(CP_ACP, 0, "Tahoma", -1, lpwsz, 50);
	lpw += nchar;

	// Define Edit
	lpw = lpdwAlign(lpw); // align DLGITEMTEMPLATE on DWORD boundary
	lpdit = (LPDLGITEMTEMPLATE)lpw;
	lpdit->x = 5;
	lpdit->y = 5;
	lpdit->cx = 190;
	lpdit->cy = 15;
	lpdit->id = 100;
	lpdit->style = WS_VISIBLE | WS_CHILD;
	lpdit->dwExtendedStyle = WS_EX_CLIENTEDGE;
	lpw = (LPWORD)(lpdit + 1);
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0081; // edit class
	*lpw++ = 0;		 // no window text
	*lpw++ = 0;		 // no creation data

	// Define Button
	lpw = lpdwAlign(lpw); // align DLGITEMTEMPLATE on DWORD boundary
	lpdit = (LPDLGITEMTEMPLATE)lpw;
	lpdit->x = 30;
	lpdit->y = 25;
	lpdit->cx = 60;
	lpdit->cy = 15;
	lpdit->id = IDOK;
	lpdit->style = WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON;
	//	lpdit->dwExtendedStyle = WS_EX_CLIENTEDGE;
	lpw = (LPWORD)(lpdit + 1);
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0080; // button class
	lpwsz = (LPWSTR)lpw;
	nchar = MultiByteToWideChar(CP_ACP, 0, "Ok", -1, lpwsz, 50);
	lpw += nchar;
	lpw = lpdwAlign(lpw); // align creation data on DWORD boundary
	*lpw++ = 0;			  // no creation data

	// Define Button
	lpw = lpdwAlign(lpw); // align DLGITEMTEMPLATE on DWORD boundary
	lpdit = (LPDLGITEMTEMPLATE)lpw;
	lpdit->x = 100;
	lpdit->y = 25;
	lpdit->cx = 60;
	lpdit->cy = 15;
	lpdit->id = IDCANCEL;
	lpdit->style = WS_VISIBLE | WS_CHILD;
	//	lpdit->dwExtendedStyle = WS_EX_CLIENTEDGE;
	lpw = (LPWORD)(lpdit + 1);
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0080; // button class
	lpwsz = (LPWSTR)lpw;
	nchar = MultiByteToWideChar(CP_ACP, 0, "Cancel", -1, lpwsz, 50);
	lpw += nchar;
	lpw = lpdwAlign(lpw); // align creation data on DWORD boundary
	*lpw++ = 0;			  // no creation data

	GlobalUnlock(hgbl);
	ret = DialogBoxIndirect(gHInstance, (LPDLGTEMPLATE)hgbl, gSexyAppBase->mHWnd, (DLGPROC)JumpToTimeDialogProc);
	GlobalFree(hgbl);

	gSexyAppBase->mLastTime = timeGetTime();

	return ret;*/
	return 0;
}

static void ToggleDemoSoundVolume()
{
	if (gSexyAppBase->GetMusicVolume() == 0.0)
		gSexyAppBase->SetMusicVolume(gSexyAppBase->mDemoMusicVolume);
	else
	{
		gSexyAppBase->mDemoMusicVolume = gSexyAppBase->mMusicVolume;
		gSexyAppBase->SetMusicVolume(0.0);
	}

	if (gSexyAppBase->GetSfxVolume() == 0.0)
		gSexyAppBase->SetSfxVolume(gSexyAppBase->mDemoSfxVolume);
	else
	{
		gSexyAppBase->mDemoSfxVolume = gSexyAppBase->mSfxVolume;
		gSexyAppBase->SetSfxVolume(0.0);
	}
}

static uint32_t gPowerSaveTick = 0;

void SexyAppBase::HandleNotifyGameMessage(int theType, int theParam)
{
	
}

void SexyAppBase::RehupFocus()
{
	bool wantHasFocus = mActive && !mMinimized;

	if (wantHasFocus != mHasFocus)
	{
		mHasFocus = wantHasFocus;

		if (mHasFocus)
		{
			if (mMuteOnLostFocus)
				Unmute(true);

			mWidgetManager->GotFocus();
			GotFocus();
		}
		else
		{
			if (mMuteOnLostFocus)
				Mute(true);

			mWidgetManager->LostFocus();
			LostFocus();

			ReleaseCapture();
			mWidgetManager->DoMouseUps();
		}
	}
}

void SexyAppBase::ClearKeysDown()
{
	if (mWidgetManager != NULL) // fix stuck alt-key problem
	{
		for (int aKeyNum = 0; aKeyNum < 0xFF; aKeyNum++)
			mWidgetManager->mKeyDown[aKeyNum] = false;
	}
	mCtrlDown = false;
	mAltDown = false;
}

void SexyAppBase::WriteDemoTimingBlock()
{
	// Demo writing functions can only be called from the main thread and after SexyAppBase::Init
	DBG_ASSERTE(std::this_thread::get_id() == mPrimaryThreadId);

	while (mUpdateCount - mLastDemoUpdateCnt > 15)
	{
		mDemoBuffer.WriteNumBits(15, 4);
		mLastDemoUpdateCnt += 15;

		mDemoBuffer.WriteNumBits(0, 1);
		mDemoBuffer.WriteNumBits(DEMO_IDLE, 5);
		mDemoCmdOrder++;
	}

	mDemoBuffer.WriteNumBits(mUpdateCount - mLastDemoUpdateCnt, 4);
	mLastDemoUpdateCnt = mUpdateCount;
	mDemoCmdOrder++;
}

int aNumBigMoveMessages = 0;
int aNumSmallMoveMessages = 0;
int aNumTimerMessages = 0;

bool SexyAppBase::PrepareDemoCommand(bool required)
{
	if (mDemoNeedsCommand)
	{
		mDemoCmdBitPos = mDemoBuffer.mReadBitPos;

		mLastDemoUpdateCnt += mDemoBuffer.ReadNumBits(4, false);

		mDemoIsShortCmd = mDemoBuffer.ReadNumBits(1, false) == 1;

		if (mDemoIsShortCmd)
			mDemoCmdNum = mDemoBuffer.ReadNumBits(1, false);
		else
			mDemoCmdNum = mDemoBuffer.ReadNumBits(5, false);

		mDemoNeedsCommand = false;

		mDemoCmdOrder++;
	}

	DBG_ASSERTE((mUpdateCount == mLastDemoUpdateCnt) || (!required));

	return mUpdateCount == mLastDemoUpdateCnt;
}

void SexyAppBase::ProcessDemo()
{
	if (mPlayingDemoBuffer)
	{
		// At end of demo buffer?  How dare you!
		DBG_ASSERTE(!mDemoBuffer.AtEnd());

		while ((!mShutdown) && (mUpdateCount == mLastDemoUpdateCnt) && (!mDemoBuffer.AtEnd()))
		{
			if (PrepareDemoCommand(false))
			{
				mDemoNeedsCommand = true;

				if (mDemoIsShortCmd)
				{
					switch (mDemoCmdNum)
					{
					case 0: {
						int aDeltaX = mDemoBuffer.ReadNumBits(6, true);
						int aDeltaY = mDemoBuffer.ReadNumBits(6, true);
						mLastDemoMouseX += aDeltaX;
						mLastDemoMouseY += aDeltaY;

						mWidgetManager->MouseMove(mLastDemoMouseX, mLastDemoMouseY);
					}
					break;
					case 1: {
						bool down = mDemoBuffer.ReadNumBits(1, false) != 0;
						int aBtnCount = mDemoBuffer.ReadNumBits(3, true);

						if (down)
							mWidgetManager->MouseDown(mLastDemoMouseX, mLastDemoMouseY, aBtnCount);
						else
							mWidgetManager->MouseUp(mLastDemoMouseX, mLastDemoMouseY, aBtnCount);
					}
					break;
					}
				}
				else
				{
					switch (mDemoCmdNum)
					{
					case DEMO_MOUSE_POSITION: {
						mLastDemoMouseX = mDemoBuffer.ReadNumBits(12, false);
						mLastDemoMouseY = mDemoBuffer.ReadNumBits(12, false);

						mWidgetManager->MouseMove(mLastDemoMouseX, mLastDemoMouseY);
					}
					break;
					case DEMO_ACTIVATE_APP: {
						mActive = mDemoBuffer.ReadNumBits(1, false) != 0;

						RehupFocus();

						if ((mActive) && (!mIsWindowed))
							mWidgetManager->MarkAllDirty();

						if ((mIsOpeningURL) && (!mActive))
							URLOpenSucceeded(mOpeningURL);
					}
					break;
					case DEMO_SIZE: {
						bool isMinimized = mDemoBuffer.ReadBoolean();

						if ((!mShutdown) && (isMinimized != mMinimized))
						{
							mMinimized = isMinimized;

							// We don't want any sounds (or music) playing while its minimized
							if (mMinimized)
								Mute(true);
							else
							{
								Unmute(true);
								mWidgetManager->MarkAllDirty();
							}
						}

						RehupFocus();
					}
					break;
					case DEMO_MOUSE_WHEEL: {
						int aScroll = mDemoBuffer.ReadNumBits(8, true);
						mWidgetManager->MouseWheel(aScroll);
					}
					break;
					case DEMO_KEY_DOWN: {
						KeyCode aKeyCode = (KeyCode)mDemoBuffer.ReadNumBits(8, false);
						mWidgetManager->KeyDown(aKeyCode);
					}
					break;
					case DEMO_KEY_UP: {
						KeyCode aKeyCode = (KeyCode)mDemoBuffer.ReadNumBits(8, false);
						mWidgetManager->KeyUp(aKeyCode);
					}
					break;
					case DEMO_KEY_CHAR: {
						int sizeMult = (int)mDemoBuffer.ReadNumBits(1, false) + 1; // will be 1 for single, 2 for double
						SexyChar aChar = (SexyChar)mDemoBuffer.ReadNumBits(8 * sizeMult, false);
						mWidgetManager->KeyChar(aChar);
					}
					break;
					case DEMO_CLOSE:
						Shutdown();
						break;
					case DEMO_MOUSE_ENTER:
						mMouseIn = true;
						EnforceCursor();
						break;
					case DEMO_MOUSE_EXIT:
						mWidgetManager->MouseExit(mLastDemoMouseX, mLastDemoMouseY);
						mMouseIn = false;
						EnforceCursor();
						break;
					case DEMO_LOADING_COMPLETE:
						mDemoLoadingComplete = true;
						break;
					case DEMO_VIDEO_DATA:
						mIsWindowed = mDemoBuffer.ReadBoolean();
						mSyncRefreshRate = mDemoBuffer.ReadByte();
						break;
					case DEMO_IDLE:
						break;
					default:
						DBG_ASSERTE("Invalid Demo Command" == 0);
						break;
					}
				}
			}
		}
	}
}

void SexyAppBase::ShowMemoryUsage()
{
	uint32_t aNumTextures = 0;
	uint32_t aTotal = 0;
	uint32_t aFree = 0;
	uint32_t aUsed = 0;

	if (mRenderer != nullptr)
	{
		RenderingInfo anInfo = mRenderer->GetRenderingInfo();
		aNumTextures = anInfo.mNumTextures;
		aFree = anInfo.mFreeVideoMem;
		aTotal = anInfo.mTotalVideoMem;
		aUsed = anInfo.mUsedVideoMemory;
	}

	MemoryImageSet::iterator anItr = mMemoryImageSet.begin();
	typedef std::pair<int, int> FormatUsage;
	typedef std::map<PixelFormat, FormatUsage> FormatMap;
	FormatMap aFormatMap;
	int aTextureMemory = 0;
	while (anItr != mMemoryImageSet.end())
	{
		MemoryImage *aMemoryImage = *anItr;
		if (aMemoryImage->mGPUData != NULL)
		{
			TextureData *aData = (TextureData *)aMemoryImage->mGPUData;
			aTextureMemory += aData->mTexMemSize;

			FormatUsage &aUsage = aFormatMap[aData->mPixelFormat];
			aUsage.first++;
			aUsage.second += aData->mTexMemSize;
		}

		++anItr;
	}

	std::string aStr;

	const char *aDesc;
	if (Is3DAccelerationRecommended())
		aDesc = "Recommended";
	else if (Is3DAccelerationSupported())
		aDesc = "Supported";
	else
		aDesc = "Unsupported";
	
	aStr += StrFormat("Current Rendering Backend: %s\n", gRenderBackends.find(mRenderer->mCurrentBackend)->second.c_str());
	aStr += StrFormat("3D-Mode is %s (3D is %s on this system)\n\n", Is3DAccelerated() ? "On" : "Off", aDesc);

	aStr += StrFormat("Num Images: %d\n", (int)mMemoryImageSet.size());
	aStr += StrFormat("Num Sounds: %d\n", mSoundManager->GetNumSounds());
	aStr += "Video Memory:\n";

	aStr += StrFormat("Used: %s KB\n", aUsed != -1 ? CommaSeperate(aUsed / 1024).c_str() : "N/A");
	aStr += StrFormat("Free: %s KB\n", aFree != -1 ? CommaSeperate(aFree / 1024).c_str() : "N/A");
	aStr += StrFormat("Total: %s KB\n", aTotal != -1 ? CommaSeperate(aTotal / 1024).c_str() : "N/A");

	aStr += StrFormat("Texture Memory: %s KB\n", CommaSeperate(aTextureMemory / 1024).c_str());
	aStr += StrFormat("Num Textures: %d \n\n", aNumTextures);

	FormatUsage aUsage = aFormatMap[PixelFormat_A8R8G8B8];
	aStr += StrFormat(
		"A8R8G8B8: %d - %s KB\n", aUsage.first, SexyStringToString(CommaSeperate(aUsage.second / 1024)).c_str());
	aUsage = aFormatMap[PixelFormat_A4R4G4B4];
	aStr += StrFormat(
		"A4R4G4B4: %d - %s KB\n", aUsage.first, SexyStringToString(CommaSeperate(aUsage.second / 1024)).c_str());
	aUsage = aFormatMap[PixelFormat_R5G6B5];
	aStr += StrFormat(
		"R5G6B5: %d - %s KB\n", aUsage.first, SexyStringToString(CommaSeperate(aUsage.second / 1024)).c_str());
	aUsage = aFormatMap[PixelFormat_Palette8];
	aStr += StrFormat(
		"Palette8: %d - %s KB\n", aUsage.first, SexyStringToString(CommaSeperate(aUsage.second / 1024)).c_str());

	MsgBox(aStr, "Video Stats", MESSAGEBOX_BTN_OK);
	mLastTime = timeGetTime();
}

bool SexyAppBase::IsAltKeyUsed()
{
	SDL_Keymod mod = SDL_GetModState();
	return mod & SDL_KMOD_ALT;
}

bool SexyAppBase::DebugKeyDown(int theKey)
{
	if (theKey == KEYCODE_F3)
	{
		if (mWidgetManager->mKeyDown[KEYCODE_SHIFT])
		{
			mShowFPS = true;
			if (++mShowFPSMode >= Num_FPS_Types)
				mShowFPSMode = 0;
		}
		else
			mShowFPS = !mShowFPS;

		mWidgetManager->MarkAllDirty();

		if (mShowFPS)
		{
			gFPSTimer.Start();
			gFrameCount = 0;
			gFPSDisplay = 0;
			gForceDisplay = true;
		}
	}
	else if (theKey == KEYCODE_F8)
	{
		if (mWidgetManager->mKeyDown[KEYCODE_SHIFT])
		{
			Set3DAcclerated(!Is3DAccelerated());

			char aBuf[512];
			sprintf(aBuf, "3D-Mode: %s", Is3DAccelerated() ? "ON" : "OFF");
			MsgBox(aBuf, "Mode Switch", MESSAGEBOX_BTN_OK);
			mLastTime = timeGetTime();
		}
		else
			ShowMemoryUsage();

		return true;
	}
	else if (theKey == KEYCODE_F10)
	{
#ifndef RELEASEFINAL
		if (mWidgetManager->mKeyDown[KEYCODE_CONTROL])
		{
			if (mUpdateMultiplier == 0.25)
				mUpdateMultiplier = 1.0;
			else
				mUpdateMultiplier = 0.25;
		}
		else if (mWidgetManager->mKeyDown[KEYCODE_SHIFT])
		{
			mStepMode = 0;
			ClearUpdateBacklog();
		}
		else
			mStepMode = 1;
#endif

		return true;
	}
	else if (theKey == KEYCODE_F11)
	{
		if (mWidgetManager->mKeyDown[KEYCODE_SHIFT])
			DumpProgramInfo();
		else
			TakeScreenshot();

		return true;
	}
	else if (theKey == KEYCODE_F2)
	{
		bool isPerfOn = !SexyPerf::IsPerfOn();
		if (isPerfOn)
		{
			//			MsgBox("Perf Monitoring: ON", "Perf Monitoring", MB_OK);
			ClearUpdateBacklog();
			SexyPerf::BeginPerf();
		}
		else
		{
			SexyPerf::EndPerf();
			MsgBox(SexyPerf::GetResults().c_str(), "Perf Results", MESSAGEBOX_BTN_OK);
			ClearUpdateBacklog();
		}
	}
	else
		return false;

	return false;
}

bool SexyAppBase::DebugKeyDownAsync(int theKey, bool ctrlDown, bool altDown)
{
	return false;
}

void SexyAppBase::CloseRequestAsync()
{
}

bool SexyAppBase::ProcessDeferredMessages(bool singleMessage)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
#if SEXY_USE_IMGUI
		ImGui_ImplSDL3_ProcessEvent(&event);
		if (ImGui::GetIO().WantCaptureMouse)
			break;
#endif
		if ((mRecordingDemoBuffer) && (!mShutdown))
		{
			switch (event.type)
			{
				case SDL_EVENT_WINDOW_FOCUS_LOST:
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
				{
					SDL_Window* theTargetWindow = SDL_GetWindowFromEvent(&event);
					if (theTargetWindow == mWindow->mInternalWindow)
					{
						WriteDemoTimingBlock();
						mDemoBuffer.WriteNumBits(0, 1);
						mDemoBuffer.WriteNumBits(DEMO_ACTIVATE_APP, 5);
						mDemoBuffer.WriteNumBits((event.type != SDL_EVENT_WINDOW_FOCUS_LOST) ? 1 : 0, 1);
					}
					break;
				}
				case SDL_EVENT_WINDOW_MAXIMIZED:
				case SDL_EVENT_WINDOW_MINIMIZED:
					WriteDemoTimingBlock();
					mDemoBuffer.WriteNumBits(0, 1);
					mDemoBuffer.WriteNumBits(DEMO_SIZE, 5);
					mDemoBuffer.WriteBoolean(event.type == SDL_EVENT_WINDOW_MINIMIZED);
					break;
				case SDL_EVENT_MOUSE_MOTION:
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				case SDL_EVENT_MOUSE_BUTTON_UP:
				{

					int aCurX = event.button.x;
					int aCurY = event.button.y;
					int aDiffX = aCurX - mLastDemoMouseX;
					int aDiffY = aCurY - mLastDemoMouseY;

					if ((abs(aCurX - mLastDemoMouseX) < 32) && (abs(aCurY - mLastDemoMouseY) < 32))
					{
						if ((aDiffX != 0) || (aDiffY != 0))
						{
							WriteDemoTimingBlock();
							mDemoBuffer.WriteNumBits(1, 1);
							mDemoBuffer.WriteNumBits(0, 1);
							mDemoBuffer.WriteNumBits(aDiffX, 6);
							mDemoBuffer.WriteNumBits(aDiffY, 6);
						}
					}
					else
					{
						WriteDemoTimingBlock();
						mDemoBuffer.WriteNumBits(0, 1);
						mDemoBuffer.WriteNumBits(DEMO_MOUSE_POSITION, 5);
						mDemoBuffer.WriteNumBits(aCurX, 12);
						mDemoBuffer.WriteNumBits(aCurY, 12);
					}

					bool down = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
					int aBtnNum = 0;
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
							aBtnNum = 1;
							break;
						case SDL_BUTTON_RIGHT:
							aBtnNum = -1;
							break;
						case SDL_BUTTON_MIDDLE:
							aBtnNum = 3;
							break;
					}


					if (aBtnNum != 0)
					{
						WriteDemoTimingBlock();
						mDemoBuffer.WriteNumBits(1, 1);
						mDemoBuffer.WriteNumBits(1, 1);
						mDemoBuffer.WriteNumBits(down ? 1 : 0, 1);
						mDemoBuffer.WriteNumBits(aBtnNum, 3);
					}

					mLastDemoMouseX = aCurX;
					mLastDemoMouseY = aCurY;
				}
				break;
				case SDL_EVENT_MOUSE_WHEEL:
				{				
					int aZDelta = event.wheel.y;

					WriteDemoTimingBlock();
					mDemoBuffer.WriteNumBits(0, 1);
					mDemoBuffer.WriteNumBits(DEMO_MOUSE_WHEEL, 5);
					mDemoBuffer.WriteNumBits(aZDelta, 8);
				}
				break;
				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP:
				{
					bool isDown = event.type == SDL_EVENT_KEY_DOWN;
					SDL_Keycode aKeyCode = event.key.key;

					WriteDemoTimingBlock();
					mDemoBuffer.WriteNumBits(0, 1);
					mDemoBuffer.WriteNumBits(isDown ? DEMO_KEY_DOWN : DEMO_KEY_UP, 5);
					mDemoBuffer.WriteNumBits((int)aKeyCode, 8);
				}
				break;
				case SDL_EVENT_TEXT_INPUT:
				{
					SexyChar aChar = event.text.text[0]; 

					WriteDemoTimingBlock();
					mDemoBuffer.WriteNumBits(0, 1);
					mDemoBuffer.WriteNumBits(DEMO_KEY_CHAR, 5);
					mDemoBuffer.WriteNumBits(sizeof(SexyChar) == 2, 1);
					mDemoBuffer.WriteNumBits(aChar, sizeof(SexyChar) * 8);
				}
				break;
				case SDL_EVENT_QUIT:
				{
					SDL_Window* theTargetWindow = SDL_GetWindowFromEvent(&event);
					if (theTargetWindow == mWindow->mInternalWindow)
					{
						WriteDemoTimingBlock();
						mDemoBuffer.WriteNumBits(0, 1);
						mDemoBuffer.WriteNumBits(DEMO_CLOSE, 5);
					}
				}

				int aBufferSize = mDemoBuffer.GetDataLen();
			}
			
		}

		if (!mPlayingDemoBuffer)
		{
			switch (event.type)
			{
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
					if ((!gInAssert) && (!mSEHOccured) && (!mShutdown))
					{
						mActive = true;
						RehupFocus();
						if (!mIsWindowed)
							mWidgetManager->MarkAllDirty();
						if (mIsOpeningURL && !mActive)
							URLOpenSucceeded(mOpeningURL);
					}
					break;
				case SDL_EVENT_WINDOW_FOCUS_LOST:
					mActive = false;
					RehupFocus();
					if (mIsOpeningURL && mActive)
						URLOpenFailed(mOpeningURL);
					break;
				case SDL_EVENT_WINDOW_MINIMIZED:
					mMinimized = true;
					if (mMuteOnLostFocus)
						Mute(true);
					break;
				case SDL_EVENT_WINDOW_MAXIMIZED:
				case SDL_EVENT_WINDOW_RESTORED:
					mMinimized = false;
					if (mMuteOnLostFocus)
						Unmute(true);
					mWidgetManager->MarkAllDirty();
					break;
				case SDL_EVENT_MOUSE_MOTION:
					if (!gInAssert && !mSEHOccured)
					{
						int x = event.motion.x;
						int y = event.motion.y;

#if SEXY_USE_CONTROLLER
						if (mUsingGamepad)
						{
							// If this is genuine physical mouse movement (SDL flags SDL_MOUSEMOTION_RELATIVE
							// as relative==true for mouse warp injections), revert to mouse mode.
							// SDL3 sets event.motion.which == SDL_TOUCH_MOUSEID for touch/synthetic events.
							if (event.motion.which != SDL_TOUCH_MOUSEID &&
								(std::abs(event.motion.xrel) > 2 || std::abs(event.motion.yrel) > 2))
							{
								mUsingGamepad = false;
								EnforceCursor();
							}
							else
							{
								break; // Synthetic/injected movement — ignore it completely
							}
						}
#endif

						if (!(x >= mRenderer->mPresentationRect.mX && x < mRenderer->mPresentationRect.mX + mRenderer->mPresentationRect.mWidth &&
							y >= mRenderer->mPresentationRect.mY && y < mRenderer->mPresentationRect.mY + mRenderer->mPresentationRect.mHeight && x > 0 && y > 0))
						{
							break;
						}
						mWidgetManager->RemapMouse(x, y);
						mLastUserInputTick = mLastTimerTime;
						mWidgetManager->MouseMove(x, y);
						if (!mMouseIn)
						{
							mMouseIn = true;
							EnforceCursor();
						}
					}
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				case SDL_EVENT_MOUSE_BUTTON_UP:
					if (!gInAssert && !mSEHOccured)
					{
						int btnCode = 0;
						bool down = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;

#if SEXY_USE_CONTROLLER
						// Do NOT revert to mouse mode on a button event alone — SDL3 on Windows
						// can fire synthetic button events without any physical mouse movement.
						// We switch back only when actual motion is detected (see MOUSE_MOTION).
						if (mUsingGamepad)
						{
							break;
						}
#endif

						switch (event.button.button)
						{
						case SDL_BUTTON_LEFT:
							btnCode = 1;
							break;
						case SDL_BUTTON_RIGHT:
							btnCode = -1;
							break;
						case SDL_BUTTON_MIDDLE:
							btnCode = 3;
							break;
						}

						int x = event.button.x;
						int y = event.button.y;
						if (!(x >= mRenderer->mPresentationRect.mX &&
							  x < mRenderer->mPresentationRect.mX + mRenderer->mPresentationRect.mWidth &&
							  y >= mRenderer->mPresentationRect.mY &&
							  y < mRenderer->mPresentationRect.mY + mRenderer->mPresentationRect.mHeight && x > 0 &&
							  y > 0))
						{
							break;
						}
						mWidgetManager->RemapMouse(x, y);

						mLastUserInputTick = mLastTimerTime;

						mWidgetManager->MouseMove(x, y);

						if (!mMouseIn)
						{
							if (mRecordingDemoBuffer)
							{
								WriteDemoTimingBlock();
								mDemoBuffer.WriteNumBits(0, 1);
								mDemoBuffer.WriteNumBits(DEMO_MOUSE_ENTER, 5);
							}

							mMouseIn = true;
							EnforceCursor();
						}


						if (down)
							mWidgetManager->MouseDown(x, y, btnCode);
						else
							mWidgetManager->MouseUp(x, y, btnCode);
					}
					break;
				case SDL_EVENT_MOUSE_WHEEL:
#if SEXY_USE_CONTROLLER
					if (mUsingGamepad)
					{
						break; // Ignore mouse wheel while gamepad is active to prevent cursor flicker
					}
#endif
					mWidgetManager->MouseWheel(event.wheel.y);
					break;
				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP: {
					bool isDown = event.type == SDL_EVENT_KEY_DOWN;
					SDL_Keycode key = event.key.key;

					mLastUserInputTick = mLastTimerTime;

					if (isDown && mDebugKeysEnabled && DebugKeyDown(GetKeyCodeFromSDLKeycode(key)))
						break;

					if (isDown)
						mWidgetManager->KeyDown(GetKeyCodeFromSDLKeycode(key));
					else
						mWidgetManager->KeyUp(GetKeyCodeFromSDLKeycode(key));
					break;
				}
				case SDL_EVENT_TEXT_INPUT: {
					mLastUserInputTick = mLastTimerTime;

					const char *it = event.text.text;
					const char *end = event.text.text + strlen(event.text.text);

					uint32_t aChar = utf8::next(it, end);

					mWidgetManager->KeyChar((SexyChar)aChar);
					break;
				}
				case SDL_EVENT_WINDOW_MOVED:
				{
					SDL_Window* theTargetWindow = SDL_GetWindowFromEvent(&event);
					if (mWindow->mInternalWindow == theTargetWindow && mIsWindowed)
					{
						mPreferredSize.mX = event.window.data1;
						mPreferredSize.mY = event.window.data2;
					}
					break;
				}
				case SDL_EVENT_WINDOW_RESIZED:
				{
					SDL_Window* theTargetWindow = SDL_GetWindowFromEvent(&event);
					if (mWindow->mInternalWindow == theTargetWindow && !mShutdown)
					{
						mPreferredSize.mWidth = event.window.data1;
						mPreferredSize.mHeight = event.window.data2;
						mMinimized = SDL_GetWindowFlags(mWindow->mInternalWindow) & SDL_WINDOW_MINIMIZED;

						// We don't want any sounds (or music) playing while its minimized
						if (mMinimized)
						{
							if (mMuteOnLostFocus)
								Mute(true);
						}
						else
						{
							if (mMuteOnLostFocus)
								Unmute(true);

							mWidgetManager->MarkAllDirty();
						}
						RehupFocus();
						mRenderer->UpdateViewport();
						mWidgetManager->Resize(mScreenBounds, mRenderer->mPresentationRect);
					}
					break;
				}
				
				case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
					mWidgetManager->SysColorChangedAll();
					mWidgetManager->MarkAllDirty();
					break;
#if SEXY_USE_CONTROLLER

				case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
						gSexyAppBase->mUsingGamepad = true;
					break;
				case SDL_EVENT_GAMEPAD_AXIS_MOTION:
					if (fabsf(event.gaxis.value / 32767.0f) > mGamepads[0]->mWeight)
						gSexyAppBase->mUsingGamepad = true;
					break;

				case SDL_EVENT_GAMEPAD_ADDED:
					{
						int anIdToUse = -1;
						for (int i = 0; i < MAX_GAMEPADS; i++)
						{
							if (mGamepads[i] == nullptr)
							{
								anIdToUse = i;
								break;
							}
						}
						if (anIdToUse != -1)
						{
							mGamepads[anIdToUse] = new Gamepad();
							mGamepads[anIdToUse]->SetDeviceID(event.gdevice.which);
						}
					}
					break;
				case SDL_EVENT_GAMEPAD_REMOVED:
					{
						int anIdRemoved = -1;
						for (int i = 0; i < MAX_GAMEPADS; i++)
						{
							if (mGamepads[i] != nullptr && mGamepads[i]->GetDeviceID() == event.gdevice.which)
							{
								anIdRemoved = i;
								break;
							}
						}
						if (anIdRemoved != -1)
						{
							delete mGamepads[anIdRemoved];
							mGamepads[anIdRemoved] = nullptr;
						}
					}
					break;
#endif
			}

		}

		//Demo independent events.
		switch (event.type)
		{
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			case SDL_EVENT_QUIT:
				// This should short-circuit all demo calls, otherwise we will get
				//  all sorts of weird asserts because we are changing
				//  program flow
				mManualShutdown = true;

				Shutdown();
				break;
			
			default:
				break;
		}
	}
	
	return SDL_HasEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
}

void SexyAppBase::Done3dTesting()
{
}

// return file name that you want to upload
std::string SexyAppBase::NotifyCrashHook()
{
	return "";
}


bool SexyAppBase::TryCreateRenderer(RenderingBackend theBackend)
{
	bool anOpenGLWorks = false;
	bool aSDL3Works = false;

#if SEXY_USE_OPENGL
	anOpenGLWorks = OpenGLRenderer::TestOpenGL(mWindow->mInternalWindow);
#endif

#if SEXY_USE_SDL3_RENDERER
	aSDL3Works = SDL3Renderer::TestSDL3();
#endif

	switch (theBackend)
	{
#if SEXY_USE_OPENGL
	case RenderingBackend::BACKEND_OPENGL:
		if (anOpenGLWorks)
		{
			mRenderer = new OpenGLRenderer(this);
			return true;
		}
		break;
#endif

#if SEXY_USE_SDL3_RENDERER
	case RenderingBackend::BACKEND_SDL3:
		if (aSDL3Works)
		{
			mRenderer = new SDL3Renderer(this);
			return true;
		}
		break;
#endif

	default:
		break;
	}

	return false;
}

void SexyAppBase::MakeWindow()
{
	if (mWindow != nullptr)
	{
		delete mWindow;
		mWidgetManager->mImage = nullptr;
	}
	
	mWindow = new Window(this);

	SDL_StartTextInput(mWindow->mInternalWindow);

	if ((mPlayingDemoBuffer) || (mIsWindowed && !mFullScreenWindow))
	{
		if (mSavedWindowedSize.mX != -1)
		{
			SDL_SetWindowSize(mWindow->mInternalWindow, mSavedWindowedSize.mWidth, mSavedWindowedSize.mHeight);

			SDL_SetWindowPosition(mWindow->mInternalWindow, mSavedWindowedSize.mX, mSavedWindowedSize.mY);

			mPreferredSize = mSavedWindowedSize;
			mSavedWindowedSize = Rect(-1, -1, -1, -1);
		}
		else
		{
			SDL_Rect aUsableBounds{};
			SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(mWindow->mInternalWindow), &aUsableBounds);

			int aWidth = (mPreferredSize.mWidth == -1) ? mWidth : mPreferredSize.mWidth;

			int aHeight = (mPreferredSize.mHeight == -1) ? mHeight : mPreferredSize.mHeight;

			int aPlaceX = aUsableBounds.x + (aUsableBounds.w - aWidth) / 2;
			int aPlaceY = aUsableBounds.y + (int)((aUsableBounds.h - aHeight) * 0.382f);

			if (mPreferredSize.mX != -1)
			{
				aPlaceX = mPreferredSize.mX;
				aPlaceY = mPreferredSize.mY;

				int aSpacing = 4;

				int aMinX = aUsableBounds.x + aSpacing;
				int aMinY = aUsableBounds.y + aSpacing;

				int aMaxX = aUsableBounds.x + aUsableBounds.w - aWidth - aSpacing;
				int aMaxY = aUsableBounds.y + aUsableBounds.h - aHeight - aSpacing;
				if (aMinX > aMaxX)
					aMaxX = aMinX;

				if (aMinY > aMaxY)
					aMaxY = aMinY;
				aPlaceX = std::clamp(aPlaceX, aMinX, aMaxX);
				aPlaceY = std::clamp(aPlaceY, aMinY, aMaxY);
			}

			SDL_SetWindowSize(mWindow->mInternalWindow, aWidth, aHeight);

			SDL_SetWindowPosition(mWindow->mInternalWindow, aPlaceX, aPlaceY);

 			mSavedWindowedSize = Rect(aPlaceX, aPlaceY, aWidth, aHeight);

		}
		mIsPhysWindowed = true;
	}
	else
		mIsPhysWindowed = false;
	SDL_SetWindowFocusable(mWindow->mInternalWindow, true);

	SDL_PropertiesID props = SDL_GetWindowProperties(mWindow->mInternalWindow);
	SDL_SetPointerProperty(props, "sexyappframework.userdata", this);

	if (mRenderer == nullptr)
	{

		if (mDesiredBackend == RenderingBackend::BACKEND_NONE)
		{
#if SEXY_USE_OPENGL
			mDesiredBackend = RenderingBackend::BACKEND_OPENGL;
#elif SEXY_USE_SDL3_RENDERER
			mDesiredBackend = RenderingBackend::BACKEND_SDL3;
#endif
		}

		TryCreateRenderer(mDesiredBackend);

#if SEXY_USE_OPENGL
		if (mRenderer == nullptr && mDesiredBackend != RenderingBackend::BACKEND_OPENGL)
			TryCreateRenderer(RenderingBackend::BACKEND_OPENGL);
#endif

#if SEXY_USE_SDL3_RENDERER
		if (mRenderer == nullptr && mDesiredBackend != RenderingBackend::BACKEND_SDL3)
		{
			TryCreateRenderer(RenderingBackend::BACKEND_SDL3);
		}
#endif

		if (mRenderer == nullptr)
		{
			SexyString anError = "Couldn't create a renderer.\n\nAvailable backends:\n";

#if SEXY_USE_OPENGL
			anError +=
				StrFormat("OpenGL: %s\n", OpenGLRenderer::TestOpenGL(mWindow->mInternalWindow) ? "OK" : "FAILED");
#endif

#if SEXY_USE_SDL3_RENDERER
			anError += StrFormat("SDL3 Renderer: %s\n", SDL3Renderer::TestSDL3() ? "OK" : "FAILED");
#endif

			MsgBox(anError, "Engine Error");
			assert(false);
		}

		printf("[SexyAppBase] - Initialized Renderer Backend - %s\n", mRenderer->getBackendType().c_str());

	}

	int aResult = InitRenderer();

	// Enable 3d setting
	bool is3D = false;
	bool is3DOptionSet = RegistryReadBoolean("Is3D", &is3D);
	if (is3DOptionSet)
	{
		if (mAutoEnable3D)
		{
			mAutoEnable3D = false;
			mIs3D = true;
		}

		if (is3D)
			mIs3D = true;
	}

	bool isActive = mActive;
	mActive = SDL_GetWindowFlags(mWindow->mInternalWindow) & SDL_WINDOW_INPUT_FOCUS;

	mPhysMinimized = false;
	if (mMinimized)
	{
		if (mMuteOnLostFocus)
			Unmute(true);

		mMinimized = false;
		isActive = mActive; // set this here so we don't call RehupFocus again.
		RehupFocus();
	}

	if (isActive != mActive)
		RehupFocus();

	ReInitImages();

	mWidgetManager->mImage = mRenderer->GetScreenImage();
	mWidgetManager->MarkAllDirty();
}

void SexyAppBase::DeleteNativeImageData()
{
	MemoryImageSet::iterator anItr = mMemoryImageSet.begin();
	while (anItr != mMemoryImageSet.end())
	{
		MemoryImage *aMemoryImage = *anItr;
		aMemoryImage->DeleteNativeData();
		++anItr;
	}
}

void SexyAppBase::DeleteExtraImageData()
{
	AutoCrit anAutoCrit(mRenderer->mCritSect);
	MemoryImageSet::iterator anItr = mMemoryImageSet.begin();
	while (anItr != mMemoryImageSet.end())
	{
		MemoryImage *aMemoryImage = *anItr;
		aMemoryImage->DeleteExtraBuffers();
		++anItr;
	}
}

void SexyAppBase::ReInitImages()
{
	MemoryImageSet::iterator anItr = mMemoryImageSet.begin();
	while (anItr != mMemoryImageSet.end())
	{
		MemoryImage *aMemoryImage = *anItr;
		aMemoryImage->ReInit();
		++anItr;
	}
}

void SexyAppBase::LoadingThreadProc()
{
}

void SexyAppBase::LoadingThreadCompleted()
{
}

void SexyAppBase::LoadingThreadProcStub(void *theArg)
{
	SexyAppBase *aSexyApp = (SexyAppBase *)theArg;

	aSexyApp->LoadingThreadProc();

	printf("[SexyAppBase] - Resource Loading Time: %d\n", (SDL_GetTicks() - aSexyApp->mTimeLoaded));

	aSexyApp->mLoadingThreadCompleted = true;
}

void SexyAppBase::StartLoadingThread()
{
	if (!mLoadingThreadStarted)
	{
		mYieldMainThread = true;
		mLoadingThreadStarted = true;
		std::thread(LoadingThreadProcStub, this).detach();
	}
}

void SexyAppBase::SwitchScreenMode(bool wantWindowed, bool is3d, bool force)
{
	if (mIsWindowed)
	{
		SDL_GetWindowSize(mWindow->mInternalWindow, &mSavedWindowedSize.mWidth, &mSavedWindowedSize.mHeight);

		SDL_GetWindowPosition(mWindow->mInternalWindow, &mSavedWindowedSize.mX, &mSavedWindowedSize.mY);
	}

	if (mForceFullscreen)
		wantWindowed = false;

	if (mIsWindowed == wantWindowed && !force)
	{
		Set3DAcclerated(is3d);
		return;
	}

	// Set 3d acceleration preference
	Set3DAcclerated(is3d, false);

	// Always make the app windowed when playing demos, in order to
	//  make it easier to track down bugs.  We place this after the
	//  sanity check just so things get re-initialized and stuff
	//if (mPlayingDemoBuffer)
	//	wantWindowed = true;

	mIsWindowed = wantWindowed;

	MakeWindow();
	/*
	// We need to do this check to allow IE to get focus instead of
	//  stealing it away for ourselves
	if (!mIsOpeningURL)
	{
		::ShowWindow(mHWnd, SW_NORMAL);
		::SetForegroundWindow(mHWnd);
	}
	else
	{
		// Show it but don't activate it
		::ShowWindow(mHWnd, SW_SHOWNOACTIVATE);
	}*/

	if (mSoundManager != NULL)
	{
		mSoundManager->SetCooperativeWindow(mIsWindowed);
	}

	mLastTime = GetTicks();
}

void SexyAppBase::SwitchScreenMode(bool wantWindowed)
{
	SwitchScreenMode(wantWindowed, Is3DAccelerated());
}

void SexyAppBase::SwitchScreenMode()
{
	SwitchScreenMode(mIsWindowed, Is3DAccelerated(), true);
}

void SexyAppBase::SetAlphaDisabled(bool isDisabled)
{
	if (mAlphaDisabled != isDisabled)
	{
		mAlphaDisabled = isDisabled;
		mRenderer->SetVideoOnlyDraw(mAlphaDisabled);
		mWidgetManager->mImage = mRenderer->GetScreenImage();
		mWidgetManager->MarkAllDirty();
	}
}

void SexyAppBase::EnforceCursor()
{
#if SEXY_USE_CONTROLLER
	// While a gamepad is active, always keep the hardware cursor hidden.
	// This guard must wrap ALL SDL_ShowCursor() calls below, not just the early-return,
	// because the game can reach EnforceCursor from widget hover/focus paths.
	if (mUsingGamepad)
	{
		SDL_HideCursor();
		return;
	}
#endif

	bool wantSysCursor = true;
	SDL_SystemCursor aNativeCursor;

	switch (mCursorNum)
	{
	case CURSOR_HAND:
		aNativeCursor = SDL_SYSTEM_CURSOR_POINTER;
		break;
	case CURSOR_DRAGGING:
		aNativeCursor = SDL_SYSTEM_CURSOR_MOVE;
		break;
	case CURSOR_TEXT:
		aNativeCursor = SDL_SYSTEM_CURSOR_TEXT;
		break;
	case CURSOR_CIRCLE_SLASH:
		aNativeCursor = SDL_SYSTEM_CURSOR_NOT_ALLOWED;
		break;
	case CURSOR_SIZEALL:
		aNativeCursor = SDL_SYSTEM_CURSOR_MOVE;
		break;
	case CURSOR_SIZENESW:
		aNativeCursor = SDL_SYSTEM_CURSOR_NESW_RESIZE;
		break;
	case CURSOR_SIZENS:
		aNativeCursor = SDL_SYSTEM_CURSOR_NS_RESIZE;
		break;
	case CURSOR_SIZENWSE:
		aNativeCursor = SDL_SYSTEM_CURSOR_NWSE_RESIZE;
		break;
	case CURSOR_SIZEWE:
		aNativeCursor = SDL_SYSTEM_CURSOR_EW_RESIZE;
		break;
	case CURSOR_WAIT:
		aNativeCursor = SDL_SYSTEM_CURSOR_WAIT;
		break;
	case CURSOR_NONE:
		SDL_HideCursor();
		return;
	case CURSOR_POINTER:
	default:
		aNativeCursor = SDL_SYSTEM_CURSOR_DEFAULT;
		break;
	}

	if ((mSEHOccured) || (!mMouseIn))
	{
		SDL_SetCursor(mSystemCursors[SDL_SYSTEM_CURSOR_DEFAULT]);
		SDL_ShowCursor();
	}
	else
	{
		if (mCursorNum >= NUM_CURSORS || (mCursorImages[mCursorNum] == nullptr) ||
			((!mPlayingDemoBuffer) && (!mCustomCursorsEnabled)))
		{
			SDL_SetCursor(mSystemCursors[aNativeCursor]);
			SDL_ShowCursor();
		}
		else
		{
			if (!mCachedCursors[mCursorNum])
			{
				SDL_Surface *aSurface =
					SDL_CreateSurfaceFrom(mCursorImages[mCursorNum]->mWidth, mCursorImages[mCursorNum]->mHeight,
										  SDL_PIXELFORMAT_ARGB8888, ((GPUImage *)mCursorImages[mCursorNum])->GetBits(),
										  mCursorImages[mCursorNum]->mWidth * sizeof(uint32_t));

				SDL_Cursor *aCursor = SDL_CreateColorCursor(aSurface, 0, 0);
				mCachedCursors[mCursorNum] = aCursor;

				SDL_DestroySurface(aSurface);
			}
			SDL_SetCursor(mCachedCursors[mCursorNum]);
			SDL_ShowCursor();

			if (!mPlayingDemoBuffer)
			{
				//SDL_HideCursor();
			}
			wantSysCursor = false;
		}
	}

	if (wantSysCursor != mSysCursor)
	{
		mSysCursor = wantSysCursor;

		if (!mPlayingDemoBuffer)
		{
			SDL_SetCursor(mSystemCursors[SDL_SYSTEM_CURSOR_DEFAULT]);
		}
	}
}

void SexyAppBase::ProcessSafeDeleteList()
{
	MTAutoDisallowRand aDisallowRand;

	WidgetSafeDeleteList::iterator anItr = mSafeDeleteList.begin();
	while (anItr != mSafeDeleteList.end())
	{
		WidgetSafeDeleteInfo *aWidgetSafeDeleteInfo = &(*anItr);
		if (mUpdateAppDepth <= aWidgetSafeDeleteInfo->mUpdateAppDepth)
		{
			delete aWidgetSafeDeleteInfo->mWidget;
			anItr = mSafeDeleteList.erase(anItr);
		}
		else
			++anItr;
	}
}

void SexyAppBase::UpdateFTimeAcc()
{
	uint32_t aCurTime = timeGetTime();

	if (mLastTimeCheck != 0)
	{
		int aDeltaTime = aCurTime - mLastTimeCheck;

		mUpdateFTimeAcc = std::min(mUpdateFTimeAcc + aDeltaTime, 200.0);

		if (mRelaxUpdateBacklogCount > 0)
			mRelaxUpdateBacklogCount = std::max(mRelaxUpdateBacklogCount - aDeltaTime, 0);
	}

	mLastTimeCheck = aCurTime;
}

//int aNumCalls = 0;
//DWORD aLastCheck = 0;

bool SexyAppBase::Process(bool allowSleep)
{
	/*DWORD aTimeNow = SDL_GetTicks();
	if (aTimeNow - aLastCheck >= 10000)
	{
		OutputDebugString(StrFormat("FUpdates: %d\n", aNumCalls).c_str());
		aLastCheck = aTimeNow;
		aNumCalls = 0;
	}*/

	if (mLoadingFailed)
		Shutdown();

	bool isVSynched = !mPlayingDemoBuffer && mVSyncUpdates && !mLastDrawWasEmpty && !mVSyncBroken;

	double aFrameFTime;
	double anUpdatesPerUpdateF;

	if (mVSyncUpdates)
	{
		aFrameFTime = (1000.0 / mSyncRefreshRate) / mUpdateMultiplier;
		anUpdatesPerUpdateF = (float)(1000.0 / (mFrameTime * mSyncRefreshRate));
	}
	else
	{
		aFrameFTime = mFrameTime / mUpdateMultiplier;
		anUpdatesPerUpdateF = 1.0;
	}

	// Do we need to fast forward?
	if (mPlayingDemoBuffer)
	{
		if (mUpdateCount < mFastForwardToUpdateNum || mFastForwardToMarker)
		{
			if (!mDemoMute && !mFastForwardStep)
			{
				mDemoMute = true;
				Mute(true);
			}

			static uint32_t aTick = SDL_GetTicks();
			while (mUpdateCount < mFastForwardToUpdateNum || mFastForwardToMarker)
			{
				ClearUpdateBacklog();
				int aLastUpdateCount = mUpdateCount;

				// Actual updating code below
				//////////////////////////////////////////////////////////////////////////

				bool hadRealUpdate = DoUpdateFrames();

				if (hadRealUpdate)
				{
					mPendingUpdatesAcc += anUpdatesPerUpdateF;
					mPendingUpdatesAcc -= 1.0;
					ProcessSafeDeleteList();

					// Process any extra updates
					while (mPendingUpdatesAcc >= 1.0)
					{
						// These should just be IDLE commands we have to clear out
						ProcessDemo();

						bool hasRealUpdate = DoUpdateFrames();
						DBG_ASSERTE(hasRealUpdate);

						if (!hasRealUpdate)
							break;

						ProcessSafeDeleteList();
						mPendingUpdatesAcc -= 1.0;
					}

					DoUpdateFramesF((float)anUpdatesPerUpdateF);
					ProcessSafeDeleteList();
				}

				//////////////////////////////////////////////////////////////////////////

				// If the update count doesn't change, its because we are
				//  playing back a demo and need to read more
				if (aLastUpdateCount == mUpdateCount)
					return true;

				uint32_t aNewTick = SDL_GetTicks();
				if (aNewTick - aTick >= 1000 || mFastForwardStep) // let the app draw some
				{
					mFastForwardStep = false;
					aTick = SDL_GetTicks();
					DrawDirtyStuff();
					return true;
				}
			}
		}

		if (mDemoMute)
		{
			mDemoMute = false;
			mSoundManager->StopAllSounds();
			Unmute(true);
		}
	}

	// Make sure we're not paused
	if ((!mPaused) && (mUpdateMultiplier > 0))
	{
		uint32_t aStartTime = timeGetTime();

		uint32_t aCurTime = aStartTime;
		int aCumSleepTime = 0;

		// When we are VSynching, only calculate this FTimeAcc right after drawing

		if (!isVSynched)
			UpdateFTimeAcc();

		// mNonDrawCount is used to make sure we draw the screen at least
		// 10 times per second, even if it means we have to slow down
		// the updates to make it draw 10 times per second in "game time"

		bool didUpdate = false;

		if (mUpdateAppState == UPDATESTATE_PROCESS_1)
		{
			if ((++mNonDrawCount < (int)ceil(10 * mUpdateMultiplier)) || (!mLoaded))
			{
				bool doUpdate = false;

				if (isVSynched)
				{
					// Synch'ed to vertical refresh, so update as soon as possible after draw
					doUpdate = (!mHasPendingDraw) || (mUpdateFTimeAcc >= (int)(aFrameFTime * 0.75));
				}
				else if (mUpdateFTimeAcc >= aFrameFTime)
				{
					doUpdate = true;
				}

				if (doUpdate)
				{
					// Do VSyncBroken test.  This test fails if we're in fullscreen and
					// "don't vsync" has been forced in Advanced settings up Display Properties
					if ((!mPlayingDemoBuffer) && (mUpdateMultiplier == 1.0))
					{
						mVSyncBrokenTestUpdates++;
						if (mVSyncBrokenTestUpdates >= (uint32_t)((1000 + mFrameTime - 1) / mFrameTime))
						{
							// It has to be running 33% fast to be "broken" (25% = 1/0.800)
							if (aStartTime - mVSyncBrokenTestStartTick <= 800)
							{
								// The test has to fail 3 times in a row before we decide that
								//  vsync is broken overall
								mVSyncBrokenCount++;
								if (mVSyncBrokenCount >= 3)
									mVSyncBroken = true;
							}
							else
								mVSyncBrokenCount = 0;

							mVSyncBrokenTestStartTick = aStartTime;
							mVSyncBrokenTestUpdates = 0;
						}
					}

					bool hadRealUpdate = DoUpdateFrames();
					if (hadRealUpdate)
						mUpdateAppState = UPDATESTATE_PROCESS_2;

					mHasPendingDraw = true;
					didUpdate = true;
				}
			}
		}
		else if (mUpdateAppState == UPDATESTATE_PROCESS_2)
		{
			mUpdateAppState = UPDATESTATE_PROCESS_DONE;

			mPendingUpdatesAcc += anUpdatesPerUpdateF;
			mPendingUpdatesAcc -= 1.0;
			ProcessSafeDeleteList();

			// Process any extra updates
			while (mPendingUpdatesAcc >= 1.0)
			{
				// These should just be IDLE commands we have to clear out
				ProcessDemo();

				++mNonDrawCount;
				bool hasRealUpdate = DoUpdateFrames();
				DBG_ASSERTE(hasRealUpdate);

				if (!hasRealUpdate)
					break;

				ProcessSafeDeleteList();
				mPendingUpdatesAcc -= 1.0;
			}

			//aNumCalls++;
			DoUpdateFramesF((float)anUpdatesPerUpdateF);
			ProcessSafeDeleteList();

			// Don't let mUpdateFTimeAcc dip below 0
			//  Subtract an extra 0.2ms, because sometimes refresh rates have some
			//  fractional component that gets truncated, and it's better to take off
			//  too much to keep our timing tending toward occuring right after
			//  redraws
			if (isVSynched)
				mUpdateFTimeAcc = std::max(mUpdateFTimeAcc - aFrameFTime - 0.2f, 0.0);
			else
				mUpdateFTimeAcc -= aFrameFTime;

			if (mRelaxUpdateBacklogCount > 0)
				mUpdateFTimeAcc = 0;

			didUpdate = true;
		}

		if (!didUpdate)
		{
			mUpdateAppState = UPDATESTATE_PROCESS_DONE;

			mNonDrawCount = 0;

			if (mHasPendingDraw)
			{
				DrawDirtyStuff();
			}
			else
			{
				// Let us take into account the time it took to draw dirty stuff
				int aTimeToNextFrame = (int)(aFrameFTime - mUpdateFTimeAcc);
				if (aTimeToNextFrame > 0)
				{
					if (!allowSleep)
						return false;

					// Wait till next processing cycle
					++mSleepCount;
					Sleep(aTimeToNextFrame);

					aCumSleepTime += aTimeToNextFrame;
				}
			}
		}

		if (mYieldMainThread)
		{
			// This is to make sure that the title screen doesn't take up any more than
			// 1/3 of the processor time

			uint32_t anEndTime = timeGetTime();
			int anElapsedTime = (anEndTime - aStartTime) - aCumSleepTime;
			int aLoadingYieldSleepTime = std::min(250, (anElapsedTime * 2) - aCumSleepTime);

			if (aLoadingYieldSleepTime >= 0)
			{
				if (!allowSleep)
					return false;

				Sleep(aLoadingYieldSleepTime);
			}
		}
	}

	ProcessSafeDeleteList();
	return true;
}

/*void SexyAppBase::DoMainLoop()
{
	Dialog* aDialog = NULL;
	if (theModalDialogId != -1)
	{
		aDialog = GetDialog(theModalDialogId);
		DBG_ASSERTE(aDialog != NULL);
		if (aDialog == NULL)
			return;
	}

	while (!mShutdown)
	{		
		MSG msg;
		while ((PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) && (!mShutdown))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ProcessDemo();		
		ProcessDeferredMessages();

		if ((aDialog != NULL) && (aDialog->mResult != -1))
			return;

		if (!mShutdown)
		{
			//++aCount;
			Process();
		}		
	}
}*/

void SexyAppBase::DoMainLoop()
{
	while (!mShutdown)
	{
		if (mExitToTop)
			mExitToTop = false;
		UpdateApp();
	}
}

bool SexyAppBase::UpdateAppStep(bool *updated)
{
	if (updated != NULL)
		*updated = false;

	if (mExitToTop)
		return false;

	if (mUpdateAppState == UPDATESTATE_PROCESS_DONE)
		mUpdateAppState = UPDATESTATE_MESSAGES;

	mUpdateAppDepth++;

	// We update in two stages to avoid doing a Process if our loop termination
	//  condition has already been met by processing windows messages
	if (mUpdateAppState == UPDATESTATE_MESSAGES)
	{
		MSG msg;
		while ((PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) && (!mShutdown))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ProcessDemo();
		if (!ProcessDeferredMessages(true))
		{
			mUpdateAppState = UPDATESTATE_PROCESS_1;
		}
	}
	else
	{
		// Process changes state by itself
		if (mStepMode)
		{
			if (mStepMode == 2)
			{
				Sleep(mFrameTime);
				mUpdateAppState = UPDATESTATE_PROCESS_DONE; // skip actual update until next step
			}
			else
			{
				mStepMode = 2;
				DoUpdateFrames();
				DoUpdateFramesF(1.0f);
				DrawDirtyStuff();
			}
		}
		else
		{
			int anOldUpdateCnt = mUpdateCount;
			Process();
			if (updated != NULL)
				*updated = mUpdateCount != anOldUpdateCnt;
		}
	}

	mUpdateAppDepth--;

	return true;
}

bool SexyAppBase::UpdateApp()
{
	bool updated;
	for (;;)
	{
		if (!UpdateAppStep(&updated))
			return false;
		if (updated)
			return true;
	}
}

int SexyAppBase::InitRenderer()
{
	PreRendererInitHook();
	DeleteNativeImageData();
	int aResult = mRenderer->Init();
	DemoSyncRefreshRate();
	if (true == aResult)
	{
		mScreenBounds.mX = (mWidth - mRenderer->mWidth) / 2;
		mScreenBounds.mY = (mHeight - mRenderer->mHeight) / 2;
		mScreenBounds.mWidth = mRenderer->mWidth;
		mScreenBounds.mHeight = mRenderer->mHeight;
		mRenderer->UpdateViewport();
		mWidgetManager->Resize(mScreenBounds, mRenderer->mPresentationRect);
		PostRendererInitHook();
	}

#if SEXY_USE_IMGUI
	if (mImGuiManager != nullptr)
	{
		mImGuiManager->Init();
	}
#endif
	
	return aResult;
}

void SexyAppBase::PreTerminate()
{
}

void SexyAppBase::Start()
{
	if (mShutdown)
		return;

	if (mAutoStartLoadingThread)
		StartLoadingThread();

	SDL_ShowWindow(mWindow->mInternalWindow);

	int aCount = 0;
	int aSleepCount = 0;

	uint32_t aStartTime = SDL_GetTicks();

	mRunning = true;
	mLastTime = aStartTime;
	mLastUserInputTick = aStartTime;
	mLastTimerTime = aStartTime;

	DoMainLoop();
	ProcessSafeDeleteList();

	mRunning = false;

	WaitForLoadingThread();

	printf("[SexyAppBase] - Seconds       = %g\n", (SDL_GetTicks() - aStartTime) / 1000.0);

	printf("[SexyAppBase] - Sleep Count   = %d\n", mSleepCount);

	printf("[SexyAppBase] - Update Count  = %d\n", mUpdateCount);

	printf("[SexyAppBase] - Draw Count    = %d\n", mDrawCount);

	printf("[SexyAppBase] - Draw Time     = %d\n", mDrawTime);

	printf("[SexyAppBase] - Screen Blt    = %d\n", mScreenBltTime);

	if (mDrawTime + mScreenBltTime > 0)
	{
		printf("[SexyAppBase] - Avg FPS       = %d\r\n", (mDrawCount * 1000) / (mDrawTime + mScreenBltTime));
	}
	PreTerminate();

	WriteToRegistry();
}

bool SexyAppBase::CheckSignature(const Buffer &theBuffer, const std::string &theFileName)
{
	// Add your own signature checking code here
	return false;
}

bool SexyAppBase::LoadProperties(const std::string &theFileName, bool required, bool checkSig)
{
	Buffer aBuffer;
	if (!ReadBufferFromFile(theFileName, &aBuffer))
	{
		if (!required)
			return true;
		else
		{
			Popup(GetString("UNABLE_OPEN_PROPERTIES", "Unable to open properties file ") +
				  StringToSexyString(theFileName));
			return false;
		}
	}
	if (checkSig)
	{
		if (!CheckSignature(aBuffer, theFileName))
		{
			Popup(GetString("PROPERTIES_SIG_FAILED", "Signature check failed on ") +
				  StringToSexyString(theFileName + "'"));
			return false;
		}
	}

	PropertiesParser aPropertiesParser(this);

	// Load required language-file properties
	if (!aPropertiesParser.ParsePropertiesBuffer(aBuffer))
	{
		Popup(aPropertiesParser.GetErrorText());
		return false;
	}
	else
		return true;
}

bool SexyAppBase::LoadProperties()
{
	// Load required language-file properties
	return LoadProperties("properties\\default.xml", true, false);
}

void SexyAppBase::LoadResourceManifest()
{
	if (!mResourceManager->ParseResourcesFile("properties\\resources.xml"))
		ShowResourceError(true);
}

void SexyAppBase::ShowResourceError(bool doExit)
{
	Popup(mResourceManager->GetErrorText());
	if (doExit)
		DoExit(0);
}

bool SexyAppBase::GetBoolean(const std::string &theId)
{
	StringBoolMap::iterator anItr = mBoolProperties.find(theId);
	DBG_ASSERTE(anItr != mBoolProperties.end());

	if (anItr != mBoolProperties.end())
		return anItr->second;
	else
		return false;
}

bool SexyAppBase::GetBoolean(const std::string &theId, bool theDefault)
{
	StringBoolMap::iterator anItr = mBoolProperties.find(theId);

	if (anItr != mBoolProperties.end())
		return anItr->second;
	else
		return theDefault;
}

int SexyAppBase::GetInteger(const std::string &theId)
{
	StringIntMap::iterator anItr = mIntProperties.find(theId);
	DBG_ASSERTE(anItr != mIntProperties.end());

	if (anItr != mIntProperties.end())
		return anItr->second;
	else
		return false;
}

int SexyAppBase::GetInteger(const std::string &theId, int theDefault)
{
	StringIntMap::iterator anItr = mIntProperties.find(theId);

	if (anItr != mIntProperties.end())
		return anItr->second;
	else
		return theDefault;
}

double SexyAppBase::GetDouble(const std::string &theId)
{
	StringDoubleMap::iterator anItr = mDoubleProperties.find(theId);
	DBG_ASSERTE(anItr != mDoubleProperties.end());

	if (anItr != mDoubleProperties.end())
		return anItr->second;
	else
		return false;
}

double SexyAppBase::GetDouble(const std::string &theId, double theDefault)
{
	StringDoubleMap::iterator anItr = mDoubleProperties.find(theId);

	if (anItr != mDoubleProperties.end())
		return anItr->second;
	else
		return theDefault;
}

SexyString SexyAppBase::GetString(const std::string &theId)
{
	StringStringMap::iterator anItr = mStringProperties.find(theId);
	DBG_ASSERTE(anItr != mStringProperties.end());

	if (anItr != mStringProperties.end())
		return anItr->second;
	else
		return "";
}

SexyString SexyAppBase::GetString(const std::string &theId, const SexyString &theDefault)
{
	StringStringMap::iterator anItr = mStringProperties.find(theId);

	if (anItr != mStringProperties.end())
		return anItr->second;
	else
		return theDefault;
}

StringVector SexyAppBase::GetStringVector(const std::string &theId)
{
	StringStringVectorMap::iterator anItr = mStringVectorProperties.find(theId);
	DBG_ASSERTE(anItr != mStringVectorProperties.end());

	if (anItr != mStringVectorProperties.end())
		return anItr->second;
	else
		return StringVector();
}

void SexyAppBase::SetString(const std::string &theId, const std::string &theValue)
{
	std::pair<StringStringMap::iterator, bool> aPair =
		mStringProperties.insert(StringStringMap::value_type(theId, theValue));
	if (!aPair.second) // Found it, change value
		aPair.first->second = theValue;
}

void SexyAppBase::SetBoolean(const std::string &theId, bool theValue)
{
	std::pair<StringBoolMap::iterator, bool> aPair = mBoolProperties.insert(StringBoolMap::value_type(theId, theValue));
	if (!aPair.second) // Found it, change value
		aPair.first->second = theValue;
}

void SexyAppBase::SetInteger(const std::string &theId, int theValue)
{
	std::pair<StringIntMap::iterator, bool> aPair = mIntProperties.insert(StringIntMap::value_type(theId, theValue));
	if (!aPair.second) // Found it, change value
		aPair.first->second = theValue;
}

void SexyAppBase::SetDouble(const std::string &theId, double theValue)
{
	std::pair<StringDoubleMap::iterator, bool> aPair =
		mDoubleProperties.insert(StringDoubleMap::value_type(theId, theValue));
	if (!aPair.second) // Found it, change value
		aPair.first->second = theValue;
}

void SexyAppBase::DoParseCmdLine()
{
	char *aCmdLine = GetCommandLineA();
	char *aCmdLinePtr = aCmdLine;
	if (aCmdLinePtr[0] == '"')
	{
		aCmdLinePtr = strchr(aCmdLinePtr + 1, '"');
		if (aCmdLinePtr != NULL)
			aCmdLinePtr++;
	}

	if (aCmdLinePtr != NULL)
	{
		aCmdLinePtr = strchr(aCmdLinePtr, ' ');
		if (aCmdLinePtr != NULL)
			ParseCmdLine(aCmdLinePtr + 1);
	}

	mCmdLineParsed = true;
}

void SexyAppBase::ParseCmdLine(const std::string &theCmdLine)
{
	// Command line example:  -play -demofile="game demo.dmo"
	// Results in HandleCmdLineParam("-play", ""); HandleCmdLineParam("-demofile", "game demo.dmo");
	std::string aCurParamName;
	std::string aCurParamValue;

	int aSpacePos = 0;
	bool inQuote = false;
	bool onValue = false;

	for (int i = 0; i < (int)theCmdLine.length(); i++)
	{
		char c = theCmdLine[i];
		bool atEnd = false;

		if (c == '"')
		{
			inQuote = !inQuote;

			if (!inQuote)
				atEnd = true;
		}
		else if ((c == ' ') && (!inQuote))
			atEnd = true;
		else if (c == '=')
			onValue = true;
		else if (onValue)
			aCurParamValue += c;
		else
			aCurParamName += c;

		if (i == theCmdLine.length() - 1)
			atEnd = true;

		if (atEnd && !aCurParamName.empty())
		{
			HandleCmdLineParam(aCurParamName, aCurParamValue);
			aCurParamName = "";
			aCurParamValue = "";
			onValue = false;
		}
	}
}

static int GetMaxDemoFileNum(const std::string &theDemoPrefix, int theMaxToKeep, bool doErase)
{
	WIN32_FIND_DATAA aData;
	HANDLE aHandle = FindFirstFileA((theDemoPrefix + "*.dmo").c_str(), &aData);
	if (aHandle == INVALID_HANDLE_VALUE)
		return 0;

	typedef std::set<int> IntSet;
	IntSet aSet;

	do
	{
		int aNum = 0;
		if (sscanf(aData.cFileName, (theDemoPrefix + "%d.dmo").c_str(), &aNum) == 1)
			aSet.insert(aNum);

	} while (FindNextFileA(aHandle, &aData));
	FindClose(aHandle);

	IntSet::iterator anItr = aSet.begin();
	if ((int)aSet.size() > theMaxToKeep - 1 && doErase)
		DeleteFile(StrFormat((theDemoPrefix + "%d.dmo").c_str(), *anItr).c_str());

	if (aSet.empty())
		return 0;

	anItr = aSet.end();
	--anItr;
	return (*anItr);
}

void SexyAppBase::HandleCmdLineParam(const std::string &theParamName, const std::string &theParamValue)
{
	if (theParamName == "-play")
	{
		mPlayingDemoBuffer = true;
		mRecordingDemoBuffer = false;
	}
	else if (theParamName == "-recnum")
	{
		int aNum = atoi(theParamValue.c_str());
		if (aNum <= 0)
			aNum = 5;

		int aDemoFileNum = GetMaxDemoFileNum(mDemoPrefix, aNum, true) + 1;
		mDemoFileName = SexyStringToString(StrFormat(StringToSexyString(mDemoPrefix + "%d.dmo").c_str(), aDemoFileNum));
		if (mDemoFileName.length() < 2 || (mDemoFileName[1] != ':' && mDemoFileName[2] != '\\'))
		{
			mDemoFileName = GetAppDataFolder() + mDemoFileName;
		}
		mRecordingDemoBuffer = true;
		mPlayingDemoBuffer = false;
	}
	else if (theParamName == "-playnum")
	{
		int aNum = atoi(theParamValue.c_str()) - 1;
		if (aNum < 0)
			aNum = 0;

		int aDemoFileNum = GetMaxDemoFileNum(mDemoPrefix, aNum, false) - aNum;
		mDemoFileName = SexyStringToString(StrFormat(StringToSexyString(mDemoPrefix + "%d.dmo").c_str(), aDemoFileNum));
		mRecordingDemoBuffer = false;
		mPlayingDemoBuffer = true;
	}
	else if (theParamName == "-record")
	{
		mRecordingDemoBuffer = true;
		mPlayingDemoBuffer = false;
	}
	else if (theParamName == "-demofile")
	{
		mDemoFileName = theParamValue;
		if (mDemoFileName.length() < 2 || (mDemoFileName[1] != ':' && mDemoFileName[2] != '\\'))
		{
			mDemoFileName = GetAppDataFolder() + mDemoFileName;
		}
	}
	else if (theParamName == "-crash")
	{
		// Try to access NULL
		char *a = 0;
		*a = '!';
	}
	else if (theParamName == "-screensaver")
	{
		mIsScreenSaver = true;
	}
	else if (theParamName == "-changedir")
	{
		mChangeDirTo = theParamValue;
	}
	else
	{
		Popup(GetString("INVALID_COMMANDLINE_PARAM", "Invalid command line parameter: ") +
			  StringToSexyString(theParamName));
		DoExit(0);
	}
}

void SexyAppBase::PreDisplayHook()
{
}

void SexyAppBase::PreRendererInitHook()
{
}

void SexyAppBase::PostRendererInitHook()
{
}

bool SexyAppBase::ChangeDirHook(const char *theIntendedPath)
{
	return false;
}

MusicInterface *SexyAppBase::CreateMusicInterface()
{
	if (mNoSoundNeeded)
		return new MusicInterface;
	else
		return new BassMusicInterface(mWindow);
}

void SexyAppBase::InitPropertiesHook()
{
}

void SexyAppBase::InitHook()
{
}

void SexyAppBase::Init()
{
	mPrimaryThreadId = std::this_thread::get_id();

	if (mShutdown)
		return;

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
	{
		DoExit(0);
	}

#if SEXY_USE_IMGUI
	mImGuiManager = new ImGuiManager(this);
#endif

	if (FT_Init_FreeType(&mFreeTypeLib))
	{
		DoExit(0);
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);

	std::string aDataPath = GetAppDataFolder() + mFullCompanyName + "/" + mProdName;
	SetAppDataFolder(aDataPath + "/");
	MkDir(aDataPath);
	if (mDemoFileName.length() < 2 || (mDemoFileName[1] != ':' && mDemoFileName[2] != '/'))
	{
		mDemoFileName = GetAppDataFolder() + mDemoFileName;
	}

	InitPropertiesHook();
	ReadFromRegistry();

	if (!mCmdLineParsed)
		DoParseCmdLine();

	if (IsScreenSaver())
		mOnlyAllowOneCopyToRun = false;

	int aTouchDeviceLength = 0;
	SDL_TouchID *aDevicesDummy = SDL_GetTouchDevices(&aTouchDeviceLength);
	if (aTouchDeviceLength > 0) // check for tablet pc
	{
		mTabletPC = true;
		mFullScreenPageFlip = false; // so that tablet keyboard can show up
	}
	else
		mTabletPC = false;

	SDL_free(aDevicesDummy);

	for (int i = 0; i < SDL_SYSTEM_CURSOR_COUNT; i++)
	{
		mSystemCursors[i] = SDL_CreateSystemCursor((SDL_SystemCursor)i);
	}

	// Change directory
	if (!ChangeDirHook(mChangeDirTo.c_str()))
		std::filesystem::current_path(mChangeDirTo);

	gPakInterface->AddPakFile("main.pak");

	if (mOnlyAllowOneCopyToRun)
	{
		#if WIN32
		mCopyMutex = CreateMutex(NULL, TRUE, (mProdName + "_OnlyAllowOneCopyToRun_Mutex").c_str());
		if (::GetLastError() == ERROR_ALREADY_EXISTS)
			HandleGameAlreadyRunning();
		#endif
	}

	mRandSeed = SDL_GetTicks();
	SRand(mRandSeed);

	// Set up demo recording stuff
	if (mPlayingDemoBuffer)
	{
		std::string anError;
		if (!ReadDemoBuffer(anError))
		{
			mPlayingDemoBuffer = false;
			Popup(anError);
			DoExit(0);
		}
	}

	srand(SDL_GetTicks());

	// Let app do something before showing window, or switching to fullscreen mode
	// NOTE: Moved call to PreDisplayHook above mIsWindowed and GetSystemsMetrics
	// checks because the checks below use values that could change in PreDisplayHook.
	// PreDisplayHook must call mWidgetManager->Resize if it changes mWidth or mHeight.
	PreDisplayHook();

	mWidgetManager->Resize(Rect(0, 0, mWidth, mHeight), Rect(0, 0, mWidth, mHeight));

	SDL_DisplayID aPrimaryDisplay = SDL_GetPrimaryDisplay();
	const SDL_DisplayMode *aMode = SDL_GetCurrentDisplayMode(aPrimaryDisplay);

	// Check to see if we CAN run windowed or not...
	if (mIsWindowed && !mFullScreenWindow)
	{

		// How can we be windowed if our screen isn't even big enough?
		if (mWidth >= aMode->w || mHeight >= aMode->h)
		{
			mIsWindowed = false;
			mForceFullscreen = true;
		}
	}


	MakeWindow();

	if (mPlayingDemoBuffer)
	{
		// Get video data

		PrepareDemoCommand(true);
		mDemoNeedsCommand = true;

		DBG_ASSERTE(!mDemoIsShortCmd);
		DBG_ASSERTE(mDemoCmdNum == DEMO_VIDEO_DATA);

		mIsWindowed = mDemoBuffer.ReadBoolean();
		mSyncRefreshRate = mDemoBuffer.ReadByte();
	}

	if (mSoundManager == nullptr)
		mSoundManager = new OpenALSoundManager();

	SetSfxVolume(mSfxVolume);

	mMusicInterface = CreateMusicInterface();

	SetMusicVolume(mMusicVolume);

	if (IsScreenSaver())
	{
		SetCursor(CURSOR_NONE);
	}

	InitHook();

	mInitialized = true;
}

void SexyAppBase::HandleGameAlreadyRunning()
{
	if (mOnlyAllowOneCopyToRun)
	{
		Popup("You can only run one instance of this game");
		DoExit(0);
	}
}

void SexyAppBase::CopyToClipboard(const std::string &theString)
{
	if (mPlayingDemoBuffer)
		return;

	SDL_SetClipboardText(theString.c_str());
}

std::string SexyAppBase::GetClipboard()
{
	std::string aString;
	aString = SDL_GetClipboardText();
	return aString;
}

void SexyAppBase::SetCursor(int theCursorNum)
{
	mCursorNum = theCursorNum;
#if SEXY_USE_CONTROLLER
	if (mUsingGamepad)
		return;
#endif
	EnforceCursor();
}

int SexyAppBase::GetCursor()
{
	return mCursorNum;
}

void SexyAppBase::EnableCustomCursors(bool enabled)
{
	mCustomCursorsEnabled = enabled;
	EnforceCursor();
}

Sexy::GPUImage *SexyAppBase::GetImage(const std::string &theFileName, bool commitBits)
{
	ImageLib::Image *aLoadedImage = ImageLib::GetImage(theFileName, true);

	if (aLoadedImage == NULL)
		return NULL;

	GPUImage *anImage = mRenderer->NewGPUImage();
	anImage->mFilePath = theFileName;
	anImage->SetBits(aLoadedImage->GetBits(), aLoadedImage->GetWidth(), aLoadedImage->GetHeight(), commitBits);
	anImage->mFilePath = theFileName;
	delete aLoadedImage;

	return anImage;
}

Sexy::GPUImage *SexyAppBase::CreateCrossfadeImage(
	Sexy::Image *theImage1, const Rect &theRect1, Sexy::Image *theImage2, const Rect &theRect2, double theFadeFactor)
{
	MemoryImage *aMemoryImage1 = dynamic_cast<MemoryImage *>(theImage1);
	MemoryImage *aMemoryImage2 = dynamic_cast<MemoryImage *>(theImage2);

	if ((aMemoryImage1 == NULL) || (aMemoryImage2 == NULL))
		return NULL;

	if ((theRect1.mX < 0) || (theRect1.mY < 0) || (theRect1.mX + theRect1.mWidth > theImage1->GetWidth()) ||
		(theRect1.mY + theRect1.mHeight > theImage1->GetHeight()))
	{
		DBG_ASSERTE("Crossfade Rect1 out of bounds");
		return NULL;
	}

	if ((theRect2.mX < 0) || (theRect2.mY < 0) || (theRect2.mX + theRect2.mWidth > theImage2->GetWidth()) ||
		(theRect2.mY + theRect2.mHeight > theImage2->GetHeight()))
	{
		DBG_ASSERTE("Crossfade Rect2 out of bounds");
		return NULL;
	}

	int aWidth = theRect1.mWidth;
	int aHeight = theRect1.mHeight;

	GPUImage *anImage = mRenderer->NewGPUImage();
	anImage->Create(aWidth, aHeight);

	uint32_t *aDestBits = anImage->GetBits();
	uint32_t *aSrcBits1 = aMemoryImage1->GetBits();
	uint32_t *aSrcBits2 = aMemoryImage2->GetBits();

	int aSrc1Width = aMemoryImage1->GetWidth();
	int aSrc2Width = aMemoryImage2->GetWidth();
	uint32_t aMult = (int)(theFadeFactor * 256);
	uint32_t aOMM = (256 - aMult);

	for (int y = 0; y < aHeight; y++)
	{
		uint32_t *s1 = &aSrcBits1[(y + theRect1.mY) * aSrc1Width + theRect1.mX];
		uint32_t *s2 = &aSrcBits2[(y + theRect2.mY) * aSrc2Width + theRect2.mX];
		uint32_t *d = &aDestBits[y * aWidth];

		for (int x = 0; x < aWidth; x++)
		{
			uint32_t p1 = *s1++;
			uint32_t p2 = *s2++;

			//p1 = 0;
			//p2 = 0xFFFFFFFF;

			*d++ = ((((p1 & 0x000000FF) * aOMM + (p2 & 0x000000FF) * aMult) >> 8) & 0x000000FF) |
				   ((((p1 & 0x0000FF00) * aOMM + (p2 & 0x0000FF00) * aMult) >> 8) & 0x0000FF00) |
				   ((((p1 & 0x00FF0000) * aOMM + (p2 & 0x00FF0000) * aMult) >> 8) & 0x00FF0000) |
				   ((((p1 >> 24) * aOMM + (p2 >> 24) * aMult) << 16) & 0xFF000000);
		}
	}

	anImage->BitsChanged();

	return anImage;
}

void SexyAppBase::ColorizeImage(Image *theImage, const Color &theColor)
{
	MemoryImage *aSrcMemoryImage = dynamic_cast<MemoryImage *>(theImage);

	if (aSrcMemoryImage == NULL)
		return;

	uint32_t *aBits;
	int aNumColors;

	if (aSrcMemoryImage->mColorTable == NULL)
	{
		aBits = aSrcMemoryImage->GetBits();
		aNumColors = theImage->GetWidth() * theImage->GetHeight();
	}
	else
	{
		aBits = aSrcMemoryImage->mColorTable;
		aNumColors = 256;
	}

	if ((theColor.mAlpha <= 255) && (theColor.mRed <= 255) && (theColor.mGreen <= 255) && (theColor.mBlue <= 255))
	{
		for (int i = 0; i < aNumColors; i++)
		{
			uint32_t aColor = aBits[i];

			aBits[i] = ((((aColor & 0xFF000000) >> 8) * theColor.mAlpha) & 0xFF000000) |
					   ((((aColor & 0x00FF0000) * theColor.mRed) >> 8) & 0x00FF0000) |
					   ((((aColor & 0x0000FF00) * theColor.mGreen) >> 8) & 0x0000FF00) |
					   ((((aColor & 0x000000FF) * theColor.mBlue) >> 8) & 0x000000FF);
		}
	}
	else
	{
		for (int i = 0; i < aNumColors; i++)
		{
			uint32_t aColor = aBits[i];

			int aAlpha = ((aColor >> 24) * theColor.mAlpha) / 255;
			int aRed = (((aColor >> 16) & 0xFF) * theColor.mRed) / 255;
			int aGreen = (((aColor >> 8) & 0xFF) * theColor.mGreen) / 255;
			int aBlue = ((aColor & 0xFF) * theColor.mBlue) / 255;

			if (aAlpha > 255)
				aAlpha = 255;
			if (aRed > 255)
				aRed = 255;
			if (aGreen > 255)
				aGreen = 255;
			if (aBlue > 255)
				aBlue = 255;

			aBits[i] = (aAlpha << 24) | (aRed << 16) | (aGreen << 8) | (aBlue);
		}
	}

	aSrcMemoryImage->BitsChanged();
}

GPUImage *SexyAppBase::CreateColorizedImage(Image *theImage, const Color &theColor)
{
	MemoryImage *aSrcMemoryImage = dynamic_cast<MemoryImage *>(theImage);

	if (aSrcMemoryImage == NULL)
		return NULL;

	GPUImage *anImage = mRenderer->NewGPUImage();

	anImage->Create(theImage->GetWidth(), theImage->GetHeight());

	uint32_t *aSrcBits;
	uint32_t *aDestBits;
	int aNumColors;

	if (aSrcMemoryImage->mColorTable == NULL)
	{
		aSrcBits = aSrcMemoryImage->GetBits();
		aDestBits = anImage->GetBits();
		aNumColors = theImage->GetWidth() * theImage->GetHeight();
	}
	else
	{
		aSrcBits = aSrcMemoryImage->mColorTable;
		aDestBits = anImage->mColorTable = new uint32_t[256];
		aNumColors = 256;

		anImage->mColorIndices = new uint8_t[anImage->mWidth * theImage->mHeight];
		memcpy(anImage->mColorIndices, aSrcMemoryImage->mColorIndices, anImage->mWidth * theImage->mHeight);
	}

	if ((theColor.mAlpha <= 255) && (theColor.mRed <= 255) && (theColor.mGreen <= 255) && (theColor.mBlue <= 255))
	{
		for (int i = 0; i < aNumColors; i++)
		{
			uint32_t aColor = aSrcBits[i];

			aDestBits[i] = ((((aColor & 0xFF000000) >> 8) * theColor.mAlpha) & 0xFF000000) |
						   ((((aColor & 0x00FF0000) * theColor.mRed) >> 8) & 0x00FF0000) |
						   ((((aColor & 0x0000FF00) * theColor.mGreen) >> 8) & 0x0000FF00) |
						   ((((aColor & 0x000000FF) * theColor.mBlue) >> 8) & 0x000000FF);
		}
	}
	else
	{
		for (int i = 0; i < aNumColors; i++)
		{
			uint32_t aColor = aSrcBits[i];

			int aAlpha = ((aColor >> 24) * theColor.mAlpha) / 255;
			int aRed = (((aColor >> 16) & 0xFF) * theColor.mRed) / 255;
			int aGreen = (((aColor >> 8) & 0xFF) * theColor.mGreen) / 255;
			int aBlue = ((aColor & 0xFF) * theColor.mBlue) / 255;

			if (aAlpha > 255)
				aAlpha = 255;
			if (aRed > 255)
				aRed = 255;
			if (aGreen > 255)
				aGreen = 255;
			if (aBlue > 255)
				aBlue = 255;

			aDestBits[i] = (aAlpha << 24) | (aRed << 16) | (aGreen << 8) | (aBlue);
		}
	}

	anImage->BitsChanged();

	return anImage;
}

GPUImage *SexyAppBase::CopyImage(Image *theImage, const Rect &theRect)
{
	GPUImage *anImage = mRenderer->NewGPUImage();

	anImage->Create(theRect.mWidth, theRect.mHeight);

	Graphics g(anImage);
	g.DrawImage(theImage, -theRect.mX, -theRect.mY);

	anImage->CopyAttributes(theImage);

	return anImage;
}

GPUImage *SexyAppBase::CopyImage(Image *theImage)
{
	return CopyImage(theImage, Rect(0, 0, theImage->GetWidth(), theImage->GetHeight()));
}

void SexyAppBase::MirrorImage(Image *theImage)
{
	MemoryImage *aSrcMemoryImage = dynamic_cast<MemoryImage *>(theImage);

	uint32_t *aSrcBits = aSrcMemoryImage->GetBits();

	int aPhysSrcWidth = aSrcMemoryImage->mWidth;
	for (int y = 0; y < aSrcMemoryImage->mHeight; y++)
	{
		uint32_t *aLeftBits = aSrcBits + (y * aPhysSrcWidth);
		uint32_t *aRightBits = aLeftBits + (aPhysSrcWidth - 1);

		for (int x = 0; x < (aPhysSrcWidth >> 1); x++)
		{
			uint32_t aSwap = *aLeftBits;

			*(aLeftBits++) = *aRightBits;
			*(aRightBits--) = aSwap;
		}
	}

	aSrcMemoryImage->BitsChanged();
}

void SexyAppBase::FlipImage(Image *theImage)
{
	MemoryImage *aSrcMemoryImage = dynamic_cast<MemoryImage *>(theImage);

	uint32_t *aSrcBits = aSrcMemoryImage->GetBits();

	int aPhysSrcHeight = aSrcMemoryImage->mHeight;
	int aPhysSrcWidth = aSrcMemoryImage->mWidth;
	for (int x = 0; x < aPhysSrcWidth; x++)
	{
		uint32_t *aTopBits = aSrcBits + x;
		uint32_t *aBottomBits = aTopBits + (aPhysSrcWidth * (aPhysSrcHeight - 1));

		for (int y = 0; y < (aPhysSrcHeight >> 1); y++)
		{
			uint32_t aSwap = *aTopBits;

			*aTopBits = *aBottomBits;
			aTopBits += aPhysSrcWidth;
			*aBottomBits = aSwap;
			aBottomBits -= aPhysSrcWidth;
		}
	}

	aSrcMemoryImage->BitsChanged();
}

void SexyAppBase::RotateImageHue(Sexy::MemoryImage *theImage, int theDelta)
{
	while (theDelta < 0)
		theDelta += 256;

	int aSize = theImage->mWidth * theImage->mHeight;
	uint32_t *aPtr = theImage->GetBits();
	for (int i = 0; i < aSize; i++)
	{
		uint32_t aPixel = *aPtr;
		int alpha = aPixel & 0xff000000;
		int r = (aPixel >> 16) & 0xff;
		int g = (aPixel >> 8) & 0xff;
		int b = aPixel & 0xff;

		int maxval = std::max(r, std::max(g, b));
		int minval = std::min(r, std::min(g, b));
		int h = 0;
		int s = 0;
		int l = (minval + maxval) / 2;
		int delta = maxval - minval;

		if (delta != 0)
		{
			s = (delta * 256) / ((l <= 128) ? (minval + maxval) : (512 - maxval - minval));

			if (r == maxval)
				h = (g == minval ? 1280 + (((maxval - b) * 256) / delta) : 256 - (((maxval - g) * 256) / delta));
			else if (g == maxval)
				h = (b == minval ? 256 + (((maxval - r) * 256) / delta) : 768 - (((maxval - b) * 256) / delta));
			else
				h = (r == minval ? 768 + (((maxval - g) * 256) / delta) : 1280 - (((maxval - r) * 256) / delta));

			h /= 6;
		}

		h += theDelta;
		if (h >= 256)
			h -= 256;

		double v = (l < 128) ? (l * (255 + s)) / 255 : (l + s - l * s / 255);

		int y = (int)(2 * l - v);

		int aColorDiv = (6 * h) / 256;
		int x = (int)(y + (v - y) * ((h - (aColorDiv * 256 / 6)) * 6) / 255);
		if (x > 255)
			x = 255;

		int z = (int)(v - (v - y) * ((h - (aColorDiv * 256 / 6)) * 6) / 255);
		if (z < 0)
			z = 0;

		switch (aColorDiv)
		{
		case 0:
			r = (int)v;
			g = x;
			b = y;
			break;
		case 1:
			r = z;
			g = (int)v;
			b = y;
			break;
		case 2:
			r = y;
			g = (int)v;
			b = x;
			break;
		case 3:
			r = y;
			g = z;
			b = (int)v;
			break;
		case 4:
			r = x;
			g = y;
			b = (int)v;
			break;
		case 5:
			r = (int)v;
			g = y;
			b = z;
			break;
		default:
			r = (int)v;
			g = x;
			b = y;
			break;
		}

		*aPtr++ = alpha | (r << 16) | (g << 8) | (b);
	}

	theImage->BitsChanged();
}

uint32_t SexyAppBase::HSLToRGB(int h, int s, int l)
{
	int r;
	int g;
	int b;

	double v = (l < 128) ? (l * (255 + s)) / 255 : (l + s - l * s / 255);

	int y = (int)(2 * l - v);

	int aColorDiv = (6 * h) / 256;
	int x = (int)(y + (v - y) * ((h - (aColorDiv * 256 / 6)) * 6) / 255);
	if (x > 255)
		x = 255;

	int z = (int)(v - (v - y) * ((h - (aColorDiv * 256 / 6)) * 6) / 255);
	if (z < 0)
		z = 0;

	switch (aColorDiv)
	{
	case 0:
		r = (int)v;
		g = x;
		b = y;
		break;
	case 1:
		r = z;
		g = (int)v;
		b = y;
		break;
	case 2:
		r = y;
		g = (int)v;
		b = x;
		break;
	case 3:
		r = y;
		g = z;
		b = (int)v;
		break;
	case 4:
		r = x;
		g = y;
		b = (int)v;
		break;
	case 5:
		r = (int)v;
		g = y;
		b = z;
		break;
	default:
		r = (int)v;
		g = x;
		b = y;
		break;
	}

	return 0xFF000000 | (r << 16) | (g << 8) | (b);
}

uint32_t SexyAppBase::RGBToHSL(int r, int g, int b)
{
	int maxval = std::max(r, std::max(g, b));
	int minval = std::min(r, std::min(g, b));
	int hue = 0;
	int saturation = 0;
	int luminosity = (minval + maxval) / 2;
	int delta = maxval - minval;

	if (delta != 0)
	{
		saturation = (delta * 256) / ((luminosity <= 128) ? (minval + maxval) : (512 - maxval - minval));

		if (r == maxval)
			hue = (g == minval ? 1280 + (((maxval - b) * 256) / delta) : 256 - (((maxval - g) * 256) / delta));
		else if (g == maxval)
			hue = (b == minval ? 256 + (((maxval - r) * 256) / delta) : 768 - (((maxval - b) * 256) / delta));
		else
			hue = (r == minval ? 768 + (((maxval - g) * 256) / delta) : 1280 - (((maxval - r) * 256) / delta));

		hue /= 6;
	}

	return 0xFF000000 | (hue) | (saturation << 8) | (luminosity << 16);
}

void SexyAppBase::HSLToRGB(const uint32_t *theSource, uint32_t *theDest, int theSize)
{
	for (int i = 0; i < theSize; i++)
	{
		uint32_t src = theSource[i];
		theDest[i] = (src & 0xFF000000) | (HSLToRGB((src & 0xFF), (src >> 8) & 0xFF, (src >> 16) & 0xFF) & 0x00FFFFFF);
	}
}

void SexyAppBase::RGBToHSL(const uint32_t *theSource, uint32_t *theDest, int theSize)
{
	for (int i = 0; i < theSize; i++)
	{
		uint32_t src = theSource[i];
		theDest[i] =
			(src & 0xFF000000) | (RGBToHSL(((src >> 16) & 0xFF), (src >> 8) & 0xFF, (src & 0xFF)) & 0x00FFFFFF);
	}
}

void SexyAppBase::PrecacheAdditive(MemoryImage *theImage)
{
	theImage->GetRLAdditiveData(mRenderer);
}

void SexyAppBase::PrecacheAlpha(MemoryImage *theImage)
{
	theImage->GetRLAlphaData();
}

void SexyAppBase::PrecacheNative(MemoryImage *theImage)
{
	theImage->GetNativeAlphaData(mRenderer);
}

void SexyAppBase::PlaySample(int theSoundNum)
{
	if (!mSoundManager)
		return;

	SoundInstance *aSoundInstance = mSoundManager->GetSoundInstance(theSoundNum);
	if (aSoundInstance != NULL)
	{
		aSoundInstance->Play(false, true);
	}
}

void SexyAppBase::PlaySample(int theSoundNum, int thePan)
{
	if (!mSoundManager)
		return;

	SoundInstance *aSoundInstance = mSoundManager->GetSoundInstance(theSoundNum);
	if (aSoundInstance != NULL)
	{
		aSoundInstance->SetPan(thePan);
		aSoundInstance->Play(false, true);
	}
}

bool SexyAppBase::IsMuted()
{
	return mMuteCount > 0;
}

void SexyAppBase::Mute(bool autoMute)
{
	mMuteCount++;
	if (autoMute)
		mAutoMuteCount++;

	SetMusicVolume(mMusicVolume);
	SetSfxVolume(mSfxVolume);
}

void SexyAppBase::Unmute(bool autoMute)
{
	if (mMuteCount > 0)
	{
		mMuteCount--;
		if (autoMute)
			mAutoMuteCount--;
	}

	SetMusicVolume(mMusicVolume);
	SetSfxVolume(mSfxVolume);
}

double SexyAppBase::GetMusicVolume()
{
	return mMusicVolume;
}

void SexyAppBase::SetMusicVolume(double theVolume)
{
	mMusicVolume = theVolume;

	if (mMusicInterface != NULL)
		mMusicInterface->SetVolume((mMuteCount > 0) ? 0.0 : mMusicVolume);
}

double SexyAppBase::GetSfxVolume()
{
	return mSfxVolume;
}

void SexyAppBase::SetSfxVolume(double theVolume)
{
	mSfxVolume = theVolume;

	if (mSoundManager != NULL)
		mSoundManager->SetVolume((mMuteCount > 0) ? 0.0 : mSfxVolume);
}

double SexyAppBase::GetMasterVolume()
{
	return mSoundManager->GetMasterVolume();
}

void SexyAppBase::SetMasterVolume(double theMasterVolume)
{
	mSfxVolume = theMasterVolume;
	mSoundManager->SetMasterVolume(mSfxVolume);
}

void SexyAppBase::AddMemoryImage(MemoryImage *theMemoryImage)
{
	AutoCrit anAutoCrit(mRenderer->mCritSect);
	mMemoryImageSet.insert(theMemoryImage);
}

void SexyAppBase::RemoveMemoryImage(MemoryImage *theMemoryImage)
{
	AutoCrit anAutoCrit(mRenderer->mCritSect);
	MemoryImageSet::iterator anItr = mMemoryImageSet.find(theMemoryImage);
	if (anItr != mMemoryImageSet.end())
		mMemoryImageSet.erase(anItr);

	Remove3DData(theMemoryImage);
}

void SexyAppBase::Remove3DData(MemoryImage *theMemoryImage)
{
	if (mRenderer)
		mRenderer->Remove3DData(theMemoryImage);
}

bool SexyAppBase::Is3DAccelerated()
{
	return mIs3D;
}

bool SexyAppBase::Is3DAccelerationSupported()
{
	return true;
}

bool SexyAppBase::Is3DAccelerationRecommended()
{
	return true;
}

void SexyAppBase::DemoSyncRefreshRate()
{
	mSyncRefreshRate = mRenderer->mRefreshRate;

	if (mRecordingDemoBuffer)
	{
		WriteDemoTimingBlock();
		mDemoBuffer.WriteNumBits(0, 1);
		mDemoBuffer.WriteNumBits(DEMO_VIDEO_DATA, 5);
		mDemoBuffer.WriteBoolean(mIsWindowed);
		uint8_t aByte = (uint8_t)mSyncRefreshRate;
		mDemoBuffer.WriteByte(aByte);
	}
}

void SexyAppBase::Set3DAcclerated(bool is3D, bool reinit)
{
	// if (mRenderer->mIs3D == is3D)
		return;
	 //
	mUserChanged3DSetting = true;
	//mDDInterface->mIs3D = is3D;

	if (reinit)
	{
		int aResult = InitRenderer();
		/*
		if (is3D && aResult != DDInterface::RESULT_OK)
		{
			Set3DAcclerated(false, reinit);
			return;
		}
		else if (aResult != DDInterface::RESULT_OK)
		{
			Popup(GetString("FAILED_INIT_DIRECTDRAW", "Failed to initialize DirectDraw: ") +
				  StringToSexyString(DDInterface::ResultToString(aResult) + " " + mDDInterface->mErrorString));
			DoExit(1);
		}*/

		ReInitImages();

		mWidgetManager->mImage = mRenderer->GetScreenImage();
		mWidgetManager->MarkAllDirty();
	}
}

SharedImageRef SexyAppBase::GetSharedImage(const std::string &theFileName, const std::string &theVariant, bool *isNew)
{
	std::string anUpperFileName = StringToUpper(theFileName);
	std::string anUpperVariant = StringToUpper(theVariant);

	std::pair<SharedImageMap::iterator, bool> aResultPair;
	SharedImageRef aSharedImageRef;

	{
		AutoCrit anAutoCrit(mRenderer->mCritSect);
		aResultPair = mSharedImageMap.insert(
			SharedImageMap::value_type(SharedImageMap::key_type(anUpperFileName, anUpperVariant), SharedImage()));
		aSharedImageRef = &aResultPair.first->second;
	}

	if (isNew != NULL)
		*isNew = aResultPair.second;

	if (aResultPair.second)
	{
		// Pass in a '!' as the first char of the file name to create a new image
		if ((theFileName.length() > 0) && (theFileName[0] == '!'))
			aSharedImageRef.mSharedImage->mImage = mRenderer->NewGPUImage();
		else
			aSharedImageRef.mSharedImage->mImage = GetImage(theFileName, false);
	}

	return aSharedImageRef;
}

void SexyAppBase::CleanSharedImages()
{
	AutoCrit anAutoCrit(mRenderer->mCritSect);

	if (mCleanupSharedImages)
	{
		// Delete shared images with reference counts of 0
		// This doesn't occur in ~SharedImageRef because sometimes we can not only access the image
		//  through the SharedImageRef returned by GetSharedImage, but also by calling GetSharedImage
		//  again with the same params -- so we can have instances where we do the 'final' deref on
		//  an image but immediately re-request it via GetSharedImage
		SharedImageMap::iterator aSharedImageItr = mSharedImageMap.begin();
		while (aSharedImageItr != mSharedImageMap.end())
		{
			SharedImage *aSharedImage = &aSharedImageItr->second;
			if (aSharedImage->mRefCount == 0)
			{
				delete aSharedImage->mImage;
				mSharedImageMap.erase(aSharedImageItr++);
			}
			else
				++aSharedImageItr;
		}

		mCleanupSharedImages = false;
	}
}
