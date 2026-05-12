#ifndef __USERDIALOG_H__
#define __USERDIALOG_H__

#include "LawnDialog.h"
#include "../../SexyAppFramework/ListListener.h"
#include "../../SexyAppFramework/EditListener.h"
#include "../../SexyAppFramework/SliderListener.h"

namespace Sexy
{
class ListWidget;
class Slider;
};

class UserDialog : public LawnDialog, public ListListener, public EditListener, public SliderListener
{
  protected:
	enum
	{
		UserDialog_RenameUser,
		UserDialog_DeleteUser,
		UserDialog_ListSlider,
	};

  public:
	ListWidget *mUserList;
	DialogButton *mRenameButton;
	DialogButton *mDeleteButton;
	Slider *mListSlider;
	int mNumUsers;

  public:
	UserDialog(LawnApp *theApp);
	virtual ~UserDialog();

	virtual void Resize(int theX, int theY, int theWidth, int theHeight);
	virtual int GetPreferredHeight(int theWidth);
	virtual void AddedToManager(WidgetManager *theWidgetManager);
	virtual void RemovedFromManager(WidgetManager *theWidgetManager);
	virtual void ListClicked(int theId, int theIdx, int theClickCount);
	virtual void ButtonDepress(int theId);
	virtual void EditWidgetText(int theId, const SexyString &theString);
	virtual bool AllowChar(int theId, SexyChar theChar);
	virtual void Draw(Graphics *g);
	virtual void Update();
	void FinishDeleteUser();
	void FinishRenameUser(const SexyString &theNewName);
	SexyString GetSelName();
};

#endif
