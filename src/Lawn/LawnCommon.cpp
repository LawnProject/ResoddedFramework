#include "Board.h"
#include "Plant.h"
#include "../LawnApp.h"
#include "LawnCommon.h"
#include "../Resources.h"
#include "../GameConstants.h"
#include "../Sexy.TodLib/TodCommon.h"
#include "../SexyAppFramework/Font.h"
#include "../SexyAppFramework/Dialog.h"
#include "../SexyAppFramework/SexyMatrix.h"
#include "../SexyAppFramework/WidgetManager.h"
#include "../SexyAppFramework/Checkbox.h"

int gLawnEditWidgetColors[][4] = {
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{240, 240, 255, 255},
	{255, 255, 255, 255},
	{0, 0, 0, 255},
};

// 判断在 [theNumber - theRange, theNumber + theRange] 区间内是否存在 theMod 的整数倍数
bool ModInRange(int theNumber, int theMod, int theRange)
{
	theRange = abs(theRange);
	for (int i = theNumber - theRange; i <= theNumber + theRange; i++)
		if (i % theMod == 0)
			return true;
	return false;
}

// 判断点 (x1, y1) 是否位于点 (x2, y2) 周围的 (theRangeX, theRangeY) 范围内
bool GridInRange(int x1, int y1, int x2, int y2, int theRangeX, int theRangeY)
{
	return x1 >= x2 - theRangeX && x1 <= x2 + theRangeX && y1 >= y2 - theRangeY && y1 <= y2 + theRangeY;
}

void TileImageHorizontally(Graphics *g, Image *theImage, int theX, int theY, int theWidth)
{
	while (theWidth > 0)
	{
		int aImageWidth = std::min(theWidth, theImage->GetWidth());
		g->DrawImage(theImage, theX, theY, Rect(0, 0, aImageWidth, theImage->GetHeight()));
		theX += aImageWidth;
		theWidth -= aImageWidth;
	}
}

void TileImageVertically(Graphics *g, Image *theImage, int theX, int theY, int theHeight)
{
	while (theHeight > 0)
	{
		int aImageHeight = std::min(theHeight, theImage->GetHeight());
		g->DrawImage(theImage, theX, theY, Rect(0, 0, theImage->GetWidth(), aImageHeight));
		theY += aImageHeight;
		theHeight -= aImageHeight;
	}
}

LawnEditWidget::LawnEditWidget(int theId, EditListener *theListener, Dialog *theDialog) : EditWidget(theId, theListener)
{
	mDialog = theDialog;
	mAutoCapFirstLetter = true;
}

//0x456700
LawnEditWidget::~LawnEditWidget()
{
}

//0x456720
void LawnEditWidget::KeyDown(KeyCode theKey)
{
	EditWidget::KeyDown(theKey);
	if (theKey == KeyCode::KEYCODE_ESCAPE)
		mDialog->KeyDown(KeyCode::KEYCODE_ESCAPE);
}

//0x456760
void LawnEditWidget::KeyChar(char theChar)
{
	if (mAutoCapFirstLetter && isalpha(theChar))
	{
		theChar = toupper(theChar);
		mAutoCapFirstLetter = false;
	}

	EditWidget::KeyChar(theChar);
}

//0x4567B0
LawnEditWidget *CreateEditWidget(int theId, EditListener *theListener, Dialog *theDialog)
{
	LawnEditWidget *aEditWidget = new LawnEditWidget(theId, theListener, theDialog);
	aEditWidget->SetFont(Sexy::FONT_BRIANNETOD16);
	aEditWidget->SetColors(gLawnEditWidgetColors, EditWidget::NUM_COLORS);
	aEditWidget->mBlinkDelay = 14;

	return aEditWidget;
}

void DrawEditBox(Graphics *g, EditWidget *theWidget)
{
	Rect aDest(theWidget->mX - 8, theWidget->mY - 4, theWidget->mWidth + 16, theWidget->mHeight + 8);
	g->DrawImageBox(aDest, IMAGE_EDITBOX);
}

//0x456860
Checkbox *MakeNewCheckbox(int theId, CheckboxListener *theListener, bool theDefault)
{
	Checkbox *aCheckbox =
		new Checkbox(Sexy::IMAGE_OPTIONS_CHECKBOX0, Sexy::IMAGE_OPTIONS_CHECKBOX1, theId, theListener);
	aCheckbox->mChecked = theDefault;
	aCheckbox->mHasAlpha = true;
	aCheckbox->mHasTransparencies = true;

	return aCheckbox;
}

//0x4568D0
std::string GetSavedGameName(GameMode theGameMode, int theProfileId)
{
	return GetAppDataFolder() + StrFormat("savefiles/game%d_%d.dat", theProfileId, (int)theGameMode);
}

//0x456980
int GetCurrentDaysSince2000()
{
	__time64_t aNow = _time64(nullptr);
	tm aNowTM;
	_localtime64_s(&aNowTM, &aNow);

	int dy = aNowTM.tm_year - 100;
	return dy * 365 + (dy - 1) / 400 - (dy - 1) / 100 + (dy - 1) / 4 + aNowTM.tm_yday + 1;
}


LawnSlider::LawnSlider(LawnApp *theApp)
{
	mApp = theApp;
	mRawValue = 0.0f;
	mStepMultiplier = 1.0f;
	mSliderHeightPercent = 0.5f;
	mScrollMultiplier = 0.09f;
	mAllowedMouseZone = Rect(0, 0, mApp->mWidth, mApp->mHeight);
	mStartedDrag = false;
}

LawnSlider::~LawnSlider()
{

}
void LawnSlider::Update()
{
	Widget::Update();
	if (!mStartedDrag || mDisabled)
		return;

	int aLocalY = mApp->mWidgetManager->mLastMouseY - mY;

	int aHeightSlider = mHeight * mSliderHeightPercent;
	int aRange = mHeight - aHeightSlider;

	float aNewValue = (float)(aLocalY - aHeightSlider * 0.5f) / aRange;

	mRawValue = std::clamp(aNewValue, 0.0f, 1.0f);
}

void LawnSlider::MouseDown(int x, int y, int theClickCount)
{
	Widget::MouseDown(x, y, theClickCount);

	if (!Rect(mX, mY, mWidth, mHeight).Contains(x + mX, y + mY)) //MouseDown is called with relative coordinates
		return;
	mStartedDrag = true;
}

void LawnSlider::MouseUp(int x, int y, int theClickCount)
{
	Widget::MouseUp(x, y, theClickCount);
	mStartedDrag = false;
}

void LawnSlider::MouseWheel(int theDelta)
{
	Widget::MouseWheel(theDelta);

	if (!mAllowedMouseZone.Contains(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY) || mDisabled)
		return;
	mRawValue -= theDelta * mScrollMultiplier;
	mRawValue = std::clamp(mRawValue, 0.0f, 1.0f);
}

float LawnSlider::GetValue()
{
	return mRawValue * mStepMultiplier;
}

void LawnSlider::Draw(Graphics* g)
{
	if (!mVisible)
		return;

	int aHeightSlider = mHeight * mSliderHeightPercent;
	int anOffsetSlider = (mHeight - aHeightSlider) * mRawValue;

	g->PushState();

	// Draw the background
	
	g->SetColor(Color(152, 149, 188));
	g->FillRect(0, 0, 8, mHeight);

	g->Translate(0, anOffsetSlider);

	// Draw the base
	g->SetColor(Color(63, 64, 86));
	g->FillRect(0, 0, mWidth, aHeightSlider);

	// Highlight
	g->SetColor(Color(80, 81, 108));
	g->FillRect(1, 1, 6, 1);
	g->FillRect(1, 1, 1, aHeightSlider - 2);
	g->SetColor(Color(84, 86, 113));
	g->FillRect(1, 1, 1, 1);

	// Border 1
	g->SetColor(Color(30, 28, 34));
	g->FillRect(mWidth, 0, 1, aHeightSlider);
	g->FillRect(0, aHeightSlider, mWidth, 1);
	g->SetColor(Color(22, 19, 21));
	g->FillRect(mWidth, aHeightSlider, 1, 1);

	// Border 2
	g->SetColor(Color(30, 28, 34));
	g->FillRect(mWidth - 1, 1, 1, aHeightSlider);
	g->FillRect(0, aHeightSlider, mWidth, 1);
	g->SetColor(Color(22, 19, 21));
	g->FillRect(mWidth, aHeightSlider - 1, 1, 1);
	g->FillRect(2, aHeightSlider - 1, mWidth - 1, 1);

	g->PopState();
}