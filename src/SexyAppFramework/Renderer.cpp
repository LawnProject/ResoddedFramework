#include "Renderer.h"
#include "SexyAppBase.h"
#include "Graphics.h"

using namespace Sexy;

std::string Renderer::mErrorString;

const char *Renderer::gDefaultVertexShader = R"glsl(
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;
layout(location = 2) in vec4 aColor;

uniform mat4 uProjection;

out vec2 vTexCoord;
out vec4 vColor;

void main() {
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    vTexCoord = aTex;
    vColor = aColor;
}
)glsl";

const char *Renderer::gDefaultFragmentShader = R"glsl(
#version 330 core

in vec2 vTexCoord;
in vec4 vColor;

uniform sampler2D uTexture;
uniform bool uUseTexture;
uniform int uBlendMode;

out vec4 FragColor;

void main() {
	if (uBlendMode == 2) // Multiply
	{
		vec4 src = uUseTexture ? texture(uTexture, vTexCoord) * vColor : vColor;
		FragColor = vec4(src.rgb * src.a, src.a);
	}
	else
	{
		if (uUseTexture)
			FragColor = texture(uTexture, vTexCoord) * vColor;
		else
			FragColor = vColor;
	}

}
)glsl";

bool Renderer::gRenderingPreDrawError = false;

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

	mDefaultShader = nullptr;
	mCurrentShader = nullptr;
}

Renderer::~Renderer()
{
	Cleanup();
}

void Renderer::Cleanup()
{

}

void Renderer::DeleteShader(Shader *theShader)
{
	if (theShader)
	{
		if (theShader == mCurrentShader)
			mCurrentShader = nullptr;
		delete theShader;
	}
		
}

Shader* Renderer::GetCurrentShader()
{
	return mCurrentShader ? mCurrentShader : mDefaultShader;
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
		aBlendMode = BLENDMODE_NORMAL;
		break;
	}
	return aBlendMode;
}

void Renderer::AddImage(Image *theImage)
{
    auto lock = std::scoped_lock(mCritSect);
	mImageSet.insert((MemoryImage *)theImage);
}

void Renderer::RemoveImage(Image *theImage)
{
    auto lock = std::scoped_lock(mCritSect);

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
