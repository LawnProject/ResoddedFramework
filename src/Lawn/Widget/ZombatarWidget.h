#pragma once
#include "../../GameConstants.h"
#include "../../SexyAppFramework/Widget.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "GameButton.h"

using namespace Sexy;

class LawnApp;
class Zombie;

class ZombatarWidget : public Widget, public ButtonListener
{
	enum
	{
		ZOMBATAR_BACK = 300,
	};
  public:
	LawnApp *mApp;
	NewLawnButton *mBackButton;
	GameButton *mDummyZombatarButton;

	Zombie *mZombie;

  public:
	ZombatarWidget(LawnApp *theApp);
	~ZombatarWidget();

	virtual void Draw(Graphics *g);
	virtual void Update();

	virtual void AddedToManager(WidgetManager *theWidgetManager);
	virtual void RemovedFromManager(WidgetManager *theWidgetManager);
	virtual void ButtonPress(int theId, int theClickCount);
	virtual void ButtonDepress(int theId);
	virtual void MouseDown(int x, int y, int theClickCount);
	virtual void MouseUp(int x, int y, int theClickCount);
  private:
};
