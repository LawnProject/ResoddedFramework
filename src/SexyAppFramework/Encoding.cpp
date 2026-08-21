#include "Encoding.h"
#include "Common.h"

namespace Sexy
{
// Convert a UTF-16 byte buffer to UTF-8. Return nullopt if input is not valid UTF-16
std::optional<std::string> TryUtf16ToUtf8(const unsigned char *theBytes, std::size_t theByteCount, bool isBigEndian)
{
	int aUnitCount = theByteCount / 2;
	std::u16string aWide;
	aWide.reserve(aUnitCount);
	for (int i = 0; i < aUnitCount; i++)
	{
		auto aFirst = theBytes[i * 2];
		auto aSecond = theBytes[i * 2 + 1];
		char16_t aUnit = isBigEndian ? (char16_t)((aFirst << 8) | aSecond) : (char16_t)((aSecond << 8) | aFirst);
		aWide.push_back(aUnit);
	}

	try
	{
		std::string aResult;
		utf8::utf16to8(aWide.begin(), aWide.end(), std::back_inserter(aResult));
		return std::move(aResult);
	}
	catch (...)
	{
		return {};
	}
}

// Decode raw file bytes to UTF-8, auto-detecting the encoding and using ANSI
// as a fallback. Returning nullopt means the input is already valid UTF8 without BOM
std::optional<std::string> ConvertToUtf8IfNeeded(const std::string &theRaw)
{
	auto aSize = theRaw.size();
	auto aBytes = reinterpret_cast<const unsigned char *>(theRaw.data());

	// Try UTF-8 first - modern devs should use this
	if (aSize >= 3 && aBytes[0] == 0xEF && aBytes[1] == 0xBB && aBytes[2] == 0xBF)
	{
		// strip BOM if present: EF BB BF
		// - used by font descriptors in chinese versions
		std::string aUtf8 = theRaw.substr(3);
		if (utf8::is_valid(aUtf8))
		{
			return aUtf8;
		}
		return ANSIToUTF8(theRaw);
	}

	// UTF-8 without BOM
	if (utf8::is_valid(theRaw))
	{
		return {};
	}

	// Try UTF-16
	if (aSize >= 2)
	{ // need at least 2 bytes to make one char16
		// UTF-16 with BOM
		if (aBytes[0] == 0xFF && aBytes[1] == 0xFE)
		{
			// LE - the chinese version of the original game uses this
			auto aMaybeUtf16 = TryUtf16ToUtf8(aBytes + 2, aSize - 2, false);
			if (aMaybeUtf16)
			{
				return std::move(*aMaybeUtf16);
			}
		}

		if (aBytes[0] == 0xFE && aBytes[1] == 0xFF)
		{
			// BE
			auto aMaybeUtf16 = TryUtf16ToUtf8(aBytes + 2, aSize - 2, true);
			if (aMaybeUtf16)
			{
				return std::move(*aMaybeUtf16);
			}
		}

		// we don't handle UTF-16 without BOM for now
	}

	// Fallback - this is what the english version uses
	return ANSIToUTF8(theRaw);
}
} // namespace Sexy
