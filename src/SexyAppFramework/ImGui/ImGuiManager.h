#if SEXY_USE_IMGUI

#pragma once

#include <vector>
#include "Renderer.h" 
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#if SEXY_USE_OPENGL
#include <imgui/imgui_impl_opengl3.h>
#endif

namespace Sexy
{

class SexyAppBase;
class ImGuiWindow;

class ImGuiManager
{
	public:
	ImGuiManager(SexyAppBase* theApp);
	~ImGuiManager();
	void Reset();
	void Frame();
	void Flush();

	void Init();

	public:
	  std::vector<ImGuiWindow*> mWindows;
	  bool mHasInitiated;

	private:
	SexyAppBase *mApp;
};

}

#endif