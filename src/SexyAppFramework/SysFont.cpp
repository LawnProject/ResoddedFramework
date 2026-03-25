#include "SysFont.h"
#include "GPUImage.h"
#include "SexyAppBase.h"
#include "Graphics.h"
#include "ImageFont.h"
#include "MemoryImage.h"
#include "Renderer.h"
#include "WidgetManager.h"
#include <stdlib.h>
#if SEXY_USE_OPENGL
#include "OpenGLRenderer.h"
#endif

using namespace Sexy;

SysFont::SysFont(const std::string &theFace, int thePointSize, bool bold, bool italics, bool underline)
{
	Init(gSexyAppBase, theFace, thePointSize, bold, italics, underline, false);
}

SysFont::SysFont(SexyAppBase *theApp,
				 const std::string &theFace,
				 int thePointSize,
				 bool bold,
				 bool italics,
				 bool underline)
{
	Init(theApp, theFace, thePointSize, bold, italics, underline, true);
}

void SysFont::Init(SexyAppBase *theApp,
				   const std::string &theFace,
				   int thePointSize,
				   bool bold,
				   bool italics,
				   bool underline,
				   bool useDevCaps)
{
	mApp = theApp;
	mApp->mRenderer->mSysFonts.insert(this);

	FT_Face aFontFace;
	FT_Error anError = FT_New_Face(mApp->mFreeTypeLib, theFace.c_str(), 0, &aFontFace);
	if (anError)
	{
		#if WIN32
		anError = FT_New_Face(mApp->mFreeTypeLib, ("C:/Windows/Fonts/" + theFace + ".ttf").c_str(), 0, &aFontFace);
		#endif
	}
	mTTData = new TrueTypeData(this, aFontFace, thePointSize);

    mAscent = aFontFace->size->metrics.ascender >> 6;
	mHeight = (aFontFace->size->metrics.ascender - aFontFace->size->metrics.descender) >> 6;

	mDrawShadow = false;
	mSimulateBold = false;
	mFontName = theFace;
}

void SysFont::Reinit()
{
	FT_Face aFontFace;
	FT_Error anError = FT_New_Face(mApp->mFreeTypeLib, mFontName.c_str(), 0, &aFontFace);
	if (anError)
	{
#if WIN32
		anError = FT_New_Face(mApp->mFreeTypeLib, ("C:/Windows/Fonts/" + mFontName + ".ttf").c_str(), 0, &aFontFace);
#endif
	}
	int aOldSize = mTTData->mSize;
	delete mTTData;
	mTTData = nullptr;
	mTTData = new TrueTypeData(this, aFontFace, aOldSize);

	mAscent = aFontFace->size->metrics.ascender >> 6;
	mHeight = (aFontFace->size->metrics.ascender - aFontFace->size->metrics.descender) >> 6;
}

SysFont::SysFont(const SysFont &theSysFont)
{
	mApp = theSysFont.mApp;
	mHeight = theSysFont.mHeight;
	mAscent = theSysFont.mAscent;
	mTTData = theSysFont.mTTData;

	mDrawShadow = false;
	mSimulateBold = false;
}

SysFont::~SysFont()
{
	if (mTTData)
		delete mTTData;
	mApp->mRenderer->mSysFonts.erase(this);
}

ImageFont *SysFont::CreateImageFont()
{ 
	//todo: uuhhh implement?
	return nullptr;
}

int SysFont::StringWidth(const SexyString &theString)
{ 
    int aWidth = 0;
	for (char c : theString)
	{
		TrueTypeGlyph g = mTTData->GetGlyph(c);
		aWidth += g.mAdvance;
	}
	return aWidth;
}

void SysFont::DrawString(
	Graphics *g, int theX, int theY, const SexyString &theString, const Color &theColor, const Rect &theClipRect)
{ 
	int posX = theX;
	int posY = theY;

	for (char c : theString)
	{
		TrueTypeGlyph aGlyph = mTTData->GetGlyph(c);
		int aDrawX = posX + aGlyph.mBearingX;
		int aDrawY = posY - aGlyph.mBearingY;

		if (g->mDestImage != &Graphics::mStaticImage)
		{

			if (mDrawShadow)
			{
				Color aShadowColor = Color(0, 0, 0, 0);
				g->mDestImage->BltRawTexture(
					aGlyph.mTexData,
					aGlyph.mWidth,
					aGlyph.mHeight,
					Rect(aDrawX + g->mTransX + 1, aDrawY - mAscent + 1 + g->mTransY, aGlyph.mWidth, aGlyph.mHeight),
					Rect(0, 0, aGlyph.mWidth, aGlyph.mHeight),
					theClipRect,
					aShadowColor,
					0);

				if (mSimulateBold)
					g->mDestImage->BltRawTexture(
						aGlyph.mTexData,
						aGlyph.mWidth,
						aGlyph.mHeight,
						Rect(aDrawX + g->mTransX + 2, aDrawY - mAscent + 1 + g->mTransY, aGlyph.mWidth, aGlyph.mHeight),
						Rect(0, 0, aGlyph.mWidth, aGlyph.mHeight),
						theClipRect,
						aShadowColor,
						0);
			}

			g->mDestImage->BltRawTexture(aGlyph.mTexData,
										aGlyph.mWidth,
										aGlyph.mHeight,
										Rect(aDrawX, aDrawY, aGlyph.mWidth, aGlyph.mHeight),
										Rect(0, 0, aGlyph.mWidth, aGlyph.mHeight),
										theClipRect,
										theColor,
										0);

			if (mSimulateBold)
				g->mDestImage->BltRawTexture(
					aGlyph.mTexData,
					aGlyph.mWidth,
					aGlyph.mHeight,
					Rect(aDrawX + g->mTransX + 1, aDrawY - mAscent + 1 + g->mTransY, aGlyph.mWidth, aGlyph.mHeight),
					Rect(0, 0, aGlyph.mWidth, aGlyph.mHeight),
					theClipRect,
					theColor,
					0);
		}
		else
		{
			if (mDrawShadow)
			{
				Color aShadowColor = Color(0, 0, 0, 0);
				mApp->mRenderer->BltRawTexture(
					aGlyph.mTexData,
					Rect(aDrawX + g->mTransX + 1, aDrawY - mAscent + 1 + g->mTransY, aGlyph.mWidth, aGlyph.mHeight),
					Rect(0, 0, aGlyph.mWidth, aGlyph.mHeight),
					theClipRect,
					aShadowColor,
					0);

				if (mSimulateBold)
					mApp->mRenderer->BltRawTexture(
						aGlyph.mTexData,
						Rect(aDrawX + g->mTransX + 2, aDrawY - mAscent + 1 + g->mTransY, aGlyph.mWidth, aGlyph.mHeight),
						Rect(0, 0, aGlyph.mWidth, aGlyph.mHeight),
						theClipRect,
						aShadowColor,
						0);
			}

			mApp->mRenderer->BltRawTexture(aGlyph.mTexData,
										   Rect(aDrawX, aDrawY, aGlyph.mWidth, aGlyph.mHeight),
										   Rect(0, 0, aGlyph.mWidth, aGlyph.mHeight),
										   theClipRect,
										   theColor,
										   0);

			if (mSimulateBold)
				mApp->mRenderer->BltRawTexture(
					aGlyph.mTexData,
					Rect(aDrawX + g->mTransX + 1, aDrawY - mAscent + 1 + g->mTransY, aGlyph.mWidth, aGlyph.mHeight),
					Rect(0, 0, aGlyph.mWidth, aGlyph.mHeight),
					theClipRect,
					theColor,
					0);
		}

		posX += aGlyph.mAdvance;
	}
}

Font *SysFont::Duplicate()
{
	return new SysFont(*this);
}
void TrueTypeData::Init()
{
	FT_Set_Pixel_Sizes(mFace, 0, mSize);
	for (auto pair : mGlyphs)
	{
		if (pair.second.mTexData != nullptr)
		{
			mFont->mApp->mRenderer->DeleteTexture(pair.second.mTexData);
		}
	}
	mGlyphs.clear();
}

TrueTypeData::~TrueTypeData()
{
	for (auto pair : mGlyphs)
	{
		if (pair.second.mTexData != nullptr)
		{
			mFont->mApp->mRenderer->DeleteTexture(pair.second.mTexData);
		}
	}
	mGlyphs.clear();
	FT_Done_Face(mFace);
}

TrueTypeGlyph TrueTypeData::GetGlyph(char &theChar)
{
	auto it = mGlyphs.find(theChar);
	if (it != mGlyphs.end())
	{
		return it->second;
	}

	TrueTypeGlyph aGlyph;

	if (!FT_Load_Char(mFace, theChar, FT_LOAD_RENDER))
	{
		//idk
	}

	FT_Bitmap &aBitmap = mFace->glyph->bitmap;

	aGlyph.mWidth = aBitmap.width;
	aGlyph.mHeight = aBitmap.rows;
	aGlyph.mBearingX = mFace->glyph->bitmap_left;
	aGlyph.mBearingY = mFace->glyph->bitmap_top;
	aGlyph.mAdvance = mFace->glyph->advance.x >> 6;
	uint32_t *aConvertedPixels = new uint32_t[aGlyph.mWidth * aGlyph.mHeight];
	for (int i = 0; i < aGlyph.mWidth * aGlyph.mHeight; i++)
	{
		uint8_t anAlpha = mFace->glyph->bitmap.buffer[i];
		aConvertedPixels[i] = (anAlpha << 24) | 0x00FFFFFF;
	}
	aGlyph.mTexData = mFont->mApp->mRenderer->CreateTexture(aConvertedPixels, aGlyph.mWidth, aGlyph.mHeight, RawPixelFormat::RAW_FORMAT_RGBA, 1);
	delete[] aConvertedPixels;

	mGlyphs.insert(std::pair<char, TrueTypeGlyph>(theChar, aGlyph));

	return aGlyph;
}