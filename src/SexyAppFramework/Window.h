#pragma once

#include <SDL3/SDL.h>
#include "Common.h"

#if WIN32
#include <windef.h>
#endif

namespace Sexy
{
    class SexyAppBase;

    class Window
    {
    public:
        SexyAppBase* mApp;
        SDL_Window* mInternalWindow;
    public:
        Window(SexyAppBase* theApp);
        ~Window();

        #if WIN32
            HWND GetHWND();
        #endif
    };

} // namespace Sexy
