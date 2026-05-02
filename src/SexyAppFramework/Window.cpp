#include "Window.h"
#include "SexyAppBase.h"

using namespace Sexy;

    
Window::Window(SexyAppBase* theApp)
{
    mApp = theApp;

	mInternalWindow = SDL_CreateWindow(mApp->mTitle.c_str(), mApp->mWidth, mApp->mHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | (mApp->mIsWindowed ? 0 : SDL_WINDOW_FULLSCREEN));
}

Window::~Window()
{
    SDL_DestroyWindow(mInternalWindow);
}

#if WIN32
 
HWND Window::GetHWND()
{
    SDL_PropertiesID props = SDL_GetWindowProperties(mInternalWindow);
    HWND hwnd = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    return hwnd;
}

#endif