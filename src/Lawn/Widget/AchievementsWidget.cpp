#include "AchievementsWidget.h"
#include "GameSelector.h"
#include "../../LawnApp.h"
#include "../System/Achievements.h"
#include "../System/PlayerInfo.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../GameConstants.h"
#include "../../Resources.h"
#include "../../FrameworkResources.h"

int gDefaultScrollValue = 30;

#if LAWN_WIDESCREEN
static const int kAchievementsHeaderY = 30 - BOARD_OFFSET_Y;
static const int kAchievementsBackButtonY = 55 - BOARD_OFFSET_Y + 30;
static const int kAchievementsMoreButtonX = 710 + BOARD_ADDITIONAL_WIDTH + 90;
static const int kAchievementsMoreButtonY = 470 - BOARD_OFFSET_Y;

static Image *GetAchievementsHeaderBgImage()
{
	if (Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG_WIDESCREEN != nullptr)
		return Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG_WIDESCREEN;
	return Sexy::IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG;
}

static Image *GetAchievementsChinaImage()
{
	if (Sexy::IMAGE_ACHEESEMENTS_CHINA_WIDESCREEN != nullptr)
		return Sexy::IMAGE_ACHEESEMENTS_CHINA_WIDESCREEN;
	return Sexy::IMAGE_ACHEESEMENTS_CHINA;
}

static Image *GetAchievementsHoleTileImage()
{
	if (Sexy::IMAGE_ACHEESEMENTS_HOLE_TILE_WIDESCREEN != nullptr)
		return Sexy::IMAGE_ACHEESEMENTS_HOLE_TILE_WIDESCREEN;
	return Sexy::IMAGE_ACHEESEMENTS_HOLE_TILE;
}
#endif

AchievementsWidget::AchievementsWidget(LawnApp *theApp)
{
	mApp = theApp;
	mWidth = BOARD_WIDTH;
#if LAWN_WIDESCREEN
	{
		Image *aChinaImage = GetAchievementsChinaImage();
		Image *aAchievementsBgImage = GetAchievementsHeaderBgImage();
		TOD_ASSERT(aChinaImage != nullptr && aAchievementsBgImage != nullptr);
		mHeight = aChinaImage->mHeight + aAchievementsBgImage->mHeight + 15700;
	}
#else
	mHeight = IMAGE_ACHEESEMENTS_CHINA->mHeight + IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG->mHeight + 15700;
#endif
	mScrollPosition = 0;
	mScrollValue = 0;
	mScrollDirection = -1;
	mScrollDecay = 1;
	mPressedMoreButton = false;
	mClip = false;

	mBackButton =
		MakeNewButton(AchievementsWidget::ACHIEVEMENTS_BACK, this, "", nullptr, Sexy::IMAGE_BLANK,
								Sexy::IMAGE_ACHEESEMENTS_BACK_HIGHLIGHT, Sexy::IMAGE_ACHEESEMENTS_BACK_HIGHLIGHT);
#if LAWN_WIDESCREEN
	mBackButton->Resize(128 + BOARD_ADDITIONAL_WIDTH, kAchievementsBackButtonY, Sexy::IMAGE_ACHEESEMENTS_MORE_ROCK->mWidth, Sexy::IMAGE_ACHEESEMENTS_MORE_ROCK->mHeight);
#else
	mBackButton->Resize(128 + BOARD_ADDITIONAL_WIDTH, 55, Sexy::IMAGE_ACHEESEMENTS_MORE_ROCK->mWidth, Sexy::IMAGE_ACHEESEMENTS_MORE_ROCK->mHeight);
#endif
	mBackButton->mClip = false;

	mMoreButton =
		MakeNewButton(AchievementsWidget::ACHIEVEMENTS_MORE, this, "", nullptr, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON,
								Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON_HIGHLIGHT, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON_HIGHLIGHT);
#if LAWN_WIDESCREEN
	mMoreButton->Resize(kAchievementsMoreButtonX, kAchievementsMoreButtonY, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON->mWidth - 25, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON->mHeight - 50);
#else
	mMoreButton->Resize(700 + BOARD_ADDITIONAL_WIDTH, 450, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON->mWidth - 25, Sexy::IMAGE_ACHEESEMENTS_MORE_BUTTON->mHeight - 50);
#endif
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
	theWidgetManager->PutInfront(mBackButton, this);
	theWidgetManager->PutInfront(mMoreButton, this);
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
	g->PushState();
	g->ClipRect(0, 0, mWidth, mApp->mHeight);

#if LAWN_WIDESCREEN
	Image *aHeaderBgImage = GetAchievementsHeaderBgImage();
	Image *aHoleTileImage = GetAchievementsHoleTileImage();
	Image *aChinaImage = GetAchievementsChinaImage();

	g->DrawImage(aHeaderBgImage, 0, kAchievementsHeaderY + mScrollPosition);
	for (int i = 1; i <= 70; i++)
		g->DrawImage(aHoleTileImage, 0, aHoleTileImage->mHeight * i - 100 + mScrollPosition);
	g->DrawImage(aChinaImage, 0, mHeight - aChinaImage->mHeight - 50 + mScrollPosition);
#else
	g->DrawImage(IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG, BOARD_ADDITIONAL_WIDTH, mScrollPosition);
	int aTileY = IMAGE_SELECTORSCREEN_ACHIEVEMENTS_BG->GetHeight() + mScrollPosition;
	for (int i = 1; i <= 70; i++)
	{
		g->DrawImage(IMAGE_ACHEESEMENTS_HOLE_TILE, BOARD_ADDITIONAL_WIDTH, aTileY);
		aTileY += IMAGE_ACHEESEMENTS_HOLE_TILE->GetHeight();
	}
	g->DrawImage(IMAGE_ACHEESEMENTS_CHINA, BOARD_ADDITIONAL_WIDTH, mHeight - IMAGE_ACHEESEMENTS_CHINA->mHeight - 50 + mScrollPosition);
#endif

	g->DrawImage(IMAGE_ACHEESEMENTS_BOOKWORM, BOARD_ADDITIONAL_WIDTH, 1125 + mScrollPosition);
	g->DrawImage(IMAGE_ACHEESEMENTS_BEJEWELED, BOARD_ADDITIONAL_WIDTH, 2250 + mScrollPosition);
	g->DrawImage(IMAGE_ACHEESEMENTS_CHUZZLE, BOARD_ADDITIONAL_WIDTH, 4500 + mScrollPosition);
	g->DrawImage(IMAGE_ACHEESEMENTS_PEGGLE, BOARD_ADDITIONAL_WIDTH, 6750 + mScrollPosition);
	g->DrawImage(IMAGE_ACHEESEMENTS_PIPE, BOARD_ADDITIONAL_WIDTH, 9000 + mScrollPosition);
	g->DrawImage(IMAGE_ACHEESEMENTS_ZUMA, BOARD_ADDITIONAL_WIDTH, 11250 + mScrollPosition);

	for (int i = 0; i < NUM_ACHIEVEMENT_TYPES; i++)
	{
		bool aHasAchievement = (mApp->mPlayerInfo != nullptr && mApp->mPlayerInfo->mEarnedAchievements[i]);
#if LAWN_WIDESCREEN
		int aImageYPos = 138 + 57 * (i / 2) + mScrollPosition;
		int aImageXPos = (i % 2 == 0 ? 90 : 380) + BOARD_ADDITIONAL_WIDTH;
		int aTextXPos = aImageXPos + 120 - 20;
		int aTextYPos = aImageYPos + 16;
#else
		int anAchivementOffset = 57 * (i / 2);
		int aImageXPos = (i % 2 == 0 ? 120 : 410) + BOARD_ADDITIONAL_WIDTH;
		int aImageYPos = 178 + anAchivementOffset + mScrollPosition;
		int aTextXPos = aImageXPos + 70;
		int aTextYPos = aImageYPos + 16;
#endif

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

#if LAWN_WIDESCREEN
		g->SetFont(FONT_DWARVENTODCRAFT12);
		g->SetColor(Color(255, 255, 255));
		g->DrawStringWordWrapped(TodStringTranslate(aDefinition.mDescription), aTextXPos, aTextYPos + 14, 215, 12);
#else
		Rect aPos = Rect(aTextXPos, aTextYPos + 3, 212, 60);

		g->SetFont(FONT_DWARVENTODCRAFT12);
		g->SetColor(Color(255, 255, 255));

		g->WriteWordWrapped(aPos, TodStringTranslate(aDefinition.mDescription), 12);
#endif
	}

	g->PopState();

#if LAWN_WIDESCREEN
	g->DrawImage(IMAGE_ACHEESEMENTS_MORE_ROCK, kAchievementsMoreButtonX, kAchievementsMoreButtonY + mScrollPosition);
#else
	g->DrawImage(IMAGE_ACHEESEMENTS_MORE_ROCK, 700 + BOARD_ADDITIONAL_WIDTH, 450 + mScrollPosition);
#endif
}

void AchievementsWidget::Update()
{
	MarkDirty();
	mBackButton->MarkDirty();
	mMoreButton->MarkDirty();
#if LAWN_WIDESCREEN
	mBackButton->mX = mX + 128 + BOARD_ADDITIONAL_WIDTH;
	mBackButton->mY = mY + kAchievementsBackButtonY + mScrollPosition;
	mMoreButton->mX = mX + kAchievementsMoreButtonX;
	mMoreButton->mY = mY + kAchievementsMoreButtonY + mScrollPosition;
#else
	mBackButton->mX = mX + 128 + BOARD_ADDITIONAL_WIDTH;
	mBackButton->mY = mY + 55 + mScrollPosition;
	mMoreButton->mX = mX + 700 + BOARD_ADDITIONAL_WIDTH;
	mMoreButton->mY = mY + 450 + mScrollPosition;
#endif

	if (mHasFocus)
	{
		if ((mMoreButton->mIsOver && !mMoreButton->mDisabled) || (mBackButton->mIsOver && !mBackButton->mDisabled))
		{
			mApp->SetCursor(CURSOR_HAND);
		}
		else
		{
			mApp->SetCursor(CURSOR_POINTER);
		}
	}

	if (mScrollValue == 0)
		return;
	
	mScrollValue -= mScrollDecay;

	int aNewScroll = mScrollPosition + mScrollValue * mScrollDirection;
	if (aNewScroll > 0)
		aNewScroll = 0;

	int aMaxScroll = mApp->mHeight - mHeight + 50;
	if (aNewScroll < aMaxScroll)
		aNewScroll = aMaxScroll;

	mScrollPosition = aNewScroll;

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
		mScrollPosition = 0;
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
