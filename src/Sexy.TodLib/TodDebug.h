#ifndef __TODDEBUG_H__
#define __TODDEBUG_H__

#ifdef _WIN32
#include <Windows.h>
#endif

class TodHesitationBracket
{
  public:
	char mMessage[256];
	int mBracketStartTime;

  public:
	TodHesitationBracket(const char *theFormat, ...)
	{
		;
	}
	~TodHesitationBracket()
	{
		;
	}

	inline void EndBracket()
	{
		;
	}
};

void TodLog(const char *theFormat, ...);
void TodLogString(const char *theMsg);
void TodTrace(const char *theFormat, ...);
void TodTraceMemory();
void TodTraceAndLog(const char *theFormat, ...);
void TodTraceWithoutSpamming(const char *theFormat, ...);
void TodHesitationTrace(...);
#if defined(_WIN32) && defined(SEXY_CRASH_HANDLER)
void TodReportError(LPEXCEPTION_POINTERS exceptioninfo, const char *theMessage);
long __stdcall TodUnhandledExceptionFilter(LPEXCEPTION_POINTERS exceptioninfo);
#endif
void TodAssertFailed(const char *theCondition, const char *theFile, int theLine, const char *theMsg = "", ...);
/*inline*/ void TodErrorMessageBox(const char *theMessage, const char *theTitle);

/*inline*/ void *TodMalloc(int theSize);
/*inline*/ void TodFree(void *theBlock);
void TodAssertInitForApp();

extern void (*gBetaSubmitFunc)();

#ifndef WIN32
#define SexyDebuggerCheck() false //The best i can do till i see linux support
#else
#define SexyDebuggerCheck() IsDebuggerPresent()
#endif

#ifdef _DEBUG
#define TOD_ASSERT(condition, ...)                                                                                     \
	{                                                                                                                  \
		if (!bool(condition))                                                                                          \
		{                                                                                                              \
			TodAssertFailed("" #condition, __FILE__, __LINE__, ##__VA_ARGS__);                                         \
			if (SexyDebuggerCheck())                                                                                   \
			{                                                                                                          \
				__debugbreak();                                                                                        \
			}                                                                                                          \
			TodTraceMemory();                                                                                          \
		}                                                                                                              \
	}
#else
#define TOD_ASSERT(condition, ...)
#endif

#endif
