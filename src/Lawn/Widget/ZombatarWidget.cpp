#include "ZombatarWidget.h"
#include "GameSelector.h"
#include "../../LawnApp.h"
#include "../../GameConstants.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/WidgetManager.h"


ZombatarWidget::ZombatarWidget(LawnApp *theApp)
{
	mApp = theApp;
	mWidth = BOARD_WIDTH;
	mHeight = BOARD_HEIGHT;
	TodLoadResources("DelayLoad_Almanac");
	TodLoadResources("DelayLoad_Zombatar");

	mBackButton = MakeNewButton(ZombatarWidget::ZOMBATAR_BACK, this, "", nullptr, Sexy::IMAGE_BLANK, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT);
	mBackButton->Resize(278, 528, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT->mHeight);
	mBackButton->mClip = false;
}

ZombatarWidget::~ZombatarWidget()
{
	if (mBackButton)
		delete mBackButton;
}

void ZombatarWidget::Draw(Graphics *g)
{
	g->DrawImage(IMAGE_ZOMBATAR_MAIN_BG, 0, 0);
	g->ClipRect(600, 300, 170, 200);
	g->DrawImage(IMAGE_ALMANAC_GROUNDDAY, 600, 300);
	g->ClearClipRect();

	int aWidgetX = 25;
	int aWidgetY = 25;
	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_BG, aWidgetX, aWidgetY);
	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_INNER_BG, aWidgetX + 127, aWidgetY + 100);
	g->DrawImage(IMAGE_ZOMBATAR_DISPLAY_WINDOW, 0, 0);

	TodDrawStringWrapped(g, "WORK IN PROGRESS COME BACK LATER", Rect(BOARD_WIDTH / 2 - 100, BOARD_HEIGHT / 2, 200, 500), FONT_BRIANNETOD16, Color(255, 255, 255), DrawStringJustification::DS_ALIGN_CENTER);
}

void ZombatarWidget::Update()
{
	MarkDirty();
}

void ZombatarWidget::AddedToManager(WidgetManager *theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	AddWidget(mBackButton);
}

void ZombatarWidget::RemovedFromManager(WidgetManager *theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	RemoveWidget(mBackButton);
}

void ZombatarWidget::ButtonPress(int theId, int theClickCount)
{
	mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
}

void ZombatarWidget::ButtonDepress(int theId)
{
	switch (theId)
	{
	case ZombatarWidget::ZOMBATAR_BACK:
		mApp->mGameSelector->SlideTo(0, 0);
		mWidgetManager->SetFocus(mApp->mGameSelector);
		break;

	}
}