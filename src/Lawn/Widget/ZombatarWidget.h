#pragma once
#include "../../GameConstants.h"
#include "../../SexyAppFramework/Widget.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "GameButton.h"
#include "../System/PlayerInfo.h"


using namespace Sexy;

class LawnApp;
class Zombie;

enum ZombatarPage
{
	PAGE_SKIN,
	PAGE_HAIR,
	PAGE_FACIAL_HAIR,
	PAGE_TIDBITS,
	PAGE_EYEWEAR,
	PAGE_CLOTHES,
	PAGE_ACCESSORIES,
	PAGE_HATS,
	PAGE_BACKDROPS,
};

enum ZombatarState
{
	STATE_AVATAR_LIST,
	STATE_AVATAR_TRANSITION,
	STATE_AVATAR_CREATION,
	STATE_CONFIRM_TRANSITION,
	STATE_CONFIRM,
};

class ZombatarWidget : public Widget, public ButtonListener
{
	enum
	{
		ZOMBATAR_BACK = 300,
		ZOMBATAR_VIEW,
		ZOMBATAR_FINISHED,
		ZOMBATAR_BACK_AVATAR,
		ZOMBATAR_NEW,
		ZOMBATAR_LEFT,
		ZOMBATAR_RIGHT,
		ZOMBATAR_DELETE,
		ZOMBATAR_SKIN,
		ZOMBATAR_HAIR,
		ZOMBATAR_FACIAL_HAIR,
		ZOMBATAR_TIDBITS,
		ZOMBATAR_EYEWEAR,
		ZOMBATAR_CLOTHES,
		ZOMBATAR_ACCESSORIES,
		ZOMBATAR_HATS,
		ZOMBATAR_BACKDROPS,
	};
  public:
	LawnApp *mApp;
	GameButton *mBackButton;
	GameButton *mViewButton;
	GameButton *mFinishedButton;
	GameButton *mAvatarBackButton;
	GameButton *mDeleteZombatarButton;
	GameButton *mNewZombatarButton;
	GameButton *mNextButton;
	GameButton *mPrevButton;


	GameButton *mToggledButton;


	GameButton *mSkinButton;
	GameButton *mHairButton;
	GameButton *mFacialHairButton;
	GameButton *mTidbitsButton;
	GameButton *mEyewearButton;
	GameButton *mClothesButton;
	GameButton *mAccessoriesButton;
	GameButton *mHatsButton;
	GameButton *mBackdropsButton;

	Rect mColorRects[18];
	Rect mItemRects[18];

	int mSubPage;
	ZombatarPage mPage;
	ZombatarState mState;
	int mTransitionTimer;
	Zombatar mZombatar;
	Zombie *mZombie;

  public:
	ZombatarWidget(LawnApp *theApp);
	~ZombatarWidget();

	void ChangePage(ZombatarPage thePage);
	void ChangeState(ZombatarState theState);
	int GetPageColorIndex(ZombatarPage thePage);
	int GetPageItemIndex(ZombatarPage thePage);
	int GetItemCount(ZombatarPage thePage);

	void DrawCurrentPortrait(Graphics *g, int theX, int theY);
	void DrawIndexedPortrait(int theIndex, Graphics *g, int theX, int theY);
	virtual void Draw(Graphics *g);
	virtual void Update();
	void DeleteCurrentZombatar();

	virtual void MouseDown(int x, int y, int theClickCount);
	virtual void MouseUp(int x, int y, int theClickCount);
  private:
};

extern Color gSkinColors[12];
extern Color gMoreColors[18];
