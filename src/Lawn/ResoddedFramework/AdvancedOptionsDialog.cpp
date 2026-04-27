#include "AdvancedOptionsDialog.h"
#include "../../Resources.h"
#include "../../SexyAppFramework/Font.h"
#include "../../LawnApp.h"

AdvancedOptionsDialog::AdvancedOptionsDialog(LawnApp *theApp)
	: LawnDialog(theApp, DIALOG_ADVANCEDOPTIONS, true, "[ADVANCED_OPTIONS_HEADER]", "", "", BUTTONS_NONE)
{
	mApp = theApp;
	mOptionsSlider = new LawnSlider(mApp);
	mOptionsSlider->mSliderHeightPercent = 0.57f;
	mOptionsSlider->mStepMultiplier = 1.0f;
	mOptionsSlider->Resize(500, 90, 8, 140);
	mOptionsSlider->mScrollMultiplier = 0.09f;

	mApplyButton = MakeButton(ADVANCED_OPTIONS_APPLY, this, "[ADVANCED_OPTIONS_APPLY]");
	mCancelButton = MakeButton(ADVANCED_OPTIONS_CANCEL, this, "[ADVANCED_OPTIONS_CANCEL]");
	CalcSize(211, 214);
}

AdvancedOptionsDialog::~AdvancedOptionsDialog()
{
	delete mOptionsSlider;
	delete mApplyButton;
	delete mCancelButton;
}

void AdvancedOptionsDialog::Draw(Graphics* g)
{
	LawnDialog::Draw(g);
	g->SetColor(Color::White);
	g->SetFont(Sexy::FONT_BRIANNETOD12);
	SexyString aVersionString = "ResoddedFramework " + mResoddedVersion.toString();
	g->WriteString(aVersionString, mWidth - Sexy::FONT_BRIANNETOD12->StringWidth(aVersionString) + 60, mHeight - Sexy::FONT_BRIANNETOD12->GetHeight() - 20);
	g->PushState();
	g->SetClipRect(Rect(mOptionsSlider->mAllowedMouseZone.mX - mX,
						mOptionsSlider->mAllowedMouseZone.mY - mY,
						mOptionsSlider->mAllowedMouseZone.mWidth,
						mOptionsSlider->mAllowedMouseZone.mHeight));
	g->Translate(35, 120 + -mOptionsSlider->GetValue() * 700);
	g->DrawImage(Sexy::IMAGE_REANIM_ZOMBIE_HEAD_SUNGLASSES1, 0, 0);
	g->DrawImage(Sexy::IMAGE_REANIM_ZOMBIE_HEAD_SUNGLASSES2, 0, 600);
	g->PopState();
}

void AdvancedOptionsDialog::AddedToManager(WidgetManager *theWidgetManager)
{
	LawnDialog::AddedToManager(theWidgetManager);
	AddWidget(mOptionsSlider);
	AddWidget(mApplyButton);
	AddWidget(mCancelButton);
}

//0x45D8E0
void AdvancedOptionsDialog::RemovedFromManager(WidgetManager *theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mOptionsSlider);
	RemoveWidget(mApplyButton);
	RemoveWidget(mCancelButton);
}

void AdvancedOptionsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	mOptionsSlider->Resize(mWidth - 60, 110, 8, 200);
	mOptionsSlider->mAllowedMouseZone = Rect(mX + 35, mY + 120, mWidth - 70, mHeight - 240);
	mApplyButton->Resize(40, 331, 140, 46);
	mCancelButton->Resize(190, 331, 140, 46);
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
		case AdvancedOptionsDialog::ADVANCED_OPTIONS_CANCEL:
		case AdvancedOptionsDialog::ADVANCED_OPTIONS_APPLY:
		{
			mApp->KillDialog(mId);
			if (theId == ADVANCED_OPTIONS_APPLY)
			{
				//special code to update some specific options
			}
			
			break;
		}
	}
}