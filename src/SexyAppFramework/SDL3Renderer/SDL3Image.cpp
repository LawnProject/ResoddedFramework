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

		float aWidth;
		float aHeight;
		void *pixels;
		int pitch;
		bool aSizeResult = SDL_GetTextureSize((SDL_Texture *)mSurface, &aWidth, &aHeight);
	
		SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture *)mSurface);

		SDL_Surface *aReadSurface = SDL_RenderReadPixels(mSDL3Renderer->mBackendRenderer, nullptr);

		mBits = new uint32_t[mWidth * mHeight + 1];
		mBits[mWidth * mHeight] = MEMORYCHECK_ID;

		if (aSizeResult)
		{
			SetBits((uint32_t *)aReadSurface->pixels, (int)aWidth, (int)aHeight);
			SDL_UnlockTexture((SDL_Texture *)mSurface);
		}
		SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);

		SDL_DestroySurface(aReadSurface);
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
	if (Check3D(this) || mSurface != nullptr)
		return;
	mSurface = SDL_CreateTexture(mSDL3Renderer->mBackendRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mWidth, mHeight);
}

void SDL3Image::PreTextureDraw()
{
	CreateImageBuffers();
}

void SDL3Image::Init()
{
	mPlatformTextureData = nullptr;
}

void SDL3Image::ImplFillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{
	PreTextureDraw();

	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture *)mSurface);

	SDL_FRect aRect = {(float)theRect.mX, (float)theRect.mY, (float)theRect.mWidth, (float)theRect.mHeight};

	SDL_SetRenderDrawColor(mSDL3Renderer->mBackendRenderer, theColor.mRed, theColor.mGreen, theColor.mBlue, theColor.mAlpha);
	SDL_SetRenderDrawBlendMode(mSDL3Renderer->mBackendRenderer, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));
	SDL_RenderFillRect(mSDL3Renderer->mBackendRenderer, &aRect);

	SDL_SetRenderDrawBlendMode(mSDL3Renderer->mBackendRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplDrawLine(double theStartX, double theStartY, double theEndX, double theEndY,
							   const Color &theColor, int theDrawMode)
{
	PreTextureDraw();
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture *)mSurface);

	SDL_SetRenderDrawBlendMode(mSDL3Renderer->mBackendRenderer, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));
	SDL_SetRenderDrawColor(mSDL3Renderer->mBackendRenderer, theColor.mRed, theColor.mGreen, theColor.mBlue, theColor.mAlpha);

	SDL_RenderLine(mSDL3Renderer->mBackendRenderer, theStartX, theStartY, theEndX, theEndY);

	SDL_SetRenderDrawBlendMode(mSDL3Renderer->mBackendRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplDrawLineAA(double theStartX, double theStartY, double theEndX, double theEndY,
								 const Color &theColor, int theDrawMode)
{
	ImplDrawLine(theStartX, theStartY, theEndX, theEndY, theColor, theDrawMode);
}

void SDL3Image::ImplBlt(Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor,
						  int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth,
						  (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {(float)theX, (float)theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture*)mSurface);

	mSDL3Renderer->SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));
	SDL_RenderTexture(mSDL3Renderer->mBackendRenderer, aTexture, &aSrcRect, &aDestRect);
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplBltF(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Rect &theClipRect,
						   const Color &theColor, int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth,
						  (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {theX, theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture*)mSurface);

	mSDL3Renderer->SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, &aClipRect);
	SDL_RenderTexture(mSDL3Renderer->mBackendRenderer, aTexture, &aSrcRect, &aDestRect);
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, nullptr);
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplBltRotated(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Rect &theClipRect,
							   const Color &theColor, int theDrawMode, double theRot, float theRotCenterX,
							   float theRotCenterY)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();

	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture *)mSurface);

	mSDL3Renderer->SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));

	SDL_FRect aDestRect = {theX, theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, &aClipRect);
	SDL_FPoint aRotCenter = {theRotCenterX, theRotCenterY};

	SDL_RenderTextureRotated(mSDL3Renderer->mBackendRenderer, aTexture, &aSrcRect, &aDestRect, theRot, &aRotCenter, SDL_FLIP_NONE);
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplStretchBlt(Image *theImage, const Rect &theDestRect, const Rect &theSrcRect,
								 const Rect &theClipRect, const Color &theColor, int theDrawMode, bool fastStretch)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();
	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth,
						  (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {(float)theDestRect.mX, (float)theDestRect.mY, (float)theDestRect.mWidth,
						   (float)theDestRect.mHeight};
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture *)mSurface);

	mSDL3Renderer->SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, &aClipRect);
	SDL_RenderTextureRotated(mSDL3Renderer->mBackendRenderer, aTexture, &aSrcRect, &aDestRect, 0, nullptr, SDL_FLIP_NONE);
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, nullptr);

	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplBltMatrix(Image *theImage, float x, float y, const SexyMatrix3 &theMatrix,
								const Rect &theClipRect, const Color &theColor, int theDrawMode, const Rect &theSrcRect,
								bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture*)mSurface);
	mSDL3Renderer->SetLinearBlend_SDL(aTexture, blend);
	SDL_SetTextureBlendMode(aTexture,
							mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, &aClipRect);
	float halfWidth = theSrcRect.mWidth * 0.5f;
	float halfHeight = theSrcRect.mHeight * 0.5f;

	float x1 = -halfWidth;
	float y1 = -halfHeight;
	float x2 = x1 + theSrcRect.mWidth;
	float y2 = y1;
	float x3 = x1;
	float y3 = y1 + theSrcRect.mHeight;
	float x4 = x2;
	float y4 = y3;

	float u1 = (float)theSrcRect.mX / theImage->mWidth;
	float v1 = (float)theSrcRect.mY / theImage->mHeight;
	float u2 = (float)(theSrcRect.mX + theSrcRect.mWidth) / theImage->mWidth;
	float v2 = (float)(theSrcRect.mY + theSrcRect.mHeight) / theImage->mHeight;

	SDL_FColor aColor = {theColor.GetRed() / 255.0f, theColor.GetGreen() / 255.0f, theColor.GetBlue() / 255.0f,
						 theColor.GetAlpha() / 255.0f};

	SDL_Vertex vertices[4] = {
		{mSDL3Renderer->TransformToPoint(x1, y1, theMatrix, x, y), aColor, {u1, v1}}, // TL
		{mSDL3Renderer->TransformToPoint(x2, y2, theMatrix, x, y), aColor, {u2, v1}},	 // TR
		{mSDL3Renderer->TransformToPoint(x3, y3, theMatrix, x, y), aColor, {u1, v2}},	 // BL
		{mSDL3Renderer->TransformToPoint(x4, y4, theMatrix, x, y), aColor, {u2, v2}}	 // BR
	};

	int indices[] = {0, 1, 2, 1, 3, 2};

	SDL_RenderGeometry(mSDL3Renderer->mBackendRenderer, aTexture, vertices, 4, indices, 6);
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, nullptr);
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplBltTrianglesTex(Image *theTexture, const TriVertex theVertices[][3], int theNumTriangles,
									  const Rect &theClipRect, const Color &theColor, int theDrawMode, float tx,
									  float ty, bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theTexture;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture *)mSurface);
	mSDL3Renderer->SetLinearBlend_SDL(aTexture, blend);
	SDL_SetTextureBlendMode(aTexture, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));

	SDL_FColor aColor = {theColor.GetRed() / 255.0f, theColor.GetGreen() / 255.0f, theColor.GetBlue() / 255.0f,
						 theColor.GetAlpha() / 255.0f};

	int indices[] = {0, 1, 2};

	for (int aTriangleIdx = 0; aTriangleIdx < theNumTriangles; aTriangleIdx++)
	{
		const float aConvertBackToCorectRange = 1.0f / 255.0f;
		TriVertex theCurrentVertex[3];
		theCurrentVertex[0] = theVertices[aTriangleIdx][0];
		theCurrentVertex[1] = theVertices[aTriangleIdx][1];
		theCurrentVertex[2] = theVertices[aTriangleIdx][2];

		SDL_FColor anExtractedColor[3];
		SDL_Vertex vertices[3];

		for (int aVert = 0; aVert < 3; aVert++)
		{

			anExtractedColor[aVert].r =
				((theCurrentVertex[aVert].color >> 16) & 0xFF) * aColor.r * aConvertBackToCorectRange;
			anExtractedColor[aVert].g =
				((theCurrentVertex[aVert].color >> 8) & 0xFF) * aColor.g * aConvertBackToCorectRange;
			anExtractedColor[aVert].b = (theCurrentVertex[aVert].color & 0xFF) * aColor.b * aConvertBackToCorectRange;
			anExtractedColor[aVert].a =
				((theCurrentVertex[aVert].color >> 24) & 0xFF) * aColor.a * aConvertBackToCorectRange;
			vertices[aVert].position.x = theCurrentVertex[aVert].x + tx;
			vertices[aVert].position.y = theCurrentVertex[aVert].y + ty;
			vertices[aVert].tex_coord.x = theCurrentVertex[aVert].u;
			vertices[aVert].tex_coord.y = theCurrentVertex[aVert].v;
			vertices[aVert].color = anExtractedColor[aVert];
		}

		SDL_RenderGeometry(mSDL3Renderer->mBackendRenderer, aTexture, vertices, 3, nullptr, 3);
	}

	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, nullptr);
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplBltMirror(Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor,
								int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth,
						  (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {(float)theX, (float)theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture *)mSurface);

	mSDL3Renderer->SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));

	SDL_RenderTextureRotated(mSDL3Renderer->mBackendRenderer, aTexture, &aSrcRect, &aDestRect, 0, nullptr, SDL_FLIP_HORIZONTAL);
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, nullptr);

	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplStretchBltMirror(Image *theImage, const Rect &theDestRectOrig, const Rect &theSrcRect,
									   const Rect &theClipRect, const Color &theColor, int theDrawMode,
									   bool fastStretch)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mSDL3Renderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();
	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {(float)theDestRectOrig.mX, (float)theDestRectOrig.mY, (float)theDestRectOrig.mWidth, (float)theDestRectOrig.mHeight};
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture *)mSurface);

	mSDL3Renderer->SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, &aClipRect);
	SDL_RenderTextureRotated(mSDL3Renderer->mBackendRenderer, aTexture, &aSrcRect, &aDestRect, 0, nullptr, SDL_FLIP_HORIZONTAL);
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, nullptr);

	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

void SDL3Image::ImplBltRawTexture(void *theTexture, int theTexWidth, int theTexHeight, const Rect &theDestRect,
									const Rect &theSrcRect, const Rect &theClipRect, const Color &theColor,
									int theDrawMode, bool fastStretch)
{
	PreTextureDraw();

	SDL_Texture *aTexture = (SDL_Texture *)theTexture;
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth,
						  (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {(float)theDestRect.mX, (float)theDestRect.mY, (float)theDestRect.mWidth,
						   (float)theDestRect.mHeight};
	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, (SDL_Texture *)mSurface);

	mSDL3Renderer->SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, mSDL3Renderer->Get_SDL_NativeBlendMode(mSDL3Renderer->ChooseBlendMode(theDrawMode)));
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, &aClipRect);
	SDL_RenderTextureRotated(mSDL3Renderer->mBackendRenderer, aTexture, &aSrcRect, &aDestRect, 0, nullptr, SDL_FLIP_NONE);
	SDL_SetRenderClipRect(mSDL3Renderer->mBackendRenderer, nullptr);

	SDL_SetRenderTarget(mSDL3Renderer->mBackendRenderer, nullptr);
}

#endif