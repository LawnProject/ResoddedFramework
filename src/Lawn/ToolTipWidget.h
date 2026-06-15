#ifndef __TOOLTIPWIDGET_H__
#define __TOOLTIPWIDGET_H__

#include "../SexyAppFramework/Common.h"
#include <vector>
namespace Sexy
{
class Graphics;
}

class ToolTipWidget
{
  public:
	SexyString mTitle;
	SexyString mLabel;
	SexyString mWarningText;
	int mX;
	int mY;
	int mWidth;
	int mHeight;
	bool mVisible;
	bool mCenter;
	int mMinLeft;
	int mMaxBottom;
	int mGetsLinesWidth;
	int mWarningFlashCounter;

  public:
	ToolTipWidget();

	/// @brief Draw the Tooltip
	/// @param g Graphics object
	void Draw(Sexy::Graphics *g);
	/// @brief Set the label of the ToolTip
	/// @param theLabel The string containing the translation key
	void SetLabel(const SexyString &theLabel);
	/// @brief Set the title of the ToolTip
	/// @param theTitle The string containing the translation key
	void SetTitle(const SexyString &theTitle);
	/// @brief Set the warning text of the ToolTip
	/// @param theWarningText The string containing the translation key
	void SetWarningText(const SexyString &theWarningText);
	/// @brief Calculate the new size of the ToolTip based on text
	void CalculateSize();
	/// @brief Get all the lines of the ToolTip
	/// @param theLines Vector to be filled with lines
	void GetLines(std::vector<SexyString> &theLines);
	/// @brief Set the flashing counter
	inline void FlashWarning()
	{
		mWarningFlashCounter = 70;
	}
	/// @brief Update the ToolTip
	inline void Update()
	{
		if (mWarningFlashCounter > 0)
			mWarningFlashCounter--;
	}
	/// @brief Set the ToolTip's position
	inline void SetPosition(int theX, int theY)
	{
		mX = theX;
		mY = theY;
	}
};

#endif
