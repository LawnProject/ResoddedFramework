#pragma once

#include <Widget/LawnDialog.h>
#include <Widget/GameButton.h>
#include <SliderListener.h>
#include <CheckboxListener.h>

class LawnApp;
namespace Sexy
{
class Slider;
class Checkbox;
}; // namespace Sexy

class ZombatarTOS : public LawnDialog, public Sexy::SliderListener, public Sexy::CheckboxListener
{
	protected:
		enum
		{
			ZombatarTOS_Slider,
			ZombatarTOS_Accept,
			ZombatarTOS_Back,
			ZombatarTOS_TOSCheckbox,
		};

	public:
		Sexy::Slider *mTOSSlider;
		NewLawnButton *mBackButton;
		NewLawnButton *mAcceptButton;
		Sexy::Checkbox *mTOSCheckbox;


	public:
		ZombatarTOS(LawnApp *theApp);
		~ZombatarTOS();

		virtual void Draw(Graphics *g);
		virtual void Update();
		virtual void AddedToManager(Sexy::WidgetManager *theWidgetManager);
		virtual void RemovedFromManager(Sexy::WidgetManager *theWidgetManager);
		virtual void Resize(int theX, int theY, int theWidth, int theHeight);
		virtual void ButtonPress(int theId, int theClickCount);
		virtual void ButtonDepress(int theId);
		virtual void CheckboxChecked(int theId, bool checked);
};
