#include "TodDebug.h"
#include "TodCommon.h"
#include "../SexyAppFramework/Debug.h"
#include "../SexyAppFramework/SEHCatcher.h"
#include "../SexyAppFramework/SexyAppBase.h"
#include <sstream>
#include <chrono>
#include <iomanip>
#include <SDL3/SDL_messagebox.h>

using namespace Sexy;

static char gLogFileName[1024];
static char gDebugDataFolder[1024];

//0x514EA0
void TodErrorMessageBox(const char *theMessage, const char *theTitle)
{
	TodTraceAndLog("[TodLib] - %s.%s", theMessage, theTitle);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, theTitle, theMessage, nullptr);
}

void TodTraceMemory()
{
}

void *TodMalloc(int theSize)
{
	TOD_ASSERT(theSize > 0);
	return malloc(theSize);
}

void TodFree(void *theBlock)
{
	if (theBlock != nullptr)
	{
		free(theBlock);
	}
}

void TodAssertFailed(const char *theCondition, const char *theFile, int theLine, const char *theMsg, ...)
{
#ifdef _DEBUG
	char aFormattedMsg[1024];
	va_list argList;
	va_start(argList, theMsg);
	int aCount = TodVsnprintf(aFormattedMsg, sizeof(aFormattedMsg), theMsg, argList);
	va_end(argList);

	if (aFormattedMsg[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aFormattedMsg[aCount] = '\n';
			aFormattedMsg[aCount + 1] = '\0';
		}
		else
		{
			aFormattedMsg[aCount - 1] = '\n';
		}
	}

	char aBuffer[1024];
	if (*theCondition != '\0')
	{
		TodSnprintf(aBuffer,
					sizeof(aBuffer),
					"\n%s(%d)\nassertion failed: '%s'\n%s\n",
					theFile,
					theLine,
					theCondition,
					aFormattedMsg);
	}
	else
	{
		TodSnprintf(aBuffer, sizeof(aBuffer), "\n%s(%d)\nassertion failed: %s\n", theFile, theLine, aFormattedMsg);
	}
	TodTrace("[TodLib] - %s", aBuffer);

	if (!SexyDebuggerCheck())
	{
		if (gInAssert)
		{
			TodLog("[TodLib] - Assert during exception processing");
			exit(0);
		}

		gInAssert = true;
#if WIN32
		LPEXCEPTION_POINTERS exp;

		__try
		{
			RaiseException(EXCEPTION_NONCONTINUABLE_EXCEPTION, NULL, NULL, NULL);
		}
		__except (exp = GetExceptionInformation(), EXCEPTION_CONTINUE_EXECUTION)
		{
			TodReportError(exp, aFormattedMsg);
		}
#endif
		gInAssert = false;
		exit(0);
	}
#endif
}

void TodLog(const char *theFormat, ...)
{
#ifdef _DEBUG
	char aButter[1024];
	va_list argList;
	va_start(argList, theFormat);
	int aCount = TodVsnprintf(aButter, sizeof(aButter), theFormat, argList);
	va_end(argList);

	if (aButter[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aButter[aCount] = '\n';
			aButter[aCount + 1] = '\0';
		}
		else
		{
			aButter[aCount - 1] = '\n';
		}
	}

	TodLogString(aButter);
#endif
}

void TodLogString(const char *theMsg)
{
#ifdef _DEBUG
	FILE *f = fopen(gLogFileName, "a");
	if (f == nullptr)
	{
		printf("[TodLib] - Failed to open log file\n");
	}

	if (f && fwrite(theMsg, strlen(theMsg), 1, f) != 1)
	{
		printf("[TodLib] - Failed to write to log file\n");
	}

	if (f)
		fclose(f);
#endif
}

void TodTrace(const char *theFormat, ...)
{
#ifdef _DEBUG
	char aButter[1024];
	va_list argList;
	va_start(argList, theFormat);
	int aCount = TodVsnprintf(aButter, sizeof(aButter), theFormat, argList);
	va_end(argList);

	if (aButter[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aButter[aCount] = '\n';
			aButter[aCount + 1] = '\0';
		}
		else
		{
			aButter[aCount - 1] = '\n';
		}
	}

	printf("%s", aButter);

#endif
}

void TodHesitationTrace(...)
{
}

void TodTraceAndLog(const char *theFormat, ...)
{
#ifdef _DEBUG
	char aButter[1024];
	va_list argList;
	va_start(argList, theFormat);
	int aCount = TodVsnprintf(aButter, sizeof(aButter), theFormat, argList);
	va_end(argList);

	if (aButter[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aButter[aCount] = '\n';
			aButter[aCount + 1] = '\0';
		}
		else
		{
			aButter[aCount - 1] = '\n';
		}
	}

	printf(aButter);
	TodLogString(aButter);
#endif
}

void TodTraceWithoutSpamming(const char *theFormat, ...)
{
#ifdef _DEBUG
	static uint64_t gLastTraceTime = 00;
	uint64_t aTime = std::time(nullptr);
	if (aTime < gLastTraceTime)
	{
		return;
	}

	gLastTraceTime = aTime;
	char aButter[1024];
	va_list argList;
	va_start(argList, theFormat);
	int aCount = TodVsnprintf(aButter, sizeof(aButter), theFormat, argList);
	va_end(argList);

	if (aButter[aCount - 1] != '\n')
	{
		if (aCount + 1 < 1024)
		{
			aButter[aCount] = '\n';
			aButter[aCount + 1] = '\0';
		}
		else
		{
			aButter[aCount - 1] = '\n';
		}
	}

	printf(aButter);
#endif
}

#if WIN32
void TodReportError(LPEXCEPTION_POINTERS exceptioninfo, const char *theMessage)
{
	Sexy::SEHCatcher::UnhandledExceptionFilter(exceptioninfo);
}


long __stdcall TodUnhandledExceptionFilter(LPEXCEPTION_POINTERS exceptioninfo)
{
	if (gInAssert)
	{
		TodLog("[TodLib] - Exception during exception processing");
	}
	else
	{
		gInAssert = true;
		TodLog("\n[TodLib] - Unhandled Exception");
		TodReportError(exceptioninfo, "[TodLib] - Unhandled Exception");
		gInAssert = false;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void (*gBetaSubmitFunc)() = nullptr;

void TodAssertInitForApp()
{
	std::chrono::system_clock::time_point aNow = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(aNow);
	std::tm aLocalTime = *std::localtime(&t);

	std::ostringstream aOss;
	aOss << std::put_time(&aLocalTime, "%d-%m-%Y-%H.%M");
	std::string aTimestamp = aOss.str();

	MkDir("logs");
	std::string aRelativeUserPath = "logs/";
	strcpy(gDebugDataFolder, GetFullPath(aRelativeUserPath).c_str());
	strcpy(gLogFileName, gDebugDataFolder);
	std::string aFormatted = StrFormat("log-%s.txt", aTimestamp.c_str());
	strcat(gLogFileName, aFormatted.c_str());

	TOD_ASSERT(strlen(gLogFileName) < 1024);

	std::time_t aclock = std::time(nullptr);
	TodLog("[TodLib] - Started %s\n", std::asctime(std::localtime(&aclock)));
#if WIN32
	SetUnhandledExceptionFilter(TodUnhandledExceptionFilter);
#endif
}
