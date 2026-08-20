#pragma once

#include <Common/ConstEnums.h>
#include <Graphics.h>

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
	/// @brief Begin the drawing a GameObject by translating the Graphics object by it's X and Y values
	/// @return True if it began to draw it, False if it was invisible
	bool BeginDraw(Graphics *g);
	/// @brief End the drawing a GameObject by translating the Graphics object by the negative of it's X and Y values
	void EndDraw(Graphics *g);
	/// @brief Frame the Graphics object relative to the GameObject (???)
	void MakeParentGraphicsFrame(Graphics *g);
};
