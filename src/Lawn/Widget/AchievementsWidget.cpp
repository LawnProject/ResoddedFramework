#include "AchievementsWidget.h"
#include "GameSelector.h"
#include "../../LawnApp.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../GameConstants.h"
#include "../../Resources.h"


AchievementDefinition gAchievementDefs[NUM_ACHIEVEMENT_TYPES] = 
{
	{"[ACHIEVEMENT_HOME_SECURITY_TITLE]", "[ACHIEVEMENT_HOME_SECURITY_DESCRIPTION]"},
	{"[ACHIEVEMENT_NOBEL_PEAS_PRIZE_TITLE]", "[ACHIEVEMENT_NOBEL_PEAS_PRIZE_DESCRIPTION]"},
	{"[ACHIEVEMENT_BETTER_OFF_DEAD_TITLE]", "[ACHIEVEMENT_BETTER_OFF_DEAD_DESCRIPTION]"},
	{"[ACHIEVEMENT_CHINA_SHOP_TITLE]", "[ACHIEVEMENT_CHINA_SHOP_DESCRIPTION]"},
	{"[ACHIEVEMENT_SPUDOW_TITLE]", "[ACHIEVEMENT_SPUDOW_DESCRIPTION]"},
	{"[ACHIEVEMENT_EXPLODONATOR_TITLE]", "[ACHIEVEMENT_EXPLODONATOR_DESCRIPTION]"},
	{"[ACHIEVEMENT_MORTICULTURALIST_TITLE]", "[ACHIEVEMENT_MORTICULTURALIST_DESCRIPTION]"},
	{"[ACHIEVEMENT_DONT_PEA_IN_POOL_TITLE]", "[ACHIEVEMENT_DONT_PEA_IN_POOL_DESCRIPTION]"},
	{"[ACHIEVEMENT_ROLL_SOME_HEADS_TITLE]", "[ACHIEVEMENT_ROLL_SOME_HEADS_DESCRIPTION]"},
	{"[ACHIEVEMENT_GROUNDED_TITLE]", "[ACHIEVEMENT_GROUNDED_DESCRIPTION]"},
	{"[ACHIEVEMENT_ZOMBOLOGIST_TITLE]", "[ACHIEVEMENT_ZOMBOLOGIST_DESCRIPTION]"},
	{"[ACHIEVEMENT_PENNY_PINCHER_TITLE]", "[ACHIEVEMENT_PENNY_PINCHER_DESCRIPTION]"},
	{"[ACHIEVEMENT_SUNNY_DAYS_TITLE]", "[ACHIEVEMENT_SUNNY_DAYS_DESCRIPTION]"},
	{"[ACHIEVEMENT_POPCORN_PARTY_TITLE]", "[ACHIEVEMENT_POPCORN_PARTY_DESCRIPTION]"},
	{"[ACHIEVEMENT_GOOD_MORNING_TITLE]", "[ACHIEVEMENT_GOOD_MORNING_DESCRIPTION]"},
	{"[ACHIEVEMENT_NO_FUNGUS_AMONG_US_TITLE]", "[ACHIEVEMENT_NO_FUNGUS_AMONG_US_DESCRIPTION]"},
	{"[ACHIEVEMENT_BEYOND_THE_GRAVE_TITLE]", "[ACHIEVEMENT_BEYOND_THE_GRAVE_DESCRIPTION]"},
	{"[ACHIEVEMENT_IMMORTAL_TITLE]", "[ACHIEVEMENT_IMMORTAL_DESCRIPTION]"},
	{"[ACHIEVEMENT_TOWERING_WISDOM_TITLE]", "[ACHIEVEMENT_TOWERING_WISDOM_DESCRIPTION]"},
	{"[ACHIEVEMENT_MUSTACHE_MODE_DESCRIPTION]", "[ACHIEVEMENT_MUSTACHE_MODE_DESCRIPTION]"},
	{"[ACHIEVEMENT_DISCO_IS_UNDEAD_DESCRIPTION]", "[ACHIEVEMENT_DISCO_IS_UNDEAD_DESCRIPTION]"},
};

int gDefaultScrollValue = 30;

AchievementsWidget::AchievementsWidget(LawnApp *theApp)
{
	mApp = theApp;
	mWidth = BOARD_WIDTH;
	mHeight = IMAGE_ACHEESEMENTS_CHINA->mHeight + IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG->mHeight + 15700;
	mScrollValue = 0;
	mScrollDirection = -1;
	mScrollDecay = 1;
	mPressedMoreButton = false;

	mBackButton =
		MakeNewButton(AchievementsWidget::ACHIEVEMENTS_BACK, this, "", nullptr, Sexy::IMAGE_BLANK,
								Sexy::IMAGE_ACHEESEMENTS_BACK_HIGHLIGHT, Sexy::IMAGE_ACHEESEMENTS_BACK_HIGHLIGHT);
	mBackButton->Resize(128, 55, Sexy::IMAGE_ACHEESEMENTS_MORE_ROCK->mWidth, Sexy::IMAGE_ACHEESEMENTS_MORE_ROCK->mHeight);
	mBackButton->mClip = false;

	mMoreButton =
		MakeNewButton(AchievementsWidget::ACHIEVEMENTS_MORE, this, "", nullptr, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON,
								Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON_HIGHLIGHT, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON_HIGHLIGHT);
	mMoreButton->Resize(700, 450, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON->mWidth - 25, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON->mHeight - 50);
	mMoreButton->mClip = false;
}

AchievementsWidget::~AchievementsWidget()
{
	if (mBackButton)
		delete mBackButton;
	if (mMoreButton)
		delete mMoreButton;
}

void AchievementsWidget::AddedToManager(WidgetManager *theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);

	theWidgetManager->AddWidget(mBackButton);
	theWidgetManager->AddWidget(mMoreButton);
}

void AchievementsWidget::RemovedFromManager(WidgetManager *theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);

	theWidgetManager->RemoveWidget(mBackButton);
	theWidgetManager->RemoveWidget(mMoreButton);
}

void AchievementsWidget::KeyDown(KeyCode theKey)
{
	if (theKey == KeyCode::KEYCODE_UP)
	{
		mScrollValue = gDefaultScrollValue;
		mScrollDirection = 1;
	}
	else if (theKey == KeyCode::KEYCODE_DOWN)
	{
		mScrollValue = gDefaultScrollValue;
		mScrollDirection = -1;
	}
}

void AchievementsWidget::Draw(Graphics *g)
{
	g->DrawImage(IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG, 0, 0);
	int aTileY = IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG->GetHeight();
	for (int i = 1; i <= 70; i++)
	{
		g->DrawImage(IMAGE_ACHEESEMENTS_HOLE_TILE, 0, aTileY);
		aTileY += IMAGE_ACHEESEMENTS_HOLE_TILE->GetHeight();
	}

	g->DrawImage(IMAGE_ACHEESEMENTS_BOOKWORM, 0, 1125);
	g->DrawImage(IMAGE_ACHEESEMENTS_BEJEWELED, 0, 2250);
	g->DrawImage(IMAGE_ACHEESEMENTS_CHUZZLE, 0, 4500);
	g->DrawImage(IMAGE_ACHEESEMENTS_PEGGLE, 0, 6750);
	g->DrawImage(IMAGE_ACHEESEMENTS_PIPE, 0, 9000);
	g->DrawImage(IMAGE_ACHEESEMENTS_ZUMA, 0, 11250);
	g->DrawImage(IMAGE_ACHEESEMENTS_CHINA, 0, mHeight - IMAGE_ACHEESEMENTS_CHINA->mHeight - 50);

	g->DrawImage(IMAGE_ACHEESEMENTS_MORE_ROCK, 700, 450);
}

void AchievementsWidget::Update()
{
	MarkDirty();
	mBackButton->MarkDirty();
	mMoreButton->MarkDirty();
	mMoreButton->mY = mY + 450;
	mBackButton->mY = mY + 55;

	if (mScrollValue == 0)
		return;
	
	mScrollValue -= mScrollDecay;

	int aNewY = mY + mScrollValue * mScrollDirection;
	if (aNewY >= 0)
		aNewY = 0;

	int aMaxScroll = mApp->mHeight - mHeight + 50;
	if (aNewY <= aMaxScroll)
		aNewY = aMaxScroll;

	mY = aNewY;

	if (mScrollValue <= 0)
		mScrollValue = 0;
}

void AchievementsWidget::MouseWheel(int theDelta)
{
	mScrollValue = gDefaultScrollValue;
	mScrollDirection = theDelta < 0 ? -1 : 1;
}

void AchievementsWidget::ButtonPress(int theId, int theClickCount)
{
	mApp->PlaySample(Sexy::SOUND_GRAVEBUTTON);
}

void AchievementsWidget::ButtonDepress(int theId)
{
	switch (theId)
	{
	case AchievementsWidget::ACHIEVEMENTS_BACK:
		mApp->mGameSelector->SlideTo(0, 0);
		mWidgetManager->SetFocus(mApp->mGameSelector);
		break;
	case AchievementsWidget::ACHIEVEMENTS_MORE:
		mScrollValue = 20;
		mPressedMoreButton = !mPressedMoreButton;
		mScrollDirection = mPressedMoreButton ? -1 : 1;
		if (mPressedMoreButton)
		{
			mMoreButton->mButtonImage = Sexy::IMAGE_ACHEESEMENTS_TOP_BUTTON;
			mMoreButton->mOverImage = Sexy::IMAGE_ACHEESEMENTS_TOP_BUTTON_HIGHLIGHT;
			mMoreButton->mDownImage = Sexy::IMAGE_ACHEESEMENTS_TOP_BUTTON_HIGHLIGHT;
		}
		else
		{
			mMoreButton->mButtonImage = Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON;
			mMoreButton->mOverImage = Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON_HIGHLIGHT;
			mMoreButton->mDownImage = Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON_HIGHLIGHT;
		}
		break;
	}
}