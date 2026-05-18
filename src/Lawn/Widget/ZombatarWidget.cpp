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

	mDummyZombatarButton = new GameButton(1);
	mDummyZombatarButton->mButtonImage = IMAGE_BRAIN;
	mDummyZombatarButton->mOverImage = IMAGE_MONEYBAG;
	mDummyZombatarButton->mDownImage = IMAGE_MONEYBAG_HI_RES;
	mDummyZombatarButton->mParentWidget = this;
	mDummyZombatarButton->Resize(0, 0, IMAGE_BRAIN->mWidth, IMAGE_BRAIN->mWidth);

	mZombie = new Zombie();
	mZombie->mBoard = nullptr;
	mZombie->ZombieInitialize(0, ZombieType::ZOMBIE_FLAG, false, nullptr, Zombie::ZOMBIE_WAVE_UI);
	mZombie->mPosX = 641;
	mZombie->mPosY = 350;
	mZombie->SetupZombatar();
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
	if (mDummyZombatarButton)
		delete mDummyZombatarButton;
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

	int aWidgetX = 25;
	int aWidgetY = 25;
	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_BG, aWidgetX, aWidgetY);
	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_INNER_BG, aWidgetX + 127, aWidgetY + 100);
	g->DrawImage(IMAGE_ZOMBATAR_DISPLAY_WINDOW, 5, 0);
	g->DrawImage(IMAGE_ZOMBATAR_COLORS_BG, 221, 335);
	mDummyZombatarButton->Draw(g);
	mBackButton->Draw(g);
}

void ZombatarWidget::Update()
{
	MarkDirty();
	mZombie->Update();
	mDummyZombatarButton->Update();
	mBackButton->Update();
}

void ZombatarWidget::MouseDown(int x, int y, int theClickCount)
{
	if (mDummyZombatarButton->IsMouseOver())
	{
		mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
	}
}

void ZombatarWidget::MouseUp(int x, int y, int theClickCount)
{
	if (mDummyZombatarButton->IsMouseOver())
	{
		mApp->mPlayerInfo->mNumZombatars++;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mHair = 1;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mHairColor = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mFacialHair = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mFacialHairColor = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mAccessories = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mAccessoriesColor = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mTidbits = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mTidbitsColor = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mClothes = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mClothesColor = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mSkin = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mSkinColor = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mEyewear = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mEyewearColor = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mHat = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mHatColor = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mBackdrop = 0;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex].mBackdropColor = 0;
		mApp->mPlayerInfo->mZombatarIndex = mApp->mPlayerInfo->mNumZombatars - 1;
		mZombie->UpdateZombatar(mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex]);
	}
	else if (mBackButton->IsMouseOver())
	{
		mApp->mGameSelector->SlideTo(0, 0);
		mWidgetManager->SetFocus(mApp->mGameSelector);
	}
}