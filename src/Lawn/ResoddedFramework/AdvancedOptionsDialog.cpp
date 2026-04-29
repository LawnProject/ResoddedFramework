#include "AdvancedOptionsDialog.h"
#include "../../Resources.h"
#include "../../SexyAppFramework/Font.h"
#include "../../SexyAppFramework/Renderer.h"
#include "../../SexyAppFramework/Checkbox.h"
#include "../../LawnApp.h"
#include "../../SexyAppFramework/GitData.h"

AdvancedOptionsDialog::AdvancedOptionsDialog(LawnApp *theApp)
	: LawnDialog(theApp, DIALOG_ADVANCEDOPTIONS, true, "[ADVANCED_OPTIONS_HEADER]", "", "", BUTTONS_NONE)
{
	mApp = theApp;
	mOptionsSlider = new LawnSlider(mApp);
	mOptionsSlider->mSliderHeightPercent = 0.57f;
	mOptionsSlider->mStepMultiplier = 1.0f;
	mOptionsSlider->Resize(500, 90, 8, 140);
	mOptionsSlider->mScrollMultiplier = 0.09f;

	mVSyncCheckbox = MakeNewCheckbox(AdvancedOptionsDialog::ADVANCED_OPTIONS_VSYNC, this, theApp->mWaitForVSync);

	mApplyButton = MakeButton(ADVANCED_OPTIONS_BACK, this, "[ADVANCED_OPTIONS_BACK]");
	CalcSize(211, 214);
}

AdvancedOptionsDialog::~AdvancedOptionsDialog()
{
	delete mOptionsSlider;
	delete mApplyButton;
	delete mVSyncCheckbox;
}

void AdvancedOptionsDialog::Draw(Graphics* g)
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
	TodDrawString(g, "[ADVANCED_OPTIONS_VIDEO]", 20, 10, Sexy::FONT_BRIANNETOD12, Color::White,
				  DrawStringJustification::DS_ALIGN_LEFT);

	TodDrawString(g, "[ADVANCED_OPTIONS_VSYNC]", mVSyncCheckbox->mX + 20, 40, Sexy::FONT_BRIANNETOD12, Color::White, DrawStringJustification::DS_ALIGN_LEFT);

	SexyString aVersionString = "ResoddedFramework " + mResoddedVersion.toString();
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

void AdvancedOptionsDialog::AddedToManager(WidgetManager *theWidgetManager)
{
	LawnDialog::AddedToManager(theWidgetManager);
	AddWidget(mOptionsSlider);
	AddWidget(mApplyButton);
	AddWidget(mVSyncCheckbox);
}

//0x45D8E0
void AdvancedOptionsDialog::RemovedFromManager(WidgetManager *theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mOptionsSlider);
	RemoveWidget(mApplyButton);
	RemoveWidget(mVSyncCheckbox);
}

void AdvancedOptionsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	mOptionsSlider->Resize(mWidth - 60, 110, 8, 200);
	mOptionsSlider->mAllowedMouseZone = Rect(mX + 35, mY + 120, mWidth - 70, mHeight - 240);
	mApplyButton->Resize(40, 331, 209, 46);
	SetWidgetClipping(Rect(35, 120, mWidth - 70, mHeight - 240));
}
void AdvancedOptionsDialog::MouseWheel(int theDelta)
{
	LawnDialog::MouseWheel(theDelta);
	mOptionsSlider->MouseWheel(theDelta);
}
void AdvancedOptionsDialog::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_GRAVEBUTTON);
}
void AdvancedOptionsDialog::ButtonDepress(int theId)
{
	switch (theId)
	{
		case AdvancedOptionsDialog::ADVANCED_OPTIONS_BACK:
		{
			mApp->KillDialog(mId);
			
			break;
		}
	}
}


void AdvancedOptionsDialog::CheckboxChecked(int theId, bool checked)
{
	switch (theId)
	{
	case AdvancedOptionsDialog::ADVANCED_OPTIONS_VSYNC:
		mApp->mWaitForVSync = checked;
		RendererError anError = mApp->mRenderer->UpdateVSync();
		if (anError == RendererError::ERROR_VSYNC)
		{
			mVSyncCheckbox->SetChecked(!checked, false);
			SexyString aFailString = StrFormat("V-Sync couldn't be toggled %s\n\nYour video card does not\nmeet the "
											   "minimum requirements\nfor this game.",
											   (checked ? "on" : "off"));
			mApp->DoDialog(Dialogs::DIALOG_INFO, true, "Failed", aFailString, "OK", Dialog::BUTTONS_FOOTER);
		}
		break;
	}
}