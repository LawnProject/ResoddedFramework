#include "Board.h"
#include "Challenge.h"
#include "../LawnApp.h"
#include "../Resources.h"
#include "MessageWidget.h"
#include "../SexyAppFramework/Font.h"
#include "../Sexy.TodLib/TodCommon.h"
#include "../Sexy.TodLib/Reanimator.h"
#include "../Sexy.TodLib/TodStringFile.h"

MessageWidget::MessageWidget(LawnApp *theApp)
{
	mApp = theApp;
	mDuration = 0;
	mLabel[0] = '\0';
	mMessageStyle = MessageStyle::MESSAGE_STYLE_OFF;
	mLabelNext[0] = '\0';
	mMessageStyleNext = MessageStyle::MESSAGE_STYLE_OFF;
	mSlideOffTime = 100;
}

void MessageWidget::ClearReanim()
{
	for (int i = 0; i < mTextReanimID.size(); i++)
	{
		if (mTextReanimID[i] == ReanimationID::REANIMATIONID_NULL)
			continue;
		Reanimation *aReanim = mApp->ReanimationTryToGet(mTextReanimID[i]);
		if (aReanim)
		{
			aReanim->ReanimationDie();
		}
		mTextReanimID[i] = ReanimationID::REANIMATIONID_NULL;
	}
}

void MessageWidget::ClearLabel()
{
	if (mReanimType != ReanimationType::REANIM_NONE)
	{
		mDuration = std::min(mDuration, 100 + mSlideOffTime + 1);
	}
	else
	{
		mDuration = 0;
	}
}

namespace
{

// copy theInput to theTarget (both UTF-8 encoded), truncating at the final codepoint boundary that fits
void CopyTruncatedToLabel(std::array<char, MAX_MESSAGE_BYTES> &theTarget, const SexyString &theInput)
{
	auto it = theInput.begin();
	const auto end = theInput.end();
	size_t aByteLen = 0;
	while (it != end)
	{
		auto itStart = it;
		utf8::next(it, end);
		auto aCodepointByteSize = it - itStart;
		if (aByteLen + aCodepointByteSize >= theTarget.size())
		{
			it = itStart; // backtrack the last codepoint we saw which doesn't fit
			break;
		}
		aByteLen += aCodepointByteSize;
	}
	std::copy(theInput.begin(), it, theTarget.begin());
	theTarget[aByteLen] = '\0';
}

} // namespace

void MessageWidget::SetLabel(const SexyString &theNewLabel, MessageStyle theMessageStyle)
{
	SexyString aLabel = TodStringTranslate(theNewLabel);
	TOD_ASSERT(utf8::is_valid(aLabel));

	if (mReanimType != ReanimationType::REANIM_NONE && mDuration > 0)
	{
		mMessageStyleNext = theMessageStyle;
		CopyTruncatedToLabel(mLabelNext, aLabel);
		ClearLabel();
	}
	else
	{
		ClearReanim();
		CopyTruncatedToLabel(mLabel, aLabel);
		mMessageStyle = theMessageStyle;
		mReanimType = ReanimationType::REANIM_NONE;

		switch (theMessageStyle)
		{
		case MessageStyle::MESSAGE_STYLE_HINT_LONG:
		case MessageStyle::MESSAGE_STYLE_BIG_MIDDLE:
		case MessageStyle::MESSAGE_STYLE_ZEN_GARDEN_LONG:
		case MessageStyle::MESSAGE_STYLE_HINT_TALL_LONG:
			mDuration = 1500;
			break;

		case MessageStyle::MESSAGE_STYLE_HINT_TALL_UNLOCKMESSAGE:
			mDuration = 500;
			break;

		case MessageStyle::MESSAGE_STYLE_HINT_FAST:
		case MessageStyle::MESSAGE_STYLE_HINT_TALL_FAST:
		case MessageStyle::MESSAGE_STYLE_BIG_MIDDLE_FAST:
		case MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL1:
		case MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL2:
		case MessageStyle::MESSAGE_STYLE_TUTORIAL_LATER:
			mDuration = 500;
			break;

		case MessageStyle::MESSAGE_STYLE_HINT_STAY:
		case MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL1_STAY:
		case MessageStyle::MESSAGE_STYLE_TUTORIAL_LATER_STAY:
			mDuration = 10000;
			break;

		case MessageStyle::MESSAGE_STYLE_HOUSE_NAME:
			mDuration = 250;
			break;

		case MessageStyle::MESSAGE_STYLE_HUGE_WAVE:
			mDuration = 750;
			mReanimType = ReanimationType::REANIM_TEXT_FADE_ON;
			break;

		case MessageStyle::MESSAGE_STYLE_SLOT_MACHINE:
			mDuration = 750;
			break;

		case MessageStyle::MESSAGE_STYLE_ACHIEVEMENT:
			mDuration = 250;
			break;

		default:
			TOD_ASSERT(false);
			break;
		}

		if (mReanimType != ReanimationType::REANIM_NONE)
		{
			LayoutReanimText();
		}
		mDisplayTime = mDuration;
	}
}

void MessageWidget::LayoutReanimText()
{
	float aMaxWidth = 0;
	Font *aFont = GetFont();
	size_t aLabelByteLen = strlen(mLabel.data());

	// calculate line widths and codepoint count
	size_t aCodepointCount = 0;
	float aLineWidth[MAX_REANIM_LINES] = {0.};
	{
		int aCurLine = 0;
		SexyString aLine;

		auto it = mLabel.begin();
		const auto end = mLabel.begin() + aLabelByteLen;
		while (it != end)
		{
			uint32_t aCodepoint = utf8::next(it, end);
			aCodepointCount++;
			if (aCodepoint == '\n')
			{
				TOD_ASSERT(aCurLine < MAX_REANIM_LINES);
				aLineWidth[aCurLine++] = aFont->StringWidth(aLine);
				aLine.clear();
				continue;
			}
			utf8::append(aCodepoint, aLine);
		}
		if (!aLine.empty())
		{
			TOD_ASSERT(aCurLine < MAX_REANIM_LINES);
			aLineWidth[aCurLine] = aFont->StringWidth(aLine);
		}
	}

	mSlideOffTime = aCodepointCount + 100;

	int aCurLine = 0;
	float aCurPosY = 0.0f;
	float aCurPosX = -aLineWidth[0] * 0.5f;
	auto it = mLabel.begin();
	const auto end = mLabel.begin() + aLabelByteLen;
	size_t aReanimIdx = 0;
	while (it != end && aReanimIdx < mTextReanimID.size())
	{
		Reanimation *aReanimText = mApp->AddReanimation(aCurPosX, aCurPosY, 0, mReanimType);
		aReanimText->mIsAttachment = true;
		aReanimText->PlayReanim("anim_enter", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0.0f, 0.0f);
		mTextReanimID[aReanimIdx++] = mApp->ReanimationGetID(aReanimText);

		uint32_t aCodepoint = utf8::next(it, end);
		if (aCodepoint == '\n')
		{
			aCurLine++;
			TOD_ASSERT(aCurLine < MAX_REANIM_LINES);
			aCurPosX = -aLineWidth[aCurLine] * 0.5f;
			aCurPosY += aFont->GetLineSpacing();
		}
		else
		{
			aCurPosX += aFont->CharWidth(aCodepoint);
		}
	}
}

void MessageWidget::Update()
{
	if (!mApp->mBoard || mApp->mBoard->mPaused)
		return;

	if (mDuration < 10000 && mDuration > 0)
	{
		mDuration--;
		if (mDuration == 0)
		{
			mMessageStyle = MessageStyle::MESSAGE_STYLE_OFF;
			if (mMessageStyleNext != MessageStyle::MESSAGE_STYLE_OFF)
			{
				SetLabel(mLabelNext.data(), mMessageStyleNext);
				mMessageStyleNext = MessageStyle::MESSAGE_STYLE_OFF;
			}
		}
	}

	for (int aPos = 0; aPos < mTextReanimID.size(); aPos++)
	{
		if (mTextReanimID[aPos] == ReanimationID::REANIMATIONID_NULL)
		{
			break;
		}
		Reanimation *aTextReanim = mApp->ReanimationTryToGet(mTextReanimID[aPos]);
		if (aTextReanim == nullptr)
		{
			break;
		}

		int aTextSpeed = mReanimType == ReanimationType::REANIM_TEXT_FADE_ON ? 100 : 1;
		if (mDuration > mSlideOffTime)
		{
			if (mReanimType == ReanimationType::REANIM_TEXT_FADE_ON)
			{
				aTextReanim->mAnimRate = 60.0f;
			}
			else
			{
				aTextReanim->mAnimRate = TodAnimateCurveFloat(0, 50, (mDisplayTime - mDuration) * aTextSpeed - aPos,
															  0.0f, 40.0f, TodCurves::CURVE_LINEAR);
			}
		}
		else
		{
			if (mDuration == mSlideOffTime)
			{
				aTextReanim->PlayReanim("anim_leave", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 0.0f);
			}
			aTextReanim->mAnimRate = TodAnimateCurveFloat(0, 50, (mSlideOffTime - mDuration) * aTextSpeed - aPos, 0.0f,
														  40.0f, TodCurves::CURVE_LINEAR);
		}

		aTextReanim->Update();
	}
}

void MessageWidget::DrawReanimatedText(Graphics *g, Font *theFont, const Color &theColor, float thePosY)
{
	auto it = mLabel.begin();
	const auto end = mLabel.begin() + strlen(mLabel.data());
	for (int aPos = 0; it != end && aPos < mTextReanimID.size(); aPos++)
	{
		if (mTextReanimID[aPos] == ReanimationID::REANIMATIONID_NULL)
		{
			break;
		}
		Reanimation *aTextReanim = mApp->ReanimationTryToGet(mTextReanimID[aPos]);
		if (aTextReanim == nullptr)
		{
			break;
		}

		ReanimatorTransform aTransform;
		aTextReanim->GetCurrentTransform(2, &aTransform);

		int anAlpha = ClampInt(FloatRoundToInt(theColor.mAlpha * aTransform.mAlpha), 0, 255);
		if (anAlpha <= 0)
		{
			break;
		}
		Color aFinalColor(theColor);
		aFinalColor.mAlpha = anAlpha;

		aTransform.mTransX += aTextReanim->mOverlayMatrix.m02;
		aTransform.mTransY += aTextReanim->mOverlayMatrix.m12 + thePosY - BOARD_HEIGHT / 2.0;
		if (mReanimType == ReanimationType::REANIM_TEXT_FADE_ON && mDisplayTime - mDuration < mSlideOffTime)
		{
			float aStretch = 1.0f - aTextReanim->mAnimTime;
			aTransform.mTransX += aStretch * aTextReanim->mOverlayMatrix.m02;
		}

		SexyMatrix3 aMatrix;
		Reanimation::MatrixFromTransform(aTransform, aMatrix);
		SexyString aSingleCodepoint;
		utf8::append(utf8::next(it, end), aSingleCodepoint);
		TodDrawStringMatrix(g, theFont, aMatrix, aSingleCodepoint, aFinalColor);
	}
}

Font *MessageWidget::GetFont()
{
	switch (mMessageStyle)
	{
	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL1:
	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL1_STAY:
	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL2:
	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LATER:
	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LATER_STAY:
	case MessageStyle::MESSAGE_STYLE_HINT_LONG:
	case MessageStyle::MESSAGE_STYLE_HINT_FAST:
	case MessageStyle::MESSAGE_STYLE_HINT_STAY:
	case MessageStyle::MESSAGE_STYLE_HINT_TALL_FAST:
	case MessageStyle::MESSAGE_STYLE_HINT_TALL_UNLOCKMESSAGE:
	case MessageStyle::MESSAGE_STYLE_HINT_TALL_LONG:
	case MessageStyle::MESSAGE_STYLE_BIG_MIDDLE:
	case MessageStyle::MESSAGE_STYLE_BIG_MIDDLE_FAST:
	case MessageStyle::MESSAGE_STYLE_HOUSE_NAME:
	case MessageStyle::MESSAGE_STYLE_HUGE_WAVE:
	case MessageStyle::MESSAGE_STYLE_ZEN_GARDEN_LONG:
	case MessageStyle::MESSAGE_STYLE_ACHIEVEMENT:
		return Sexy::FONT_HOUSEOFTERROR28;

	case MessageStyle::MESSAGE_STYLE_SLOT_MACHINE:
		return Sexy::FONT_HOUSEOFTERROR16;
	}

	TOD_ASSERT(false);
	return Sexy::FONT_HOUSEOFTERROR16;
}

void MessageWidget::Draw(Graphics *g)
{
	if (mDuration <= 0)
		return;

	Font *aFont = GetFont();
	Font *aOutlineFont = nullptr;
	int aPosX = BOARD_WIDTH / 2;
	int aPosY = 596;
	int aTextOffsetY = 0;
	int aRectHeight = 0;
	int aMinAlpha = 255;
	Color aColor(250, 250, 0, 255);
	Color aOutlineColor(0, 0, 0, 255);
	bool aFadeOut = false;
	if (aFont == Sexy::FONT_CONTINUUMBOLD14)
	{
		aOutlineFont = Sexy::FONT_CONTINUUMBOLD14OUTLINE;
	}

	switch (mMessageStyle)
	{
	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL1:
	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL1_STAY:
		aPosY = 400;
		aRectHeight = 110;
		aTextOffsetY = -4;
		aColor = Color(253, 245, 173);
		aMinAlpha = 192;
		break;

	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LEVEL2:
	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LATER:
	case MessageStyle::MESSAGE_STYLE_TUTORIAL_LATER_STAY:
	case MessageStyle::MESSAGE_STYLE_HINT_TALL_FAST:
	case MessageStyle::MESSAGE_STYLE_HINT_TALL_UNLOCKMESSAGE:
	case MessageStyle::MESSAGE_STYLE_HINT_TALL_LONG:
	case MessageStyle::MESSAGE_STYLE_ACHIEVEMENT:
		aPosY = 476;
		aRectHeight = 100;
		aTextOffsetY = -4;
		aColor = Color(253, 245, 173);
		aMinAlpha = 192;
		break;

	case MessageStyle::MESSAGE_STYLE_HINT_LONG:
	case MessageStyle::MESSAGE_STYLE_HINT_FAST:
	case MessageStyle::MESSAGE_STYLE_HINT_STAY:
		aPosY = 527;
		aRectHeight = 55;
		aTextOffsetY = -4;
		aColor = Color(253, 245, 173);
		aMinAlpha = 192;
		break;

	case MessageStyle::MESSAGE_STYLE_BIG_MIDDLE:
	case MessageStyle::MESSAGE_STYLE_BIG_MIDDLE_FAST:
		aPosY = 300;
		aRectHeight = 110;
		aColor = Color(253, 245, 173);
		aMinAlpha = 192;
		break;

	case MessageStyle::MESSAGE_STYLE_HOUSE_NAME:
		aPosY = 550;
		aColor = Color(255, 255, 255, 255);
		aFadeOut = true;
		break;

	case MessageStyle::MESSAGE_STYLE_HUGE_WAVE:
		aPosY = 330;
		aColor = Color(255, 0, 0);
		break;

	case MessageStyle::MESSAGE_STYLE_SLOT_MACHINE:
		aPosY = 93;
		aPosX = 340;
		aMinAlpha = 64;
		break;

	case MessageStyle::MESSAGE_STYLE_ZEN_GARDEN_LONG:
		aPosY = 514;
		aRectHeight = 55;
		aTextOffsetY = -4;
		aColor = Color(253, 245, 173);
		aMinAlpha = 192;
		break;

	default:
		TOD_ASSERT(false);
		break;
	}

	if (mReanimType != ReanimationType::REANIM_NONE)
	{
		if (aFont == Sexy::FONT_CONTINUUMBOLD14)
		{
			DrawReanimatedText(g, Sexy::FONT_CONTINUUMBOLD14OUTLINE, Color::Black, aPosY);
		}
		DrawReanimatedText(g, aFont, aColor, aPosY);
	}
	else
	{
		if (aMinAlpha != 255)
		{
			aColor.mAlpha =
				TodAnimateCurve(75, 0, mApp->mAppCounter % 75, aMinAlpha, 255, TodCurves::CURVE_BOUNCE_SLOW_MIDDLE);
			aOutlineColor.mAlpha = aColor.mAlpha;
		}
		if (aFadeOut)
		{
			aColor.mAlpha = ClampInt(mDuration * 15, 0, 255);
			aOutlineColor.mAlpha = aColor.mAlpha;
		}

		if (aRectHeight > 0)
		{
			aOutlineColor = Color(0, 0, 0, 128);
			Rect aRect(0, aPosY, BOARD_WIDTH, aRectHeight);
			g->SetColor(aOutlineColor);
			g->FillRect(aRect);

			aRect.mY += aTextOffsetY;
			TodDrawStringWrapped(g, mLabel.data(), aRect, aFont, aColor,
								 DrawStringJustification::DS_ALIGN_CENTER_VERTICAL_MIDDLE);
		}
		else
		{
			SexyString aLabelStr = mLabel.data();
			Rect aRect(aPosX - mApp->mBoard->mX - BOARD_WIDTH / 2, aPosY - aFont->mAscent, BOARD_WIDTH, BOARD_HEIGHT);
			if (aOutlineFont)
			{
				TodDrawStringWrapped(g, aLabelStr, aRect, aOutlineFont, aOutlineColor,
									 DrawStringJustification::DS_ALIGN_CENTER);
			}
			TodDrawStringWrapped(g, aLabelStr, aRect, aFont, aColor, DrawStringJustification::DS_ALIGN_CENTER);
		}

		if (mMessageStyle == MessageStyle::MESSAGE_STYLE_HOUSE_NAME)
		{
			SexyString aSubStr;
			if (mApp->IsSurvivalMode() && mApp->mBoard->mChallenge->mSurvivalStage > 0)
			{
				int aFlags = mApp->mBoard->GetNumWavesPerSurvivalStage() * mApp->mBoard->mChallenge->mSurvivalStage /
							 mApp->mBoard->GetNumWavesPerFlag();
				SexyString aFlagStr = mApp->Pluralize(aFlags, "[ONE_FLAG]", "[COUNT_FLAGS]");
				SexyString aSubStr = TodReplaceString("[FLAGS_COMPLETED]", "{FLAGS}", aFlagStr);
			}

			if (aSubStr.size() > 0)
			{
				TodDrawString(g, aSubStr, BOARD_WIDTH / 2 - mApp->mBoard->mX, aPosY + 26, Sexy::FONT_HOUSEOFTERROR16,
							  Color(224, 187, 62, aColor.mAlpha), DrawStringJustification::DS_ALIGN_CENTER);
			}
		}
	}
}
