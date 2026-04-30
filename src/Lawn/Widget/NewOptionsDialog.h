#ifndef __NEWOPTIONSDIALOG_H__
#define __NEWOPTIONSDIALOG_H__

#include "../../SexyAppFramework/Dialog.h"
#include "../../SexyAppFramework/SliderListener.h"
#include "../../SexyAppFramework/CheckboxListener.h"

class LawnApp;
class LawnStoneButton;
class NewLawnButton;
namespace Sexy
{
class Slider;
class Checkbox;
}; // namespace Sexy

class NewOptionsDialog : public Sexy::Dialog, public Sexy::SliderListener, public Sexy::CheckboxListener
{
  protected:
	enum
	{
		NewOptionsDialog_Almanac,
		NewOptionsDialog_MainMenu,
		NewOptionsDialog_Restart,
		NewOptionsDialog_Update,
		NewOptionsDialog_MusicVolume,
		NewOptionsDialog_SoundVolume,
		NewOptionsDialog_Fullscreen,
		NewOptionsDialog_AdvancedOptions,

	};

  public:
	LawnApp *mApp;
	Sexy::Slider *mMusicVolumeSlider;
	Sexy::Slider *mSfxVolumeSlider;
	Sexy::Checkbox *mFullscreenCheckbox;
	LawnStoneButton *mAlmanacButton;
	LawnStoneButton *mBackToMainButton;
	LawnStoneButton *mRestartButton;
	NewLawnButton *mBackToGameButton;
	LawnStoneButton *mAdvancedButtons;
	bool mFromGameSelector;

  public:
	NewOptionsDialog(LawnApp *theApp, bool theFromGameSelector);
	~NewOptionsDialog();

	int GetPreferredHeight(int theWidth);
	void AddedToManager(Sexy::WidgetManager *theWidgetManager);
	void RemovedFromManager(Sexy::WidgetManager *theWidgetManager);
	void Resize(int theX, int theY, int theWidth, int theHeight);
	void Draw(Sexy::Graphics *g);
	void SliderVal(int theId, double theVal);
	void CheckboxChecked(int theId, bool checked);
	void ButtonPress(int theId);
	void ButtonDepress(int theId);
	void KeyDown(Sexy::KeyCode theKey);
};

#endif
