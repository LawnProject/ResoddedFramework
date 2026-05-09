#pragma once

#include "../../ConstEnums.h"
#include "../../SexyAppFramework/Widget.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "GameButton.h"

class LawnApp;

using namespace Sexy;

class AchievementDefinition
{
  public:
	const SexyCharByte *mName;
	const SexyCharByte *mDescription;
};

extern AchievementDefinition gAchievementDefs[NUM_ACHIEVEMENT_TYPES];

class AchievementsWidget : public Widget, public ButtonListener
{
	enum
	{
		ACHIEVEMENTS_BACK = 200,
		ACHIEVEMENTS_MORE,
	};
  public:
	LawnApp *mApp;
	int mScrollValue;
	int mScrollDirection;
	int mScrollDecay;
	bool mPressedMoreButton;
	NewLawnButton *mBackButton;
	NewLawnButton *mMoreButton;


  public:
	AchievementsWidget(LawnApp* theApp);
	virtual ~AchievementsWidget();

	virtual void Draw(Graphics *g);
	virtual void Update();
	virtual void MouseWheel(int theDelta);
	virtual void AddedToManager(WidgetManager *theWidgetManager);
	virtual void RemovedFromManager(WidgetManager *theWidgetManager);
	virtual void KeyDown(KeyCode theKey);
	virtual void ButtonPress(int theId, int theClickCount);
	virtual void ButtonDepress(int theId);
  private:
};
