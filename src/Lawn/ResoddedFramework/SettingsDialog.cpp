#include "SettingsDialog.h"
#include "../../Resources.h"
#include "../../SexyAppFramework/Font.h"
#include "../../SexyAppFramework/Renderer.h"
#include "../../SexyAppFramework/Checkbox.h"
#include "../../LawnApp.h"
#include "../../SexyAppFramework/GitData.h"
#if WIN32
#include <ShlObj_core.h>
#include <locale>
#include <codecvt>
#endif

SettingsDialog::SettingsDialog(LawnApp *theApp)
	: LawnDialog(theApp, DIALOG_SETTINGS, true, "[SETTINGS_HEADER]", "", "", BUTTONS_NONE)
{
	mApp = theApp;
	mOptionsSlider = new LawnSlider(mApp);
	mOptionsSlider->mSliderHeightPercent = 0.57f;
	mOptionsSlider->mStepMultiplier = 1.0f;
	mOptionsSlider->Resize(500, 90, 8, 140);
	mOptionsSlider->mScrollMultiplier = 0.09f;

	mFullscreenCheckbox = MakeNewCheckbox(SettingsDialog::SETTINGS_FULLSCREEN, this, !theApp->mIsWindowed);

	mVSyncCheckbox = MakeNewCheckbox(SettingsDialog::SETTINGS_VSYNC, this, theApp->mWaitForVSync);

	mSaveFileButton = MakeButton(SETTINGS_OPEN_SAVE_FOLDER, this, "[SETTINGS_OPEN_SAVE_FOLDER]");

	mApplyButton = MakeButton(SETTINGS_BACK, this, "[SETTINGS_BACK]");
	CalcSize(211, 214);
}

SettingsDialog::~SettingsDialog()
{
	delete mOptionsSlider;
	delete mApplyButton;
	delete mSaveFileButton;
	delete mVSyncCheckbox;
	delete mFullscreenCheckbox;
}

void SettingsDialog::Draw(Graphics* g)
{
	LawnDialog::Draw(g);

	int aMaxContentHeight = 600;
	float aMaxScroll = std::max(0.0f, (float)aMaxContentHeight - mOptionsSlider->mAllowedMouseZone.mHeight);

	float aScrollOffset = mOptionsSlider->GetValue() * aMaxScroll;

	g->PushState();
	g->Translate(mApplyButton->mX, mApplyButton->mY);
	mApplyButton->Draw(g);
	g->PopState();
	g->PushState();
	g->SetClipRect(Rect(mOptionsSlider->mAllowedMouseZone.mX - mX,
						mOptionsSlider->mAllowedMouseZone.mY - mY,
						mOptionsSlider->mAllowedMouseZone.mWidth,
						mOptionsSlider->mAllowedMouseZone.mHeight));
	g->Translate(35, 120 - aScrollOffset);


	mVSyncCheckbox->Resize(40, 140 - aScrollOffset, 46, 45);
	mVSyncCheckbox->mDisabled = (mVSyncCheckbox->mY + mY) < mOptionsSlider->mAllowedMouseZone.mY;
	mFullscreenCheckbox->Resize(40, 190 - aScrollOffset, 46, 45);
	mFullscreenCheckbox->mDisabled = (mFullscreenCheckbox->mY + mY) < mOptionsSlider->mAllowedMouseZone.mY;

	TodDrawString(g, "[SETTINGS_VIDEO]", 20, 10, Sexy::FONT_BRIANNETOD12, Color::White,
				  DrawStringJustification::DS_ALIGN_LEFT);

	TodDrawString(g, "[SETTINGS_VSYNC]", mVSyncCheckbox->mX + 20, 40, Sexy::FONT_BRIANNETOD12, Color::White, DrawStringJustification::DS_ALIGN_LEFT);
	TodDrawString(g, "[SETTINGS_FULLSCREEN]", mFullscreenCheckbox->mX + 20, 90, Sexy::FONT_BRIANNETOD12, Color::White, DrawStringJustification::DS_ALIGN_LEFT);

	mSaveFileButton->Resize(40, 260 - aScrollOffset, 330, 46);
	mSaveFileButton->mDisabled = (mSaveFileButton->mY + mY) < mOptionsSlider->mAllowedMouseZone.mY;

	TodDrawString(g, "[SETTINGS_MISC]", 20, 130, Sexy::FONT_BRIANNETOD12, Color::White, DrawStringJustification::DS_ALIGN_LEFT);

	SexyString aVersionString = "ResoddedFramework " + LawnApp::gResoddedVersion.toString();
	TodDrawString(g, aVersionString, 
					mOptionsSlider->mAllowedMouseZone.mWidth - Sexy::FONT_BRIANNETOD12->StringWidth(aVersionString) - 27,
					aMaxContentHeight - Sexy::FONT_BRIANNETOD12->GetHeight(),
					Sexy::FONT_BRIANNETOD12, 
					Color::White, 
					DrawStringJustification::DS_ALIGN_LEFT);
#if GIT_AVAILABLE
	SexyString aHash = GIT_HASH;
	SexyString aGitString = "Git: Hash (" + aHash + ")" + (GIT_IS_DIRTY ? " WORK IN PROGRESS" : "");
	TodDrawString(g, aGitString, 
					mOptionsSlider->mAllowedMouseZone.mWidth - Sexy::FONT_BRIANNETOD12->StringWidth(aGitString) - 27,
					aMaxContentHeight,
					Sexy::FONT_BRIANNETOD12,
					Color::White, DrawStringJustification::DS_ALIGN_LEFT);
#endif
	g->PopState();
}

void SettingsDialog::AddedToManager(WidgetManager *theWidgetManager)
{
	LawnDialog::AddedToManager(theWidgetManager);
	AddWidget(mOptionsSlider);
	AddWidget(mApplyButton);
	AddWidget(mVSyncCheckbox);
	AddWidget(mFullscreenCheckbox);
	AddWidget(mSaveFileButton);
}

//0x45D8E0
void SettingsDialog::RemovedFromManager(WidgetManager *theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mOptionsSlider);
	RemoveWidget(mApplyButton);
	RemoveWidget(mVSyncCheckbox);
	RemoveWidget(mFullscreenCheckbox);
	RemoveWidget(mSaveFileButton);
}

void SettingsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	mOptionsSlider->Resize(mWidth - 60, 110, 8, 200);
	mOptionsSlider->mAllowedMouseZone = Rect(mX + 35, mY + 120, mWidth - 70, mHeight - 240);
	mApplyButton->Resize(40, 331, 209, 46);
	SetWidgetClipping(Rect(35, 120, mWidth - 70, mHeight - 240));
}

void SettingsDialog::MouseWheel(int theDelta)
{
	LawnDialog::MouseWheel(theDelta);
	mOptionsSlider->MouseWheel(theDelta);
}

void SettingsDialog::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_GRAVEBUTTON);
}

void SettingsDialog::ButtonDepress(int theId)
{
	switch (theId)
	{
		case SettingsDialog::SETTINGS_BACK:
		{
			mApp->KillDialog(mId);
			break;
		}
		case SettingsDialog::SETTINGS_OPEN_SAVE_FOLDER:
		{
			SexyString aSaveFileFolder = GetAppDataFolder();
			#if WIN32
			ShellExecuteA(NULL, "open", aSaveFileFolder.c_str(), NULL, NULL, SW_SHOWDEFAULT);
			#else
			SexyString aFailString = StrFormat("Couldn't open the folder on this platform.\nThe path is: \n%s", aSaveFileFolder.c_str());

			mApp->DoDialog(Dialogs::DIALOG_INFO, true, "Failed", aFailString, "OK", Dialog::BUTTONS_FOOTER);
			#endif
			break;
		}
	}
}


void SettingsDialog::CheckboxChecked(int theId, bool checked)
{
	switch (theId)
	{
	case SettingsDialog::SETTINGS_VSYNC:
		{
			mApp->mWaitForVSync = checked;
			RendererError anError = mApp->mRenderer->UpdateVSync();
			if (anError == RendererError::ERROR_VSYNC)
			{
				mVSyncCheckbox->SetChecked(!checked, false);
				SexyString aFailString = StrFormat("V-Sync couldn't be toggled %s\n\nYour video card does not\nmeet the "
												   "minimum requirements\nfor this feature.",
												   (checked ? "on" : "off"));
				mApp->DoDialog(Dialogs::DIALOG_INFO, true, "Failed", aFailString, "OK", Dialog::BUTTONS_FOOTER);
			}
		}
		break;
	case SettingsDialog::SETTINGS_FULLSCREEN:
		if (!checked && mApp->mForceFullscreen)
		{
			mApp->DoDialog(Dialogs::DIALOG_COLORDEPTH_EXP, true, "No Windowed Mode",
						   "Windowed mode is only available if your desktop was running in either\n"
						   "16 bit or 32 bit color mode when you started the game.\n\n"
						   "If you'd like to run in Windowed mode then you need to quit the game and switch your "
						   "desktop to 16 or 32 bit color mode.",
						   "OK", Dialog::BUTTONS_FOOTER);

			mFullscreenCheckbox->SetChecked(true, false);
		}
		else
		{
			mApp->SwitchScreenMode(!mFullscreenCheckbox->IsChecked(), true, false);
		}
		break;
	}
}