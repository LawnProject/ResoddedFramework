#if SEXY_USE_IMGUI

#pragma once

#include "ImGuiManager.h"

namespace Sexy
{

class ImGuiWindow
{
  public:
	ImGuiWindow(ImGuiManager *theManager);

	~ImGuiWindow();

	virtual void Update();

  public:
	bool mEnabled;

  private:
	ImGuiManager *mManager;
};

}

#endif