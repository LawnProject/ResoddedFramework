#ifndef __SYSFONT_H__
#define __SYSFONT_H__

#include "Font.h"
#include <unordered_map>
#include <freetype/freetype.h>
#include <memory>

namespace Sexy
{

class ImageFont;
class SexyAppBase;

struct GlpyhAtlasEntry
{
	int mX;
	int mY;
	int mWidth;
	int mHeight;
	int mBearingX;
	int mBearingY;
	int mAdvance;
};
struct GlyphAtlas
{
	int mPadding = 1;
	int mWidth = 4096;
	int mHeight = 1024;
	void *mAtlas = nullptr; // void* to put data for other renderers
	std::unordered_map<uint32_t, GlpyhAtlasEntry> mGlyphs;
};

class TrueTypeData
{
  public:
	GlyphAtlas mAtlas;
	SexyAppBase *mApp;
	FT_Face mFace;
	int mSize;
	bool mBold;
	bool mIsDirty;

	TrueTypeData(SexyAppBase *theApp, FT_Face theFace, int theSize, bool theBold) : mApp(theApp), mFace(theFace), mSize(theSize), mBold(theBold)
	{
		Init();
	}

	~TrueTypeData();

	void Init();
};

class SysFont : public Font
{
  public:
	std::shared_ptr<TrueTypeData> mFontData;
	SexyAppBase *mApp;
	std::string mFontName;
	bool mDrawShadow;
	bool mBold;
	bool mItalic;
	bool mUnderlined;

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

} // namespace Sexy

#endif //__SYSFONT_H__
