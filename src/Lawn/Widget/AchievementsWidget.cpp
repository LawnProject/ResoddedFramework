#include "AchievementsWidget.h"
#include "GameSelector.h"
#include "../../LawnApp.h"
#include "../System/Achievements.h"
#include "../System/PlayerInfo.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../GameConstants.h"
#include "../../Resources.h"
#include "../../FrameworkResources.h"

int gDefaultScrollValue = 30;

AchievementsWidget::AchievementsWidget(LawnApp *theApp)
{
	mApp = theApp;
	mWidth = BOARD_WIDTH;
#if LAWN_WIDESCREEN
	mHeight = IMAGE_ACHEESEMENTS_CHINA_WIDESCREEN->mHeight + IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG_WIDESCREEN->mHeight + 15700;
#else
	mHeight = IMAGE_ACHEESEMENTS_CHINA->mHeight + IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG->mHeight + 15700;
#endif
	mScrollValue = 0;
	mScrollDirection = -1;
	mScrollDecay = 1;
	mPressedMoreButton = false;

	mBackButton =
		MakeNewButton(AchievementsWidget::ACHIEVEMENTS_BACK, this, "", nullptr, Sexy::IMAGE_BLANK,
								Sexy::IMAGE_ACHEESEMENTS_BACK_HIGHLIGHT, Sexy::IMAGE_ACHEESEMENTS_BACK_HIGHLIGHT);
	mBackButton->Resize(128 + BOARD_ADDITIONAL_WIDTH, 55, Sexy::IMAGE_ACHEESEMENTS_MORE_ROCK->mWidth, Sexy::IMAGE_ACHEESEMENTS_MORE_ROCK->mHeight);
	mBackButton->mClip = false;

	mMoreButton =
		MakeNewButton(AchievementsWidget::ACHIEVEMENTS_MORE, this, "", nullptr, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON,
								Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON_HIGHLIGHT, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON_HIGHLIGHT);
	mMoreButton->Resize(700 + BOARD_ADDITIONAL_WIDTH, 450, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON->mWidth - 25, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON->mHeight - 50);
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
#if LAWN_WIDESCREEN
	g->DrawImage(IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG_WIDESCREEN, 0, 0);
	int aTileY = IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG_WIDESCREEN->GetHeight();
#else
	g->DrawImage(IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG, BOARD_ADDITIONAL_WIDTH, 0);
	int aTileY = IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG->GetHeight();
#endif
	for (int i = 1; i <= 70; i++)
	{
#if LAWN_WIDESCREEN
		g->DrawImage(IMAGE_ACHEESEMENTS_HOLE_TILE_WIDESCREEN, 0, aTileY);
		aTileY += IMAGE_ACHEESEMENTS_HOLE_TILE_WIDESCREEN->GetHeight();
#else
		g->DrawImage(IMAGE_ACHEESEMENTS_HOLE_TILE, BOARD_ADDITIONAL_WIDTH, aTileY);
		aTileY += IMAGE_ACHEESEMENTS_HOLE_TILE->GetHeight();
#endif
	}

	g->DrawImage(IMAGE_ACHEESEMENTS_BOOKWORM, BOARD_ADDITIONAL_WIDTH, 1125);
	g->DrawImage(IMAGE_ACHEESEMENTS_BEJEWELED, BOARD_ADDITIONAL_WIDTH, 2250);
	g->DrawImage(IMAGE_ACHEESEMENTS_CHUZZLE, BOARD_ADDITIONAL_WIDTH, 4500);
	g->DrawImage(IMAGE_ACHEESEMENTS_PEGGLE, BOARD_ADDITIONAL_WIDTH, 6750);
	g->DrawImage(IMAGE_ACHEESEMENTS_PIPE, BOARD_ADDITIONAL_WIDTH, 9000);
	g->DrawImage(IMAGE_ACHEESEMENTS_ZUMA, BOARD_ADDITIONAL_WIDTH, 11250);

#if LAWN_WIDESCREEN
	g->DrawImage(IMAGE_ACHEESEMENTS_CHINA_WIDESCREEN, 0, mHeight - IMAGE_ACHEESEMENTS_CHINA_WIDESCREEN->mHeight - 50);
#else
	g->DrawImage(IMAGE_ACHEESEMENTS_CHINA, BOARD_ADDITIONAL_WIDTH, mHeight - IMAGE_ACHEESEMENTS_CHINA->mHeight - 50);
#endif

	for (int i = 0; i < NUM_ACHIEVEMENT_TYPES; i++)
	{
		bool aHasAchievement = (mApp->mPlayerInfo != nullptr && mApp->mPlayerInfo->mEarnedAchievements[i]);
		int anAchivementOffset = 57 * (i / 2);
		int aImageXPos = (i % 2 == 0 ? 120 : 410) + BOARD_ADDITIONAL_WIDTH;
		int aImageYPos = 178 + anAchivementOffset;
		int aTextXPos = aImageXPos + 70;
		int aTextYPos = aImageYPos + 16;

		Rect aSrcRect(70 * (i % 7), 70 * (i / 7), 70, 70);
		Rect aDestRect(aImageXPos, aImageYPos, 56, 56);
		g->SetColor(aHasAchievement ? Color(255, 255, 255) : Color(255, 255, 255, 32));
		g->SetColorizeImages(true);
		g->DrawImage(IMAGE_ACHEESEMENTS_ICONS, aDestRect, aSrcRect);
		g->SetColorizeImages(false);

		AchievementDefinition aDefinition = Achievements::GetAchievementDefinition((AchievementID)i);

		g->SetFont(FONT_DWARVENTODCRAFT15);
		g->SetColor(Color(21, 175, 0));

		g->DrawString(TodStringTranslate(aDefinition.mName), aTextXPos, aTextYPos);

		Rect aPos = Rect(aTextXPos, aTextYPos + 3, 212, 60);

		g->SetFont(FONT_DWARVENTODCRAFT12);
		g->SetColor(Color(255, 255, 255));

		g->WriteWordWrapped(aPos, TodStringTranslate(aDefinition.mDescription), 12);
	}

	g->DrawImage(IMAGE_ACHEESEMENTS_MORE_ROCK, 700 + BOARD_ADDITIONAL_WIDTH, 450);
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
		mScrollValue = 22;
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