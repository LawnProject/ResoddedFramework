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

class LawnScrollbar : public Widget
{
  public:
	LawnApp *mApp;
	float mValue;
	float mMaxValue;
	float mSliderHeightPercent;
	int mScrollbarMode;

	bool mIsDown;
	bool mIsOver;
	bool mInitiallyOver;

	Rect mAllowedMouseZone;
	Color mThumbColor;
	Color mBaseColor;
	Rect mThumbRect;
	bool mIsHorizontal;
	float mGrabOffsetX;
	float mGrabOffsetY;

  public:
	LawnScrollbar(LawnApp *theApp);
	~LawnScrollbar();

	virtual void Update();
	virtual void MouseWheel(int theDelta);
	virtual void MouseDown(int x, int y, int theClickCount);
	virtual void MouseUp(int x, int y, int theClickCount);
	virtual void Draw(Graphics *g);
	virtual void Resize(int theX, int theY, int theWidth, int theHeight);
	virtual void DrawScrollThumb(Graphics *g);
	bool IsThumbOver();
	bool IsThumbDown();
	float GetValue();
	void SetValue(float theValue);
};

bool ModInRange(int theNumber, int theMod, int theRange = 0);
bool GridInRange(int x1, int y1, int x2, int y2, int theRangeX = 1, int theRangeY = 1);

void TileImageHorizontally(Graphics *g, Image *theImage, int theX, int theY, int theWidth);
void TileImageVertically(Graphics *g, Image *theImage, int theX, int theY, int theHeight);

Checkbox *MakeNewCheckbox(int theId, CheckboxListener *theListener, bool theDefault);
LawnEditWidget *CreateEditWidget(int theId, EditListener *theListener, Dialog *theDialog);
void DrawEditBox(Graphics *g, EditWidget *theWidget);

std::string GetSavedGameName(GameMode theGameMode, int theProfileId);
int GetCurrentDaysSince2000();

#endif
