#include "SysFont.h"
#include "GPUImage.h"
#include "SexyAppBase.h"
#include "Graphics.h"
#include "ImageFont.h"
#include "MemoryImage.h"
#include "Renderer.h"
#include "WidgetManager.h"
#include <stdlib.h>
#include FT_OUTLINE_H
#include FT_SYNTHESIS_H
#if SEXY_USE_OPENGL
#include "OpenGL/OpenGLRenderer.h"
#endif
#if SEXY_USE_SDL3_RENDERER
#include "SDL3Renderer/SDL3Renderer.h"
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
	mBold = bold;
	mItalic = italics;
	mUnderlined = underline;

	FT_Face aFontFace;
	FT_Error anError = FT_New_Face(mApp->mFreeTypeLib, theFace.c_str(), 0, &aFontFace);
	if (anError)
	{
		#if WIN32
		anError = FT_New_Face(mApp->mFreeTypeLib, ("C:/Windows/Fonts/" + theFace + ".ttf").c_str(), 0, &aFontFace);
		#endif
	}
	FT_Select_Charmap(aFontFace, FT_ENCODING_UNICODE);
	if (mItalic)
	{
		FT_Matrix matrix = {1 << 16, 0.3 * (1 << 16), 0, 1 << 16};
		FT_Set_Transform(aFontFace, &matrix, nullptr);
	}

	mFontData = new TrueTypeData(this, aFontFace, thePointSize);

    mAscent = aFontFace->size->metrics.ascender >> 6;
	mHeight = (aFontFace->size->metrics.ascender - aFontFace->size->metrics.descender) >> 6;

	mDrawShadow = false;
	mFontName = theFace;
}

void SysFont::Reinit()
{
	FT_Face aFontFace;
	bool aPrevFlags = mFontData->mFace->style_flags;
	FT_Error anError = FT_New_Face(mApp->mFreeTypeLib, mFontName.c_str(), 0, &aFontFace);
	if (anError)
	{
#if WIN32
		anError = FT_New_Face(mApp->mFreeTypeLib, ("C:/Windows/Fonts/" + mFontName + ".ttf").c_str(), 0, &aFontFace);
#endif
	}

	aFontFace->style_flags = aPrevFlags;

	int aOldSize = mFontData->mSize;
	delete mFontData;
	mFontData = nullptr;
	mFontData = new TrueTypeData(this, aFontFace, aOldSize);

	mAscent = aFontFace->size->metrics.ascender >> 6;
	mHeight = (aFontFace->size->metrics.ascender - aFontFace->size->metrics.descender) >> 6;
}

SysFont::SysFont(const SysFont &theSysFont)
{
	mApp = theSysFont.mApp;
	mHeight = theSysFont.mHeight;
	mAscent = theSysFont.mAscent;
	mFontData = theSysFont.mFontData;
	mFontData->mFont = this;
	mBold = theSysFont.mBold;
	mItalic = theSysFont.mItalic;

	mDrawShadow = false;
}

SysFont::~SysFont()
{
	if (mFontData)
		delete mFontData;
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
	auto it = theString.begin();
	auto end = theString.end();
	while (it != end)
	{
		uint32_t c = utf8::next(it, end);

		aWidth += mFontData->mAtlas.mGlyphs[c].mAdvance;
	}
	return aWidth;
}

void SysFont::DrawString(
	Graphics *g, int theX, int theY, const SexyString &theString, const Color &theColor, const Rect &theClipRect)
{ 
	int posX = theX;
	int posY = theY;
	int underlineY = posY - ((mFontData->mFace->underline_position * mFontData->mFace->size->metrics.y_scale) >> 16 >> 6);
	
	auto it = theString.begin();
	auto end = theString.end();
	while (it != end)
	{
		uint32_t c = utf8::next(it, end);
		GlpyhAtlasEntry aGlyph = mFontData->mAtlas.mGlyphs[c];
		int aDrawX = posX + aGlyph.mBearingX;
		int aDrawY = posY - aGlyph.mBearingY;

		if (g->mDestImage != &Graphics::mStaticImage)
		{
			if (mDrawShadow)
			{
				Color aShadowColor = Color(0, 0, 0, 0);
				g->mDestImage->BltRawTexture(
					mFontData->mAtlas.mAtlas,
					aGlyph.mWidth,
					aGlyph.mHeight,
					Rect(aDrawX + g->mTransX + 1, aDrawY - mAscent + 1 + g->mTransY, aGlyph.mWidth, aGlyph.mHeight),
					Rect(aGlyph.mX, aGlyph.mY, aGlyph.mWidth, aGlyph.mHeight),
					theClipRect,
					aShadowColor,
					0);
			}

			g->mDestImage->BltRawTexture(
										mFontData->mAtlas.mAtlas,
										mFontData->mAtlas.mWidth,
										mFontData->mAtlas.mHeight,
										Rect(aDrawX, aDrawY, aGlyph.mWidth, aGlyph.mHeight),
										Rect(aGlyph.mX, aGlyph.mY, aGlyph.mWidth, aGlyph.mHeight),
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
					mFontData->mAtlas.mAtlas,
					mFontData->mAtlas.mWidth,
					mFontData->mAtlas.mHeight,
					Rect(aDrawX + g->mTransX + 1, aDrawY - mAscent + 1 + g->mTransY, aGlyph.mWidth, aGlyph.mHeight),
					Rect(aGlyph.mX, aGlyph.mY, aGlyph.mWidth, aGlyph.mHeight),
					theClipRect,
					aShadowColor,
					0);

			}

			mApp->mRenderer->BltRawTexture(
						mFontData->mAtlas.mAtlas,
						mFontData->mAtlas.mWidth,
						mFontData->mAtlas.mHeight,
						Rect(aDrawX + g->mTransX + 1, aDrawY - mAscent + 1 + g->mTransY, aGlyph.mWidth, aGlyph.mHeight),
						Rect(aGlyph.mX, aGlyph.mY, aGlyph.mWidth, aGlyph.mHeight),
						theClipRect,
						theColor,
						0);
		}

		posX += aGlyph.mAdvance;
	}
	if (g->mDestImage != &Graphics::mStaticImage)
	{
		if (mUnderlined)
			g->mDestImage->DrawLine(theX, underlineY, StringWidth(theString), underlineY, theColor, 0);
	}
	else
	{
		if (mUnderlined)
			mApp->mRenderer->DrawLine(
				theX + g->mTransX + 1, underlineY, StringWidth(theString), underlineY, theColor, 0);

	}
}

Font *SysFont::Duplicate()
{
	return new SysFont(*this);
}
void TrueTypeData::Init()
{
	FT_Set_Pixel_Sizes(mFace, 0, mSize);
	if (mAtlas.mAtlas != nullptr)
	{
		mFont->mApp->mRenderer->DeleteTexture(mAtlas.mAtlas);
	}
	mAtlas.mGlyphs.clear();

	int aLastX = mAtlas.mPadding;
	int aLastY = mAtlas.mPadding;
	int aRowHeight = 0;
	uint32_t *anAtlasPixels = new uint32_t[mAtlas.mWidth * mAtlas.mHeight]; //TODO: RESIZE TO FIT ALL CHARACTERS PROPERLY
	std::vector<uint32_t> aWantedChar;

	for (char c = ' '; c <= '~'; c++)
		aWantedChar.push_back(c);
	for (uint32_t c = 0x00A0; c < 0x024F; c++)
		aWantedChar.push_back(c);
	for (uint32_t aSetupChar : aWantedChar)
	{
		GlpyhAtlasEntry aGlyph;
		FT_Load_Char(mFace, aSetupChar, FT_LOAD_RENDER);
		
		if (mFont->mBold)
		{
			FT_GlyphSlot_Embolden(mFace->glyph);
		}

		FT_Bitmap &aBitmap = mFace->glyph->bitmap;

		aGlyph.mX = aLastX;
		aGlyph.mY = aLastY;
		aGlyph.mWidth = aBitmap.width;
		aGlyph.mHeight = aBitmap.rows;
		aGlyph.mBearingX = mFace->glyph->bitmap_left;
		aGlyph.mBearingY = mFace->glyph->bitmap_top;
		aGlyph.mAdvance = mFace->glyph->advance.x >> 6;
		if (aLastX + aGlyph.mWidth + mAtlas.mPadding > mAtlas.mWidth)
		{
			aLastX = mAtlas.mPadding;
			aLastY += aRowHeight + mAtlas.mPadding;
			aRowHeight = 0;
		}
		aRowHeight = std::max(aRowHeight, aGlyph.mHeight);
		aLastX += aGlyph.mWidth + mAtlas.mPadding;
		uint32_t *aConvertedPixels = new uint32_t[aGlyph.mWidth * aGlyph.mHeight];
		int i = 0;
		for (int y = 0; y < aGlyph.mHeight; y++)
		{
			for (int x = 0; x < aGlyph.mWidth; x++)
			{
				uint8_t anAlpha = aBitmap.buffer[y * aBitmap.pitch + x];
				aConvertedPixels[i++] = (anAlpha << 24) | 0x00FFFFFF;
			}
		}

		for (int y = 0; y < aGlyph.mHeight; ++y)
		{
			uint32_t *aDest = anAtlasPixels + (aGlyph.mY + y) * mAtlas.mWidth + aGlyph.mX;
			uint32_t *aSrc = aConvertedPixels + y * aGlyph.mWidth;

			memcpy(aDest, aSrc, aGlyph.mWidth * sizeof(uint32_t));
		}

		delete[] aConvertedPixels;

		mAtlas.mGlyphs[aSetupChar] = aGlyph;
	}
	mAtlas.mAtlas = mFont->mApp->mRenderer->CreateTexture(anAtlasPixels, mAtlas.mWidth, mAtlas.mHeight, RawPixelFormat::RAW_FORMAT_RGBA, 1);

	delete[] anAtlasPixels;
}

TrueTypeData::~TrueTypeData()
{
	if (mAtlas.mAtlas != nullptr)
	{
		mFont->mApp->mRenderer->DeleteTexture(mAtlas.mAtlas);
	}
	mAtlas.mGlyphs.clear();
	FT_Done_Face(mFace);
}
