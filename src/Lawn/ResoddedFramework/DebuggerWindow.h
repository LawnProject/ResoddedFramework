#if LAWN_DEBUG_TOOLS

#pragma once

#include <ImGui/ImGuiWindow.h>
#include "../BoardInclude.h"

using namespace Sexy;

class LawnApp;

class DebuggerWindow : public ImGuiWindow
{
  public:
	DebuggerWindow(LawnApp* theApp);
	~DebuggerWindow();

	virtual void Update();

  private:
	LawnApp *mApp;
};


#endif