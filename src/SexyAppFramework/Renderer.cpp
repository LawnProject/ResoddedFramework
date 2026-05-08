#include "Renderer.h"
#include "SexyAppBase.h"
#include "Graphics.h"
#include "AutoCrit.h"

using namespace Sexy;

std::string Renderer::mErrorString;

Renderer::Renderer(SexyAppBase* theApp) : mApp(theApp)
{
	mWidth = mApp->mWidth;
	mHeight = mApp->mHeight;
	mPresentationRect = Rect(0, 0, mWidth, mHeight);
	mScreenImage = nullptr;
	mSceneBegun = false;
	mTriedToSetVSync = false;
	mMillisecondsPerFrame = 0;
	mRefreshRate = 0;

	mRedBits = 8;
	mGreenBits = 8;
	mBlueBits = 8;

	mRedShift = 0;
	mGreenShift = 8;
	mBlueShift = 16;

	mRedMask = (0xFFU << mRedShift);
	mGreenMask = (0xFFU << mGreenShift);
	mBlueMask = (0xFFU << mBlueShift);
	mCurrentUVWrapMode = UV_CLAMP;
	mCurrentBackend = RenderingBackend::BACKEND_NONE;
}

Renderer::~Renderer()
{
	Cleanup();
}

void Renderer::Cleanup()
{

}

void Renderer::PushTransform(const SexyMatrix3& theTransform, bool concatenate)
{
	if (mTransformStack.empty() || !concatenate)
		mTransformStack.push_back(theTransform);
	else
	{
		SexyMatrix3 &aTrans = mTransformStack.back();
		mTransformStack.push_back(theTransform * aTrans);
	}
}

void Renderer::PopTransform()
{
	if (!mTransformStack.empty())
		mTransformStack.pop_back();
}

BlendMode Renderer::ChooseBlendMode(int theBlendMode)
{
	BlendMode aBlendMode;
	switch (theBlendMode)
	{
	case Graphics::DRAWMODE_ADDITIVE:
		aBlendMode = BLENDMODE_ADD;
		break;
	case Graphics::DRAWMODE_MULTIPLY:
		aBlendMode = BLENDMODE_MULTIPLY;
		break;
	case Graphics::DRAWMODE_NORMAL:
	default:

		aBlendMode = BLENDMODE_BLEND;
		break;
	}
	return aBlendMode;
}

void Renderer::AddImage(Image *theImage)
{
	AutoCrit anAutoCrit(mCritSect);

	mImageSet.insert((MemoryImage *)theImage);
}

void Renderer::RemoveImage(Image *theImage)
{
	AutoCrit anAutoCrit(mCritSect);

	ImageSet::iterator anItr = mImageSet.find((MemoryImage *)theImage);
	if (anItr != mImageSet.end())
		mImageSet.erase(anItr);
}

TextureData::TextureData()
{
	mWidth = 0;
	mHeight = 0;
	mBitsChangedCount = 0;
	mTexMemSize = 0;
	mTexData = nullptr;
}

TextureData::~TextureData()
{
	ReleaseTextures();
}