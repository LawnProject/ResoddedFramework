#include "ZombatarWidget.h"
#include "GameSelector.h"
#include "../../LawnApp.h"
#include "../Zombie.h"
#include "../../GameConstants.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/WidgetManager.h"


Color gSkinColors[12] = {
	Color(134, 147, 122), Color(79, 135, 94), Color(127, 135, 94),	Color(120, 130, 50),
	Color(156, 163, 105), Color(96, 151, 11), Color(147, 184, 77),	Color(82, 143, 54),
	Color(121, 168, 99),  Color(65, 156, 74), Color(107, 178, 114), Color(104, 121, 90),
};
Color gMoreColors[18] = {Color(151, 33, 33),  Color(199, 53, 53),  Color(220, 112, 47),	 Color(251, 251, 172),
						 Color(240, 210, 87), Color(165, 126, 65), Color(106, 72, 32),	 Color(72, 35, 5),
						 Color(50, 56, 61),	  Color(0, 0, 0),	   Color(197, 239, 239), Color(63, 109, 242),
						 Color(13, 202, 151), Color(158, 183, 19), Color(30, 210, 64),	 Color(225, 65, 230),
						 Color(128, 47, 204), Color::White};

Rect mColorRects[18];

ZombatarWidget::ZombatarWidget(LawnApp *theApp)
{
	mPage = PAGE_SKIN;
	mApp = theApp;
	mWidth = BOARD_WIDTH;
	mHeight = BOARD_HEIGHT;
	TodLoadResources("DelayLoad_Almanac");
	TodLoadResources("DelayLoad_Zombatar");
	mZombatar.mSkinColor = 0;
	mZombatar.mClothes = -1;
	mZombatar.mClothesColor = 17;
	mZombatar.mTidbits = -1;
	mZombatar.mTidbitsColor = 17;
	mZombatar.mAccessories = -1;
	mZombatar.mAccessoriesColor = 17;
	mZombatar.mFacialHair = -1;
	mZombatar.mFacialHairColor = 17;
	mZombatar.mHair =-1;
	mZombatar.mHairColor = 17;
	mZombatar.mEyewear = -1;
	mZombatar.mEyewearColor = 17;
	mZombatar.mHat = -1;
	mZombatar.mHatColor = 17;
	mZombatar.mBackdrop = 0;
	mZombatar.mBackdropColor = 17;

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

	for (int i = 0; i < 18; i++)
	{
		mColorRects[i] = Rect(238 + (i % 9) * (Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth + 4),
							  367 + (i / 9) * (Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight + 4),
							  Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth, 
							  Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight);
	}

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

int ZombatarWidget::GetPageColorIndex(ZombatarPage thePage)
{

	switch (thePage)
	{
		case PAGE_SKIN: {
			return mZombatar.mSkinColor;
		}
		case PAGE_HAIR: {
			return mZombatar.mHairColor;
		}
		case PAGE_FACIAL_HAIR: {
			return mZombatar.mFacialHairColor;
		}
		case PAGE_TIDBITS: {
			return mZombatar.mTidbitsColor;
		}
		case PAGE_EYEWEAR: {
			return mZombatar.mEyewearColor;
		}
		case PAGE_CLOTHES: {
			return mZombatar.mClothesColor;
		}
		case PAGE_ACCESSORIES: {
			return mZombatar.mAccessoriesColor;
		}
		case PAGE_HATS: {
			return mZombatar.mHatColor;
		}
		case PAGE_BACKDROPS: {
			return mZombatar.mBackdropColor;
		}
	}
}

void ZombatarWidget::DrawPortrait(Graphics *g, int theX, int theY)
{
	int aSkinIndex = mZombatar.mSkinColor;
	int aBackdropColor = mZombatar.mBackdropColor;
	int aBackdropIndex = mZombatar.mBackdrop;
	g->PushState();
	g->Translate(theX, theY);
	g->SetColorizeImages(true);
	g->SetColor(Color::White);
	if (aBackdropIndex == 0 && aBackdropColor != 17)
	{
		g->SetColor(gMoreColors[aBackdropColor]);
	}
	g->DrawImage(Sexy::IMAGE_ZOMBATAR_BACKGROUND_BLANK, 0, 0);
	g->SetColor(gSkinColors[aSkinIndex]);
	g->DrawImage(Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN, 38, 40);
	g->SetColor(Color::White);
	g->SetColorizeImages(false);

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
	int aMaxColor = 18;
	if (mPage == PAGE_SKIN)
		aMaxColor = 12;
	int aCurrentItemColor = GetPageColorIndex(mPage);
	for (int i = 0; i < aMaxColor; i++)
	{

		g->SetColorizeImages(true);
		g->SetColor(mPage == PAGE_SKIN ? gSkinColors[i] : gMoreColors[i]);
		if (aCurrentItemColor != i)
		{
			g->mColor.mAlpha = 66;
			if (mColorRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY))
			{
				g->mColor.mAlpha = 127;
			}
		}
			

		g->DrawImage(i == 17 ? Sexy::IMAGE_ZOMBATAR_COLORPICKER_NONE : Sexy::IMAGE_ZOMBATAR_COLORPICKER, mColorRects[i],
					 Rect(0, 0, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight));
		g->SetColorizeImages(false);

	}
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
	bool anOverlapsColor = false;
	int aMaxColor = 18;
	if (mPage == PAGE_SKIN)
		aMaxColor = 12;
	for (int i = 0; i < aMaxColor; i++)
	{
		if (mColorRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY))
		{
			anOverlapsColor = true;
			break;
		}
	}
	if (mHasFocus)
	{
		if (mViewButton->IsMouseOver() || mBackButton->IsMouseOver() || mFinishedButton->IsMouseOver() ||
			mSkinButton->IsMouseOver() || mHairButton->IsMouseOver() || mFacialHairButton->IsMouseOver() ||
			mTidbitsButton->IsMouseOver() || mEyewearButton->IsMouseOver() || mClothesButton->IsMouseOver() ||
			mAccessoriesButton->IsMouseOver() || mHatsButton->IsMouseOver() || mBackdropsButton->IsMouseOver() ||
			anOverlapsColor)
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
		mApp->mPlayerInfo->mNumZombatars++;
		mApp->mPlayerInfo->mZombatarIndex = mApp->mPlayerInfo->mNumZombatars - 1;
		mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex] = mZombatar;
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

	int aMaxColor = 18;
	if (mPage == PAGE_SKIN)
		aMaxColor = 12;
	for (int i = 0; i < aMaxColor; i++)
	{
		if (mColorRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY))
		{
			switch (mPage)
			{
				case PAGE_SKIN: 
				{
					mZombatar.mSkinColor = i;
					break;
				}
				case PAGE_HAIR: 
				{
					mZombatar.mHairColor = i;
					break;
				}
				case PAGE_FACIAL_HAIR: 
				{
					mZombatar.mFacialHairColor = i;
					break;
				}
				case PAGE_TIDBITS: 
				{
					mZombatar.mTidbitsColor = i;
					break;
				}
				case PAGE_EYEWEAR: 
				{
					mZombatar.mEyewearColor = i;
					break;
				}
				case PAGE_CLOTHES: 
				{
					mZombatar.mClothesColor = i;
					break;
				}
				case PAGE_ACCESSORIES: 
				{
					mZombatar.mAccessoriesColor = i;
					break;
				}
				case PAGE_HATS: 
				{
					mZombatar.mHatColor = i;
					break;
				}
				case PAGE_BACKDROPS: 
				{
					mZombatar.mBackdropColor = i;
					break;
				}
			}
			break;
		}
	}
}