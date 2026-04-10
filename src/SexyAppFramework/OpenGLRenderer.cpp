#if SEXY_USE_OPENGL
#include "OpenGLRenderer.h"
#include "SexyAppBase.h"
#include "TriVertex.h"
#include "SexyMatrix.h"
#include "Window.h"
#include "AutoCrit.h"
#include "SysFont.h"
#include <SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace Sexy;

const char *gVertexShaderSrc = R"glsl(
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

const char *gFragmentShaderSrc = R"glsl(
#version 330 core

in vec2 vTexCoord;
in vec4 vColor;

uniform sampler2D uTexture;
uniform bool uUseTexture;

out vec4 FragColor;

void main() {
    if (uUseTexture)
        FragColor = texture(uTexture, vTexCoord) * vColor;
    else
        FragColor = vColor;
}
)glsl";

OpenGLRenderer::OpenGLRenderer(SexyAppBase *theApp) : Renderer(theApp)
{
	mRGBBits = 32;

	mRedBits = 8;
	mGreenBits = 8;
	mBlueBits = 8;

	mRedShift = 0;
	mGreenShift = 8;
	mBlueShift = 16;

	mRedMask = (0xFFU << mRedShift);
	mGreenMask = (0xFFU << mGreenShift);
	mBlueMask = (0xFFU << mBlueShift);
	mCurrentBackend = RenderingBackend::BACKEND_OPENGL;
}

OpenGLRenderer::~OpenGLRenderer()
{
	
}

bool OpenGLRenderer::Init()
{
	int aResult = true;

	if (mSceneBegun)
		Cleanup();

	aResult = InitGLContext() && InitBuffers();
	const SDL_DisplayMode *aMode = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(mApp->mWindow->mInternalWindow));
	mRefreshRate = aMode->refresh_rate;
	if (!mRefreshRate)
		mRefreshRate = 60;
	mMillisecondsPerFrame = 1000 / mRefreshRate;

	mSceneBegun = true;
	return aResult;
}

void OpenGLRenderer::Cleanup()
{
	mSceneBegun = false;

	Renderer::Cleanup();

	if (mDefaultShader)
		delete mDefaultShader;
	
	if (mScreenImage)
		delete (OpenGLImage*)mScreenImage;
	mScreenImage = nullptr;

	ImageSet::iterator anItr;
	for (anItr = mImageSet.begin(); anItr != mImageSet.end(); ++anItr)
	{
		MemoryImage *anImage = *anItr;

		delete (OpenGLTextureData *)anImage->mD3DData;
		anImage->mD3DData = nullptr;

		OpenGLImage *anNativeImage = dynamic_cast<OpenGLImage *>(anImage);
		if (anNativeImage != nullptr && anNativeImage->mFBO != 0) //Delete the FBO incase the renderer resets
		{
			anNativeImage->DeleteSurface();
			glDeleteTextures(1, &anNativeImage->mTexID);
			glDeleteFramebuffers(1, &anNativeImage->mFBO);
			anNativeImage->mFBO = 0;
		}
	}

	mImageSet.clear();

	std::set<SysFont*>::iterator anFontItr;
	for (anFontItr = mSysFonts.begin(); anFontItr != mSysFonts.end(); ++anFontItr)
	{
		SysFont *aFont = *anFontItr;

		aFont->Reinit();
	}

	mCommandBuffer.clear();

	SDL_GL_DestroyContext(mContext);

	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
	mVBO = 0;
	mVAO = 0;
	
	glDeleteSamplers(1, &mSamplers.mWrap);
	glDeleteSamplers(1, &mSamplers.mClamp);

	//Delete the buffers that OpenGLImage has
	glDeleteBuffers(1, &OpenGLImage::gOpenGLImageVBO);
	glDeleteVertexArrays(1, &OpenGLImage::gOpenGLImageVAO);
	OpenGLImage::gOpenGLImageVBO = 0;
	OpenGLImage::gOpenGLImageVAO = 0;
}

void OpenGLRenderer::SetVideoOnlyDraw(bool videoOnly)
{
	if (mScreenImage)
		delete (OpenGLImage*)mScreenImage;
	mScreenImage = nullptr;
	mScreenImage = new OpenGLImage(this);
	mScreenImage->Create(mWidth, mHeight);
	mScreenImage->mWidth = mWidth;
	mScreenImage->mHeight = mHeight;
	mScreenImage->SetImageMode(false, false);
}

void OpenGLRenderer::Remove3DData(MemoryImage *theImage)
{
	if (theImage->mD3DData != nullptr)
	{
		delete (OpenGLTextureData *)theImage->mD3DData;
		theImage->mD3DData = nullptr;

		AutoCrit aCrit(mCritSect); // Make images thread safe
		mImageSet.erase(theImage);
	}
}

bool OpenGLRenderer::PreDraw()
{
	return true;
}

bool OpenGLRenderer::InitGLContext()
{
	mContext = SDL_GL_CreateContext(mApp->mWindow->mInternalWindow);

	if (!gladLoadGL())
	{
		return false;
	}

	SDL_GL_MakeCurrent(mApp->mWindow->mInternalWindow, mContext);

	mDefaultShader = new GLShader();
	mDefaultShader->LoadFromSource(gVertexShaderSrc, gFragmentShaderSrc);

	SetVideoOnlyDraw(false);

	return true;
}

bool OpenGLRenderer::InitBuffers()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, mPos));

	// texcoord
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, mTexCoord));

	// color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, mColor));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenSamplers(1, &mSamplers.mWrap);
	glSamplerParameteri(mSamplers.mWrap, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(mSamplers.mWrap, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(mSamplers.mWrap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplers.mWrap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenSamplers(1, &mSamplers.mClamp);
	glSamplerParameteri(mSamplers.mClamp, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(mSamplers.mClamp, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(mSamplers.mClamp, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplers.mClamp, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}
bool gRenderingPreDrawError = false;
bool OpenGLRenderer::Redraw(Rect *theClipRect)
{

	if (mCommandBuffer.empty())
		return !gRenderingPreDrawError;

	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(mPresentationRect.mX, mPresentationRect.mY, mPresentationRect.mWidth, mPresentationRect.mHeight);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	mDefaultShader->Use();
	mDefaultShader->SetUniform("uProjection", mProjection);
	glActiveTexture(GL_TEXTURE0);
	for (const auto cmd : mCommandBuffer)
	{
		if (cmd.mVertices.size() > MAX_VERTICES)
			continue; // Add a warning

		ApplyBlendMode(cmd.mBlendMode);

		glBindSampler(0, cmd.mUVWrapMode == UV_WRAP ? mSamplers.mWrap : mSamplers.mClamp);
		if (cmd.mHasClipRect)
		{
			glEnable(GL_SCISSOR_TEST);
			float scaleX = (float)mPresentationRect.mWidth / mWidth;
			float scaleY = (float)mPresentationRect.mHeight / mHeight;

			// convert logical rect to window pixels for glScissor
			int scissorX = mPresentationRect.mX + (int)(cmd.mClipRect.mX * scaleX);
			int scissorY = mPresentationRect.mY + (int)((mHeight - (cmd.mClipRect.mY + cmd.mClipRect.mHeight)) * scaleY);
			int scissorW = (int)(cmd.mClipRect.mWidth * scaleX);
			int scissorH = (int)(cmd.mClipRect.mHeight * scaleY);
			glScissor(scissorX, scissorY, scissorW, scissorH);
		}
		else
			glDisable(GL_SCISSOR_TEST);

		mDefaultShader->SetUniform("uUseTexture", (cmd.mTextureID != 0));
		
		glBindTexture(GL_TEXTURE_2D, cmd.mTextureID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, cmd.mVertices.size() * sizeof(Vertex), cmd.mVertices.data());
		glDrawArrays(cmd.mPrimitiveType, 0, (GLsizei)cmd.mVertices.size());
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mCommandBuffer.clear();
	SDL_GL_SwapWindow(mApp->mWindow->mInternalWindow);

	return !gRenderingPreDrawError;
}

void OpenGLRenderer::ApplyBlendMode(BlendMode mode)
{
	auto it = blend_mode_funcs.find(mode);
	if (it == blend_mode_funcs.end())
		return;

	const auto &blend = it->second;

	if (blend.theEnableBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(blend.theSrc, blend.theDst);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

//yeaaa so you can't really get em. -Electr0Gunner

RenderingInfo OpenGLRenderer::GetRenderingInfo()
{
	RenderingInfo anInfo;
	anInfo.mFreeVideoMem = 0;
	anInfo.mTotalVideoMem = 0;
	anInfo.mNumTextures = 0;
	return anInfo;
}

void OpenGLRenderer::UpdateViewport()
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

	// Set the OpenGL viewport
	glViewport(vpX, vpY, vpW, vpH);

	mPresentationRect = Rect(vpX, vpY, vpW, vpH);

	mProjection = glm::ortho(0.0f, (float)mWidth, (float)mHeight, 0.0f, -1.0f, 1.0f) *
		glm::mat4(1.0f);
}

bool OpenGLRenderer::CreateImageTexture(MemoryImage *theImage)
{
	bool wantPurge = false;

	if (theImage->mD3DData == nullptr)
	{
		theImage->mD3DData = new OpenGLTextureData();

		// The actual purging was deferred
		wantPurge = theImage->mPurgeBits;

		AutoCrit aCrit(mCritSect); // Make images thread safe
		mImageSet.insert(theImage);
	}

	OpenGLTextureData *aData = static_cast<OpenGLTextureData *>(theImage->mD3DData);
	aData->CheckCreateTextures(theImage, nullptr); //We don't need extra variables when creating OpenGL Textures

	if (wantPurge)
		theImage->PurgeBits();

	return true;
}

bool OpenGLRenderer::RecoverBits(MemoryImage *theImage)
{
	if (theImage->mD3DData == nullptr)
		return false;

	OpenGLTextureData *aData = (OpenGLTextureData *)theImage->mD3DData;
	if (aData->mBitsChangedCount != theImage->mBitsChangedCount) // bits have changed since texture was created
		return false;

	// Reverse the process: copy texture data to the image
	uint32_t *aPixels = new uint32_t[aData->mWidth * aData->mHeight];
	GLuint aTexID = aData->GetTextureID();
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, aPixels);

	theImage->SetBits(aPixels, aData->mWidth, aData->mHeight);
	delete aPixels;

	return true;
}
uint32_t *OpenGLRenderer::GetBitsFromTexture(void *theTexture, int theWidth, int theHeight)
{
	uint32_t *aPixels = new uint32_t[theWidth * theHeight];
	GLuint aTexID = *(GLuint *)theTexture;
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, aPixels);

	return aPixels;
}

void OpenGLRenderer::DeleteTexture(void* theTexture)
{
	glDeleteTextures(1, (GLuint *)theTexture);
	delete (GLuint *)theTexture;
}

void *OpenGLRenderer::CreateTexture(void *thePixels, int theWidth, int theHeight, RawPixelFormat thePixelFormat, int theAlignment)
{
	GLuint aTexID;
	glGenTextures(1, &aTexID);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, theAlignment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, theWidth, theHeight, 0, thePixelFormat == RAW_FORMAT_R ? GL_RED : GL_BGRA, GL_UNSIGNED_BYTE, thePixels);

	GLuint *aTexPtr = new GLuint(aTexID);
	return aTexPtr;
}

// OpenGLTextureData

OpenGLTextureData::OpenGLTextureData()
{
	mTexID = 0;
	mSourceIsFBO = false;
}

void OpenGLTextureData::ReleaseTextures()
{
	if (mSourceIsFBO) //releasing is handled by the source
		return;
	if (mTexData != nullptr)
	{
		GLuint aTexID = GetTextureID();
		glDeleteTextures(1, &aTexID);
		delete (GLuint *)mTexData;
		mTexID = 0;
	}
		
	mTexData = nullptr;
}

GLuint OpenGLTextureData::GetTextureID()
{
	if (mTexID == 0)
	{
		if (mTexData == nullptr)
			return 0;
		GLuint aTexID = *(GLuint *)mTexData;
		mTexID = aTexID;
	}
	return mTexID;
}
	
void OpenGLTextureData::CreateTextures(MemoryImage* theImage, void* theRendererData)
{
	if (mSourceIsFBO)
		return;

	theImage->DeleteSWBuffers(); // we don't need the software buffers anymore
	theImage->CommitBits();

	bool createTexture = false;
	mSourceIsFBO = false;

	// only recreate the texture if the dimensions or image data have changed
	if (mWidth != theImage->mWidth || mHeight != theImage->mHeight || mBitsChangedCount != theImage->mBitsChangedCount)
	{
		ReleaseTextures();
		createTexture = true;
	}

	OpenGLImage *aNativeImage = dynamic_cast<OpenGLImage*>(theImage);
	if (aNativeImage != nullptr && aNativeImage->mTexID != 0 && aNativeImage->mFBO != 0)
	{
		createTexture = false;
		mTexID = aNativeImage->mTexID;
		mSourceIsFBO = true;
	}

	int aWidth = theImage->GetWidth();
	int aHeight = theImage->GetHeight();

	if (createTexture)
	{
		glGenTextures(1, &mTexID);
		glBindTexture(GL_TEXTURE_2D, mTexID);

		GLenum aTargetFormat = GL_RGBA8;
		if (theImage->mD3DFlags & ImageFlag_UseA4R4G4B4)
			aTargetFormat = GL_RGBA4;

		glTexImage2D(GL_TEXTURE_2D, 0, aTargetFormat, aWidth, aHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, theImage->GetBits());

	}
	else if (mBitsChangedCount != theImage->mBitsChangedCount && !mSourceIsFBO)
	{
		void *bits = theImage->GetBits();
		if (bits)
		{
			glGenTextures(1, &mTexID);
			glBindTexture(GL_TEXTURE_2D, mTexID);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, aWidth, aHeight, GL_RGBA8, GL_UNSIGNED_BYTE, bits);
		}
		else
		{
			//todo: log error
		}
	}

	mWidth = theImage->mWidth;
	mHeight = theImage->mHeight;
	mBitsChangedCount = theImage->mBitsChangedCount;
	if (mTexData != nullptr)
		delete mTexData;
	mTexData = new GLuint(mTexID);
}

void OpenGLTextureData::CheckCreateTextures(MemoryImage* theImage, void* theRendererData)
{
	if (GetTextureID() != 0)
	{
		if (mWidth != theImage->mWidth || mHeight != theImage->mHeight || mBitsChangedCount != theImage->mBitsChangedCount)
			CreateTextures(theImage, nullptr);
		return;
	}
	CreateTextures(theImage, nullptr);
}

//Rendering

void OpenGLRenderer::AddCommand(const GLDrawCommand &theCommand)
{
	mCommandBuffer.push_back(theCommand);
}

void OpenGLRenderer::Blt(Image *theImage,
						float theX,
						float theY,
						const Rect &theSrcRect,
						const Color &theColor,
						int theDrawMode,
						bool linearFilter)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	GLDrawCommand aCmd;
	aCmd.mTextureID = static_cast<OpenGLTextureData *>(aImg->mD3DData)->GetTextureID();
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec2 p0 = {theX, theY};
	glm::vec2 p1 = {theX + theSrcRect.mWidth, theY};
	glm::vec2 p2 = {theX + theSrcRect.mWidth, theY + theSrcRect.mHeight};
	glm::vec2 p3 = {theX, theY + theSrcRect.mHeight};

	float u0 = (float)theSrcRect.mX / (float)theImage->mWidth;
	float v0 = (float)theSrcRect.mY / (float)theImage->mHeight;
	float u1 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theImage->mWidth;
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theImage->mHeight;

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({p0, uv0, aColor});
	aCmd.mVertices.push_back({p1, uv1, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p3, uv3, aColor});
	aCmd.mVertices.push_back({p0, uv0, aColor});

	AddCommand(aCmd);
}

void OpenGLRenderer::BltClipF(Image *theImage,
							  float theX,
							  float theY,
							  const Rect &theSrcRect,
							  const Rect &theClipRect,
							  const Color &theColor,
							  int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	GLDrawCommand aCmd;
	if (theClipRect != mPresentationRect)
	{
		aCmd.mHasClipRect = true;
		aCmd.mClipRect = theClipRect;
	}

	aCmd.mTextureID = static_cast<OpenGLTextureData *>(aImg->mD3DData)->GetTextureID();
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec2 p0 = {theX, theY};
	glm::vec2 p1 = {theX + theSrcRect.mWidth, theY};
	glm::vec2 p2 = {theX + theSrcRect.mWidth, theY + theSrcRect.mHeight};
	glm::vec2 p3 = {theX, theY + theSrcRect.mHeight};

	float u0 = (float)theSrcRect.mX / (float)theImage->mWidth;
	float v0 = (float)theSrcRect.mY / (float)theImage->mHeight;
	float u1 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theImage->mWidth;
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theImage->mHeight;

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({p0, uv0, aColor});
	aCmd.mVertices.push_back({p1, uv1, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p3, uv3, aColor});
	aCmd.mVertices.push_back({p0, uv0, aColor});

	AddCommand(aCmd);
}

void OpenGLRenderer::BltMirror(Image *theImage,
							   float theX,
							   float theY,
							   const Rect &theSrcRect,
							   const Color &theColor,
							   int theDrawMode,
							   bool linearFilter)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	GLDrawCommand aCmd;
	aCmd.mTextureID = static_cast<OpenGLTextureData *>(aImg->mD3DData)->GetTextureID();
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec2 p0 = {theX, theY};
	glm::vec2 p1 = {theX + theSrcRect.mWidth, theY};
	glm::vec2 p2 = {theX + theSrcRect.mWidth, theY + theSrcRect.mHeight};
	glm::vec2 p3 = {theX, theY + theSrcRect.mHeight};

	float u0 = (float)theSrcRect.mX / (float)theImage->mWidth;
	float v0 = (float)theSrcRect.mY / (float)theImage->mHeight;
	float u1 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theImage->mWidth;
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theImage->mHeight;

	std::swap(u0, u1);

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({p0, uv0, aColor});
	aCmd.mVertices.push_back({p1, uv1, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p3, uv3, aColor});
	aCmd.mVertices.push_back({p0, uv0, aColor});

	AddCommand(aCmd);
}

void OpenGLRenderer::StretchBlt(Image *theImage,
								const Rect &theDestRect,
								const Rect &theSrcRect,
								const Rect &theClipRect,
								const Color &theColor,
								int theDrawMode,
								bool fastStretch,
								bool mirror)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	GLDrawCommand aCmd;
	if (theClipRect != mPresentationRect)
	{
		aCmd.mHasClipRect = true;
		aCmd.mClipRect = theClipRect;
	}
	aCmd.mTextureID = static_cast<OpenGLTextureData *>(aImg->mD3DData)->GetTextureID();
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec2 p0 = {theDestRect.mX, theDestRect.mY};
	glm::vec2 p1 = {theDestRect.mX + theDestRect.mWidth, theDestRect.mY};
	glm::vec2 p2 = {theDestRect.mX + theDestRect.mWidth, theDestRect.mY + theDestRect.mHeight};
	glm::vec2 p3 = {theDestRect.mX, theDestRect.mY + theDestRect.mHeight};

	float u0 = (float)theSrcRect.mX / (float)theImage->mWidth;
	float v0 = (float)theSrcRect.mY / (float)theImage->mHeight;
	float u1 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theImage->mWidth;
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theImage->mHeight;

	if (mirror)
	{
		std::swap(u0, u1);
	}

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({p0, uv0, aColor});
	aCmd.mVertices.push_back({p1, uv1, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p3, uv3, aColor});
	aCmd.mVertices.push_back({p0, uv0, aColor});

	AddCommand(aCmd);
}

void OpenGLRenderer::BltRotated(Image *theImage,
								float theX,
								float theY,
								const Rect &theClipRect,
								const Color &theColor,
								int theDrawMode,
								double theRot,
								float theRotCenterX,
								float theRotCenterY,
								const Rect &theSrcRect)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	GLDrawCommand aCmd;
	if (theClipRect != mPresentationRect)
	{
		aCmd.mHasClipRect = true;
		aCmd.mClipRect = theClipRect;
	}
	aCmd.mTextureID = static_cast<OpenGLTextureData *>(aImg->mD3DData)->GetTextureID();
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec2 p0 = {theX, theY};
	glm::vec2 p1 = {theX + theSrcRect.mWidth, theY};
	glm::vec2 p2 = {theX + theSrcRect.mWidth, theY + theSrcRect.mHeight};
	glm::vec2 p3 = {theX, theY + theSrcRect.mHeight};

	float radians = glm::radians(theRot);
	glm::vec2 center = {theRotCenterX + theX, theRotCenterY + theY};
	p0 = RotatePointAroundPivot(p0, center, radians);
	p1 = RotatePointAroundPivot(p1, center, radians);
	p2 = RotatePointAroundPivot(p2, center, radians);
	p3 = RotatePointAroundPivot(p3, center, radians);

	float u0 = (float)theSrcRect.mX / (float)theImage->mWidth;
	float v0 = (float)theSrcRect.mY / (float)theImage->mHeight;
	float u1 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theImage->mWidth;
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theImage->mHeight;

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({p0, uv0, aColor});
	aCmd.mVertices.push_back({p1, uv1, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p3, uv3, aColor});
	aCmd.mVertices.push_back({p0, uv0, aColor});

	AddCommand(aCmd);
}

void OpenGLRenderer::BltTransformed(Image *theImage,
									const Rect &theClipRect,
									const Color &theColor,
									int theDrawMode,
									const Rect &theSrcRect,
									const SexyMatrix3 &theTransform,
									bool linearFilter,
									float theX,
									float theY,
									bool center)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!CreateImageTexture(aImg))
		return;

	GLDrawCommand aCmd;
	if (theClipRect != mPresentationRect)
	{
		aCmd.mHasClipRect = true;
		aCmd.mClipRect = theClipRect;
	}
	aCmd.mTextureID = static_cast<OpenGLTextureData *>(aImg->mD3DData)->GetTextureID();
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	float aWidth = static_cast<float>(theSrcRect.mWidth);
	float aHeight = static_cast<float>(theSrcRect.mHeight);

	glm::vec2 origin = {0.0f, 0.0f};
	if (center)
		origin = {aWidth * 0.5f, aHeight * 0.5f};

	glm::vec2 localP0 = {-origin.x, -origin.y};
	glm::vec2 localP1 = {aWidth - origin.x, -origin.y};
	glm::vec2 localP2 = {aWidth - origin.x, aHeight - origin.y};
	glm::vec2 localP3 = {-origin.x, aHeight - origin.y};

	glm::vec2 p0 = TransformToPoint(localP0.x, localP0.y, theTransform, theX, theY);
	glm::vec2 p1 = TransformToPoint(localP1.x, localP1.y, theTransform, theX, theY);
	glm::vec2 p2 = TransformToPoint(localP2.x, localP2.y, theTransform, theX, theY);
	glm::vec2 p3 = TransformToPoint(localP3.x, localP3.y, theTransform, theX, theY);

	float u0 = (float)theSrcRect.mX / (float)theImage->mWidth;
	float v0 = (float)theSrcRect.mY / (float)theImage->mHeight;
	float u1 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theImage->mWidth;
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theImage->mHeight;

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({p0, uv0, aColor});
	aCmd.mVertices.push_back({p1, uv1, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p3, uv3, aColor});
	aCmd.mVertices.push_back({p0, uv0, aColor});

	AddCommand(aCmd);
}

void OpenGLRenderer::DrawLine(
	double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode)
{
	GLDrawCommand aCmd;
	aCmd.mPrimitiveType = GL_LINES;
	aCmd.mTextureID = 0;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	glm::vec4 color =
		glm::vec4(theColor.mRed / 255.0f, theColor.mGreen / 255.0f, theColor.mBlue / 255.0f, theColor.mAlpha / 255.0f);
	aCmd.mVertices.push_back({{theStartX, theStartY}, {}, color});
	aCmd.mVertices.push_back({{theEndX, theEndY}, {}, color});

	AddCommand(aCmd);
}

void OpenGLRenderer::FillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{
	GLDrawCommand aCmd;
	aCmd.mTextureID = 0;
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec2 p0 = {theRect.mX, theRect.mY};
	glm::vec2 p1 = {theRect.mX + theRect.mWidth, theRect.mY};
	glm::vec2 p2 = {theRect.mX + theRect.mWidth, theRect.mY + theRect.mHeight};
	glm::vec2 p3 = {theRect.mX, theRect.mY + theRect.mHeight};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({p0, {}, aColor});
	aCmd.mVertices.push_back({p1, {}, aColor});
	aCmd.mVertices.push_back({p2, {}, aColor});
	aCmd.mVertices.push_back({p2, {}, aColor});
	aCmd.mVertices.push_back({p3, {}, aColor});
	aCmd.mVertices.push_back({p0, {}, aColor});

	AddCommand(aCmd);
}

void OpenGLRenderer::DrawTriangle(
	const TriVertex &p1, const TriVertex &p2, const TriVertex &p3, const Color &theColor, int theDrawMode)
{
	GLDrawCommand aCmd;
	aCmd.mTextureID = 0;
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec2 vert0 = {p1.x, p1.y};
	glm::vec2 vert1 = {p2.x, p2.y};
	glm::vec2 vert2 = {p3.x, p3.y};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({vert0, {p1.u, p1.v}, aColor});
	aCmd.mVertices.push_back({vert1, {p2.u, p2.v}, aColor});
	aCmd.mVertices.push_back({vert2, {p3.u, p3.v}, aColor});

	AddCommand(aCmd);
}

void OpenGLRenderer::DrawTriangleTex(const TriVertex &p1,
									 const TriVertex &p2,
									 const TriVertex &p3,
									 const Color &theColor,
									 int theDrawMode,
									 Image *theTexture,
									 bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theTexture;

	if (!CreateImageTexture(aImg))
		return;

	GLDrawCommand aCmd;
	aCmd.mTextureID = static_cast<OpenGLTextureData *>(aImg->mD3DData)->GetTextureID();
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec2 vert0 = {p1.x, p1.y};
	glm::vec2 vert1 = {p2.x, p2.y};
	glm::vec2 vert2 = {p3.x, p3.y};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({vert0, {p1.u, p1.v}, aColor});
	aCmd.mVertices.push_back({vert1, {p2.u, p2.v}, aColor});
	aCmd.mVertices.push_back({vert2, {p3.u, p3.v}, aColor});

	AddCommand(aCmd);
}

void OpenGLRenderer::DrawTrianglesTex(const TriVertex theVertices[][3],
								  int theNumTriangles,
								  const Color &theColor,
								  int theDrawMode,
								  Image *theTexture,
								  float tx,
								  float ty,
								  bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theTexture;

	if (!CreateImageTexture(aImg))
		return;

	GLDrawCommand aCmd;
	aCmd.mTextureID = static_cast<OpenGLTextureData *>(aImg->mD3DData)->GetTextureID();
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.reserve(theNumTriangles * 3);

    for (int i = 0; i < theNumTriangles; i++)
	{
		for (int v = 0; v < 3; v++)
		{
			TriVertex tv = theVertices[i][v];
			tv.x += tx;
			tv.y += ty;

			float vertexA = ((tv.color >> 24) & 0xFF) / 255.0f;
			float vertexR = ((tv.color >> 16) & 0xFF) / 255.0f;
			float vertexG = ((tv.color >> 8) & 0xFF) / 255.0f;
			float vertexB = ((tv.color) & 0xFF) / 255.0f;

			glm::vec4 aVertColor;
			aVertColor.r = aColor.r * vertexR;
			aVertColor.g = aColor.g * vertexG;
			aVertColor.b = aColor.b * vertexB;
			aVertColor.a = aColor.a * vertexA;

			aCmd.mVertices.push_back({{tv.x, tv.y}, {tv.u, tv.v}, aVertColor});
		}
	}

	AddCommand(aCmd);
}

void OpenGLRenderer::DrawTrianglesTexStrip(const TriVertex theVertices[],
									   int theNumTriangles,
									   const Color &theColor,
									   int theDrawMode,
									   Image *theTexture,
									   float tx,
									   float ty,
									   bool blend)
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

void OpenGLRenderer::FillPoly(const Point theVertices[],
							int theNumVertices,
							const Rect &theClipRect,
							const Color &theColor,
							int theDrawMode,
							int tx,
							int ty)
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

void OpenGLRenderer::BltRawTexture(void *theTexture,
							  const Rect &theDestRect,
							  const Rect &theSrcRect,
							  const Rect &theClipRect,
							  const Color &theColor,
							  int theDrawMode)
{
	GLuint aTextureID = *(GLuint *)theTexture;

	GLDrawCommand aCmd;
	aCmd.mTextureID = aTextureID;
	aCmd.mPrimitiveType = GL_TRIANGLES;
	aCmd.mBlendMode = ChooseBlendMode(theDrawMode);
	aCmd.mUVWrapMode = mCurrentUVWrapMode;

	glm::vec2 p0 = {theDestRect.mX, theDestRect.mY};
	glm::vec2 p1 = {theDestRect.mX + theDestRect.mWidth, theDestRect.mY};
	glm::vec2 p2 = {theDestRect.mX + theDestRect.mWidth, theDestRect.mY + theDestRect.mHeight};
	glm::vec2 p3 = {theDestRect.mX, theDestRect.mY + theDestRect.mHeight};

	float u0 = (float)theSrcRect.mX / (float)theSrcRect.mWidth;
	float v0 = (float)theSrcRect.mY / (float)theSrcRect.mHeight;
	float u1 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theSrcRect.mWidth;
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theSrcRect.mHeight;

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	aCmd.mVertices.push_back({p0, uv0, aColor});
	aCmd.mVertices.push_back({p1, uv1, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p2, uv2, aColor});
	aCmd.mVertices.push_back({p3, uv3, aColor});
	aCmd.mVertices.push_back({p0, uv0, aColor});

	AddCommand(aCmd);
}

#endif