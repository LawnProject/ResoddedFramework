#if SEXY_CRASH_HANDLER
#pragma once

#include "Common.h"
#include "HTTPTransfer.h"

namespace Sexy
{

class SexyAppBase;

class SEHCatcher
{
  public:
	static SexyAppBase *mApp;
	static bool mHasDemoFile;
	static bool mDebugError;
	static std::string mErrorTitle;
	static std::string mErrorText;
	static std::string mUserText;
	static std::string mUploadFileName;
	static std::string mCrashMessage;
	static std::string mSubmitHost;
	static std::string mSubmitErrorMessage;
	static std::string mSubmitMessage;
	static bool mExiting;
	static bool mShowUI;
	static bool mAllowSubmit;

  protected:
#if WIN32
	static LPTOP_LEVEL_EXCEPTION_FILTER mPreviousFilter;
#endif


  public:
	static void SEHWindowProc();
#if WIN32
	static long __stdcall UnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptPtr);
	static void DoHandleDebugEvent(LPEXCEPTION_POINTERS lpEP);
#endif
	static std::string GetFilename(const std::string &thePath);
	static void WriteToFile(const std::string &theErrorText);
	static void ShowErrorDialog(const std::string &theErrorTitle, const std::string &theErrorText);
	static std::string GetSysInfo();

  public:
	SEHCatcher();
	~SEHCatcher();
};

extern SEHCatcher gSEHCatcher;

} // namespace Sexy

#endif