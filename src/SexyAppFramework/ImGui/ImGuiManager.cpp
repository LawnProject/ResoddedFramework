#if SEXY_USE_IMGUI

#include "ImGuiManager.h"
#include "ImGuiWindow.h"
#include "SexyAppBase.h"
#include "Window.h"
#if SEXY_USE_OPENGL
#include "OpenGL/OpenGLRenderer.h"
#endif

using namespace Sexy;

ImGuiManager::ImGuiManager(SexyAppBase *theApp)
{
	mApp = theApp;
	mHasInitiated = false;
}

ImGuiManager::~ImGuiManager()
{
	Reset();
}

void ImGuiManager::Frame()
{
	switch (mApp->mRenderer->mCurrentBackend)
	{
#if SEXY_USE_OPENGL
		case RenderingBackend::BACKEND_OPENGL: 
		{
			ImGui_ImplOpenGL3_NewFrame();
			break;
		}
#endif
	}
	ImGui_ImplSDL3_NewFrame();

	ImGui::NewFrame();
	for (const auto &entry : mWindows)
	{
		if (!entry->mEnabled)
			continue;

		entry->Update();
	}
	ImGui::Render();
}

void ImGuiManager::Flush()
{
	switch (mApp->mRenderer->mCurrentBackend)
	{
#if SEXY_USE_OPENGL
		case RenderingBackend::BACKEND_OPENGL: 
		{

			{
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				ImGuiIO &io = ImGui::GetIO();
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					SDL_Window *aBKWindow = SDL_GL_GetCurrentWindow();
					SDL_GLContext aBKContext = SDL_GL_GetCurrentContext();

					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();

					SDL_GL_MakeCurrent(aBKWindow, aBKContext);
				}
			}
		
		break;

		}
#endif
	}
}

void ImGuiManager::Reset()
{
	mHasInitiated = false;

	switch (mApp->mRenderer->mCurrentBackend)
	{
#if SEXY_USE_OPENGL
		case RenderingBackend::BACKEND_OPENGL: 
		{
			ImGui_ImplOpenGL3_Shutdown();
			break;

		}
#endif
		default:
			mApp->Popup("INVALID RENDERING BACKEND GIVEN TO IMGUI");
			break;
	}
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Init()
{
	if (mHasInitiated)
		Reset();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
#if SEXY_USE_OPENGL
	if (mApp->mRenderer->mCurrentBackend == RenderingBackend::BACKEND_OPENGL)
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	(void)io;
	ImGui::StyleColorsDark();
	switch (mApp->mRenderer->mCurrentBackend)
	{
#if SEXY_USE_OPENGL
		case RenderingBackend::BACKEND_OPENGL:
		{
			{
				OpenGLRenderer *aRenderer = static_cast<OpenGLRenderer *>(mApp->mRenderer);
				ImGui_ImplSDL3_InitForOpenGL(mApp->mWindow->mInternalWindow, aRenderer->mContext);
				ImGui_ImplOpenGL3_Init();
				break;
			}
		}
#endif
		default:
			mApp->Popup("INVALID RENDERING BACKEND GIVEN TO IMGUI");
			break;
	}
	mHasInitiated = true;
}


#endif