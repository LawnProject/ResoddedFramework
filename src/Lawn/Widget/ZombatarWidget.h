#pragma once
#include "../../ConstEnums.h"
#include "../../SexyAppFramework/Widget.h"
#include "../../SexyAppFramework/ButtonListener.h"
#include "GameButton.h"

using namespace Sexy;

class LawnApp;

class ZombatarWidget : public Widget, public ButtonListener
{
	enum
	{
		ZOMBATAR_BACK = 300,
	};
  public:
	LawnApp *mApp;
	NewLawnButton *mBackButton;

  public:
	ZombatarWidget(LawnApp *theApp);
	~ZombatarWidget();

	virtual void Draw(Graphics *g);
	virtual void Update();

	virtual void AddedToManager(WidgetManager *theWidgetManager);
	virtual void RemovedFromManager(WidgetManager *theWidgetManager);
	virtual void ButtonPress(int theId, int theClickCount);
	virtual void ButtonDepress(int theId);
  private:
};
