#ifndef __GAMESELECTOR_H__
#define __GAMESELECTOR_H__

#include <Common/ConstEnums.h>
#include <SexyAppFramework/Widget.h>
#include <SexyAppFramework/ButtonListener.h>
#include <Widget/GameButton.h>

class LawnApp;
class AchievementsWidget;
class ZombatarWidget;
class ToolTipWidget;
namespace Sexy
{
class DialogButton;
}

using namespace Sexy;

enum SelectorAnimState
{
	SELECTOR_OPEN,
	SELECTOR_NEW_USER,
	SELECTOR_SHOW_SIGN,
	SELECTOR_IDLE
};

class GameSelector : public Widget, public ButtonListener
{
  private:
	enum
	{
		GameSelector_Adventure = 100,
		GameSelector_Minigame,
		GameSelector_Puzzle,
		GameSelector_Options,
		GameSelector_Help,
		GameSelector_Quit,
		GameSelector_ChangeUser,
		GameSelector_Store,
		GameSelector_Almanac,
		GameSelector_ZenGarden,
		GameSelector_Survival,
		GameSelector_Achievements,
		GameSelector_Zombatar,
	};

  public:
	LawnApp *mApp;
	NewLawnButton *mAdventureButton;
	NewLawnButton *mMinigameButton;
	NewLawnButton *mPuzzleButton;
	NewLawnButton *mOptionsButton;
	NewLawnButton *mQuitButton;
	NewLawnButton *mHelpButton;
	NewLawnButton *mStoreButton;
	NewLawnButton *mAlmanacButton;
	NewLawnButton *mZenGardenButton;
	NewLawnButton *mSurvivalButton;
	NewLawnButton *mChangeUserButton;
	NewLawnButton *mAchievementsButton;
	NewLawnButton *mZombatarButton;
	Widget *mOverlayWidget;
	bool mStartingGame;
	int mStartingGameCounter;
	bool mMinigamesLocked;
	bool mPuzzleLocked;
	bool mSurvivalLocked;
	bool mShowStartButton;
	ParticleSystemID mTrophyParticleID;
	ReanimationID mSelectorReanimID;
	ReanimationID mCloudReanimID[6];
	int mCloudCounter[6];
	ReanimationID mFlowerReanimID[3];
	ReanimationID mLeafReanimID;
	ReanimationID mHandReanimID;
	int mLeafCounter;
	SelectorAnimState mSelectorState;
	int mLevel;
	bool mLoading;
	ToolTipWidget *mToolTip;
	bool mHasTrophy;
	bool mUnlockSelectorCheat;
	int mSlideCounter;
	int mStartX;
	int mStartY;
	int mDestX;
	int mDestY;
	AchievementsWidget *mAchievementsWidget;
	ZombatarWidget *mZombatarWidget;

  public:
	GameSelector(LawnApp *theApp);
	virtual ~GameSelector();

	void SyncProfile(bool theShowLoading);
	virtual void Draw(Graphics *g);
	virtual void DrawOverlay(Graphics *g);
	virtual void Update();
	virtual void AddedToManager(WidgetManager *theWidgetManager);
	virtual void RemovedFromManager(WidgetManager *theWidgetManager);
	virtual void OrderInManagerChanged();
	virtual void ButtonMouseEnter(int theId);
	virtual void ButtonPress(int theId, int theClickCount);
	virtual void ButtonDepress(int theId);
	virtual void KeyDown(KeyCode theKey);
	virtual void KeyChar(SexyChar theChar);
	virtual void MouseDown(int x, int y, int theClickCount);
	void TrackButton(DialogButton *theButton, const char *theTrackName, float theOffsetX, float theOffsetY);
	void SyncButtons();
	void AddTrophySparkle();
	void ClickedAdventure();
	void UpdateTooltip();
	bool ShouldDoZenTuturialBeforeAdventure();
	void AddPreviewProfiles();
	void SlideTo(int theX, int theY);
};

class GameSelectorOverlay : public Widget
{
  public:
	GameSelector *mParent;

  public:
	GameSelectorOverlay(GameSelector *theGameSelector);
	virtual ~GameSelectorOverlay()
	{
	}

	virtual void Draw(Graphics *g);
};

#endif
