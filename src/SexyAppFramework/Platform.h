#pragma once

// Use this include instead of <windows.h> directly
// to avoid conflicts

#ifdef _WIN32
#include <windows.h>

// NOTE: this comment exists to ensure formatters don't put <mmsystem.h>
// above <windows.h>, which causes a very obsecure error

#include <shellapi.h>
#include <mmsystem.h>
#include <accctrl.h>

// Unfuck windows

#undef _WIN32_WINNT
#undef WIN32_LEAN_AND_MEAN

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#undef _UNICODE
#undef UNICODE

#ifdef LoadImage
#undef LoadImage // Windows, i fucking hate you -Electr0Gunner
#endif

#endif

// timeGetTime() is a Win32 multimedia timer. SDL3 exposes the equivalent
// millisecond tick counter on every platform, so provide it as a shim
// rather than rewriting every call site.
#ifndef _WIN32
#include <SDL3/SDL_timer.h>
static inline uint32_t timeGetTime()
{
	return (uint32_t)SDL_GetTicks();
}
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
