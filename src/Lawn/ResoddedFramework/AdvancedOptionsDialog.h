#pragma once

#include "../Widget/LawnDialog.h"
#include "../Widget/GameButton.h"
#include "../../SexyAppFramework/CheckboxListener.h"

class LawnApp;

class AdvancedOptionsDialog : public LawnDialog, public Sexy::CheckboxListener
{
  protected:
	enum
	{
		ADVANCED_OPTIONS_BACK,
		ADVANCED_OPTIONS_VSYNC,
		ADVANCED_OPTIONS_SAVE_FILE,
	};

  public:
	LawnApp *mApp;				  //+0x16C
	LawnStoneButton *mApplyButton;
	LawnStoneButton *mSaveFileButton;
	Sexy::Checkbox *mVSyncCheckbox;
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
	void CheckboxChecked(int theId, bool checked);
};