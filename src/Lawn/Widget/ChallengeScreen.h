#ifndef __CHALLENGESCREEN_H__
#define __CHALLENGESCREEN_H__

#include <Common/ConstEnums.h>
#include <SexyAppFramework/Dialog.h>
using namespace Sexy;

#define NUM_CHALLENGE_MODES ((int)GameMode::NUM_GAME_MODES - 1)

class LawnApp;
class ToolTipWidget;
class NewLawnButton;
class ChallengeScreen : public Widget, public ButtonListener
{
  private:
	enum
	{
		ChallengeScreen_Back = 100,
		ChallengeScreen_Mode = 200,
		ChallengeScreen_Page = 300
	};

  public:
	NewLawnButton *mBackButton;
	ButtonWidget *mPageButton[MAX_CHALLANGE_PAGES];
	ButtonWidget *mChallengeButtons[NUM_CHALLENGE_MODES];
	LawnApp *mApp;
	ToolTipWidget *mToolTip;
	ChallengePage mPageIndex;
	bool mCheatEnableChallenges;
	UnlockingState mUnlockState;
	int mUnlockStateCounter;
	int mUnlockChallengeIndex;
	float mLockShakeX;
	float mLockShakeY;

  public:
	ChallengeScreen(LawnApp *theApp, ChallengePage thePage);
	virtual ~ChallengeScreen();
	void SetUnlockChallengeIndex(ChallengePage thePage, bool theIsIZombie = false);
	int MoreTrophiesNeeded(int theChallengeIndex);
	bool ShowPageButtons();
	void UpdateButtons();
	int AccomplishmentsNeeded(int theChallengeIndex);
	void DrawButton(Graphics *g, int theChallengeIndex);
	virtual void Draw(Graphics *g);
	virtual void Update();
	virtual void AddedToManager(WidgetManager *theWidgetManager);
	virtual void RemovedFromManager(WidgetManager *theWidgetManager);
	virtual void ButtonPress(int theId);
	virtual void ButtonDepress(int theId);
	void UpdateToolTip();
	virtual void KeyChar(SexyChar theChar)
	{
		;
	}

	bool IsScaryPotterLevel(GameMode theGameMode);
	bool IsIZombieLevel(GameMode theGameMode);
};

class ChallengeDefinition
{
  public:
	GameMode mChallengeMode;
	int mChallengeIconIndex;
	ChallengePage mPage;
	int mRow;
	int mCol;
	const SexyCharByte *mChallengeName;
};
extern ChallengeDefinition gChallengeDefs[NUM_CHALLENGE_MODES];

ChallengeDefinition &GetChallengeDefinition(int theChallengeMode);

#endif