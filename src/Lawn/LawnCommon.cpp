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
	{0, 0, 0, 0}, {0, 0, 0, 0}, {240, 240, 255, 255}, {255, 255, 255, 255}, {0, 0, 0, 255},
};

bool ModInRange(int theNumber, int theMod, int theRange)
{
	theRange = abs(theRange);
	for (int i = theNumber - theRange; i <= theNumber + theRange; i++)
		if (i % theMod == 0)
			return true;
	return false;
}

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

LawnEditWidget::~LawnEditWidget()
{
}

void LawnEditWidget::KeyDown(KeyCode theKey)
{
	EditWidget::KeyDown(theKey);
	if (theKey == KeyCode::KEYCODE_ESCAPE)
		mDialog->KeyDown(KeyCode::KEYCODE_ESCAPE);
}

void LawnEditWidget::KeyChar(SexyChar theChar)
{
	if (mAutoCapFirstLetter && isalpha(theChar))
	{
		theChar = toupper(theChar);
		mAutoCapFirstLetter = false;
	}

	EditWidget::KeyChar(theChar);
}

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

Checkbox *MakeNewCheckbox(int theId, CheckboxListener *theListener, bool theDefault)
{
	Checkbox *aCheckbox =
		new Checkbox(Sexy::IMAGE_OPTIONS_CHECKBOX0, Sexy::IMAGE_OPTIONS_CHECKBOX1, theId, theListener);
	aCheckbox->mChecked = theDefault;
	aCheckbox->mHasAlpha = true;
	aCheckbox->mHasTransparencies = true;

	return aCheckbox;
}

std::string GetSavedGameName(GameMode theGameMode, int theProfileId)
{
	return GetAppDataFolder() + StrFormat("savefiles/%d/game%d", theProfileId, (int)theGameMode);
}

int GetCurrentDaysSince2000()
{
	time_t aNow = std::time(0);
	tm aNowTM = *std::localtime(&aNow);

	int dy = aNowTM.tm_year - 100;
	return dy * 365 + (dy - 1) / 400 - (dy - 1) / 100 + (dy - 1) / 4 + aNowTM.tm_yday + 1;
}

LawnScrollbar::LawnScrollbar(LawnApp *theApp)
{
	mApp = theApp;
	mValue = 0.0f;
	mMaxValue = 0.0f;
	mSliderHeightPercent = 1.0f;
	mAllowedMouseZone = Rect(0, 0, mApp->mWidth, mApp->mHeight);
	mIsDown = false;
	mIsOver = false;
	mInitiallyOver = false;
	mBaseColor = Color(152, 149, 188);
	mThumbColor = Color(63, 64, 86);
	mIsHorizontal = false;
	mThumbRect = Rect(0, 0, 0, 0);
}

LawnScrollbar::~LawnScrollbar()
{
}
void LawnScrollbar::Update()
{
	if (mDisabled)
		return;

	int aMouseX = mApp->mWidgetManager->mLastMouseX, aMouseY = mApp->mWidgetManager->mLastMouseY;
	if (mParent)
	{
		Point anAbsPos = mParent->GetAbsPos();
		aMouseX -= anAbsPos.mX;
		aMouseY -= anAbsPos.mY;
	}
	aMouseX -= mX;
	aMouseY -= mY;
	mIsOver = mThumbRect.Contains(aMouseX, aMouseY);
	mIsDown = mApp->mWidgetManager->mDownButtons & 5;

	if (IsThumbDown())
	{
		bool aIsOver = mInitiallyOver;
		float aNormalized = (mMaxValue > 0.0f) ? mValue / mMaxValue : 0.0f;
		if (mIsHorizontal)
		{
			if (aIsOver)
				mValue = std::max(
					0.0f, std::min((aMouseX - mGrabOffsetX) / (mWidth - mThumbRect.mWidth) * mMaxValue, mMaxValue));
			mThumbRect = Rect(aNormalized * (mWidth - mThumbRect.mWidth), 0, mSliderHeightPercent * mWidth, mHeight);
		}
		else
		{

			if (aIsOver)
				mValue = std::max(
					0.0f, std::min((aMouseY - mGrabOffsetY) / (mHeight - mThumbRect.mHeight) * mMaxValue, mMaxValue));
			mThumbRect = Rect(0, aNormalized * (mHeight - mThumbRect.mHeight), mWidth, mSliderHeightPercent * mHeight);
		}
	}
}

bool LawnScrollbar::IsThumbOver()
{
	return !mDisabled && mIsOver;
}
bool LawnScrollbar::IsThumbDown()
{
	return !mDisabled && mIsDown;
}

void LawnScrollbar::MouseDown(int x, int y, int theClickCount)
{
	Widget::MouseDown(x, y, theClickCount);
	if (mThumbRect.Contains(x, y))
	{
		mInitiallyOver = true;
		mGrabOffsetX = x - mThumbRect.mX;
		mGrabOffsetY = y - mThumbRect.mY;
	}
	else if (Rect(0, 0, mWidth, mHeight).Contains(x, y))
	{
		mInitiallyOver = true;
		mGrabOffsetX = mThumbRect.mWidth / 2.0f;
		mGrabOffsetY = mThumbRect.mHeight / 2.0f;
	}
}

void LawnScrollbar::MouseUp(int x, int y, int theClickCount)
{
	Widget::MouseUp(x, y, theClickCount);
	mInitiallyOver = false;
}

void LawnScrollbar::MouseWheel(int theDelta)
{
	Widget::MouseWheel(theDelta);

	if (!mAllowedMouseZone.Contains(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY) || mDisabled)
		return;

	float aStep = mMaxValue * 0.1f;
	mValue = std::max(0.0f, std::min(mValue - theDelta * aStep, mMaxValue));
	float aNormalized = (mMaxValue > 0.0f) ? mValue / mMaxValue : 0.0f;
	if (mIsHorizontal)
	{
		mThumbRect = Rect(aNormalized * (mWidth - mThumbRect.mWidth), 0, mSliderHeightPercent * mWidth, mHeight);
	}
	else
	{
		mThumbRect = Rect(0, aNormalized * (mHeight - mThumbRect.mHeight), mWidth, mSliderHeightPercent * mHeight);
	}
}

void LawnScrollbar::SetValue(float theValue)
{
	mValue = theValue;
	float aNormalized = (mMaxValue > 0.0f) ? mValue / mMaxValue : 0.0f;
	if (mIsHorizontal)
	{
		mThumbRect = Rect(aNormalized * (mWidth - mThumbRect.mWidth), 0, mSliderHeightPercent * mWidth, mHeight);
	}
	else
	{
		mThumbRect = Rect(0, aNormalized * (mHeight - mThumbRect.mHeight), mWidth, mSliderHeightPercent * mHeight);
	}
}

float LawnScrollbar::GetValue()
{
	return mValue;
}

Color DeriveLawnSliderColor(Color aColor, int r, int g, int b)
{
	return Color(std::clamp(aColor.mRed + r, 0, 255), std::clamp(aColor.mGreen + g, 0, 255),
				 std::clamp(aColor.mBlue + b, 0, 255));
}

void LawnScrollbar::Resize(int theX, int theY, int theWidth, int theHeight)
{
	Widget::Resize(theX, theY, theWidth, theHeight);
	float aNormalized = (mMaxValue > 0.0f) ? mValue / mMaxValue : 0.0f;
	if (mIsHorizontal)
	{
		mThumbRect = Rect(aNormalized * (mWidth - mThumbRect.mWidth), 0, mSliderHeightPercent * mWidth, mHeight);
	}
	else
	{
		mThumbRect = Rect(0, aNormalized * (mHeight - mThumbRect.mHeight), mWidth, mSliderHeightPercent * mHeight);
	}
}

void LawnScrollbar::DrawScrollThumb(Graphics *g)
{
	g->PushState();

	g->Translate(mIsHorizontal ? mThumbRect.mX : 0, !mIsHorizontal ? mThumbRect.mY : 0);

	const int aWidth = mIsHorizontal ? mThumbRect.mWidth : mWidth;

	const int aHeight = !mIsHorizontal ? mThumbRect.mHeight : mHeight;

	// Base
	g->SetColor(mThumbColor);
	g->FillRect(0, 0, aWidth, aHeight);

	g->SetColor(DeriveLawnSliderColor(mThumbColor, 17, 17, 22));
	if (mIsHorizontal)
	{
		g->FillRect(1, 1, 1, aHeight - 2);
		g->FillRect(1, 1, aWidth - 2, 1);
	}
	else
	{
		g->FillRect(1, 1, 6, 1);
		g->FillRect(1, 1, 1, aHeight - 2);
	}

	g->SetColor(DeriveLawnSliderColor(mThumbColor, 21, 22, 27));
	g->FillRect(1, 1, 1, 1);

	// Outer border
	g->SetColor(DeriveLawnSliderColor(mThumbColor, -33, -36, -52));
	g->FillRect(aWidth - 1, 0, 1, aHeight);
	g->FillRect(0, aHeight - 1, aWidth, 1);

	// Inner border
	g->FillRect(aWidth - 2, 1, 1, aHeight - 2);
	g->FillRect(2, aHeight - 2, aWidth - 2, 1);

	g->SetColor(DeriveLawnSliderColor(mThumbColor, -41, -45, -65));
	g->FillRect(aWidth - 1, aHeight - 1, 1, 1);

	g->PopState();
}

void LawnScrollbar::Draw(Graphics *g)
{
	if (!mVisible)
		return;
	g->PushState();

	g->SetColor(mBaseColor);
	g->FillRect(0, 0, mWidth, mHeight);

	DrawScrollThumb(g);

	g->PopState();
}
