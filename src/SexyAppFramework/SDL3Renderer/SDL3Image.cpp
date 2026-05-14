#if SEXY_USE_SDL3_RENDERER

#include "SDL3Image.h"
#include "SDL3Renderer.h"
#include "Rect.h"
#include "Graphics.h"
#include "SexyAppBase.h"
#include "AutoCrit.h"
#include "Debug.h"
#include "PerfTimer.h"

using namespace Sexy;

SDL3Image::SDL3Image(Renderer *theRenderer) : GPUImage(theRenderer)
{
	mSDL3Renderer = (SDL3Renderer *)theRenderer;
	Init();
}

SDL3Image::SDL3Image() : GPUImage(gSexyAppBase->mRenderer)
{
	mSDL3Renderer = (SDL3Renderer *)gSexyAppBase->mRenderer;
	Init();
}

SDL3Image::~SDL3Image()
{

}

uint32_t *SDL3Image::GetBits()
{
	if (mBits == nullptr)
	{
		if (mSurface == nullptr)
			return MemoryImage::GetBits();

		if (mNoLock)
			return nullptr;

		if (!LockSurface())
			return nullptr;

		mBits = new uint32_t[mWidth * mHeight + 1];
		mBits[mWidth * mHeight] = MEMORYCHECK_ID;

		float aWidth;
		float aHeight;
		void *pixels;
		int pitch;

		if (SDL_LockTexture(mPlatformTextureData->GetTexture(), nullptr, &pixels, &pitch) && SDL_GetTextureSize(mPlatformTextureData->GetTexture(), &aWidth, &aHeight))
		{
			SetBits((uint32_t *)pixels, (int)aWidth, (int)aHeight);
			SDL_UnlockTexture(mPlatformTextureData->GetTexture());
		}

		UnlockSurface();
	}

	return mBits;
}

void SDL3Image::DeleteSurface()
{
	if (mSurface != nullptr)
	{
		if ((mColorTable == NULL) && (mBits == NULL) && (mGPUData == NULL))
			GetBits();

		SDL_DestroyTexture((SDL_Texture *)mSurface);
		mSurface = nullptr;
	}
}

void SDL3Image::BitsChanged()
{
	MemoryImage::BitsChanged();

	SDL_DestroyTexture((SDL_Texture *)mSurface);
	mSurface = nullptr;
}

bool SDL3Image::GenerateSurface()
{
	if (mSurface != nullptr)
		return true;

	CommitBits();

	if (mHasAlpha)
		return false;

	mWantSurface = true;

	// Force into non-palletized mode for this
	if (mColorTable != NULL)
		GetBits();

	AutoCrit aCrit(mRenderer->mCritSect); // prevent mSurface from being released while we're in this code

	if (!LockSurface())
		return false;

	SDL3Renderer::gSDLTextureCount++;
	mSurface = SDL_CreateTexture(mSDL3Renderer->mBackendRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, mWidth, mHeight);

	UnlockSurface();

	return true;
}

void SDL3Image::SetSurface(void *theSurface) // this should be a texture id or something.
{
	mSurfaceSet = true;
	mSurface = theSurface;


	mNoLock = false;
}

bool SDL3Image::LockSurface()
{
	if (mLockCount != 0)
		return true;

	SDL_LockTexture((SDL_Texture*)mSurface, nullptr, nullptr, nullptr);

	mLockCount++;
	return true;
}

bool SDL3Image::UnlockSurface()
{
	if (mLockCount == 0)
		return true;
	SDL_UnlockTexture((SDL_Texture *)mSurface);

	mLockCount--;
	return true;
}

void SDL3Image::CreateImageBuffers()
{
	if (Check3D(this))
		return;

}

void SDL3Image::PreTextureDraw()
{
	CreateImageBuffers();

}

void SDL3Image::Init()
{

}

void SDL3Image::ImplFillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{

	PreTextureDraw();

}

void SDL3Image::ImplDrawLine(double theStartX, double theStartY, double theEndX, double theEndY,
							   const Color &theColor, int theDrawMode)
{
	PreTextureDraw();

}

void SDL3Image::ImplDrawLineAA(double theStartX, double theStartY, double theEndX, double theEndY,
								 const Color &theColor, int theDrawMode)
{

}

void SDL3Image::ImplBlt(Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor,
						  int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mSDL3Renderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
}

void SDL3Image::ImplBltF(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Rect &theClipRect,
						   const Color &theColor, int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mSDL3Renderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
}

void SDL3Image::ImplBltRotated(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Rect &theClipRect,
							   const Color &theColor, int theDrawMode, double theRot, float theRotCenterX,
							   float theRotCenterY)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mSDL3Renderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
}

void SDL3Image::ImplStretchBlt(Image *theImage, const Rect &theDestRect, const Rect &theSrcRect,
								 const Rect &theClipRect, const Color &theColor, int theDrawMode, bool fastStretch)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mSDL3Renderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
}

void SDL3Image::ImplBltMatrix(Image *theImage, float x, float y, const SexyMatrix3 &theMatrix,
								const Rect &theClipRect, const Color &theColor, int theDrawMode, const Rect &theSrcRect,
								bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mSDL3Renderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
}

void SDL3Image::ImplBltTrianglesTex(Image *theTexture, const TriVertex theVertices[][3], int theNumTriangles,
									  const Rect &theClipRect, const Color &theColor, int theDrawMode, float tx,
									  float ty, bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theTexture;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mSDL3Renderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
}

void SDL3Image::ImplBltMirror(Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor,
								int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mSDL3Renderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
}

void SDL3Image::ImplStretchBltMirror(Image *theImage, const Rect &theDestRectOrig, const Rect &theSrcRect,
									   const Rect &theClipRect, const Color &theColor, int theDrawMode,
									   bool fastStretch)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mSDL3Renderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
}

void SDL3Image::ImplBltRawTexture(void *theTexture, int theTexWidth, int theTexHeight, const Rect &theDestRect,
									const Rect &theSrcRect, const Rect &theClipRect, const Color &theColor,
									int theDrawMode, bool fastStretch)
{
	PreTextureDraw();

	mSDL3Renderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
}

#endif