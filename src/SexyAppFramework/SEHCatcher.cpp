#if SEXY_CRASH_HANDLER
#include "SEHCatcher.h"
#include <SDL3/SDL.h>

#if SEXY_USE_OPENGL
#include <glad/glad.h>
#endif
#include <AL/al.h>

#include "SexyAppBase.h"
#include <fstream>

#if SEXY_USE_IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdlrenderer3.h> //use SDL3 Renderer for the handler.
#include <imgui/imgui_impl_sdl3.h> //use SDL3 Renderer for the handler.
#endif

#include <cpptrace/cpptrace.hpp>

using namespace Sexy;

#if WIN32
LPTOP_LEVEL_EXCEPTION_FILTER SEHCatcher::mPreviousFilter;
#endif
SexyAppBase *SEHCatcher::mApp = NULL;
bool SEHCatcher::mHasDemoFile = false;
bool SEHCatcher::mDebugError = false;
std::string SEHCatcher::mErrorTitle;
std::string SEHCatcher::mErrorText;
std::string SEHCatcher::mUserText;
std::string SEHCatcher::mUploadFileName;
bool SEHCatcher::mExiting = false;
bool SEHCatcher::mShowUI = true;
bool SEHCatcher::mAllowSubmit = true;

std::string SEHCatcher::mCrashMessage = "An unexpected error has occured!";
std::string SEHCatcher::mSubmitHost;
std::string SEHCatcher::mSubmitMessage =
	"Please help out by providing as much information as you can about this crash.";
std::string SEHCatcher::mSubmitErrorMessage = "Failed to redirect to issue page.";

static bool gUseDefaultFonts = true;

#if WIN32
struct
{
	DWORD dwExceptionCode;
	char *szMessage;
} gMsgTable[] = {{STATUS_SEGMENT_NOTIFICATION, "Segment Notification"},
				 {STATUS_BREAKPOINT, "Breakpoint"},
				 {STATUS_SINGLE_STEP, "Single step"},
				 {STATUS_WAIT_0, "Wait 0"},
				 {STATUS_ABANDONED_WAIT_0, "Abandoned Wait 0"},
				 {STATUS_USER_APC, "User APC"},
				 {STATUS_TIMEOUT, "Timeout"},
				 {STATUS_PENDING, "Pending"},
				 {STATUS_GUARD_PAGE_VIOLATION, "Guard Page Violation"},
				 {STATUS_DATATYPE_MISALIGNMENT, "Data Type Misalignment"},
				 {STATUS_ACCESS_VIOLATION, "Access Violation"},
				 {STATUS_IN_PAGE_ERROR, "In Page Error"},
				 {STATUS_NO_MEMORY, "No Memory"},
				 {STATUS_ILLEGAL_INSTRUCTION, "Illegal Instruction"},
				 {STATUS_NONCONTINUABLE_EXCEPTION, "Noncontinuable Exception"},
				 {STATUS_INVALID_DISPOSITION, "Invalid Disposition"},
				 {STATUS_ARRAY_BOUNDS_EXCEEDED, "Array Bounds Exceeded"},
				 {STATUS_FLOAT_DENORMAL_OPERAND, "Float Denormal Operand"},
				 {STATUS_FLOAT_DIVIDE_BY_ZERO, "Divide by Zero"},
				 {STATUS_FLOAT_INEXACT_RESULT, "Float Inexact Result"},
				 {STATUS_FLOAT_INVALID_OPERATION, "Float Invalid Operation"},
				 {STATUS_FLOAT_OVERFLOW, "Float Overflow"},
				 {STATUS_FLOAT_STACK_CHECK, "Float Stack Check"},
				 {STATUS_FLOAT_UNDERFLOW, "Float Underflow"},
				 {STATUS_INTEGER_DIVIDE_BY_ZERO, "Integer Divide by Zero"},
				 {STATUS_INTEGER_OVERFLOW, "Integer Overflow"},
				 {STATUS_PRIVILEGED_INSTRUCTION, "Privileged Instruction"},
				 {STATUS_STACK_OVERFLOW, "Stack Overflow"},
				 {STATUS_CONTROL_C_EXIT, "Ctrl+C Exit"},
				 {0xFFFFFFFF, ""}};

#endif

SEHCatcher::SEHCatcher()
{
#if WIN32
	mPreviousFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);
#endif
}

SEHCatcher::~SEHCatcher()
{
#if WIN32
	SetUnhandledExceptionFilter(mPreviousFilter);
#endif
}
#if WIN32
long __stdcall SEHCatcher::UnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptPtr)
{
	if (mApp != NULL)
		mApp->SEHOccured();

	DoHandleDebugEvent(lpExceptPtr);

	if (!mDebugError)
		SetErrorMode(SEM_NOGPFAULTERRORBOX);

	return EXCEPTION_CONTINUE_SEARCH;
}
#endif

std::string GetCallStack()
{
	std::string aCallStack;
	auto trace = cpptrace::generate_trace();
	size_t startIdx = 0;
	size_t endIdx = trace.frames.size();
#if WIN32

	for (size_t i = 0; i < trace.frames.size(); i++)
	{
		//KiUserExceptionDispatcher is the border between the main code and the exception so we start the stack from there -Electr0Gunner
		if (trace.frames[i].symbol.find("KiUserExceptionDispatcher") != std::string::npos)
		{
			startIdx = i + 1;
		}
		if (trace.frames[i].symbol.find("main") != std::string::npos)
		{
			endIdx = i + 1;
			break;
		}
	}
#endif
	aCallStack += "Stack Trace\n";
	aCallStack += std::string(40, '-') + "\n";

	int displayIdx = 0;
	for (size_t i = startIdx; i < endIdx; i++)
	{
		auto &f = trace.frames[i];
		aCallStack += StrFormat("  #%d ", displayIdx++);

		if (!f.symbol.empty())
			aCallStack += f.symbol;
		else
			aCallStack += StrFormat("0x%p" , f.raw_address);

		if (!f.filename.empty())
			aCallStack += StrFormat("  (%s:%zu)", GetFileName(f.filename).c_str(), f.line);

		aCallStack += "\n";
	}

	return aCallStack;
}

#if WIN32
void SEHCatcher::DoHandleDebugEvent(LPEXCEPTION_POINTERS lpEP)
{
	std::string anErrorTitle;
	std::string aDebugDump;

	char aBuffer[2048];

	///////////////////////////
	// first name the exception
	char *szName = NULL;
	for (int i = 0; gMsgTable[i].dwExceptionCode != 0xFFFFFFFF; i++)
	{
		if (gMsgTable[i].dwExceptionCode == lpEP->ExceptionRecord->ExceptionCode)
		{
			szName = gMsgTable[i].szMessage;
			break;
		}
	}

	if (szName != NULL)
	{
		sprintf(aBuffer,
				"Exception: %s (code 0x%x) at address %08X in thread %X\r\n",
				szName,
				lpEP->ExceptionRecord->ExceptionCode,
				lpEP->ExceptionRecord->ExceptionAddress,
				GetCurrentThreadId());
	}
	else
	{
		sprintf(aBuffer,
				"Unknown exception: (code 0x%x) at address %08X in thread %X\r\n",
				lpEP->ExceptionRecord->ExceptionCode,
				lpEP->ExceptionRecord->ExceptionAddress,
				GetCurrentThreadId());
	}

	aDebugDump += aBuffer;



	aDebugDump += "\n";
	aDebugDump += GetCallStack();
	aDebugDump += "\n";
	aDebugDump += GetSysInfo();

	if (mApp != NULL)
	{
		std::string aGameSEHInfo = mApp->GetGameSEHInfo();
		if (aGameSEHInfo.length() > 0)
		{
			aDebugDump += "\n";
			aDebugDump += aGameSEHInfo;
		}

		mApp->CopyToClipboard(aDebugDump);
	}

	WriteToFile(aDebugDump);

	if (mApp != NULL)
	{
		if (mApp->mRecordingDemoBuffer)
		{
			// Make sure we have enough update block things in there to
			//  get to the final crashing update
			mApp->WriteDemoTimingBlock();
			mApp->mDemoBuffer.WriteNumBits(0, 1);
			mApp->mDemoBuffer.WriteNumBits(DEMO_IDLE, 5);
			mApp->WriteDemoBuffer();
			mUploadFileName = mApp->mDemoFileName;
		}

		mHasDemoFile = mApp->mRecordingDemoBuffer;
		std::string anUploadFile = mApp->NotifyCrashHook();
		if (!anUploadFile.empty())
		{
			mUploadFileName = anUploadFile;
			mHasDemoFile = true;
		}

		mApp->mRecordingDemoBuffer = false;
		mApp->mPlayingDemoBuffer = false;
	}

	if (mShowUI)
		ShowErrorDialog(anErrorTitle, aDebugDump);
}
#endif

std::string SEHCatcher::GetFilename(const std::string &thePath)
{
	int aLastSlash = std::max((int)thePath.rfind('\\'), (int)thePath.rfind('/'));

	if (aLastSlash >= 0)
	{
		return thePath.substr(aLastSlash + 1);
	}
	else
		return thePath;
}

void SEHCatcher::SEHWindowProc()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_QUIT)
			mExiting = true;
		ImGui_ImplSDL3_ProcessEvent(&e);
	}
}

void SEHCatcher::WriteToFile(const std::string &theErrorText)
{
	std::fstream aStream("crash.txt", std::ios::out);
	aStream << theErrorText.c_str() << std::endl;
}

void SEHCatcher::ShowErrorDialog(const std::string &theErrorTitle, const std::string &theErrorText)
{
	mErrorTitle = theErrorTitle;
	mErrorText = theErrorText;
	// ImGui based Crash Handler from PopLib
	ImGuiContext *aPrevCtx = ImGui::GetCurrentContext();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *aWindow = SDL_CreateWindow("Fatal Error!", 600, 400, 0);
	SDL_Renderer *aRenderer = SDL_CreateRenderer(aWindow, NULL);

	IMGUI_CHECKVERSION();
	ImGuiContext *aLocalImGuiCTX = ImGui::CreateContext();
	ImGui::SetCurrentContext(aLocalImGuiCTX);
	ImGuiIO &io = ImGui::GetIO();
	(void)io; // uhhhhhh
	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForSDLRenderer(aWindow, aRenderer);
	ImGui_ImplSDLRenderer3_Init(aRenderer);

	// @ThePixelMoon: Oopsie!
	SDL_SetClipboardText(mErrorText.c_str()); // we're not lying.

	while (!mExiting)
	{
		SEHWindowProc();

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		int windowWidth;
		int windowHeight;
		SDL_GetWindowSize(aWindow, &windowWidth, &windowHeight);

		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		ImGui::Begin("An unexpected error has occurred!", nullptr,
					 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
						 ImGuiWindowFlags_NoSavedSettings);

		ImGui::TextWrapped("%s", mCrashMessage.c_str());
		ImGui::Spacing();

		float contentHeight = ImGui::GetContentRegionAvail().y;
		float logHeight = 180.0f;
		float buttonHeight = 30.0f;
		float spacingE = 10.0f;
		float reservedBottom = logHeight + buttonHeight + spacingE * 3;
		ImGui::Dummy(ImVec2(0.0f, contentHeight - reservedBottom));

		ImGui::BeginChild("ErrorDetails", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::TextWrapped("%s", mErrorText.c_str());
		ImGui::EndChild();

		ImGui::Spacing();

		float buttonWidth = 120.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x;

		int numButtons = 1;
#ifdef _DEBUG
		numButtons += 1;
#endif
		numButtons += 1;

		float totalWidth = numButtons * buttonWidth + (numButtons - 1) * spacing;
		float availWidth = ImGui::GetContentRegionAvail().x;
		float startX = (availWidth - totalWidth) * 0.5f;
		ImGui::SetCursorPosX(startX);

		if (ImGui::Button("Send Issue", ImVec2(buttonWidth, 0)))
		{
			if (!SDL_OpenURL(mSubmitHost.c_str()))
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", mSubmitErrorMessage.c_str(), aWindow);
			}

			mExiting = true;
		}
#ifdef _DEBUG
		ImGui::SameLine();
		if (ImGui::Button("Debug", ImVec2(buttonWidth, 0)))
		{
			mDebugError = true;
			mExiting = true;
		}
#endif
		ImGui::SameLine();
		if (ImGui::Button("Close Now", ImVec2(buttonWidth, 0)))
		{
			mExiting = true;
		}

		ImGui::End();

		ImGui::Render();
		SDL_SetRenderDrawColor(aRenderer, 240, 240, 240, 255);
		SDL_RenderClear(aRenderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), aRenderer);
		SDL_RenderPresent(aRenderer);

		SDL_Delay(16);
	}
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();

	ImGui::DestroyContext(aLocalImGuiCTX);
	if (aRenderer)
		SDL_DestroyRenderer(aRenderer);
	if (aWindow)
		SDL_DestroyWindow(aWindow);

	if (aPrevCtx)
		ImGui::SetCurrentContext(aPrevCtx);
}

std::string SEHCatcher::GetSysInfo()
{
	std::string aDebugDump;
#if WIN32
	OSVERSIONINFOA aVersionInfo;
	aVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionExA(&aVersionInfo);

	aDebugDump += "Windows Ver: ";
	if (aVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		aDebugDump += "NT ";
	else
		aDebugDump += "9x ";

	char aVersionStr[20];
	sprintf(aVersionStr, "%d.%d", aVersionInfo.dwMajorVersion, aVersionInfo.dwMinorVersion);
	aDebugDump += aVersionStr;
	aDebugDump += " ";
	aDebugDump += aVersionInfo.szCSDVersion;
	aDebugDump += " ";
	sprintf(aVersionStr, "%d", aVersionInfo.dwBuildNumber);
	aDebugDump += "Build ";
	aDebugDump += aVersionStr;
	aDebugDump += "\n";
#endif

	if (mApp != NULL)
	{
		aDebugDump += StrFormat("SDL - %d.%d.%d\r\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);

#if SEXY_USE_OPENGL
		aDebugDump += StrFormat("OpenGL - %s\r\n", glGetString(GL_VERSION));
#endif
		aDebugDump += StrFormat("OpenAL - %s\r\n", alGetString(AL_VERSION));

	}

	return aDebugDump;
}
#endif