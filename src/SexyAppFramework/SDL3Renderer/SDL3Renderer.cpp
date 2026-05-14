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
	mTargetTexture = SDL_CreateTexture(mBackendRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mWidth, mHeight);
	if (mTargetTexture == nullptr)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Screen Texture-Buffer Creation Failed", SDL_GetError(), nullptr);
		return false;
	}

	SetVideoOnlyDraw(true);
	return false;
}

void SDL3Renderer::ApplyBlendMode(BlendMode mode)
{
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

	SDL_Rect clipRect =
		theClipRect
			? SDL_Rect{theClipRect->mX, theClipRect->mY, theClipRect->mWidth, theClipRect->mHeight}
			: SDL_Rect{mPresentationRect.mX, mPresentationRect.mY, mPresentationRect.mWidth, mPresentationRect.mHeight};

	SDL_SetRenderClipRect(mBackendRenderer, &clipRect);

	gRenderingPreDrawError = !SDL_RenderTexture(mBackendRenderer, mTargetTexture, nullptr, nullptr);

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
	SDL_Texture *aTex = SDL_CreateTexture(mBackendRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, theWidth, theHeight);

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

	int windowWidth, windowHeight;
	if (!SDL_GetWindowSize(mApp->mWindow->mInternalWindow, &windowWidth, &windowHeight))
		return;

	if (!SDL_SetRenderLogicalPresentation(mBackendRenderer, windowWidth, windowHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX))
		return;

	mPresentationRect = Rect(0, 0, windowWidth, windowHeight);
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


	int aWidth = theImage->GetWidth();
	int aHeight = theImage->GetHeight();
	if (createTexture)
	{
		mPixelFormat = theImage->mGPUFlags & ImageFlag_UseA4R4G4B4 ? PixelFormat_A4R4G4B4 : PixelFormat_A8R8G8B8;
		SDL_PixelFormat aFmt =
			mPixelFormat == PixelFormat_A8R8G8B8 ? SDL_PIXELFORMAT_ARGB8888 : SDL_PIXELFORMAT_RGBA4444;
		mTexturePtr = SDL_CreateTexture((SDL_Renderer*)theRendererData, aFmt, SDL_TEXTUREACCESS_STREAMING, aWidth, aHeight);

		
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

void SDL3Renderer::Blt(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Color &theColor,
						 int theDrawMode, bool linearFilter)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

}

void SDL3Renderer::BltClipF(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Rect &theClipRect,
							  const Color &theColor, int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

}

void SDL3Renderer::BltMirror(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Color &theColor,
							   int theDrawMode, bool linearFilter)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

}

void SDL3Renderer::StretchBlt(Image *theImage, const Rect &theDestRect, const Rect &theSrcRect,
								const Rect &theClipRect, const Color &theColor, int theDrawMode, bool fastStretch,
								bool mirror)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

}

void SDL3Renderer::BltRotated(Image *theImage, float theX, float theY, const Rect &theClipRect, const Color &theColor,
								int theDrawMode, double theRot, float theRotCenterX, float theRotCenterY,
								const Rect &theSrcRect)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

}

void SDL3Renderer::BltTransformed(Image *theImage, const Rect &theClipRect, const Color &theColor, int theDrawMode,
									const Rect &theSrcRect, const SexyMatrix3 &theTransform, bool linearFilter,
									float theX, float theY, bool center)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

}

void SDL3Renderer::DrawLine(double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor,
							  int theDrawMode)
{

}

void SDL3Renderer::FillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{

}

void SDL3Renderer::DrawTriangle(const TriVertex &p1, const TriVertex &p2, const TriVertex &p3, const Color &theColor,
								  int theDrawMode)
{

}

void SDL3Renderer::DrawTriangleTex(const TriVertex &p1, const TriVertex &p2, const TriVertex &p3,
									 const Color &theColor, int theDrawMode, Image *theTexture, bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theTexture;

	if (!CreateImageTexture(aImg))
		return;

}

void SDL3Renderer::DrawTrianglesTex(const TriVertex theVertices[][3], int theNumTriangles, const Color &theColor,
									  int theDrawMode, Image *theTexture, float tx, float ty, bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theTexture;

	if (!CreateImageTexture(aImg))
		return;


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

}

bool SDL3Renderer::TestSDL3()
{
	int aNumRenderers = SDL_GetNumRenderDrivers();
	return aNumRenderers > 0;
}

#endif
