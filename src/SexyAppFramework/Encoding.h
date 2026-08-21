#pragma once

#include <optional>
#include <string>

namespace Sexy
{

// Convert a UTF-16 byte buffer to UTF-8. Return nullopt if input is not valid UTF-16
std::optional<std::string> TryUtf16ToUtf8(const unsigned char *theBytes, std::size_t theByteCount, bool isBigEndian);

// Convert a buffer to UTF-8 (wihout BOM) if it's not already. Return nullopt if the input is already UTF-8 (without BOM)
// Supports UTF-16 LE/BE with BOM. Fallback to ANSI
std::optional<std::string> ConvertToUtf8IfNeeded(const std::string &theRaw);

} // namespace Sexy
