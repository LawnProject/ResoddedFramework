#pragma once

#include "../Widget/LawnDialog.h"
#include "../Widget/GameButton.h"

class LawnApp;

class AdvancedOptionsDialog : public LawnDialog
{
  protected:
	enum
	{
		ADVANCED_OPTIONS_APPLY,
		ADVANCED_OPTIONS_CANCEL,
	};

  public:
	LawnApp *mApp;				  //+0x16C
	LawnStoneButton *mApplyButton;
	LawnStoneButton *mCancelButton;
	LawnSlider *mOptionsSlider;

  public:
	AdvancedOptionsDialog(LawnApp *theApp);
	virtual ~AdvancedOptionsDialog();
	virtual void Draw(Graphics *g);
	virtual void AddedToManager(WidgetManager *theWidgetManager);
	virtual void RemovedFromManager(WidgetManager *theWidgetManager);
	virtual void Resize(int theX, int theY, int theWidth, int theHeight);
	virtual void MouseWheel(int theDelta);
	virtual void ButtonPress(int theId);
	virtual void ButtonDepress(int theId);
};