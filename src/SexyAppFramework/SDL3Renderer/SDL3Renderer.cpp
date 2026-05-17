#if SEXY_USE_SDL3_RENDERER

#include "SDL3Renderer.h"
#include "SysFont.h"
#include "TriVertex.h"
#include "SexyMatrix.h"
#include "AutoCrit.h"
#include "SexyAppBase.h"
#include "Window.h"
#if SEXY_USE_IMGUI
#include "ImGui/ImGuiManager.h"
#endif
#include <SDL3/SDL_messagebox.h>

using namespace Sexy;

int SDL3Renderer::gSDLTextureCount = 0;
uint64_t SDL3Renderer::gSDLUsedMemoryCount = 0;

SDL3Renderer::SDL3Renderer(SexyAppBase *theApp) : Renderer(theApp)
{
	mCurrentBackend = RenderingBackend::BACKEND_SDL3;
	mTargetTexture = nullptr;
	mBackendRenderer = nullptr;
}

SDL3Renderer::~SDL3Renderer()
{
}

bool SDL3Renderer::InitRendererObject()
{
	mBackendRenderer = SDL_CreateRenderer(mApp->mWindow->mInternalWindow, nullptr);
	if (mBackendRenderer == nullptr)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Renderer Creation Failed", SDL_GetError(), nullptr);
		return false;
	}
	SDL_SetRenderLogicalPresentation(mBackendRenderer, mWidth, mHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	mTargetTexture = SDL_CreateTexture(mBackendRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mWidth, mHeight);
	if (mTargetTexture == nullptr)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Screen Texture-Buffer Creation Failed", SDL_GetError(), nullptr);
		return false;
	}

	SetVideoOnlyDraw(true);
	return true;
}

void SDL3Renderer::ApplyBlendMode(BlendMode mode)
{
}

void SDL3Renderer::SetLinearBlend_SDL(SDL_Texture *theTexture, bool linearBlend)
{
	SDL_SetTextureScaleMode(theTexture, linearBlend ? SDL_ScaleMode::SDL_SCALEMODE_LINEAR : SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
}

uint32_t *SDL3Renderer::CaptureFrameBuffer()
{
	SDL_Surface *surface = SDL_RenderReadPixels(mBackendRenderer, nullptr);
	if (!surface)
		return nullptr;

	const int w = surface->w;
	const int h = surface->h;
	const int pitch = surface->pitch / sizeof(uint32_t);

	uint32_t *thePixels = new uint32_t[w * h];

    for (int y = 0; y < h; ++y)
	{
		const uint32_t *srcRow = (uint32_t *)surface->pixels + y * pitch;
		uint32_t *dstRow = thePixels + y * w;
		std::memcpy(dstRow, srcRow, w * sizeof(uint32_t));
	}

	SDL_DestroySurface(surface);
	return thePixels; // caller owns this, must delete[]
}

void SDL3Renderer::Cleanup()
{
	mSceneBegun = false;

	Renderer::Cleanup();

	if (mScreenImage)
		delete (SDL3Image *)mScreenImage;
	mScreenImage = nullptr;

	ImageSet::iterator anItr;
	for (anItr = mImageSet.begin(); anItr != mImageSet.end(); ++anItr)
	{
		MemoryImage *anImage = *anItr;

		delete (SDL3TextureData *)anImage->mGPUData;
		anImage->mGPUData = nullptr;

		SDL3Image *anNativeImage = dynamic_cast<SDL3Image *>(anImage);
		if (anNativeImage != nullptr) //Delete the FBO incase the renderer resets
		{
			anNativeImage->DeleteSurface();

		}
	}

	mImageSet.clear();

	std::set<SysFont *>::iterator anFontItr;
	for (anFontItr = mSysFonts.begin(); anFontItr != mSysFonts.end(); ++anFontItr)
	{
		SysFont *aFont = *anFontItr;

		aFont->Reinit();
	}

	if (mTargetTexture)
		SDL_DestroyTexture(mTargetTexture);

	if (mBackendRenderer)
		SDL_DestroyRenderer(mBackendRenderer);

	gSDLTextureCount = 0;
	gSDLUsedMemoryCount = 0;
}

bool SDL3Renderer::PreDraw()
{
	return true;
}

bool SDL3Renderer::Redraw(Rect *theClipRect)
{

	SDL_SetRenderTarget(mBackendRenderer, nullptr);

	SDL_SetRenderDrawColor(mBackendRenderer, 0, 0, 0, 0);
	SDL_SetTextureBlendMode(mTargetTexture, SDL_BLENDMODE_BLEND);
	SDL_RenderClear(mBackendRenderer);

	if (theClipRect)
	{
		SDL_Rect clipRect = SDL_Rect{theClipRect->mX, theClipRect->mY, theClipRect->mWidth, theClipRect->mHeight};

		SDL_SetRenderClipRect(mBackendRenderer, &clipRect);

	}

	gRenderingPreDrawError = !SDL_RenderTexture(mBackendRenderer, mTargetTexture, nullptr, nullptr);
	SDL_SetRenderClipRect(mBackendRenderer, nullptr);
#if SEXY_USE_IMGUI
	mApp->mImGuiManager->Flush();
#endif

	SDL_RenderPresent(mBackendRenderer);

	return !gRenderingPreDrawError;
}

void SDL3Renderer::SetVideoOnlyDraw(bool videoOnly)
{
	if (mScreenImage)
		delete (SDL3Image *)mScreenImage;
	mScreenImage = nullptr;
	mScreenImage = new SDL3Image(this);
	mScreenImage->Create(mWidth, mHeight);
	mScreenImage->mWidth = mWidth;
	mScreenImage->mHeight = mHeight;
	mScreenImage->SetImageMode(false, false);
	CreateImageTexture(mScreenImage);
	SDL3TextureData *aScreenImageData = (SDL3TextureData *)(mScreenImage->mGPUData);
	aScreenImageData->mTexturePtr = mTargetTexture;
}

void SDL3Renderer::Remove3DData(MemoryImage *theImage)
{
	if (theImage->mGPUData != nullptr)
	{
		delete (SDL3TextureData *)theImage->mGPUData;
		theImage->mGPUData = nullptr;

		AutoCrit aCrit(mCritSect); // Make images thread safe
		mImageSet.erase(theImage);
	}
}

void SDL3Renderer::DeleteTexture(void *theTexture)
{
	if (!theTexture)
		return;

	SDL_DestroyTexture((SDL_Texture *)theTexture);
	gSDLTextureCount--;
}

void *SDL3Renderer::CreateTexture(void *thePixels, int theWidth, int theHeight, RawPixelFormat thePixelFormat, int theAlignment)
{
	SDL_Texture *aTex = SDL_CreateTexture(mBackendRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, theWidth, theHeight);

	SDL_SetTextureScaleMode(aTex, SDL_SCALEMODE_LINEAR);
	SDL_UpdateTexture(aTex, nullptr, thePixels, theWidth * SDL_BYTESPERPIXEL(SDL_PIXELFORMAT_ARGB8888));
	gSDLTextureCount++;

	return aTex;
}

RenderingInfo SDL3Renderer::GetRenderingInfo()
{
	RenderingInfo anInfo;
	anInfo.mFreeVideoMem = -1;
	anInfo.mTotalVideoMem = -1;
	anInfo.mUsedVideoMemory = gSDLUsedMemoryCount;
	anInfo.mNumTextures = gSDLTextureCount;
	return anInfo;
}

void SDL3Renderer::UpdateViewport()
{
	if (SDL_GetCurrentThreadID() != SDL_GetThreadID(nullptr))
		return;

	int aWindowWidth, aWindowHeight;
	if (!SDL_GetWindowSize(mApp->mWindow->mInternalWindow, &aWindowWidth, &aWindowHeight))
		return;

	float windowAspect = (float)aWindowWidth / aWindowHeight;
	float logicalAspect = (float)mWidth / mHeight;

	int vpX, vpY, vpW, vpH;

	if (windowAspect > logicalAspect)
	{
		vpH = aWindowHeight;
		vpW = (int)(logicalAspect * vpH);
		vpX = (aWindowWidth - vpW) / 2;
		vpY = 0;
	}
	else
	{
		vpW = aWindowWidth;
		vpH = (int)(vpW / logicalAspect);
		vpX = 0;
		vpY = (aWindowHeight - vpH) / 2;
	}

	mPresentationRect = Rect(vpX, vpY, vpW, vpH);
}

bool SDL3Renderer::Init()
{
	int aResult = true;

	if (mSceneBegun)
		Cleanup();

	aResult = InitRendererObject();
	const SDL_DisplayMode *aMode = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(mApp->mWindow->mInternalWindow));
	mRefreshRate = aMode->refresh_rate;
	if (!mRefreshRate)
		mRefreshRate = 60;
	mMillisecondsPerFrame = 1000 / mRefreshRate;

	if (!mTriedToSetVSync)
	{
		if (!SDL_SetRenderVSync(mBackendRenderer, mApp->mWaitForVSync ? 1 : 0))
		{
			SDL_SetRenderVSync(mBackendRenderer, -1);
		}
	}
	int aVSync = 0;
	SDL_GetRenderVSync(mBackendRenderer , & aVSync);
	mApp->mWaitForVSync = aVSync != 0;
	mApp->mVSyncBroken = aVSync == 0;

	mSceneBegun = true;
	mTriedToSetVSync = true;
	return aResult;
}

RendererError SDL3Renderer::UpdateVSync()
{
	if (!SDL_SetRenderVSync(mBackendRenderer, mApp->mWaitForVSync ? 1 : 0))
	{
		return RendererError::ERROR_VSYNC;
	}
	return RendererError::ERROR_NONE;
}

bool SDL3Renderer::CreateImageTexture(MemoryImage *theImage)
{
	bool wantPurge = false;

	if (theImage->mGPUData == nullptr)
	{
		theImage->mGPUData = new SDL3TextureData();
		/* SDL3Image *aNative = (SDL3Image *)theImage;
		if (aNative)
		{
			aNative->mPlatformTextureData = (SDL3TextureData*)theImage->mGPUData;
		}*/

		// The actual purging was deferred
		wantPurge = theImage->mPurgeBits;

		AutoCrit aCrit(mCritSect); // Make images thread safe
		mImageSet.insert(theImage);
	}

	SDL3TextureData *aData = static_cast<SDL3TextureData *>(theImage->mGPUData);
	aData->CheckCreateTextures(theImage, mBackendRenderer);

	if (wantPurge)
		theImage->PurgeBits();

	return true;
}

bool SDL3Renderer::RecoverBits(MemoryImage *theImage)
{
	if (theImage->mGPUData == nullptr)
		return false;

	SDL3TextureData *aData = (SDL3TextureData *)theImage->mGPUData;
	if (aData->mBitsChangedCount != theImage->mBitsChangedCount) // bits have changed since texture was created
		return false;

	float aWidth;
	float aHeight;
	void *pixels;
	int pitch;

	if (SDL_LockTexture(aData->GetTexture(), nullptr, &pixels, &pitch) &&
		SDL_GetTextureSize(aData->GetTexture(), &aWidth, &aHeight))
	{
		theImage->SetBits((uint32_t *)pixels, (int)aWidth, (int)aHeight);
		SDL_UnlockTexture(aData->GetTexture());
	}

	return true;
}

uint32_t *SDL3Renderer::GetBitsFromTexture(void *theTexture, int theWidth, int theHeight)
{
	return nullptr;
}

SDL_Texture *SDL3TextureData::GetTexture()
{
	return mTexturePtr;
}

SDL3TextureData::SDL3TextureData()
{
	mTexturePtr = nullptr;
	mSourceIsTarget = false;
}

void SDL3TextureData::ReleaseTextures()
{
	SDL3Renderer::gSDLUsedMemoryCount -= mTexMemSize;
	mTexMemSize = 0;
	mWidth = 0;
	mHeight = 0;

	if (mSourceIsTarget) //releasing is handled by the source
		return;
	if (mTexData != nullptr)
	{
		SDL_DestroyTexture(mTexturePtr);
		mTexturePtr = nullptr;
	}

	mTexData = nullptr;
}

void SDL3TextureData::CreateTextures(MemoryImage *theImage, void *theRendererData)
{
	if (mSourceIsTarget)
		return;

	theImage->DeleteSWBuffers(); // we don't need the software buffers anymore
	theImage->CommitBits();

	bool createTexture = false;

	// only recreate the texture if the dimensions or image data have changed
	if (mWidth != theImage->mWidth || mHeight != theImage->mHeight || mBitsChangedCount != theImage->mBitsChangedCount)
	{
		ReleaseTextures();
		createTexture = true;
	}

	SDL3Image *aNativeImage = dynamic_cast<SDL3Image *>(theImage);
	if (aNativeImage != nullptr && aNativeImage->mSurface != nullptr)
	{
		createTexture = false;
		mTexturePtr = (SDL_Texture *)aNativeImage->mSurface;
		mSourceIsTarget = true;
	}


	int aWidth = theImage->GetWidth();
	int aHeight = theImage->GetHeight();
	if (createTexture)
	{
		mPixelFormat = theImage->mGPUFlags & ImageFlag_UseA4R4G4B4 ? PixelFormat_A4R4G4B4 : PixelFormat_A8R8G8B8;
		SDL_PixelFormat aFmt =
			mPixelFormat == PixelFormat_A8R8G8B8 ? SDL_PIXELFORMAT_ARGB8888 : SDL_PIXELFORMAT_RGBA4444;
		mTexturePtr = SDL_CreateTexture((SDL_Renderer*)theRendererData, aFmt, SDL_TEXTUREACCESS_TARGET, aWidth, aHeight);

		
		SDL_SetTextureScaleMode(mTexturePtr, SDL_SCALEMODE_LINEAR);
		SDL_UpdateTexture(mTexturePtr, nullptr, theImage->GetBits(), aWidth * SDL_BYTESPERPIXEL(aFmt));

		SDL3Renderer::gSDLTextureCount++;
	}
	else if (mBitsChangedCount != theImage->mBitsChangedCount)
	{
		SDL_UpdateTexture(mTexturePtr, nullptr, theImage->GetBits(),
						  aWidth * SDL_BYTESPERPIXEL(mPixelFormat == PixelFormat_A8R8G8B8 ? SDL_PIXELFORMAT_ARGB8888
																						  : SDL_PIXELFORMAT_RGBA4444));

	}

	mWidth = theImage->mWidth;
	mHeight = theImage->mHeight;
	mBitsChangedCount = theImage->mBitsChangedCount;
	if (mTexData != nullptr)
		delete mTexData;
	mTexData = mTexturePtr;
	mTexMemSize = mWidth * mHeight * 4; //Using ARGB
	SDL3Renderer::gSDLUsedMemoryCount += mTexMemSize;
}

void SDL3TextureData::CheckCreateTextures(MemoryImage *theImage, void *theRendererData)
{
	if (GetTexture())
	{
		if (mWidth != theImage->mWidth || mHeight != theImage->mHeight ||
			mBitsChangedCount != theImage->mBitsChangedCount)
			CreateTextures(theImage, theRendererData);
		return;
	}
	CreateTextures(theImage, theRendererData);
}

SDL_BlendMode SDL3Renderer::Get_SDL_NativeBlendMode(BlendMode theBlendMode)
{
	switch (theBlendMode)
	{
		case BlendMode::BLENDMODE_NORMAL:
		{
			return SDL_BLENDMODE_BLEND;
		}
		case BlendMode::BLENDMODE_ADD:
		{
			return SDL_BLENDMODE_ADD;
		}
		case BlendMode::BLENDMODE_MULTIPLY:
		{
			return SDL_BLENDMODE_MUL;
		}
	}
}

void SDL3Renderer::Blt(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Color &theColor,
						 int theDrawMode, bool linearFilter)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {(float)theX, (float)theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);

	SetLinearBlend_SDL(aTexture, linearFilter);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));
	SDL_RenderTexture(mBackendRenderer, aTexture, &aSrcRect, &aDestRect);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::BltClipF(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Rect &theClipRect,
							  const Color &theColor, int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {theX, theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);

	SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mBackendRenderer, &aClipRect);
	SDL_RenderTexture(mBackendRenderer, aTexture, &aSrcRect, &aDestRect);
	SDL_SetRenderClipRect(mBackendRenderer, nullptr);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::BltMirror(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Color &theColor,
							   int theDrawMode, bool linearFilter)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {theX, theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);

	SetLinearBlend_SDL(aTexture, linearFilter);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));

	SDL_RenderTextureRotated(mBackendRenderer, aTexture, &aSrcRect, &aDestRect, 0, nullptr, SDL_FLIP_HORIZONTAL);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::StretchBlt(Image *theImage, const Rect &theDestRect, const Rect &theSrcRect,
								const Rect &theClipRect, const Color &theColor, int theDrawMode, bool fastStretch,
								bool mirror)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {(float)theDestRect.mX, (float)theDestRect.mY, (float)theDestRect.mWidth, (float)theDestRect.mHeight};
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);

	SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mBackendRenderer, &aClipRect);
	SDL_RenderTextureRotated(mBackendRenderer, aTexture, &aSrcRect, &aDestRect, 0, nullptr, SDL_FLIP_NONE);
	SDL_SetRenderClipRect(mBackendRenderer, nullptr);

	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::BltRotated(Image *theImage, float theX, float theY, const Rect &theClipRect, const Color &theColor,
								int theDrawMode, double theRot, float theRotCenterX, float theRotCenterY,
								const Rect &theSrcRect)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);

	SDL_SetRenderDrawBlendMode(mBackendRenderer, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));
	SDL_SetRenderDrawColor(mBackendRenderer, theColor.mRed, theColor.mGreen, theColor.mBlue, theColor.mAlpha);

	SDL_FRect aDestRect = {theX, theY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth, (float)theSrcRect.mHeight};
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mBackendRenderer, &aClipRect);
	SDL_FPoint aRotCenter = {theRotCenterX, theRotCenterY};

	SDL_RenderTextureRotated(mBackendRenderer, aTexture, &aSrcRect, &aDestRect, theRot, &aRotCenter, SDL_FLIP_NONE);
	SDL_SetRenderClipRect(mBackendRenderer, nullptr);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::BltTransformed(Image *theImage, const Rect &theClipRect, const Color &theColor, int theDrawMode,
									const Rect &theSrcRect, const SexyMatrix3 &theTransform, bool linearFilter,
									float theX, float theY, bool center)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);
	SetLinearBlend_SDL(aTexture, linearFilter);
	SDL_SetTextureBlendMode(aTexture, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mBackendRenderer, &aClipRect);
	float halfWidth = theSrcRect.mWidth * 0.5f;
	float halfHeight = theSrcRect.mHeight * 0.5f;

	float x1 = center ? -halfWidth : 0;
	float y1 = center ? -halfHeight : 0;
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

	SDL_FColor aColor = {theColor.GetRed() / 255.0f, theColor.GetGreen() / 255.0f, theColor.GetBlue() / 255.0f,theColor.GetAlpha() / 255.0f};

	SDL_Vertex vertices[4] = {
		{TransformToPoint(x1, y1, theTransform, theX, theY), aColor, {u1, v1}},		// TL
		{TransformToPoint(x2, y2, theTransform, theX, theY), aColor, {u2, v1}},		// TR
		{TransformToPoint(x3, y3, theTransform, theX, theY), aColor, {u1, v2}},		// BL
		{TransformToPoint(x4, y4, theTransform, theX, theY), aColor, {u2, v2}}		// BR
	};

	int indices[] = {0, 1, 2, 1, 3, 2};

	SDL_RenderGeometry(mBackendRenderer, aTexture, vertices, 4, indices, 6);
	SDL_SetRenderClipRect(mBackendRenderer, nullptr);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::DrawLine(double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode)
{
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);

	SDL_SetRenderDrawBlendMode(mBackendRenderer, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));
	SDL_SetRenderDrawColor(mBackendRenderer, theColor.mRed, theColor.mGreen, theColor.mBlue, theColor.mAlpha);

	SDL_RenderLine(mBackendRenderer, theStartX, theStartY, theEndX, theEndY);

	SDL_SetRenderDrawBlendMode(mBackendRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::FillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);

	SDL_FRect aRect = {(float)theRect.mX, (float)theRect.mY, (float)theRect.mWidth, (float)theRect.mHeight};

	SDL_SetRenderDrawColor(mBackendRenderer, theColor.mRed, theColor.mGreen, theColor.mBlue, theColor.mAlpha);
	SDL_SetRenderDrawBlendMode(mBackendRenderer, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));
	SDL_RenderFillRect(mBackendRenderer, &aRect);

	SDL_SetRenderDrawBlendMode(mBackendRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::DrawTriangle(const TriVertex &p1, const TriVertex &p2, const TriVertex &p3, const Color &theColor,
								  int theDrawMode)
{
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);
	SDL_SetRenderDrawBlendMode(mBackendRenderer, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));

	SDL_FColor aColor = {theColor.GetRed() / 255.0f, theColor.GetGreen() / 255.0f, theColor.GetBlue() / 255.0f,
						 theColor.GetAlpha() / 255.0f};

	SDL_Vertex vertices[3] = {{SDL_FPoint{p1.x, p1.y}, aColor, {p1.u, p1.v}},
							  {SDL_FPoint{p2.x, p2.y}, aColor, {p2.u, p2.v}},
							  {SDL_FPoint{p3.x, p3.y}, aColor, {p3.u, p3.v}}};

	int indices[] = {0, 1, 2};

	SDL_RenderGeometry(mBackendRenderer, nullptr, vertices, 3, indices, 3);

	SDL_SetRenderClipRect(mBackendRenderer, nullptr);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
	SDL_SetRenderDrawBlendMode(mBackendRenderer, SDL_BLENDMODE_BLEND);
}

void SDL3Renderer::DrawTriangleTex(const TriVertex &p1, const TriVertex &p2, const TriVertex &p3,
									 const Color &theColor, int theDrawMode, Image *theTexture, bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theTexture;

	if (!CreateImageTexture(aImg))
		return;

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);
	SetLinearBlend_SDL(aTexture, blend);
	SDL_SetTextureBlendMode(aTexture, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));

	SDL_FColor aColor = {theColor.GetRed() / 255.0f, theColor.GetGreen() / 255.0f, theColor.GetBlue() / 255.0f,
						 theColor.GetAlpha() / 255.0f};

	SDL_Vertex vertices[3] = {{SDL_FPoint{p1.x, p1.y}, aColor, {p1.u, p1.v}},
							  {SDL_FPoint{p2.x, p2.y}, aColor, {p2.u, p2.v}},
							  {SDL_FPoint{p3.x, p3.y}, aColor, {p3.u, p3.v}}};

	int indices[] = {0, 1, 2};
	SDL_TextureAddressMode aMode = mCurrentUVWrapMode == UV_WRAP ? SDL_TEXTURE_ADDRESS_WRAP : SDL_TEXTURE_ADDRESS_CLAMP;
	SDL_SetRenderTextureAddressMode(mBackendRenderer, aMode, aMode);
	SDL_RenderGeometry(mBackendRenderer, aTexture, vertices, 3, indices, 3);
	SDL_SetRenderClipRect(mBackendRenderer, nullptr);
	SDL_SetRenderTextureAddressMode(mBackendRenderer, SDL_TEXTURE_ADDRESS_AUTO, SDL_TEXTURE_ADDRESS_AUTO);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::DrawTrianglesTex(const TriVertex theVertices[][3], int theNumTriangles, const Color &theColor,
									  int theDrawMode, Image *theTexture, float tx, float ty, bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theTexture;

	if (!CreateImageTexture(aImg))
		return;

	SDL_Texture *aTexture = static_cast<SDL3TextureData *>(aImg->mGPUData)->GetTexture();
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);
	SetLinearBlend_SDL(aTexture, blend);
	SDL_SetTextureBlendMode(aTexture, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));
	SDL_TextureAddressMode aMode = mCurrentUVWrapMode == UV_WRAP ? SDL_TEXTURE_ADDRESS_WRAP : SDL_TEXTURE_ADDRESS_CLAMP;
	SDL_SetRenderTextureAddressMode(mBackendRenderer, aMode, aMode);
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

			anExtractedColor[aVert].r = ((theCurrentVertex[aVert].color >> 16) & 0xFF) * aColor.r * aConvertBackToCorectRange;
			anExtractedColor[aVert].g = ((theCurrentVertex[aVert].color >> 8) & 0xFF) * aColor.g * aConvertBackToCorectRange;
			anExtractedColor[aVert].b = (theCurrentVertex[aVert].color & 0xFF) * aColor.b * aConvertBackToCorectRange;
			anExtractedColor[aVert].a = ((theCurrentVertex[aVert].color >> 24) & 0xFF) * aColor.a * aConvertBackToCorectRange;
			vertices[aVert].position.x = theCurrentVertex[aVert].x + tx;
			vertices[aVert].position.y = theCurrentVertex[aVert].y + ty;
			vertices[aVert].tex_coord.x = theCurrentVertex[aVert].u;
			vertices[aVert].tex_coord.y = theCurrentVertex[aVert].v;
			vertices[aVert].color = anExtractedColor[aVert];
		}

		SDL_RenderGeometry(mBackendRenderer, aTexture, vertices, 3, nullptr, 3);

	}
	SDL_SetRenderTextureAddressMode(mBackendRenderer, SDL_TEXTURE_ADDRESS_AUTO, SDL_TEXTURE_ADDRESS_AUTO);

	SDL_SetRenderClipRect(mBackendRenderer, nullptr);
	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

void SDL3Renderer::DrawTrianglesTexStrip(const TriVertex theVertices[], int theNumTriangles, const Color &theColor,
										   int theDrawMode, Image *theTexture, float tx, float ty, bool blend)
{
	TriVertex aList[100][3];
	int aTriNum = 0;
	while (aTriNum < theNumTriangles)
	{
		int aMaxTriangles = std::min(100, theNumTriangles - aTriNum);
		for (int i = 0; i < aMaxTriangles; i++)
		{
			aList[i][0] = theVertices[aTriNum + i];
			aList[i][1] = theVertices[aTriNum + i + 1];
			aList[i][2] = theVertices[aTriNum + i + 2];
		}
		DrawTrianglesTex(aList, aMaxTriangles, theColor, theDrawMode, theTexture, tx, ty, blend);
	}
}

void SDL3Renderer::FillPoly(const Point theVertices[], int theNumVertices, const Rect &theClipRect,
							  const Color &theColor, int theDrawMode, int tx, int ty)
{
	if (theNumVertices < 3)
		return;

	for (int i = 1; i < theNumVertices - 1; ++i)
	{
		TriVertex v0, v1, v2;

		v0.x = theVertices[0].mX + tx;
		v0.y = theVertices[0].mY + ty;

		v1.x = theVertices[i].mX + tx;
		v1.y = theVertices[i].mY + ty;

		v2.x = theVertices[i + 1].mX + tx;
		v2.y = theVertices[i + 1].mY + ty;

		DrawTriangle(v0, v1, v2, theColor, theDrawMode);
	}
}

void SDL3Renderer::BltRawTexture(void *theTexture, int theTexWidth, int theTexHeight, const Rect &theDestRect,
								   const Rect &theSrcRect, const Rect &theClipRect, const Color &theColor,
								   int theDrawMode)
{

	SDL_Texture *aTexture = (SDL_Texture *)theTexture;
	SDL_FRect aSrcRect = {(float)theSrcRect.mX, (float)theSrcRect.mY, (float)theSrcRect.mWidth,
						  (float)theSrcRect.mHeight};
	SDL_FRect aDestRect = {(float)theDestRect.mX, (float)theDestRect.mY, (float)theDestRect.mWidth,
						   (float)theDestRect.mHeight};
	SDL_SetRenderTarget(mBackendRenderer, mTargetTexture);

	SetLinearBlend_SDL(aTexture, true);
	SDL_SetTextureColorMod(aTexture, theColor.mRed, theColor.mGreen, theColor.mBlue);
	SDL_SetTextureAlphaMod(aTexture, theColor.mAlpha);
	SDL_SetTextureBlendMode(aTexture, Get_SDL_NativeBlendMode(ChooseBlendMode(theDrawMode)));
	SDL_Rect aClipRect = {theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight};
	SDL_SetRenderClipRect(mBackendRenderer, &aClipRect);
	SDL_RenderTextureRotated(mBackendRenderer, aTexture, &aSrcRect, &aDestRect, 0, nullptr, SDL_FLIP_NONE);
	SDL_SetRenderClipRect(mBackendRenderer, nullptr);

	SDL_SetRenderTarget(mBackendRenderer, nullptr);
}

bool SDL3Renderer::TestSDL3()
{
	int aNumRenderers = SDL_GetNumRenderDrivers();
	return aNumRenderers > 0;
}

#endif
