#pragma once

#include <SDL3/SDL.h>
#include "Common.h"

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

        #ifdef WIN32
            HWND GetHWND();
        #endif
    };

} // namespace Sexy
