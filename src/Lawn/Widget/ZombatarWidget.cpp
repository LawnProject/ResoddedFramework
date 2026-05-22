#include "ZombatarWidget.h"
#include "GameSelector.h"
#include "../../LawnApp.h"
#include "../Zombie.h"
#include "../System/PlayerInfo.h"
#include "../../GameConstants.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/WidgetManager.h"


ZombatarWidget::ZombatarWidget(LawnApp *theApp)
{
	mPage = PAGE_SKIN;
	mApp = theApp;
	mWidth = BOARD_WIDTH;
	mHeight = BOARD_HEIGHT;
	TodLoadResources("DelayLoad_Almanac");
	TodLoadResources("DelayLoad_Zombatar");

	mBackButton = new GameButton(ZombatarWidget::ZOMBATAR_BACK);
	mBackButton->mButtonImage = Sexy::IMAGE_BLANK;
	mBackButton->mOverImage = Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT;
	mBackButton->mDownImage = Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT;
	mBackButton->Resize(278, 528, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT->mHeight);

	mViewButton = new GameButton(ZombatarWidget::ZOMBATAR_VIEW);
	mViewButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON;
	mViewButton->mOverImage = Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON_HIGHLIGHT;
	mViewButton->mDownImage = Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON_HIGHLIGHT;
	mViewButton->Resize(65, 472, Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON_HIGHLIGHT->mHeight);

	mFinishedButton = new GameButton(ZombatarWidget::ZOMBATAR_FINISHED);
	mFinishedButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON;
	mFinishedButton->mOverImage = Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT;
	mFinishedButton->mDownImage = Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT;
	mFinishedButton->Resize(445, 472, Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT->mHeight);

	mToggledButton = mSkinButton = new GameButton(ZombatarWidget::ZOMBATAR_SKIN);
	mSkinButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
	mSkinButton->mOverImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
	mSkinButton->mDownImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
	mSkinButton->Resize(58, 128, Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT->mHeight);

	mHairButton = new GameButton(ZombatarWidget::ZOMBATAR_HAIR);
	mHairButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON;
	mHairButton->mOverImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
	mHairButton->mDownImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
	mHairButton->Resize(58, 164, Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT->mHeight);

	mFacialHairButton = new GameButton(ZombatarWidget::ZOMBATAR_FACIAL_HAIR);
	mFacialHairButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON;
	mFacialHairButton->mOverImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
	mFacialHairButton->mDownImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
	mFacialHairButton->Resize(58, 200, Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT->mHeight);

	mTidbitsButton = new GameButton(ZombatarWidget::ZOMBATAR_TIDBITS);
	mTidbitsButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON;
	mTidbitsButton->mOverImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
	mTidbitsButton->mDownImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
	mTidbitsButton->Resize(58, 236, Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT->mHeight);

	mEyewearButton = new GameButton(ZombatarWidget::ZOMBATAR_TIDBITS);
	mEyewearButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON;
	mEyewearButton->mOverImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
	mEyewearButton->mDownImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
	mEyewearButton->Resize(58, 272, Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT->mHeight);

	mClothesButton = new GameButton(ZombatarWidget::ZOMBATAR_CLOTHES);
	mClothesButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON;
	mClothesButton->mOverImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
	mClothesButton->mDownImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
	mClothesButton->Resize(58, 308, Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT->mHeight);

	mAccessoriesButton = new GameButton(ZombatarWidget::ZOMBATAR_CLOTHES);
	mAccessoriesButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON;
	mAccessoriesButton->mOverImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
	mAccessoriesButton->mDownImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
	mAccessoriesButton->Resize(58, 344, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT->mHeight);

	mHatsButton = new GameButton(ZombatarWidget::ZOMBATAR_HATS);
	mHatsButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON;
	mHatsButton->mOverImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
	mHatsButton->mDownImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
	mHatsButton->Resize(58, 380, Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT->mHeight);

	mBackdropsButton = new GameButton(ZombatarWidget::ZOMBATAR_HATS);
	mBackdropsButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON;
	mBackdropsButton->mOverImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;
	mBackdropsButton->mDownImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;
	mBackdropsButton->Resize(58, 416, Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT->mHeight);

	mZombie = new Zombie();
	mZombie->mBoard = nullptr;
	mZombie->ZombieInitialize(0, ZombieType::ZOMBIE_FLAG, false, nullptr, Zombie::ZOMBIE_WAVE_UI);
	mZombie->mPosX = 641;
	mZombie->mPosY = 350;
	mZombie->SetupZombatar();

	ChangePage(mPage);
}

ZombatarWidget::~ZombatarWidget()
{
	if (mZombie)
	{
		if (mApp->mEffectSystem)
			mZombie->DieNoLoot();
		delete mZombie;
		mZombie = nullptr;
	}
	if (mBackButton)
		delete mBackButton;
	if (mViewButton)
		delete mViewButton;
	if (mFinishedButton)
		delete mFinishedButton;
	if (mSkinButton)
		delete mSkinButton;
	if (mHairButton)
		delete mHairButton;
	if (mFacialHairButton)
		delete mFacialHairButton;
	if (mTidbitsButton)
		delete mTidbitsButton;
	if (mEyewearButton)
		delete mEyewearButton;
	if (mClothesButton)
		delete mClothesButton;
	if (mAccessoriesButton)
		delete mAccessoriesButton;
	if (mHatsButton)
		delete mHatsButton;
	if (mBackdropsButton)
		delete mBackdropsButton;
}

void ZombatarWidget::ChangePage(ZombatarPage thePage)
{
	switch (mPage)
	{
		case PAGE_SKIN: 
		{
			mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
			mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
			mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
			break;
		}
		case PAGE_HAIR: 
		{
			mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON;
			mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
			mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
			break;
		}
		case PAGE_FACIAL_HAIR: 
		{
			mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON;
			mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
			mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
			break;
		}
		case PAGE_TIDBITS: 
		{
			mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON;
			mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
			mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
			break;
		}
		case PAGE_EYEWEAR: 
		{
			mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON;
			mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
			mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
			break;
		}
		case PAGE_CLOTHES: 
		{
			mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON;
			mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
			mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
			break;
		}
		case PAGE_ACCESSORIES: 
		{
			mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON;
			mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
			mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
			break;
		}
		case PAGE_HATS: 
		{
			mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON;
			mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
			mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
			break;
		}
		case PAGE_BACKDROPS: 
		{
			mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON;
			mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;
			mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;
			break;
		}
	}
	mToggledButton->mDisabled = false;
	mPage = thePage;
	switch (mPage)
	{
		case PAGE_SKIN:
		{
			mToggledButton = mSkinButton;
			break;
		}
		case PAGE_HAIR:
		{
			mToggledButton = mHairButton;
			break;
		}
		case PAGE_FACIAL_HAIR:
		{
			mToggledButton = mFacialHairButton;
			break;
		}
		case PAGE_TIDBITS:
		{
			mToggledButton = mTidbitsButton;
			break;
		}
		case PAGE_EYEWEAR:
		{
			mToggledButton = mEyewearButton;
			break;
		}
		case PAGE_CLOTHES:
		{
			mToggledButton = mClothesButton;
			break;
		}
		case PAGE_ACCESSORIES:
		{
			mToggledButton = mAccessoriesButton;
			break;
		}
		case PAGE_HATS:
		{
			mToggledButton = mHatsButton;
			break;
		}
		case PAGE_BACKDROPS:
		{
			mToggledButton = mBackdropsButton;
			break;
		}
	}

	Image *aNewImage = nullptr;
	switch (mPage)
	{
		case PAGE_SKIN: 
		{
			aNewImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT;
			break;
		}
		case PAGE_HAIR: 
		{
			aNewImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT;
			break;
		}
		case PAGE_FACIAL_HAIR: 
		{
			aNewImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT;
			break;
		}
		case PAGE_TIDBITS: 
		{
			aNewImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT;
			break;
		}
		case PAGE_EYEWEAR: 
		{
			aNewImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT;
			break;
		}
		case PAGE_CLOTHES: 
		{
			aNewImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT;
			break;
		}
		case PAGE_ACCESSORIES: 
		{
			aNewImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT;
			break;
		}
		case PAGE_HATS: 
		{
			aNewImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT;
			break;
		}
		case PAGE_BACKDROPS: 
		{
			aNewImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT;
			break;
		}
	}

	mToggledButton->mButtonImage = aNewImage;
	mToggledButton->mOverImage = aNewImage;
	mToggledButton->mDownImage = aNewImage;
	mToggledButton->mDisabled = true;

}

void ZombatarWidget::DrawPortrait(Graphics *g, int theX, int theY)
{
	g->PushState();
	g->Translate(theX, theY);

	g->DrawImage(Sexy::IMAGE_ZOMBATAR_BACKGROUND_BLANK, 0, 0);
	g->DrawImage(Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN, 38, 40);
	g->DrawImage(Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK, 38, 40);

	g->PopState();
}

void ZombatarWidget::Draw(Graphics *g)
{
	g->DrawImage(IMAGE_ZOMBATAR_MAIN_BG, 0, 0);
	g->ClipRect(600, 300, 170, 200);
	g->DrawImage(IMAGE_ALMANAC_GROUNDDAY, 600, 300);

	Graphics aZombieGraphics = Graphics(*g);
	mZombie->BeginDraw(&aZombieGraphics);
	mZombie->Draw(&aZombieGraphics);

	g->ClearClipRect();

	DrawPortrait(g, 592, 115);

	int aWidgetX = 25;
	int aWidgetY = 25;
	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_BG, aWidgetX, aWidgetY);
	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_INNER_BG, aWidgetX + 127, aWidgetY + 100);
	g->DrawImage(IMAGE_ZOMBATAR_DISPLAY_WINDOW, 5, 0);
	g->DrawImage(IMAGE_ZOMBATAR_COLORS_BG, 221, 335);
	mBackButton->Draw(g);
	mViewButton->Draw(g);
	mFinishedButton->Draw(g);
	mSkinButton->Draw(g);
	mHairButton->Draw(g);
	mFacialHairButton->Draw(g);
	mTidbitsButton->Draw(g);
	mEyewearButton->Draw(g);
	mClothesButton->Draw(g);
	mAccessoriesButton->Draw(g);
	mHatsButton->Draw(g);
	mBackdropsButton->Draw(g);
}

void ZombatarWidget::Update()
{

	MarkDirty();
	mZombie->Update();
	mBackButton->Update();
	mViewButton->Update();
	mFinishedButton->Update();
	mSkinButton->Update();
	mHairButton->Update();
	mFacialHairButton->Update();
	mTidbitsButton->Update();
	mEyewearButton->Update();
	mClothesButton->Update();
	mHatsButton->Update();
	mAccessoriesButton->Update();
	mBackdropsButton->Update();
	if (mHasFocus)
	{
		if (mViewButton->IsMouseOver() || mBackButton->IsMouseOver() || mFinishedButton->IsMouseOver() ||
			mSkinButton->IsMouseOver() || mHairButton->IsMouseOver() || mFacialHairButton->IsMouseOver() ||
			mTidbitsButton->IsMouseOver() || mEyewearButton->IsMouseOver() || mClothesButton->IsMouseOver() ||
			mAccessoriesButton->IsMouseOver() || mHatsButton->IsMouseOver() || mBackdropsButton->IsMouseOver())
		{
			mApp->SetCursor(CURSOR_HAND);
		}
		else
		{
			mApp->SetCursor(CURSOR_POINTER);
		}
	}

}

void ZombatarWidget::MouseDown(int x, int y, int theClickCount)
{

}

void ZombatarWidget::MouseUp(int x, int y, int theClickCount)
{
	if (mBackButton->IsMouseOver())
	{
		mApp->mGameSelector->SlideTo(0, 0);
		mWidgetManager->SetFocus(mApp->mGameSelector);
	}
	else if (mViewButton->IsMouseOver())
	{

	}
	else if (mFinishedButton->IsMouseOver())
	{

	}
	else if (mSkinButton->IsMouseOver())
	{
		ChangePage(PAGE_SKIN);

	}
	else if (mHairButton->IsMouseOver())
	{
		ChangePage(PAGE_HAIR);
	}
	else if (mFacialHairButton->IsMouseOver())
	{
		ChangePage(PAGE_FACIAL_HAIR);
	}
	else if (mTidbitsButton->IsMouseOver())
	{
		ChangePage(PAGE_TIDBITS);
	}
	else if (mEyewearButton->IsMouseOver())
	{
		ChangePage(PAGE_EYEWEAR);
	}
	else if (mClothesButton->IsMouseOver())
	{
		ChangePage(PAGE_CLOTHES);
	}
	else if (mAccessoriesButton->IsMouseOver())
	{
		ChangePage(PAGE_ACCESSORIES);
	}
	else if (mHatsButton->IsMouseOver())
	{
		ChangePage(PAGE_HATS);
	}
	else if (mBackdropsButton->IsMouseOver())
	{
		ChangePage(PAGE_BACKDROPS);
	}
}