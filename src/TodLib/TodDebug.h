#ifndef __TODDEBUG_H__
#define __TODDEBUG_H__

#include <SexyAppFramework/Platform.h> // for Windows - do not remove
#include <SexyAppFramework/Common.h>

class TodHesitationBracket
{
  public:
	SexyString mMessage;
	uint64_t mBracketStartTime;
	uint64_t mTraceDuration;

  public:
	TodHesitationBracket(const char *theFormat, ...);
	~TodHesitationBracket();
};

void TodLog(const char *theFormat, ...);
void TodLogString(const char *theMsg);
void TodTrace(const char *theFormat, ...);
void TodTraceMemory();
void TodTraceAndLog(const char *theFormat, ...);
void TodTraceWithoutSpamming(const char *theFormat, ...);
#if defined(_WIN32) && defined(SEXY_CRASH_HANDLER)
void TodReportError(LPEXCEPTION_POINTERS exceptioninfo, const char *theMessage);
long __stdcall TodUnhandledExceptionFilter(LPEXCEPTION_POINTERS exceptioninfo);
#endif
void TodAssertFailed(const char *theCondition, const char *theFile, int theLine, const char *theMsg = "", ...);
void TodErrorMessageBox(const char *theMessage, const char *theTitle);

void *TodMalloc(int theSize);
void TodFree(void *theBlock);
void TodAssertInitForApp();

extern void (*gBetaSubmitFunc)();

#ifdef WIN32
#define SexyDebuggerCheck() IsDebuggerPresent()
#else
#define SexyDebuggerCheck() false //The best i can do till i see linux support
#endif

#ifdef _DEBUG
#define TOD_ASSERT(condition, ...)                                             \
	{                                                                          \
		if (!bool(condition))                                                  \
		{                                                                      \
			TodAssertFailed("" #condition, __FILE__, __LINE__, ##__VA_ARGS__); \
			if (SexyDebuggerCheck())                                           \
			{                                                                  \
				__debugbreak();                                                \
			}                                                                  \
			TodTraceMemory();                                                  \
		}                                                                      \
	}
#else
#define TOD_ASSERT(condition, ...)
#endif

#endif
