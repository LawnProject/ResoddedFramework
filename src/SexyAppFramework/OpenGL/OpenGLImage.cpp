#if SEXY_USE_OPENGL

#include "OpenGLImage.h"
#include "OpenGLRenderer.h"
#include "Rect.h"
#include "Graphics.h"
#include "SexyAppBase.h"
#include "AutoCrit.h"
#include "Debug.h"
#include "PerfTimer.h"
#include <glm/gtc/matrix_transform.hpp>

#pragma warning(disable : 4005) // macro redefinition
#pragma warning(disable : 4244) // conversion possible loss of data

GLuint Sexy::OpenGLImage::gOpenGLImageVAO = 0;
GLuint Sexy::OpenGLImage::gOpenGLImageVBO = 0;

using namespace Sexy;

OpenGLImage::OpenGLImage(Renderer *theRenderer) : GPUImage(theRenderer)
{
	mGLRenderer = (OpenGLRenderer *)theRenderer;
	Init();
}

OpenGLImage::OpenGLImage() : GPUImage(gSexyAppBase->mRenderer)
{
	mGLRenderer = (OpenGLRenderer*)gSexyAppBase->mRenderer;
	Init();
}

OpenGLImage::~OpenGLImage()
{
	if (mTexID != 0)
	{
		glDeleteTextures(1, &mTexID);
		OpenGLRenderer::gGLTextureCount--;
	}
		
	if (mFBO != 0)
		glDeleteFramebuffers(1, &mFBO);
}

uint32_t *OpenGLImage::GetBits()
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

		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		glReadBuffer(GL_COLOR_ATTACHMENT0); 
		glBindTexture(GL_TEXTURE_2D, mTexID);
		glReadPixels(0,
					0,
					mWidth,
					mHeight,
					GL_RGBA, GL_UNSIGNED_BYTE,
					mBits);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		UnlockSurface();
	}

	return mBits;
}

void OpenGLImage::DeleteSurface()
{
	if (mSurface != nullptr)
	{
		if ((mColorTable == NULL) && (mBits == NULL) && (mGPUData == NULL))
			GetBits();

		delete (GLuint *)mSurface;
		mSurface = nullptr;
	}
}

void OpenGLImage::BitsChanged()
{
	MemoryImage::BitsChanged();

	delete (GLuint *)mSurface;
	mSurface = nullptr;
}

bool OpenGLImage::GenerateSurface()
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
	
	glGenTextures(1, &mTexID);
	glBindTexture(GL_TEXTURE_2D, mTexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, mBits);
	glGenerateMipmap(GL_TEXTURE_2D);
	OpenGLRenderer::gGLTextureCount++;
	mSurface = new GLuint(mTexID);

	UnlockSurface();

	return true;
}

void OpenGLImage::SetSurface(void* theSurface) // this should be a texture id or something.
{
	mSurfaceSet = true;
	mSurface = theSurface;
	mTexID = *(GLuint *)theSurface;
	if (mTexID != 0)
	{
		glBindTexture(GL_TEXTURE_2D, mTexID);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, mBits);
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexID, 0);
		GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, drawBuffers);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	mNoLock = false;
}

bool OpenGLImage::LockSurface()
{
	if (mLockCount != 0)
		return true;

	mLockCount++;
	return true;
}

bool OpenGLImage::UnlockSurface()
{
	if (mLockCount == 0)
		return true;

	mLockCount--;
	return true;
}

void OpenGLImage::CreateImageBuffers()
{
	if (Check3D(this) || mFBO != 0)
		return;

	mProjection = glm::ortho(0.0f, (float)mWidth, 0.0f, (float)mHeight, -1.0f, 1.0f);
	glGenTextures(1, &mTexID);
	glBindTexture(GL_TEXTURE_2D, mTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	OpenGLRenderer::gGLTextureCount++;
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexID, 0);
	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

	mSurface = new GLuint(mTexID);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLImage::PreTextureDraw()
{
	CreateImageBuffers();

	glBindVertexArray(gOpenGLImageVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gOpenGLImageVBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glViewport(0, 0, mWidth, mHeight);

	glDisable(GL_SCISSOR_TEST);
}

void OpenGLImage::Init()
{
	mFBO = 0;
	mTexID = 0;
	if (gOpenGLImageVAO == 0 || gOpenGLImageVBO == 0)
	{
		glGenVertexArrays(1, &gOpenGLImageVAO);
		glGenBuffers(1, &gOpenGLImageVBO);

		glBindVertexArray(gOpenGLImageVAO);
		glBindBuffer(GL_ARRAY_BUFFER, gOpenGLImageVBO);

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
	}
}

void OpenGLImage::ImplFillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{

	PreTextureDraw();

	glm::vec2 p0 = {theRect.mX, theRect.mY};
	glm::vec2 p1 = {theRect.mX + theRect.mWidth, theRect.mY};
	glm::vec2 p2 = {theRect.mX + theRect.mWidth, theRect.mY + theRect.mHeight};
	glm::vec2 p3 = {theRect.mX, theRect.mY + theRect.mHeight};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, {0, 0}, aColor});
	aVertices.push_back({p1, {0, 0}, aColor});
	aVertices.push_back({p2, {0, 0}, aColor});
	aVertices.push_back({p2, {0, 0}, aColor});
	aVertices.push_back({p3, {0, 0}, aColor});
	aVertices.push_back({p0, {0, 0}, aColor});

	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture",0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void OpenGLImage::ImplDrawLine(
	double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode)
{
	PreTextureDraw();

	glm::vec2 p0 = {theStartX, theStartY};
	glm::vec2 p1 = {theEndX, theEndY};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, {}, aColor});
	aVertices.push_back({p1, {}, aColor});

	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_LINES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLImage::ImplDrawLineAA(
	double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode)
{
	glEnable(GL_MULTISAMPLE);
	ImplDrawLine(theStartY, theStartY, theEndX, theEndY, theColor, theDrawMode);
	glDisable(GL_MULTISAMPLE);
}

void OpenGLImage::ImplBlt(
	Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor, int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mGLRenderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mGLRenderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	int aTexID = static_cast<OpenGLTextureData *>(aImg->mGPUData)->GetTextureID();

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

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, uv0, aColor});
	aVertices.push_back({p1, uv1, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p3, uv3, aColor});
	aVertices.push_back({p0, uv0, aColor});

	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", (aTexID != 0));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void doScissorFromTLCauseOpenGL(int x, int y, int w, int h, int screenHeight)
{
	glScissor(x, screenHeight - h - y, w, h);
}
void OpenGLImage::ImplBltF(Image *theImage,
						float theX,
						float theY,
						const Rect &theSrcRect,
						const Rect &theClipRect,
						const Color &theColor,
						int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mGLRenderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mGLRenderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	if (theClipRect != Rect(0, 0, mWidth, mHeight))
	{
		glEnable(GL_SCISSOR_TEST);

		doScissorFromTLCauseOpenGL(theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight, mHeight);
	}
	else
		glDisable(GL_SCISSOR_TEST);

	int aTexID = static_cast<OpenGLTextureData *>(aImg->mGPUData)->GetTextureID();

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

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, uv0, aColor});
	aVertices.push_back({p1, uv1, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p3, uv3, aColor});
	aVertices.push_back({p0, uv0, aColor});

	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", (aTexID != 0));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void OpenGLImage::ImplBltRotated(Image *theImage,
							  float theX,
							  float theY,
							  const Rect &theSrcRect,
							  const Rect &theClipRect,
							  const Color &theColor,
							  int theDrawMode,
							  double theRot,
							  float theRotCenterX,
							  float theRotCenterY)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mGLRenderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mGLRenderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	if (theClipRect != Rect(0, 0, mWidth, mHeight))
	{
		glEnable(GL_SCISSOR_TEST);

		doScissorFromTLCauseOpenGL(theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight, mHeight);
	}
	else
		glDisable(GL_SCISSOR_TEST);

	int aTexID = static_cast<OpenGLTextureData *>(aImg->mGPUData)->GetTextureID();

	glm::vec2 p0 = {theX, theY};
	glm::vec2 p1 = {theX + theSrcRect.mWidth, theY};
	glm::vec2 p2 = {theX + theSrcRect.mWidth, theY + theSrcRect.mHeight};
	glm::vec2 p3 = {theX, theY + theSrcRect.mHeight};

	float radians = glm::radians(theRot);
	glm::vec2 center = {theRotCenterX + theX, theRotCenterY + theY};
	p0 = mGLRenderer->RotatePointAroundPivot(p0, center, radians);
	p1 = mGLRenderer->RotatePointAroundPivot(p1, center, radians);
	p2 = mGLRenderer->RotatePointAroundPivot(p2, center, radians);
	p3 = mGLRenderer->RotatePointAroundPivot(p3, center, radians);

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

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, uv0, aColor});
	aVertices.push_back({p1, uv1, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p3, uv3, aColor});
	aVertices.push_back({p0, uv0, aColor});

	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", (aTexID != 0));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void OpenGLImage::ImplStretchBlt(Image *theImage,
							  const Rect &theDestRect,
							  const Rect &theSrcRect,
							  const Rect &theClipRect,
							  const Color &theColor,
							  int theDrawMode,
							  bool fastStretch)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mGLRenderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mGLRenderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	if (theClipRect != Rect(0, 0, mWidth, mHeight))
	{
		glEnable(GL_SCISSOR_TEST);

		doScissorFromTLCauseOpenGL(theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight, mHeight);
	}
	else
		glDisable(GL_SCISSOR_TEST);

	int aTexID = static_cast<OpenGLTextureData *>(aImg->mGPUData)->GetTextureID();

	glm::vec2 p0 = {theDestRect.mX, theDestRect.mY};
	glm::vec2 p1 = {theDestRect.mX + theDestRect.mWidth, theDestRect.mY};
	glm::vec2 p2 = {theDestRect.mX + theDestRect.mWidth, theDestRect.mY + theDestRect.mHeight};
	glm::vec2 p3 = {theDestRect.mX, theDestRect.mY + theDestRect.mHeight};

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

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, uv0, aColor});
	aVertices.push_back({p1, uv1, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p3, uv3, aColor});
	aVertices.push_back({p0, uv0, aColor});

	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", (aTexID != 0));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void OpenGLImage::ImplBltMatrix(Image *theImage,
							 float x,
							 float y,
							 const SexyMatrix3 &theMatrix,
							 const Rect &theClipRect,
							 const Color &theColor,
							 int theDrawMode,
							 const Rect &theSrcRect,
							 bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mGLRenderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mGLRenderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	if (theClipRect != Rect(0, 0, mWidth, mHeight))
	{
		glEnable(GL_SCISSOR_TEST);

		doScissorFromTLCauseOpenGL(theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight, mHeight);
	}
	else
		glDisable(GL_SCISSOR_TEST);

	int aTexID = static_cast<OpenGLTextureData *>(aImg->mGPUData)->GetTextureID();

	float aWidth = static_cast<float>(theSrcRect.mWidth);
	float aHeight = static_cast<float>(theSrcRect.mHeight);

	glm::vec2 origin = {aWidth * 0.5f, aHeight * 0.5f}; //Matrix Draw calls on Images are centered by default

	glm::vec2 localP0 = {-origin.x, -origin.y};
	glm::vec2 localP1 = {aWidth - origin.x, -origin.y};
	glm::vec2 localP2 = {aWidth - origin.x, aHeight - origin.y};
	glm::vec2 localP3 = {-origin.x, aHeight - origin.y};

	glm::vec2 p0 = mGLRenderer->TransformToPoint(localP0.x, localP0.y, theMatrix, x, y);
	glm::vec2 p1 = mGLRenderer->TransformToPoint(localP1.x, localP1.y, theMatrix, x, y);
	glm::vec2 p2 = mGLRenderer->TransformToPoint(localP2.x, localP2.y, theMatrix, x, y);
	glm::vec2 p3 = mGLRenderer->TransformToPoint(localP3.x, localP3.y, theMatrix, x, y);

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

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, uv0, aColor});
	aVertices.push_back({p1, uv1, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p3, uv3, aColor});
	aVertices.push_back({p0, uv0, aColor});

	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", (aTexID != 0));

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, StrFormat("BLT_OPENGL_IMAGE_%s", theImage->mFilePath.c_str()).c_str());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glPopDebugGroup();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLImage::ImplBltTrianglesTex(Image *theTexture,
								   const TriVertex theVertices[][3],
								   int theNumTriangles,
								   const Rect &theClipRect,
								   const Color &theColor,
								   int theDrawMode,
								   float tx,
								   float ty,
								   bool blend)
{
	MemoryImage *aImg = (MemoryImage *)theTexture;

	if (!mGLRenderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mGLRenderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	std::vector<Vertex> aVertices;
	aVertices.reserve(theNumTriangles * 3);
	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};
	int aTexID = static_cast<OpenGLTextureData *>(aImg->mGPUData)->GetTextureID();

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

			aVertices.push_back({{tv.x, tv.y}, {tv.u, tv.v}, aVertColor});
		}
	}

	if (theClipRect != Rect(0, 0, mWidth, mHeight))
	{
		glEnable(GL_SCISSOR_TEST);

		doScissorFromTLCauseOpenGL(theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight, mHeight);
	}
	else
		glDisable(GL_SCISSOR_TEST);
	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", (aTexID != 0));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLImage::ImplBltMirror(
	Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor, int theDrawMode)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mGLRenderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mGLRenderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	int aTexID = static_cast<OpenGLTextureData *>(aImg->mGPUData)->GetTextureID();

	glm::vec2 p0 = {theX, theY};
	glm::vec2 p1 = {theX + theSrcRect.mWidth, theY};
	glm::vec2 p2 = {theX + theSrcRect.mWidth, theY + theSrcRect.mHeight};
	glm::vec2 p3 = {theX, theY + theSrcRect.mHeight};

	float u0 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theImage->mWidth;
	float v0 = (float)theSrcRect.mY / (float)theImage->mHeight;
	float u1 = (float)theSrcRect.mX / (float)theImage->mWidth;
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theImage->mHeight;

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, uv0, aColor});
	aVertices.push_back({p1, uv1, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p3, uv3, aColor});
	aVertices.push_back({p0, uv0, aColor});

	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", (aTexID != 0));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void OpenGLImage::ImplStretchBltMirror(Image *theImage,
									const Rect &theDestRectOrig,
									const Rect &theSrcRect,
									const Rect &theClipRect,
									const Color &theColor,
									int theDrawMode,
									bool fastStretch)
{
	MemoryImage *aImg = (MemoryImage *)theImage;

	if (!mGLRenderer->CreateImageTexture(aImg))
		return;

	PreTextureDraw();

	mGLRenderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);
	if (theClipRect != Rect(0, 0, mWidth, mHeight))
	{
		glEnable(GL_SCISSOR_TEST);

		doScissorFromTLCauseOpenGL(theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight, mHeight);
	}
	else
		glDisable(GL_SCISSOR_TEST);

	int aTexID = static_cast<OpenGLTextureData *>(aImg->mGPUData)->GetTextureID();

	glm::vec2 p0 = {theDestRectOrig.mX, theDestRectOrig.mY};
	glm::vec2 p1 = {theDestRectOrig.mX + theDestRectOrig.mWidth, theDestRectOrig.mY};
	glm::vec2 p2 = {theDestRectOrig.mX + theDestRectOrig.mWidth, theDestRectOrig.mY + theDestRectOrig.mHeight};
	glm::vec2 p3 = {theDestRectOrig.mX, theDestRectOrig.mY + theDestRectOrig.mHeight};

	float u0 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theImage->mWidth;
	float v0 = (float)theSrcRect.mY / (float)theImage->mHeight;
	float u1 = (float)theSrcRect.mX / (float)theImage->mWidth; 
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theImage->mHeight;

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, uv0, aColor});
	aVertices.push_back({p1, uv1, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p3, uv3, aColor});
	aVertices.push_back({p0, uv0, aColor});

	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", (aTexID != 0));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLImage::ImplBltRawTexture(void *theTexture,
								int theTexWidth,
								int theTexHeight,
								const Rect &theDestRect,
								const Rect &theSrcRect,
								const Rect &theClipRect,
								const Color &theColor,
								int theDrawMode,
								bool fastStretch)
{
	PreTextureDraw();

	mGLRenderer->ApplyBlendMode(mRenderer->ChooseBlendMode(theDrawMode));
	GLShader *aShaderToUse = mGLRenderer->mDefaultShader;
	aShaderToUse->SetUniform("uBlendMode", mRenderer->ChooseBlendMode(theDrawMode) - 1);

	if (theClipRect != Rect(0, 0, mWidth, mHeight))
	{
		glEnable(GL_SCISSOR_TEST);

		doScissorFromTLCauseOpenGL(theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight, mHeight);
	}
	else
		glDisable(GL_SCISSOR_TEST);

	int aTexID = *(GLuint *)theTexture;

	glm::vec2 p0 = {theDestRect.mX, theDestRect.mY};
	glm::vec2 p1 = {theDestRect.mX + theDestRect.mWidth, theDestRect.mY};
	glm::vec2 p2 = {theDestRect.mX + theDestRect.mWidth, theDestRect.mY + theDestRect.mHeight};
	glm::vec2 p3 = {theDestRect.mX, theDestRect.mY + theDestRect.mHeight};

	float u0 = (float)theSrcRect.mX / (float)theTexWidth;
	float v0 = (float)theSrcRect.mY / (float)theTexHeight;
	float u1 = (float)(theSrcRect.mX + theSrcRect.mWidth) / (float)theTexWidth;
	float v1 = (float)(theSrcRect.mY + theSrcRect.mHeight) / (float)theTexHeight;

	glm::vec2 uv0 = {u0, v0};
	glm::vec2 uv1 = {u1, v0};
	glm::vec2 uv2 = {u1, v1};
	glm::vec2 uv3 = {u0, v1};

	glm::vec4 aColor = {(float)theColor.mRed / 255.0f,
						(float)theColor.mGreen / 255.0f,
						(float)theColor.mBlue / 255.0f,
						(float)theColor.mAlpha / 255.0f};

	std::vector<Vertex> aVertices;

	aVertices.push_back({p0, uv0, aColor});
	aVertices.push_back({p1, uv1, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p2, uv2, aColor});
	aVertices.push_back({p3, uv3, aColor});
	aVertices.push_back({p0, uv0, aColor});

	aShaderToUse->Use();
	aShaderToUse->SetUniform("uProjection", mProjection);
	aShaderToUse->SetUniform("uUseTexture", (aTexID != 0));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(Vertex), aVertices.data());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)aVertices.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif