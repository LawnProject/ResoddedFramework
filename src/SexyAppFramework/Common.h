#ifndef __SEXYAPPFRAMEWORK_COMMON_H__
#define __SEXYAPPFRAMEWORK_COMMON_H__

#pragma warning(disable : 4786)
#pragma warning(disable : 4503)

//Libray Headers Start

#include <SDL3/SDL_timer.h>
#include <utf8.h>

//Libray Headers End

// Platform Headers Start

#if WIN32
#include <windows.h>
#include <shellapi.h>
#include <mmsystem.h>

#undef _WIN32_WINNT
#undef WIN32_LEAN_AND_MEAN

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#undef _UNICODE
#undef UNICODE

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef LoadImage
#undef LoadImage // Windows, i fucking hate you -Electr0Gunner
#endif

#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

// Platform Headers End

// STL Headers Start

#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <algorithm>
#include <cstdlib>
#include <cstdint>

// STL Headers End

#include "ModVal.h"

// SexyString Start

#if __clang__
#define stricmp _stricmp
#define strnicmp _strnicmp
#endif

typedef std::string SexyString;
#define _S(x) static_assert(true, "_S has been removed");

#define sexystrncmp strncmp
#define sexystrcmp strcmp
#define sexystricmp stricmp
#define sexysscanf sscanf
#define sexyatoi atoi
#define sexystrcpy strcpy

#define LONG_BIGE_TO_NATIVE(l)                                                                                         \
	(((l >> 24) & 0xFF) | ((l >> 8) & 0xFF00) | ((l << 8) & 0xFF0000) | ((l << 24) & 0xFF000000))
#define WORD_BIGE_TO_NATIVE(w) (((w >> 8) & 0xFF) | ((w << 8) & 0xFF00))
#define LONG_LITTLEE_TO_NATIVE(l) (l)
#define WORD_LITTLEE_TO_NATIVE(w) (w)

#define LENGTH(anyarray) (sizeof(anyarray) / sizeof(anyarray[0]))

typedef std::map<std::string, std::string> DefinesMap;
typedef SexyString::value_type SexyChar;
#define HAS_SEXYCHAR

// SexyString End

namespace Sexy
{

const uint32_t SEXY_RAND_MAX = 0x7FFFFFFF;

extern bool gDebug;

int Rand();
int Rand(int range);
float Rand(float range);
void SRand(uint32_t theSeed);
extern std::string vformat(const char *fmt, va_list argPtr);
extern std::string StrFormat(const char *fmt...);
std::string GetAppDataFolder();
void SetAppDataFolder(const std::string &thePath);
std::string URLEncode(const std::string &theString);
std::string StringToUpper(const std::string &theString);
std::string StringToLower(const std::string &theString);
SexyString StringToSexyString(const std::string &theString);
std::string SexyStringToString(const SexyString &theString);
std::string Upper(const std::string &theData);
std::string Lower(const std::string &theData);
std::string Trim(const std::string &theString);
bool StringToInt(const std::string theString, int *theIntVal);
bool StringToDouble(const std::string theString, double *theDoubleVal);
int StrFindNoCase(const char *theStr, const char *theFind);
bool StrPrefixNoCase(const char *theStr, const char *thePrefix, int maxLength = 10000000);
SexyString CommaSeperate(int theValue);
std::string Evaluate(const std::string &theString, const DefinesMap &theDefinesMap);
std::string XMLDecodeString(const std::string &theString);
std::string XMLEncodeString(const std::string &theString);

bool Deltree(const std::string &thePath);
bool FileExists(const std::string &theFileName);
void MkDir(const std::string &theDir);
std::string GetFileName(const std::string &thePath, bool noExtension = false);
std::string GetFileDir(const std::string &thePath, bool withSlash = false);
std::string RemoveTrailingSlash(const std::string &theDirectory);
std::string AddTrailingSlash(const std::string &theDirectory, bool backSlash = false);
uint64_t GetLastWriteFileDate(const std::string &theFileName);
std::string GetCurDir();
std::string GetFullPath(const std::string &theRelPath);
std::string GetPathFrom(const std::string &theRelPath, const std::string &theDir);
bool AllowAllAccess(const std::string &theFileName);

uint64_t GetTicks();

// Read memory and then move the pointer
void SMemR(void *&_Src, void *_Dst, size_t _Size);
void SMemRStr(void *&_Src, std::string &theString);

// Write memory and then move the pointer
void SMemW(void *&_Dst, const void *_Src, size_t _Size);
void SMemWStr(void *&_Dst, const std::string &theString);

inline void inlineUpper(std::string &theData)
{
	//std::transform(theData.begin(), theData.end(), theData.begin(), toupper);

	int aStrLen = (int)theData.length();
	for (int i = 0; i < aStrLen; i++)
	{
		theData[i] = toupper(theData[i]);
	}
}

inline void inlineLower(std::string &theData)
{
	std::transform(theData.begin(), theData.end(), theData.begin(), tolower);
}


inline void inlineLTrim(std::string &theData, const std::string &theChars = " \t\r\n")
{
	theData.erase(0, theData.find_first_not_of(theChars));
}

inline void inlineRTrim(std::string &theData, const std::string &theChars = " \t\r\n")
{
	theData.resize(theData.find_last_not_of(theChars) + 1);
}

inline void inlineTrim(std::string &theData, const std::string &theChars = " \t\r\n")
{
	inlineRTrim(theData, theChars);
	inlineLTrim(theData, theChars);
}

struct StringLessNoCase
{
	bool operator()(const std::string &s1, const std::string &s2) const
	{
		return _stricmp(s1.c_str(), s2.c_str()) < 0;
	}
};

} // namespace Sexy

#endif //__SEXYAPPFRAMEWORK_COMMON_H__
