#pragma once
#include "../../GameConstants.h"
#include "../../SexyAppFramework/Widget.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "GameButton.h"

using namespace Sexy;

class LawnApp;
class Zombie;
class Zombatar;

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

class ZombatarWidget : public Widget, public ButtonListener
{
	enum
	{
		ZOMBATAR_BACK = 300,
		ZOMBATAR_VIEW,
		ZOMBATAR_FINISHED,
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

	ZombatarPage mPage;
	Zombatar *mZombatar;
	Zombie *mZombie;

  public:
	ZombatarWidget(LawnApp *theApp);
	~ZombatarWidget();

	void ChangePage(ZombatarPage thePage);

	void DrawPortrait(Graphics *g, int theX, int theY);
	virtual void Draw(Graphics *g);
	virtual void Update();

	virtual void MouseDown(int x, int y, int theClickCount);
	virtual void MouseUp(int x, int y, int theClickCount);
  private:
};

extern Color gSkinColors[12];
extern Color gMoreColors[17];
