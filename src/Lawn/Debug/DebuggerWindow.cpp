#if LAWN_DEBUG_TOOLS

#include "DebuggerWindow.h"
#include "../Board.h"
#include "../Widget/ChallengeScreen.h"
#include "../Challenge.h"
#include "../../LawnApp.h"
#include "../../Sexy.TodLib/TodStringFile.h"

using namespace Sexy;

DebuggerWindow::DebuggerWindow(LawnApp *theApp) : ImGuiWindow(theApp->mImGuiManager)
{
	mApp = theApp;
}

DebuggerWindow::~DebuggerWindow()
{
	ImGuiWindow::~ImGuiWindow();
}

void DebuggerWindow::Update()
{
	ImGui::Begin("LawnTools");
	if (ImGui::BeginTabBar("LawnTabs"))
	{
		if (ImGui::BeginTabItem("Board"))
		{
			if (mApp->mBoard == nullptr)
			{
				ImGui::Text("Not currently on Board");
			}
			else
			{
				ImGui::Text("Currently on Board");
				ImGui::SeparatorText("Challenge");
				ImGui::Text("Current Challenge: %s", TodStringTranslate(mApp->GetCurrentChallengeDef().mChallengeName).c_str());

			}
				
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Save File"))
		{
			ImGui::Text("Set some data here");
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	
	ImGui::End();
}

#endif