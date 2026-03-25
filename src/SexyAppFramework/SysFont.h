#ifndef __SYSFONT_H__
#define __SYSFONT_H__

#include "Font.h"
#include <freetype/freetype.h>

namespace Sexy
{

class ImageFont;
class SexyAppBase;

struct TrueTypeGlyph
{
	int mWidth;
	int mHeight;
	int mBearingX;
	int mBearingY;
	int mAdvance;
	void *mTexData; //backend dynamic, GLuint - OpenGL, etc
};

class TrueTypeData;

class SysFont : public Font
{
  public:
	TrueTypeData* mTTData;
	SexyAppBase *mApp;
	std::string mFontName;
	bool mDrawShadow;
	bool mSimulateBold;

	void Init(SexyAppBase *theApp,
			  const std::string &theFace,
			  int thePointSize,
			  bool bold,
			  bool italics,
			  bool underline,
			  bool useDevCaps);

	void Reinit();

  public:
	SysFont(
		const std::string &theFace, int thePointSize, bool bold = false, bool italics = false, bool underline = false);
	SysFont(SexyAppBase *theApp,
			const std::string &theFace,
			int thePointSize,
			bool bold = false,
			bool italics = false,
			bool underline = false);
	SysFont(const SysFont &theSysFont);

	virtual ~SysFont();

	ImageFont *CreateImageFont();
	virtual int StringWidth(const SexyString &theString);
	virtual void DrawString(
		Graphics *g, int theX, int theY, const SexyString &theString, const Color &theColor, const Rect &theClipRect);

	virtual Font *Duplicate();

};

struct TrueTypeData
{
	std::map<char, TrueTypeGlyph> mGlyphs;
	SysFont *mFont;
	FT_Face mFace;
	int mSize;
	bool mIsDirty;

	TrueTypeData(SysFont *theFontPtr, FT_Face &theFace, int theSize) : mFont(theFontPtr), mFace(theFace), mSize(theSize)
	{
		Init();
	}

	~TrueTypeData();

	void Init();

	TrueTypeGlyph GetGlyph(char &theChar);
};

} // namespace Sexy

#endif //__SYSFONT_H__