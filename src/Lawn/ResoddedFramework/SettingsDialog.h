#pragma once

#include "../Widget/LawnDialog.h"
#include "../Widget/GameButton.h"
#include "../../SexyAppFramework/CheckboxListener.h"
#include "../../SexyAppFramework/ListListener.h"

class LawnApp;
namespace Sexy
{
class ListWidget;
};


class SettingsDialog : public LawnDialog, public Sexy::CheckboxListener, public Sexy::ListListener
{
  protected:
	enum
	{
		SETTINGS_BACK,
		SETTINGS_VSYNC,
		SETTINGS_FULLSCREEN,
		SETTINGS_HIGHQUALITY,
		SETTINGS_OPEN_SAVE_FOLDER,
		SETTINGS_RENDER_LIST,
		SETTINGS_RENDER_BACKEND_OPENGL,
		SETTINGS_RENDER_BACKEND_SDL3,
	};

  public:
	LawnApp *mApp;				  //+0x16C
	LawnStoneButton *mApplyButton;
	LawnStoneButton *mSaveFileButton;
	Sexy::Checkbox *mFullscreenCheckbox;
	Sexy::Checkbox *mVSyncCheckbox;
	Sexy::Checkbox *mHighQualityCheckbox;
	Sexy::ListWidget *mRendererList;
	LawnScrollbar *mOptionsSlider;

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
	virtual void ListClicked(int theId, int theIdx, int theClickCount);

	void CheckboxChecked(int theId, bool checked);
};