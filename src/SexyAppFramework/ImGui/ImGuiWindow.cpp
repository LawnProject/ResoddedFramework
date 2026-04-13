#if SEXY_USE_IMGUI

#include "ImGuiWindow.h"

using namespace Sexy;

ImGuiWindow::ImGuiWindow(ImGuiManager *theManager) : mManager(theManager)
{
	mEnabled = true;
	mManager->mWindows.push_back(this);
}

ImGuiWindow::~ImGuiWindow()
{
	mManager->mWindows.erase(std::remove(mManager->mWindows.begin(), mManager->mWindows.end(), this), mManager->mWindows.end());
}

void ImGuiWindow::Update()
{

}

#endif