#pragma once

#include "../Widget/LawnDialog.h"
#include "../Widget/GameButton.h"
#include "../../SexyAppFramework/CheckboxListener.h"

class LawnApp;

class SettingsDialog : public LawnDialog, public Sexy::CheckboxListener
{
  protected:
	enum
	{
		SETTINGS_BACK,
		SETTINGS_VSYNC,
		SETTINGS_FULLSCREEN,
		SETTINGS_OPEN_SAVE_FOLDER,
	};

  public:
	LawnApp *mApp;				  //+0x16C
	LawnStoneButton *mApplyButton;
	LawnStoneButton *mSaveFileButton;
	Sexy::Checkbox *mFullscreenCheckbox;
	Sexy::Checkbox *mVSyncCheckbox;
	LawnSlider *mOptionsSlider;

  public:
	SettingsDialog(LawnApp *theApp);
	virtual ~SettingsDialog();
	virtual void Draw(Graphics *g);
	virtual void AddedToManager(WidgetManager *theWidgetManager);
	virtual void RemovedFromManager(WidgetManager *theWidgetManager);
	virtual void Resize(int theX, int theY, int theWidth, int theHeight);
	virtual void MouseWheel(int theDelta);
	virtual void ButtonPress(int theId);
	virtual void ButtonDepress(int theId);
	void CheckboxChecked(int theId, bool checked);
};