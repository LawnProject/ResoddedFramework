#if SEXY_USE_IMGUI

#include "ImGuiManager.h"
#include "ImGuiWindow.h"
#include "SexyAppBase.h"
#include "Window.h"
#if SEXY_USE_OPENGL
#include "OpenGLRenderer.h"
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
		case RenderingBackend::BACKEND_OPENGL: 
		{
#if SEXY_USE_OPENGL
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		break;
#endif
		}
	}

	ImGui::NewFrame();
	for (const auto &entry : mWindows)
	{
		if (entry->mEnabled)
			entry->Update();
	}
	ImGui::Render();
}

void ImGuiManager::Flush()
{
	switch (mApp->mRenderer->mCurrentBackend)
	{
		case RenderingBackend::BACKEND_OPENGL: 
		{
#if SEXY_USE_OPENGL
			{
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				ImGuiIO &io = ImGui::GetIO();
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					SDL_Window *aBackupWindow = SDL_GL_GetCurrentWindow();
					SDL_GLContext aBackupContext = SDL_GL_GetCurrentContext();

					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();

					SDL_GL_MakeCurrent(aBackupWindow, aBackupContext);
				}
			}
		
		break;
#endif
		}
	}
}

void ImGuiManager::Reset()
{
	mHasInitiated = false;

	switch (mApp->mRenderer->mCurrentBackend)
	{
		case RenderingBackend::BACKEND_OPENGL: 
		{
#if SEXY_USE_OPENGL
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL3_Shutdown();
			ImGui::DestroyContext();
			break;
#endif
			default:
				mApp->Popup("INVALID RENDERING BACKEND GIVEN TO IMGUI");
				break;
		}
	}
}

void ImGuiManager::Init()
{
	if (mHasInitiated)
		Reset();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	(void)io;
	ImGui::StyleColorsDark();
	switch (mApp->mRenderer->mCurrentBackend)
	{
		case RenderingBackend::BACKEND_OPENGL:
		{
#if SEXY_USE_OPENGL
			{
				OpenGLRenderer *aRenderer = static_cast<OpenGLRenderer *>(mApp->mRenderer);
				ImGui_ImplSDL3_InitForOpenGL(mApp->mWindow->mInternalWindow, aRenderer->mContext);
				ImGui_ImplOpenGL3_Init();
				break;
			}
#endif
		default:
			mApp->Popup("INVALID RENDERING BACKEND GIVEN TO IMGUI");
			break;
		}
	}
	mHasInitiated = true;
}


#endif