#include "../LawnCommon.h"
#include "NewUserDialog.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../SexyAppFramework/WidgetManager.h"

NewUserDialog::NewUserDialog(LawnApp *theApp, bool isRename)
	: LawnDialog(theApp,
				 isRename ? Dialogs::DIALOG_RENAMEUSER : Dialogs::DIALOG_CREATEUSER,
				 true,
				 isRename ? "[RENAME_USER]" : "[NEW_USER]",
				 "[PLEASE_ENTER_NAME]",
				 "[DIALOG_BUTTON_OK]",
				 Dialog::BUTTONS_OK_CANCEL)
{
	mApp = theApp;
	mVerticalCenterText = false;
	mNameEditWidget = CreateEditWidget(0, this, this);
	mNameEditWidget->mMaxChars = 12;
	mNameEditWidget->AddWidthCheckFont(FONT_BRIANNETOD16, 220);
	CalcSize(110, 40);
}

NewUserDialog::~NewUserDialog()
{
	delete mNameEditWidget;
}

void NewUserDialog::AddedToManager(WidgetManager *theWidgetManager)
{
	LawnDialog::AddedToManager(theWidgetManager);
	AddWidget(mNameEditWidget);
	theWidgetManager->SetFocus(mNameEditWidget);
}

void NewUserDialog::RemovedFromManager(WidgetManager *theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mNameEditWidget);
}

int NewUserDialog::GetPreferredHeight(int theWidth)
{
	return LawnDialog::GetPreferredHeight(theWidth) + 40;
}

void NewUserDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	mNameEditWidget->Resize(
		mContentInsets.mLeft + 12, mHeight - 155, mWidth - mContentInsets.mLeft - mContentInsets.mRight - 24, 28);
}

void NewUserDialog::Draw(Graphics *g)
{
	LawnDialog::Draw(g);
	DrawEditBox(g, mNameEditWidget);
}

void NewUserDialog::EditWidgetText(int theId, const SexyString &theString)
{
	mApp->ButtonDepress(mId + 2000);
}

bool NewUserDialog::AllowChar(int, SexyChar theChar)
{
	return isalnum(theChar) || theChar == ' ';
}

SexyString NewUserDialog::GetName()
{
	SexyString aString;
	SexyChar aLastChar = ' ';

	for (int i = 0; i < mNameEditWidget->mString.size(); i++)
	{
		SexyChar aChar = mNameEditWidget->mString[i];
		if (aChar != ' ')
		{
			aString.append(1, aChar);
		}
		else if (aChar != aLastChar)
		{
			aString.append(1, ' ');
		}

		aLastChar = aChar;
	}

	if (aString.size() && aString[aString.size() - 1] == ' ')
	{
		aString.resize(aString.size() - 1);
	}

	return aString;
}

void NewUserDialog::SetName(const SexyString &theName)
{
	mNameEditWidget->SetText(theName, true);
	mNameEditWidget->mCursorPos = theName.size();
	mNameEditWidget->mHilitePos = 0;
}
