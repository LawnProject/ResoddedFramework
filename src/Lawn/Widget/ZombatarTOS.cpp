#include "ZombatarTOS.h"
#include "ZombatarWidget.h"
#include "GameSelector.h"
#include "../System/PlayerInfo.h"
#include "../../LawnApp.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../Resources.h"
#include "../../SexyAppFramework/Slider.h"
#include "../../SexyAppFramework/Checkbox.h"
#include "../../SexyAppFramework/WidgetManager.h"

using namespace Sexy;

ZombatarTOS::ZombatarTOS(LawnApp *theApp) : LawnDialog(theApp, DIALOG_ZOMBATAR_TOS, true, "[ZOMBATAR_TOS_HEADER]", "", "", BUTTONS_NONE)
{
	mApp = theApp;

	
	mTOSSlider = new Slider(IMAGE_ZOMBATAR_TOS_SLIDER, IMAGE_ZOMBATAR_TOS_SLIDER_THUMB, ZombatarTOS::ZombatarTOS_Slider, this);
	mTOSSlider->mHorizontal = false;
	mTOSSlider->SetValue(0);

	mBackButton = MakeNewButton(ZombatarTOS::ZombatarTOS_Back, this, "", nullptr, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON,
								Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT);

	mAcceptButton = MakeNewButton(ZombatarTOS::ZombatarTOS_Accept, this, "", nullptr, Sexy::IMAGE_ZOMBATAR_ACCEPT_BUTTON,
								Sexy::IMAGE_ZOMBATAR_ACCEPT_BUTTON_HIGHLIGHT, Sexy::IMAGE_ZOMBATAR_ACCEPT_BUTTON_HIGHLIGHT);
	mAcceptButton->mDisabled = true;

	mTOSCheckbox = MakeNewCheckbox(ZombatarTOS::ZombatarTOS_TOSCheckbox, this, !theApp->mIsWindowed);

	CalcSize(110, 200);
}

ZombatarTOS::~ZombatarTOS()
{
	if (mTOSSlider)
		delete mTOSSlider;
	if (mBackButton)
		delete mBackButton;
	if (mAcceptButton)
		delete mAcceptButton;
	if (mTOSCheckbox)
		delete mTOSCheckbox;
}


void ZombatarTOS::AddedToManager(Sexy::WidgetManager *theWidgetManager)
{
	Dialog::AddedToManager(theWidgetManager);
	AddWidget(mTOSSlider);
	AddWidget(mBackButton);
	AddWidget(mAcceptButton);
	AddWidget(mTOSCheckbox);
}

void ZombatarTOS::RemovedFromManager(Sexy::WidgetManager *theWidgetManager)
{
	Dialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mTOSSlider);
	RemoveWidget(mBackButton);
	RemoveWidget(mAcceptButton);
	RemoveWidget(mTOSCheckbox);
}

void ZombatarTOS::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	mTOSSlider->Resize(Rect(GetWidth() - 15, GetTop() + 5, 40, 135));
	mBackButton->Resize(GetLeft(), 340, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON->mWidth, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON->mHeight);
	mAcceptButton->Resize(GetWidth() - 60, 340, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON->mWidth, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON->mHeight);
	mTOSCheckbox->Resize(GetWidth() - 110, 335, 46, 45);
}


void ZombatarTOS::Draw(Graphics *g)
{
	LawnDialog::Draw(g);
	g->ClipRect(Rect(GetLeft(), GetTop(), GetWidth(), mHeight - 260));
	TodDrawStringWrapped(g, "[ZOMBATAR_TOS]", Rect(GetLeft() + 10, GetTop() - (mTOSSlider->mVal * 510), GetWidth() - 90, mHeight), Sexy::FONT_PICO129, Color(255, 255, 255), DrawStringJustification::DS_ALIGN_LEFT);
}

void ZombatarTOS::Update()
{
}

void ZombatarTOS::ButtonPress(int theId, int theClickCount)
{
	mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
}

void ZombatarTOS::ButtonDepress(int theId)
{
	switch (theId)
	{
	case ZombatarTOS::ZombatarTOS_Back:
		mApp->KillDialog(mId);
		break;
	case ZombatarTOS::ZombatarTOS_Accept:
		mApp->KillDialog(mId);
		mApp->mGameSelector->SlideTo(-mApp->mWidth, 0);
		mApp->mGameSelector->mWidgetManager->SetFocus(mApp->mGameSelector->mZombatarWidget);
		mApp->mPlayerInfo->mAcceptedZombatarTOS = true;
		break;
	}
}

void ZombatarTOS::CheckboxChecked(int theId, bool checked)
{
	switch (theId)
	{
	case ZombatarTOS::ZombatarTOS_TOSCheckbox:
		mAcceptButton->mDisabled = !checked;
		break;
	}
}
