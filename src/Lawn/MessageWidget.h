#ifndef __MESSAGEWIDGET_H__
#define __MESSAGEWIDGET_H__

#include <array>
#include "../ConstEnums.h"
#include "../SexyAppFramework/Common.h"

// max length of mLabel and mLabelNext
#define MAX_MESSAGE_BYTES 128
#define MAX_REANIM_LINES 5

class LawnApp;
namespace Sexy
{
class Font;
class Graphics;
class Color;
} // namespace Sexy

class MessageWidget
{
  public:
	LawnApp *mApp;
	// UTF-8 encoded; due to this being saved in SaveGame it has to be an array
	// this also means we can show fewer than MAX_MESSAGE_BYTES codepoints when there are
	// non-ascii characters
	std::array<char, MAX_MESSAGE_BYTES> mLabel;
	int mDisplayTime;
	int mDuration;
	MessageStyle mMessageStyle;
	// Each ID corresponds to a codepoint in the label
	// i.e. NOT a 1-1 mapping to mLabel!
	std::array<ReanimationID, MAX_MESSAGE_BYTES> mTextReanimID = {ReanimationID::REANIMATIONID_NULL};
	ReanimationType mReanimType;
	int mSlideOffTime;
	std::array<char, MAX_MESSAGE_BYTES> mLabelNext; // UTF-8 encoded, similar to mLabel
	MessageStyle mMessageStyleNext;

  public:
	MessageWidget(LawnApp *theApp);
	~MessageWidget()
	{
		ClearReanim();
	}

	/// @brief Set the text label of the MessageWidget
	/// @param theNewLabel The next text of the label (UTF-8 Friendly)
	/// @param theMessageStyle The style to use to display the message
	void SetLabel(const SexyString &theNewLabel, MessageStyle theMessageStyle);
	/// @brief Update the MessageWidget
	void Update();
	/// @brief Draw the MessageWidget
	/// @param g Graphics object
	void Draw(Sexy::Graphics *g);
	/// @brief Clear the Reanimation Instance of the MessageWidget
	void ClearReanim();
	/// @brief Clear the text Label of the Message
	void ClearLabel();

	/// @brief Is the Message being displayed at the moment
	/// @return True if the duration != 0, aka it's being displayed at the moment, false otherwise
	inline bool IsBeingDisplayed()
	{
		return mDuration != 0;
	}
	/// @brief Get the font used to render this MessageWidget
	/// @return The font of the MessageStyle
	Sexy::Font *GetFont();
	/// @brief Draw the Reanimation Message
	/// @param g Graphics object
	/// @param theFont The font to use to draw the text
	/// @param theColor The color to use
	/// @param thePosY The Y coordinate of the message
	void DrawReanimatedText(Sexy::Graphics *g, Sexy::Font *theFont, const Sexy::Color &theColor, float thePosY);
	/// @brief Setup the Reanimation Message
	void LayoutReanimText();
};

#endif
