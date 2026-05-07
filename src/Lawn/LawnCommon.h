#ifndef __LAWN_COMMON_H__
#define __LAWN_COMMON_H__

#include "../ConstEnums.h"
#include "../SexyAppFramework/Graphics.h"
#include "../SexyAppFramework/EditWidget.h"

using namespace Sexy;

class LawnApp;
class Board;
class LawnStoneButton;
class NewLawnButton;
namespace Sexy
{
class Dialog;
class Checkbox;
class DialogButton;
class CheckboxListener;
} // namespace Sexy

class LawnEditWidget : public EditWidget
{
  public:
	Dialog *mDialog;
	bool mAutoCapFirstLetter;

  public:
	LawnEditWidget(int theId, EditListener *theListener, Dialog *theDialog);
	~LawnEditWidget();

	virtual void KeyDown(KeyCode theKey);
	virtual void KeyChar(SexyChar theChar);
};

class LawnSlider : public Widget
{
  public:
	LawnApp *mApp;
	float mRawValue;
	float mStepMultiplier;
	float mSliderHeightPercent;
	float mScrollMultiplier;
	Rect mAllowedMouseZone;
	bool mStartedDrag;

  public:
	LawnSlider(LawnApp* theApp);
	~LawnSlider();

	virtual void Update();
	virtual void MouseWheel(int theDelta);
	virtual void MouseDown(int x, int y, int theClickCount);
	virtual void MouseUp(int x, int y, int theClickCount);
	virtual void Draw(Graphics *g);
	float GetValue();
};


// ====================================================================================================
// ★ 常用逻辑判断
// ====================================================================================================
/*inline*/ bool ModInRange(int theNumber, int theMod, int theRange = 0);
/*inline*/ bool GridInRange(int x1, int y1, int x2, int y2, int theRangeX = 1, int theRangeY = 1);

// ====================================================================================================
// ★ 动画、特效与绘制相关
// ====================================================================================================
/*inline*/ void TileImageHorizontally(Graphics *g, Image *theImage, int theX, int theY, int theWidth);
/*inline*/ void TileImageVertically(Graphics *g, Image *theImage, int theX, int theY, int theHeight);

// ====================================================================================================
// ★ 控件
// ====================================================================================================
Checkbox *MakeNewCheckbox(int theId, CheckboxListener *theListener, bool theDefault);
LawnEditWidget *CreateEditWidget(int theId, EditListener *theListener, Dialog *theDialog);
void DrawEditBox(Graphics *g, EditWidget *theWidget);

// ====================================================================================================
// ★ 其他
// ====================================================================================================
std::string GetSavedGameName(GameMode theGameMode, int theProfileId);
int GetCurrentDaysSince2000();

#endif
