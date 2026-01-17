#if SEXY_USE_OPENGL

#include "OpenGLImage.h"
#include "OpenGLRenderer.h"
#include "Rect.h"
#include "Graphics.h"
#include "SexyAppBase.h"
#include "AutoCrit.h"
#include "Debug.h"
#include "PerfTimer.h"

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
		glDeleteTextures(1, &mTexID);

	if (mFBO != 0)
		glDeleteFramebuffers(1, &mFBO);
}

ulong *OpenGLImage::GetBits()
{
	if (mBits == nullptr)
	{
		if (mSurface == nullptr)
			return MemoryImage::GetBits();

		if (mNoLock)
			return nullptr;

		if (!LockSurface())
			return nullptr;

		mBits = new ulong[mWidth * mHeight + 1];
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
		if ((mColorTable == NULL) && (mBits == NULL) && (mD3DData == NULL))
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

	glGenTextures(1, &mTexID);
	glBindTexture(GL_TEXTURE_2D, mTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexID, 0);
	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

	mSurface = new GLuint(mTexID);

	glClear(GL_COLOR_BUFFER_BIT);
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

}

void OpenGLImage::ImplDrawLine(
	double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode)
{

}

void OpenGLImage::ImplDrawLineAA(
	double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode)
{

}

void OpenGLImage::ImplBlt(
	Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor, int theDrawMode)
{

}

void OpenGLImage::ImplBltF(Image *theImage,
						float theX,
						float theY,
						const Rect &theSrcRect,
						const Rect &theClipRect,
						const Color &theColor,
						int theDrawMode)
{

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

}

void OpenGLImage::ImplStretchBlt(Image *theImage,
							  const Rect &theDestRect,
							  const Rect &theSrcRect,
							  const Rect &theClipRect,
							  const Color &theColor,
							  int theDrawMode,
							  bool fastStretch)
{

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

}

void OpenGLImage::ImplBltMirror(
	Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor, int theDrawMode)
{

}


void OpenGLImage::ImplStretchBltMirror(Image *theImage,
									const Rect &theDestRectOrig,
									const Rect &theSrcRect,
									const Rect &theClipRect,
									const Color &theColor,
									int theDrawMode,
									bool fastStretch)
{

}

#endif