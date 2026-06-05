#ifndef __MESSAGEWIDGET_H__
#define __MESSAGEWIDGET_H__

#include "../ConstEnums.h"
#include "../SexyAppFramework/Common.h"

#define MAX_MESSAGE_LENGTH 128
#define MAX_REANIM_LINES 5

class LawnApp;
namespace Sexy
{
class Font;
class Graphics;
} // namespace Sexy

class MessageWidget
{
  public:
	LawnApp *mApp;
	SexyCharByte mLabel[MAX_MESSAGE_LENGTH];
	int mDisplayTime;
	int mDuration;
	MessageStyle mMessageStyle;
	ReanimationID mTextReanimID[MAX_MESSAGE_LENGTH];
	ReanimationType mReanimType;
	int mSlideOffTime;
	SexyCharByte mLabelNext[MAX_MESSAGE_LENGTH];
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
	/// @return True if the duration != 0, aka it's being displayed at the moment
	inline bool IsBeingDisplayed()
	{
		return mDuration != 0;
	}
	/// @brief Get the font used to render this MessageWidget
	/// @return The font of the MessageStyle
	Font *GetFont();
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