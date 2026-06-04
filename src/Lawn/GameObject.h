#pragma once

#include "../ConstEnums.h"
#include "../SexyAppFramework/Graphics.h"

using namespace Sexy;

class LawnApp;
class Board;

class GameObject
{
  public:
	LawnApp *mApp;
	Board *mBoard;
	int mX;
	int mY;
	int mWidth;
	int mHeight;
	bool mVisible;
	int mRow;
	int mRenderOrder;

  public:
	GameObject();
	bool BeginDraw(Graphics *g);
	void EndDraw(Graphics *g);
	void MakeParentGraphicsFrame(Graphics *g);
};
