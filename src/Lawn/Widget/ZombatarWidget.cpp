#include "ZombatarWidget.h"
#include "GameSelector.h"
#include "../../LawnApp.h"
#include "../Zombie.h"
#include "LawnDialog.h"
#include "../../GameConstants.h"
#include "../../Resources.h"
#include "../../FrameworkResources.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../SexyAppFramework/Dialog.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../SexyAppFramework/Renderer.h"
#include "../../SexyAppFramework/GPUImage.h"
#include "../../SexyAppFramework/Font.h"
#include "../../ImageLib/ImageLib.h"

PortraitItem gPortraitItems[ZombatarItem::NUM_ZOMBATAR_ITEMS]
{
	{&IMAGE_ZOMBATAR_CLOTHES_1, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_2, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_3, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_4, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_5, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_6, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_7, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_8, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_9, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_10, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_11, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_CLOTHES_12, nullptr, false, 0, 0, 0, 0},

	{&IMAGE_ZOMBATAR_HATS_1, &IMAGE_ZOMBATAR_HATS_1_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_2, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_3, &IMAGE_ZOMBATAR_HATS_3_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_4, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_5, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_6, &IMAGE_ZOMBATAR_HATS_6_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_7, &IMAGE_ZOMBATAR_HATS_7_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_8, &IMAGE_ZOMBATAR_HATS_8_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_9, &IMAGE_ZOMBATAR_HATS_9_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_10, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_11, &IMAGE_ZOMBATAR_HATS_11_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_12, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_13, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HATS_14, nullptr, true, 0, 0, 0, 0},

	{&IMAGE_ZOMBATAR_HAIR_1, &IMAGE_ZOMBATAR_HAIR_1_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_2, &IMAGE_ZOMBATAR_HAIR_2_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_3, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_4, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_5, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_6, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_7, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_8, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_9, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_10, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_11, &IMAGE_ZOMBATAR_HAIR_11_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_12, &IMAGE_ZOMBATAR_HAIR_12_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_13, &IMAGE_ZOMBATAR_HAIR_13_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_14, &IMAGE_ZOMBATAR_HAIR_14_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_15, &IMAGE_ZOMBATAR_HAIR_15_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_HAIR_16, nullptr, true, 0, 0, 0, 0},

	{&IMAGE_ZOMBATAR_EYEWEAR_1, &IMAGE_ZOMBATAR_EYEWEAR_1_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_2, &IMAGE_ZOMBATAR_EYEWEAR_2_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_3, &IMAGE_ZOMBATAR_EYEWEAR_3_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_4, &IMAGE_ZOMBATAR_EYEWEAR_4_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_5, &IMAGE_ZOMBATAR_EYEWEAR_5_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_6, &IMAGE_ZOMBATAR_EYEWEAR_6_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_7, &IMAGE_ZOMBATAR_EYEWEAR_7_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_8, &IMAGE_ZOMBATAR_EYEWEAR_8_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_9, &IMAGE_ZOMBATAR_EYEWEAR_9_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_10, &IMAGE_ZOMBATAR_EYEWEAR_10_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_11, &IMAGE_ZOMBATAR_EYEWEAR_11_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_12, &IMAGE_ZOMBATAR_EYEWEAR_12_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_13, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_14, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_15, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_EYEWEAR_16, nullptr, false, 0, 0, 0, 0},

    {&IMAGE_ZOMBATAR_FACIALHAIR_1, &IMAGE_ZOMBATAR_FACIALHAIR_1_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_2, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_3, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_4, &IMAGE_ZOMBATAR_FACIALHAIR_4_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_5, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_6, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_7, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_8, &IMAGE_ZOMBATAR_FACIALHAIR_8_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_9, &IMAGE_ZOMBATAR_FACIALHAIR_9_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_10, &IMAGE_ZOMBATAR_FACIALHAIR_10_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_11, &IMAGE_ZOMBATAR_FACIALHAIR_11_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_12, &IMAGE_ZOMBATAR_FACIALHAIR_12_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_13, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_14, &IMAGE_ZOMBATAR_FACIALHAIR_14_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_15, &IMAGE_ZOMBATAR_FACIALHAIR_15_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_16, &IMAGE_ZOMBATAR_FACIALHAIR_16_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_17, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_18, &IMAGE_ZOMBATAR_FACIALHAIR_18_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_19, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_20, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_21, &IMAGE_ZOMBATAR_FACIALHAIR_21_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_22, &IMAGE_ZOMBATAR_FACIALHAIR_22_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_23, &IMAGE_ZOMBATAR_FACIALHAIR_23_MASK, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_FACIALHAIR_24, &IMAGE_ZOMBATAR_FACIALHAIR_24_MASK, true, 0, 0, 0, 0},

    {&IMAGE_ZOMBATAR_TIDBITS_1, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_2, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_3, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_4, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_5, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_6, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_7, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_8, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_9, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_10, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_11, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_12, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_13, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_TIDBITS_14, nullptr, false, 0, 0, 0, 0},

	{&IMAGE_ZOMBATAR_ACCESSORY_1, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_2, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_3, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_4, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_5, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_6, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_8, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_9, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_10, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_11, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_12, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_13, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_14, nullptr, true, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_15, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_ACCESSORY_16, nullptr, false, 0, 0, 0, 0},

	{&IMAGE_ZOMBATAR_BACKGROUND_CRAZYDAVE, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_BACKGROUND_MENU, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_BACKGROUND_MENU_DOS, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_BACKGROUND_ROOF, nullptr, false, 0, 0, 0, 0},
	{&IMAGE_ZOMBATAR_BACKGROUND_BLANK, nullptr, true, 0, 0, 0, 0},
};


Color gSkinColors[12] = {
	Color(134, 147, 122), Color(79, 135, 94), Color(127, 135, 94),	Color(120, 130, 50),
	Color(156, 163, 105), Color(96, 151, 11), Color(147, 184, 77),	Color(82, 143, 54),
	Color(121, 168, 99),  Color(65, 156, 74), Color(107, 178, 114), Color(104, 121, 90),
};
Color gMoreColors[18] = {Color(151, 33, 33),  Color(199, 53, 53),  Color(220, 112, 47),	 Color(251, 251, 172),
						 Color(240, 210, 87), Color(165, 126, 65), Color(106, 72, 32),	 Color(72, 35, 5),
						 Color(50, 56, 61),	  Color(0, 0, 0),	   Color(197, 239, 239), Color(63, 109, 242),
						 Color(13, 202, 151), Color(158, 183, 19), Color(30, 210, 64),	 Color(225, 65, 230),
						 Color(128, 47, 204), Color(255, 255, 255)};

static const int TimeTransitionEnd = 100;


ZombatarWidget::ZombatarWidget(LawnApp *theApp)
{
	mPage = PAGE_SKIN;

	mApp = theApp;
	mWidth = BOARD_WIDTH;
	mHeight = BOARD_HEIGHT;
	mTransitionTimer = 0;
	mSubPage = 0;
	if (mApp->mPlayerInfo != nullptr && mApp->mPlayerInfo->mNumZombatars <= 0)
		mState = STATE_AVATAR_CREATION;
	else
		mState = STATE_AVATAR_LIST;
	TodLoadResources("DelayLoad_Almanac");
	TodLoadResources("DelayLoad_Zombatar");
	ResetPortrait();

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

	mAvatarBackButton = new GameButton(ZombatarWidget::ZOMBATAR_BACK_AVATAR);
	mAvatarBackButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_BACK_BUTTON;
	mAvatarBackButton->mOverImage = Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT;
	mAvatarBackButton->mDownImage = Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT;
	mAvatarBackButton->Resize(385, 345, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT->mHeight);

	mNewZombatarButton = new GameButton(ZombatarWidget::ZOMBATAR_NEW);
	mNewZombatarButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON;
	mNewZombatarButton->mOverImage = Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT;
	mNewZombatarButton->mDownImage = Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT;
	mNewZombatarButton->Resize(195, 395, Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT->mHeight);
	
	mNextButton = new GameButton(ZombatarWidget::ZOMBATAR_LEFT);
	mNextButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON;
	mNextButton->mOverImage = Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT;
	mNextButton->mDownImage = Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT;
	mNextButton->Resize(467, 398, Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT->mWidth,
						Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT->mHeight);
	
	mPrevButton = new GameButton(ZombatarWidget::ZOMBATAR_LEFT);
	mPrevButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_PREV_BUTTON;
	mPrevButton->mOverImage = Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT;
	mPrevButton->mDownImage = Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT;
	mPrevButton->Resize(120, 398, Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT->mWidth,
						Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT->mHeight);
	
	mDeleteZombatarButton = new GameButton(ZombatarWidget::ZOMBATAR_DELETE);
	mDeleteZombatarButton->mButtonImage = Sexy::IMAGE_BLANK;
	mDeleteZombatarButton->mOverImage = Sexy::IMAGE_BLANK;
	mDeleteZombatarButton->mDownImage = Sexy::IMAGE_BLANK;
	mDeleteZombatarButton->SetLabel("[ZOMBATAR_DELETE_BUTTON2]");
	mDeleteZombatarButton->SetFont(Sexy::FONT_DWARVENTODCRAFT12);
	mDeleteZombatarButton->mColors[0] = Color(255, 255, 255);
	mDeleteZombatarButton->mColors[1] = Color(0, 255, 40);
	mDeleteZombatarButton->Resize(337, 145, Sexy::FONT_DWARVENTODCRAFT12->StringWidth(mDeleteZombatarButton->mLabel), 20);

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

	for (int i = 0; i < 18; i++)
	{
		mItemRects[i] = Rect(166 + (i % 6) * (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth - 4),
							  137 + (i / 6) * (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight - 1), Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
	}

	ChangePage(mPage);
	ChangeState(mState);
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
	if (mAvatarBackButton)
		delete mAvatarBackButton;
	if (mNewZombatarButton)
		delete mNewZombatarButton;
	if (mNextButton)
		delete mNextButton;
	if (mPrevButton)
		delete mPrevButton;
	if (mDeleteZombatarButton)
		delete mDeleteZombatarButton;
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
	mSubPage = 0;

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

void ZombatarWidget::ChangeState(ZombatarState theState)
{
	if (theState == STATE_AVATAR_CREATION)
	{
		mFinishedButton->mX = 445;
		mFinishedButton->mY = 472;
	}
	else if (theState == STATE_CONFIRM)
	{
		mFinishedButton->mX = 155;
		mFinishedButton->mY = 345;
	}

	mState = theState;
	mViewButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mSkinButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mHairButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mFacialHairButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mTidbitsButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mEyewearButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mClothesButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mHatsButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mAccessoriesButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mBackdropsButton->mDisabled = mState != STATE_AVATAR_CREATION;

	mAvatarBackButton->mDisabled = mState != STATE_CONFIRM;
	mFinishedButton->mDisabled = mState == STATE_AVATAR_LIST;
	mNewZombatarButton->mDisabled = mState != STATE_AVATAR_LIST;
	mNextButton->mDisabled = mState != STATE_AVATAR_LIST || (mApp->mPlayerInfo && mApp->mPlayerInfo->mZombatarIndex == mApp->mPlayerInfo->mNumZombatars - 1);
	mPrevButton->mDisabled = mState != STATE_AVATAR_LIST || (mApp->mPlayerInfo && mApp->mPlayerInfo->mZombatarIndex == 0);
	mDeleteZombatarButton->mDisabled = mState != STATE_AVATAR_LIST;
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
	return -1;
}

int ZombatarWidget::GetPageItemIndex(ZombatarPage thePage)
{
	switch (thePage)
	{
		case PAGE_HAIR: {
			return mZombatar.mHair;
		}
		case PAGE_FACIAL_HAIR: {
			return mZombatar.mFacialHair;
		}
		case PAGE_TIDBITS: {
			return mZombatar.mTidbits;
		}
		case PAGE_EYEWEAR: {
			return mZombatar.mEyewear;
		}
		case PAGE_CLOTHES: {
			return mZombatar.mClothes;
		}
		case PAGE_ACCESSORIES: {
			return mZombatar.mAccessories;
		}
		case PAGE_HATS: {
			return mZombatar.mHat;
		}
		case PAGE_BACKDROPS: {
			return mZombatar.mBackdrop;
		}
	}
	return -1;
}

int ZombatarWidget::GetItemCount(ZombatarPage thePage)
{
	switch (thePage)
	{
		case PAGE_HAIR: {
			return 16;
		}
		case PAGE_FACIAL_HAIR: {
			return mSubPage == 0 ? 17 : 7;
		}
		case PAGE_TIDBITS: {
			return 14;
		}
		case PAGE_EYEWEAR: {
			return 16;
		}
		case PAGE_CLOTHES: {
			return 12;
		}
		case PAGE_ACCESSORIES: {
			return 15;
		}
		case PAGE_HATS: {
			return 14;
		}
		case PAGE_BACKDROPS: {
			return 5;
		}
	}
	return -1;
}

void ZombatarWidget::DrawCurrentPortrait(Graphics *g, int theX, int theY)
{
	DrawIndexedPortrait(-1, g, theX, theY);
}

void ZombatarWidget::DrawIndexedPortrait(int theIndex, Graphics *g, int theX, int theY)
{
	TOD_ASSERT(theIndex < mApp->mPlayerInfo->mNumZombatars);
	Zombatar aZombatar;

	if (theIndex >= 0)
		aZombatar = mApp->mPlayerInfo->mZombatars[theIndex];
	else
		aZombatar = mZombatar; // the placeholder/ current zombatar
	int aSkinIndex = aZombatar.mSkinColor;

	g->PushState();
	g->Translate(theX, theY);
	g->SetColorizeImages(true);
	g->SetColor(Color::White);
	DrawPortraitItem(ZOMBATAR_BACKGROUND_CRAZYDAVE + aZombatar.mBackdrop, g, aZombatar);
	g->SetColor(gSkinColors[aSkinIndex]);
	g->DrawImage(Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN, 38, 40);
	g->SetColor(Color::White);

	g->SetColorizeImages(false);

	g->DrawImage(Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK, 38, 40);
	if (aZombatar.mClothes != -1)
		DrawPortraitItem(ZOMBATAR_CLOTHES_1 + aZombatar.mClothes, g, aZombatar);
	if (aZombatar.mAccessoriesColor != -1)
		DrawPortraitItem(ZOMBATAR_ACCESSORY_1 + aZombatar.mAccessories, g, aZombatar);
	if (aZombatar.mTidbits != -1)
		DrawPortraitItem(ZOMBATAR_TIDBITS_1 + aZombatar.mTidbits, g, aZombatar);
	if (aZombatar.mFacialHair != -1)
		DrawPortraitItem(ZOMBATAR_FACIALHAIR_1 + aZombatar.mFacialHair, g, aZombatar);
	if (aZombatar.mHair != -1)
		DrawPortraitItem(ZOMBATAR_HAIR_1 + aZombatar.mHair, g, aZombatar);
	if (aZombatar.mEyewear != -1)
		DrawPortraitItem(ZOMBATAR_EYEWEAR_1 + aZombatar.mEyewear, g, aZombatar);
	if (aZombatar.mHat != -1)
		DrawPortraitItem(ZOMBATAR_HATS_1 + aZombatar.mHat, g, aZombatar);

	g->PopState();
}

void ZombatarWidget::DrawPortraitItem(int theItem, Graphics *g, const Zombatar &theZombatarInstance)
{
	g->PushState();
	PortraitItem anItem = gPortraitItems[theItem];

	g->SetColorizeImages(true);
	g->SetColor(Color::White);
	if (anItem.mAllowColor)
	{
		if (theItem >= ZOMBATAR_HATS_1 && theItem <= ZOMBATAR_HATS_14)
			g->SetColor(gMoreColors[theZombatarInstance.mHatColor]);
		else if (theItem >= ZOMBATAR_HAIR_1 && theItem <= ZOMBATAR_HAIR_16)
			g->SetColor(gMoreColors[theZombatarInstance.mHairColor]);
		else if (theItem >= ZOMBATAR_EYEWEAR_1 && theItem <= ZOMBATAR_EYEWEAR_16)
			g->SetColor(gMoreColors[theZombatarInstance.mEyewearColor]);
		else if (theItem >= ZOMBATAR_FACIALHAIR_1 && theItem <= ZOMBATAR_FACIALHAIR_24)
			g->SetColor(gMoreColors[theZombatarInstance.mFacialHairColor]);
		else if (theItem >= ZOMBATAR_TIDBITS_1 && theItem <= ZOMBATAR_TIDBITS_14)
			g->SetColor(gMoreColors[theZombatarInstance.mTidbits]);
		else if (theItem >= ZOMBATAR_ACCESSORY_1 && theItem <= ZOMBATAR_ACCESSORY_15)
			g->SetColor(gMoreColors[theZombatarInstance.mAccessoriesColor]);
		else if (theItem >= ZOMBATAR_BACKGROUND_CRAZYDAVE && theItem <= ZOMBATAR_BACKGROUND_BLANK)
			g->SetColor(gMoreColors[theZombatarInstance.mBackdropColor]);
	}
	if (anItem.mColor == nullptr)
		g->DrawImage(*anItem.mLine, anItem.mOffsetX, anItem.mOffsetY);
	else
	{
		g->DrawImage(*anItem.mColor, anItem.mOffsetX + anItem.mColorOffsetX, anItem.mOffsetY + anItem.mColorOffsetY);
		g->SetColor(Color::White);
		g->DrawImage(*anItem.mLine, anItem.mOffsetX, anItem.mOffsetY);
	}
	g->PopState();

}

int ItemTypeOffset(ZombatarPage thePage)
{
	if (thePage == PAGE_HATS)
		return ZOMBATAR_HATS_1;
	else if (thePage == PAGE_HAIR)
		return ZOMBATAR_HAIR_1;
	else if (thePage == PAGE_EYEWEAR)
		return ZOMBATAR_EYEWEAR_1;
	else if (thePage == PAGE_FACIAL_HAIR)
		return ZOMBATAR_FACIALHAIR_1;
	else if (thePage == PAGE_TIDBITS)
		return ZOMBATAR_TIDBITS_1;
	else if (thePage == PAGE_ACCESSORIES)
		return ZOMBATAR_ACCESSORY_1;
	else if (thePage == PAGE_BACKDROPS)
		return ZOMBATAR_BACKGROUND_CRAZYDAVE;
	return 0;
}

void ZombatarWidget::Draw(Graphics *g)
{
	if (mApp->mPlayerInfo == nullptr)
		return; //no need to draw zombatar if the save file isn't created.
#if LAWN_WIDESCREEN
	g->DrawImage(Sexy::IMAGE_ZOMBATAR_BG_WIDESCREEN, 0, 0);
#else
	g->DrawImage(IMAGE_ZOMBATAR_MAIN_BG, 0, 0);
#endif 
	mBackButton->Draw(g);

	g->ClipRect(600, 300, 170, 200);
	g->DrawImage(IMAGE_ALMANAC_GROUNDDAY, 600, 300);

	Graphics aZombieGraphics = Graphics(*g);
	mZombie->BeginDraw(&aZombieGraphics);
	mZombie->Draw(&aZombieGraphics);

	g->ClearClipRect();

	DrawCurrentPortrait(g, 592, 115);

	g->DrawImage(IMAGE_ZOMBATAR_DISPLAY_WINDOW, 5, 0);

	int aWidgetX = 25;
	int aWidgetY = 25;
	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_BG, aWidgetX, aWidgetY);

	if (mState == STATE_AVATAR_LIST)
	{
		TodDrawString(g, StrFormat("%d / %d",  mApp->mPlayerInfo->mZombatarIndex + 1, mApp->mPlayerInfo->mNumZombatars), 221, 161, Sexy::FONT_BRIANNETOD12, Color::White, DS_ALIGN_LEFT);
		g->ClipRect(58, 113, 497, 365);

		for (int i = 0; i < mApp->mPlayerInfo->mNumZombatars; i++)
		{
			int x = 220 + (i - mApp->mPlayerInfo->mZombatarIndex) * 220;
			DrawIndexedPortrait(i, g, x, 175);

		}
		g->ClearClipRect();
		mNewZombatarButton->Draw(g);
		if (!mNextButton->mDisabled)
			mNextButton->Draw(g);
		if (!mPrevButton->mDisabled)
			mPrevButton->Draw(g);
		mDeleteZombatarButton->Draw(g);

		return;
	}

	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_INNER_BG, aWidgetX + 127, aWidgetY + 100);
	g->DrawImage(IMAGE_ZOMBATAR_COLORS_BG, 221, 335);

	int aCurrentItemIndex = GetPageItemIndex(mPage);
	for (int i = 0; i < GetItemCount(mPage) + 1; i++)
	{
		if (mPage == PAGE_BACKDROPS && GetItemCount(mPage) == i)
			continue;
		g->SetColor(Color::White);

		g->SetColorizeImages(true);
		Image *aBGImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG;

		if (aCurrentItemIndex != i)
		{
			g->mColor.mAlpha = 66;
			if (mItemRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY) && GetItemCount(mPage) != i)
			{
				g->mColor.mAlpha = 255;
			}
		}
		else
		{
			g->mColor.mAlpha = 255;
			aBGImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG_HIGHLIGHT;
		}

		if (GetItemCount(mPage) == i)
			aBGImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG_NONE;
		g->DrawImage(aBGImage, mItemRects[i], Rect(0, 0, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight));

		g->SetColorizeImages(false);
	}
	int aItemIndex = ItemTypeOffset(mPage) + aCurrentItemIndex;
	if (mPage == PAGE_SKIN || (aCurrentItemIndex != -1 && gPortraitItems[aItemIndex].mAllowColor))
	{
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

			g->DrawImage(
				i == 17 ? Sexy::IMAGE_ZOMBATAR_COLORPICKER_NONE : Sexy::IMAGE_ZOMBATAR_COLORPICKER, mColorRects[i],
				Rect(0, 0, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth, Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight));
			g->SetColorizeImages(false);
		}
	}
	else
	{
		if (aCurrentItemIndex == -1)
			TodDrawString(g, "[ZOMBATAR_COLOR_ITEM_NOT_CHOSEN]", 349, 394, Sexy::FONT_BRIANNETOD12, Color(255, 255, 255), DS_ALIGN_CENTER);
		else
			TodDrawString(g, "[ZOMBATAR_COLOR_NOT_APPLICABLE]", 349, 394, Sexy::FONT_BRIANNETOD12, Color(255, 255, 255), DS_ALIGN_CENTER);

	}

	if (mPage == PAGE_SKIN)
	{
		TodDrawStringWrapped(g, "[ZOMBATAR_START_TEXT]", Rect(145, 185, 420, 120), Sexy::FONT_DWARVENTODCRAFT15,
							 Color(208, 190, 44), DS_ALIGN_CENTER);
	}

	mSkinButton->Draw(g);
	mHairButton->Draw(g);
	mFacialHairButton->Draw(g);
	mTidbitsButton->Draw(g);
	mEyewearButton->Draw(g);
	mClothesButton->Draw(g);
	mAccessoriesButton->Draw(g);
	mHatsButton->Draw(g);
	mBackdropsButton->Draw(g);

	if (mTransitionTimer == 0 && mApp->mPlayerInfo->mNumZombatars > 0)
		mViewButton->Draw(g);


	if (mState == STATE_CONFIRM_TRANSITION)
	{
		g->SetColorizeImages(true);
		g->SetColor(Color::White);
		g->mColor.mAlpha = TodAnimateCurve(0, TimeTransitionEnd, mTransitionTimer, 0, 255, TodCurves::CURVE_LINEAR);
		g->DrawImage(IMAGE_ZOMBATAR_WIDGET_BG, aWidgetX, aWidgetY);
		g->SetColorizeImages(false);

		mFinishedButton->mX = TodAnimateCurve(0, TimeTransitionEnd, mTransitionTimer, 445, 155, TodCurves::CURVE_LINEAR);
		mFinishedButton->mY = TodAnimateCurve(0, TimeTransitionEnd, mTransitionTimer, 472, 345, TodCurves::CURVE_LINEAR);

	}
	else if (mState == STATE_AVATAR_TRANSITION)
	{
		g->SetColorizeImages(true);
		g->SetColor(Color::White);
		g->mColor.mAlpha = TodAnimateCurve(0, TimeTransitionEnd, mTransitionTimer, 255, 0, TodCurves::CURVE_LINEAR);
		g->DrawImage(IMAGE_ZOMBATAR_WIDGET_BG, aWidgetX, aWidgetY);
		g->SetColorizeImages(false);

		mFinishedButton->mX = TodAnimateCurve(0, TimeTransitionEnd, mTransitionTimer, 155, 445, TodCurves::CURVE_LINEAR);
		mFinishedButton->mY = TodAnimateCurve(0, TimeTransitionEnd, mTransitionTimer, 345, 472, TodCurves::CURVE_LINEAR);

	}
	else if (mState == STATE_CONFIRM)
	{
		g->DrawImage(IMAGE_ZOMBATAR_WIDGET_BG, aWidgetX, aWidgetY);
		mAvatarBackButton->Draw(g);
		TodDrawString(g, "[ZOMBATAR_FINISHED_WARNING_HEADER]", 305, 185, Sexy::FONT_HOUSEOFTERROR28, Color(254, 227, 0), DS_ALIGN_CENTER);
		TodDrawString(g, "[ZOMBATAR_FINISHED_BUTTON_TEXT]", 195, 335, Sexy::FONT_BRIANNETOD12, Color(255, 255, 255), DS_ALIGN_CENTER);
		TodDrawString(g, "[ZOMBATAR_BACK_BUTTON_TEXT]", 435, 335, Sexy::FONT_BRIANNETOD12, Color(255, 255, 255), DS_ALIGN_CENTER);
		TodDrawStringWrapped(g, "[ZOMBATAR_FINISHED_WARNING_TEXT]", Rect(70, 225, 480, 80), Sexy::FONT_CONTINUUMBOLD14, Color(255, 255, 255), DS_ALIGN_CENTER);
	}

	mFinishedButton->Draw(g);
}

void ZombatarWidget::Update()
{

	MarkDirty();
	mZombie->Update();
	mBackButton->Update();
	mViewButton->Update();
	mFinishedButton->Update();
	mAvatarBackButton->Update();
	mNewZombatarButton->Update();
	mNextButton->Update();
	mPrevButton->Update();
	mDeleteZombatarButton->Update();
	mSkinButton->Update();
	mHairButton->Update();
	mFacialHairButton->Update();
	mTidbitsButton->Update();
	mEyewearButton->Update();
	mClothesButton->Update();
	mHatsButton->Update();
	mAccessoriesButton->Update();
	mBackdropsButton->Update();

	int aCurrentItemIndex = GetPageItemIndex(mPage);
	bool anOverlapsItem = false;

	for (int i = 0; i < GetItemCount(mPage) + 1; i++)
	{
		if (mPage == PAGE_BACKDROPS && GetItemCount(mPage) == i)
			continue;
		if (mItemRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY) && i != aCurrentItemIndex)
		{
			anOverlapsItem = mState == STATE_AVATAR_CREATION;
			break;
		}
	}

	bool anOverlapsColor = false;
	if (mPage == PAGE_SKIN || (aCurrentItemIndex != -1 && gPortraitItems[ItemTypeOffset(mPage) + aCurrentItemIndex].mAllowColor))
	{
		int aMaxColor = 18;
		if (mPage == PAGE_SKIN)
			aMaxColor = 12;
		int aCurrentItemColor = GetPageColorIndex(mPage);
		for (int i = 0; i < aMaxColor; i++)
		{
			if (mColorRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY) && i != aCurrentItemColor)
			{
				anOverlapsColor = mState == STATE_AVATAR_CREATION;
				break;
			}
		}

	}

	if (mHasFocus)
	{
		if (mViewButton->IsMouseOver() || mBackButton->IsMouseOver() || mFinishedButton->IsMouseOver() ||
			mSkinButton->IsMouseOver() || mHairButton->IsMouseOver() || mFacialHairButton->IsMouseOver() ||
			mTidbitsButton->IsMouseOver() || mEyewearButton->IsMouseOver() || mClothesButton->IsMouseOver() ||
			mAccessoriesButton->IsMouseOver() || mHatsButton->IsMouseOver() || mBackdropsButton->IsMouseOver() ||
			mAvatarBackButton->IsMouseOver() || mNewZombatarButton->IsMouseOver() || mNextButton->IsMouseOver() || 
			mPrevButton->IsMouseOver() || mDeleteZombatarButton->IsMouseOver() || anOverlapsColor || anOverlapsItem)
		{
			mApp->SetCursor(CURSOR_HAND);
		}
		else
		{
			mApp->SetCursor(CURSOR_POINTER);
		}
	}

	if (mState == STATE_AVATAR_TRANSITION || mState == STATE_CONFIRM_TRANSITION)
	{
		mTransitionTimer++;
		if (mTransitionTimer > TimeTransitionEnd)
		{
			ChangeState(mState == STATE_AVATAR_TRANSITION ? STATE_AVATAR_CREATION : STATE_CONFIRM);
			mTransitionTimer = 0;
		}
	}
}

void ZombatarWidget::ResetPortrait()
{
	mZombatar.mSkinColor = 0;
	mZombatar.mClothes = -1;
	mZombatar.mClothesColor = 17;
	mZombatar.mTidbits = -1;
	mZombatar.mTidbitsColor = 17;
	mZombatar.mAccessories = -1;
	mZombatar.mAccessoriesColor = 17;
	mZombatar.mFacialHair = -1;
	mZombatar.mFacialHairColor = 17;
	mZombatar.mHair = -1;
	mZombatar.mHairColor = 17;
	mZombatar.mEyewear = -1;
	mZombatar.mEyewearColor = 17;
	mZombatar.mHat = -1;
	mZombatar.mHatColor = 17;
	mZombatar.mBackdrop = 4;
	mZombatar.mBackdropColor = 17;
}

void ZombatarWidget::DeleteCurrentZombatar()
{
	for (int i = mApp->mPlayerInfo->mZombatarIndex; i < mApp->mPlayerInfo->mNumZombatars; i++)
	{
		mApp->mPlayerInfo->mZombatars[i] = mApp->mPlayerInfo->mZombatars[i + 1];
	}
	mApp->mPlayerInfo->mNumZombatars--;
	if (mApp->mPlayerInfo->mZombatarIndex >= mApp->mPlayerInfo->mNumZombatars)
		mApp->mPlayerInfo->mZombatarIndex = mApp->mPlayerInfo->mNumZombatars - 1;
	if (mApp->mPlayerInfo->mNumZombatars == 0)
	{
		ChangeState(STATE_AVATAR_CREATION);
		ChangePage(PAGE_SKIN);
		ResetPortrait();
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
		ResetPortrait();
		mZombie->ResetZombatar();
		ChangePage(PAGE_SKIN);
		if (mApp->mPlayerInfo->mNumZombatars <= 0)
			ChangeState(STATE_AVATAR_CREATION);
		else
			ChangeState(STATE_AVATAR_LIST);
	}
	else if (mNextButton->IsMouseOver())
	{
		if (mApp->mPlayerInfo->mZombatarIndex < mApp->mPlayerInfo->mNumZombatars - 1)
			mApp->mPlayerInfo->mZombatarIndex++;
		mNextButton->mDisabled = mApp->mPlayerInfo->mZombatarIndex == mApp->mPlayerInfo->mNumZombatars - 1;
		mPrevButton->mDisabled = mApp->mPlayerInfo->mZombatarIndex == 0;
	}
	else if (mPrevButton->IsMouseOver())
	{
		if (mApp->mPlayerInfo->mZombatarIndex > 0)
			mApp->mPlayerInfo->mZombatarIndex--;
		mNextButton->mDisabled = mApp->mPlayerInfo->mZombatarIndex == mApp->mPlayerInfo->mNumZombatars - 1;
		mPrevButton->mDisabled = mApp->mPlayerInfo->mZombatarIndex == 0;

	}
	else if (mDeleteZombatarButton->IsMouseOver())
	{
		LawnDialog *aDialog = (LawnDialog *)mApp->DoDialog(Dialogs::DIALOG_ZOMBATAR_DELETE, true, "[ZOMBATAR_DELETE_HEADER]", "[ZOMBATAR_DELETE_BODY]", "", Dialog::BUTTONS_OK_CANCEL);
		aDialog->mLawnYesButton->mLabel = TodStringTranslate("[ZOMBATAR_DELETE_BUTTON]");
	}
	else if (mViewButton->IsMouseOver())
	{
		ChangeState(STATE_AVATAR_LIST);
	}
	else if (mAvatarBackButton->IsMouseOver())
	{
		ChangeState(STATE_AVATAR_TRANSITION);
	}
	else if (mNewZombatarButton->IsMouseOver())
	{
		ChangeState(STATE_AVATAR_CREATION);
		ChangePage(PAGE_SKIN);
		ResetPortrait();
	}
	else if (mFinishedButton->IsMouseOver())
	{
		if (mState == STATE_AVATAR_CREATION)
		{
			ChangeState(STATE_CONFIRM_TRANSITION);
		}
		else if (mState == STATE_CONFIRM)
		{
			mApp->mPlayerInfo->mNumZombatars++;
			mApp->mPlayerInfo->mZombatarIndex = mApp->mPlayerInfo->mNumZombatars - 1;
			mApp->mPlayerInfo->mZombatars[mApp->mPlayerInfo->mZombatarIndex] = mZombatar;

			GPUImage *anExportImage = mApp->mRenderer->NewGPUImage();

			int aBitsCount =
				Sexy::IMAGE_ZOMBATAR_BACKGROUND_BLANK->mWidth * Sexy::IMAGE_ZOMBATAR_BACKGROUND_BLANK->mHeight;
			anExportImage->mBits = new uint32_t[aBitsCount + 1];
			anExportImage->mWidth = Sexy::IMAGE_ZOMBATAR_BACKGROUND_BLANK->mWidth;
			anExportImage->mHeight = Sexy::IMAGE_ZOMBATAR_BACKGROUND_BLANK->mHeight;
			anExportImage->mHasTrans = false;
			anExportImage->mHasAlpha = false;
			memset(anExportImage->mBits, 0, aBitsCount * 4);
			anExportImage->mBits[aBitsCount] = Sexy::MEMORYCHECK_ID;

			Graphics aGraphics(anExportImage);
			aGraphics.SetLinearBlend(true);
			DrawCurrentPortrait(&aGraphics, 0, 0);

			ImageLib::Image aImage;
			aImage.mWidth = anExportImage->mWidth;
			aImage.mHeight = anExportImage->mHeight;
			aImage.mNumChannels = 4;
			aImage.mBits = new uint32_t[aBitsCount + 1];
			memcpy(aImage.mBits, anExportImage->GetBits(), aBitsCount * 4);
			ImageLib::WriteImage(StrFormat("Zombatar_%d", mApp->mPlayerInfo->mNumZombatars), &aImage, ".png");
			delete anExportImage;

			ResetPortrait();
			ChangeState(STATE_AVATAR_LIST);
		}

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

	if (mState == STATE_AVATAR_CREATION)
	{
		for (int i = 0; i < GetItemCount(mPage) + 1; i++)
		{
			if (mPage == PAGE_BACKDROPS && GetItemCount(mPage) == i)
				continue;
			if (mItemRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY))
			{
				int aItemIndex = i;
				if (aItemIndex == GetItemCount(mPage)) //Is this delete item
					aItemIndex = -1;
				switch (mPage)
				{
				case PAGE_HAIR: {
					mZombatar.mHair = aItemIndex;
					break;
				}
				case PAGE_FACIAL_HAIR: {
					mZombatar.mFacialHair = aItemIndex;
					break;
				}
				case PAGE_TIDBITS: {
					mZombatar.mTidbits = aItemIndex;
					break;
				}
				case PAGE_EYEWEAR: {
					mZombatar.mEyewear = aItemIndex;
					break;
				}
				case PAGE_CLOTHES: {
					mZombatar.mClothes = aItemIndex;
					break;
				}
				case PAGE_ACCESSORIES: {
					mZombatar.mAccessories = aItemIndex;
					break;
				}
				case PAGE_HATS: {
					mZombatar.mHat = aItemIndex;
					break;
				}
				case PAGE_BACKDROPS: {
					mZombatar.mBackdrop = aItemIndex;
					break;
				}
				}
				mZombie->UpdateZombatar(mZombatar);
				break;
			}
		}

		int aCurrentItemIndex = GetPageItemIndex(mPage);

		if (mPage == PAGE_SKIN || (aCurrentItemIndex != -1 && gPortraitItems[ItemTypeOffset(mPage) + aCurrentItemIndex].mAllowColor))
		{
			int aMaxColor = 18;
			if (mPage == PAGE_SKIN)
				aMaxColor = 12;
			for (int i = 0; i < aMaxColor; i++)
			{
				if (mColorRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY))
				{
					switch (mPage)
					{
					case PAGE_SKIN: {
						mZombatar.mSkinColor = i;
						break;
					}
					case PAGE_HAIR: {
						mZombatar.mHairColor = i;
						break;
					}
					case PAGE_FACIAL_HAIR: {
						mZombatar.mFacialHairColor = i;
						break;
					}
					case PAGE_TIDBITS: {
						mZombatar.mTidbitsColor = i;
						break;
					}
					case PAGE_EYEWEAR: {
						mZombatar.mEyewearColor = i;
						break;
					}
					case PAGE_CLOTHES: {
						mZombatar.mClothesColor = i;
						break;
					}
					case PAGE_ACCESSORIES: {
						mZombatar.mAccessoriesColor = i;
						break;
					}
					case PAGE_HATS: {
						mZombatar.mHatColor = i;
						break;
					}
					case PAGE_BACKDROPS: {
						mZombatar.mBackdropColor = i;
						break;
					}
					}
					mZombie->UpdateZombatar(mZombatar);

					break;
				}
			}
		}
	}

}