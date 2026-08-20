#ifndef __AWARDSCREEN_H__
#define __AWARDSCREEN_H__

#include <Common/ConstEnums.h>
#include <SexyAppFramework/Widget.h>
#include <Common/LawnCommon.h>
using namespace Sexy;

class LawnApp;
class GameButton;

class AchievementScreenItem
{
  public:
	AchievementID mAchievement;
	int mStartAnimTime;
	int mEndAnimTime;
	int mDestY;
	int mStartY;
	int mY;
};


class AwardScreen : public Widget
{
  private:
	enum
	{
		AwardScreen_Start = 100,
		AwardScreen_Menu = 101
	};

  public:
	GameButton *mStartButton;
	GameButton *mMenuButton;
	LawnApp *mApp;
	int mFadeInCounter;
	AwardType mAwardType;
	int mAchievementTime;
	bool mHasAchievementsToShow;
	GameButton *mAchievementButton;
	std::vector<AchievementScreenItem> mAchievementItems;
	LawnScrollbar *mScrollBar;


  public:
	AwardScreen(LawnApp *theApp, AwardType theAwardType);
	virtual ~AwardScreen();

	bool IsPaperNote();
	virtual void Resize(int theX, int theY, int theWidth, int theHeight)
	{
		Widget::Resize(theX, theY, theWidth, theHeight);
	}
	static void DrawBottom(Graphics *g,
						   const SexyString &theTitle,
						   const SexyString &theAward,
						   const SexyString &theMessage);
	void DrawAwardSeed(Graphics *g);
	virtual void Draw(Graphics *g);
	virtual void Update();
	virtual void AddedToManager(WidgetManager *theWidgetManager)
	{
		Widget::AddedToManager(theWidgetManager);
	}
	virtual void RemovedFromManager(WidgetManager *theWidgetManager)
	{
		Widget::RemovedFromManager(theWidgetManager);
	}
	virtual void KeyChar(SexyChar theChar);
	void StartButtonPressed();
	virtual void MouseDown(int x, int y, int theClickCount);
	virtual void MouseUp(int x, int y, int theClickCount);
	virtual void MouseWheel(int theDelta);
};

#endif
