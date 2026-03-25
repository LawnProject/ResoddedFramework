#include "gpuimage.h"
#include "SexyAppBase.h"
#include "Renderer.h"
#include "Graphics.h"
#include "Debug.h"

using namespace Sexy;

GPUImage::GPUImage() : GPUImage(gSexyAppBase->mRenderer)
{

}
GPUImage::GPUImage(Renderer *theRenderer) : MemoryImage(theRenderer->mApp)
{
	mRenderer = theRenderer;
	Init();
}
GPUImage::~GPUImage()
{
	mRenderer->RemoveImage(this);
}

void GPUImage::Init()
{
	mSurface = NULL;
	mRenderer->AddImage(this);

	mNoLock = false;
	mVideoMemory = false;
	mFirstPixelTrans = false;
	mWantSurface = false;
	mDrawToBits = false;
	mSurfaceSet = false;

	mLockCount = 0;
}
void GPUImage::FillScanLinesWithCoverage(Span *theSpans,
										 int theSpanCount,
										 const Color &theColor,
										 int theDrawMode,
										 const uint8_t *theCoverage,
										 int theCoverX,
										 int theCoverY,
										 int theCoverWidth,
										 int theCoverHeight)
{
	if (theSpanCount == 0)
		return;

	int l = theSpans[0].mX, t = theSpans[0].mY;
	int r = l + theSpans[0].mWidth, b = t;
	for (int i = 1; i < theSpanCount; ++i)
	{
		l = std::min(theSpans[i].mX, l);
		r = std::max(theSpans[i].mX + theSpans[i].mWidth - 1, r);
		t = std::min(theSpans[i].mY, t);
		b = std::max(theSpans[i].mY + theSpans[i].mWidth - 1, b);
	}
	for (int i = 0; i < theSpanCount; ++i)
	{
		theSpans[i].mX -= l;
		theSpans[i].mY -= t;
	}

	MemoryImage aTempImage;
	aTempImage.Create(r - l + 1, b - t + 1);
	aTempImage.FillScanLinesWithCoverage(theSpans,
										 theSpanCount,
										 theColor,
										 theDrawMode,
										 theCoverage,
										 theCoverX - l,
										 theCoverY - t,
										 theCoverWidth,
										 theCoverHeight);
	Blt(&aTempImage, l, t, Rect(0, 0, r - l + 1, b - t + 1), Color::White, theDrawMode);
}

void GPUImage::Create(int theWidth, int theHeight)
{
	delete[] mBits;

	mBits = nullptr;

	mWidth = theWidth;
	mHeight = theHeight;

	mHasTrans = true;
	mHasAlpha = true;

	BitsChanged();
}

void GPUImage::CommitBits()
{
	if (mSurface == NULL)
	{
		MemoryImage::CommitBits();
		return;
	}
}

bool GPUImage::LockSurface()
{
	return false;
}

bool GPUImage::UnlockSurface()
{
	return false;
}

void GPUImage::DeleteAllNonSurfaceData()
{
	delete[] mBits;
	mBits = NULL;

	delete[] mNativeAlphaData;
	mNativeAlphaData = NULL;

	delete[] mRLAdditiveData;
	mRLAdditiveData = NULL;

	delete[] mRLAlphaData;
	mRLAlphaData = NULL;

	delete[] mColorTable;
	mColorTable = NULL;

	delete[] mColorIndices;
	mColorIndices = NULL;
}

void GPUImage::DeleteNativeData()
{
	if (mSurfaceSet)
		return;

	MemoryImage::DeleteNativeData();
	DeleteSurface();
}

void GPUImage::DeleteExtraBuffers()
{
	if (mSurfaceSet)
		return;

	MemoryImage::DeleteExtraBuffers();
	DeleteSurface();
}

void GPUImage::ReInit()
{
	MemoryImage::ReInit();

	if (mWantSurface)
		GenerateSurface();
}

bool GPUImage::Palletize()
{
	if (MemoryImage::Palletize())
	{
		// Don't keep around the DDSurface if we palletize the image, that
		// would be a waste of memory
		DeleteSurface();
		return true;
	}
	else
	{
		return false;
	}
}

void GPUImage::SetVideoMemory(bool wantVideoMemory)
{
	if (wantVideoMemory != mVideoMemory)
	{
		mVideoMemory = wantVideoMemory;

		// Make sure that we have the bits
		GetBits();

		DeleteSurface();
	}
}

bool GPUImage::GenerateSurface()
{
	return false;
}

void GPUImage::DeleteSurface()
{

}

void GPUImage::SetSurface(void* theSurface)
{
	mSurfaceSet = true;
	mSurface = theSurface;
	mWidth = 0;
	mHeight = 0;
}

void* GPUImage::GetSurface()
{
	if (mSurface == nullptr)
		GenerateSurface();

	return mSurface;
}

bool GPUImage::PolyFill3D(const Point theVertices[],
						  int theNumVertices,
						  const Rect &theClipRect,
						  const Color &theColor,
						  int theDrawMode,
						  int tx,
						  int ty,
						  bool comvex)
{
	if (Check3D(this))
	{
		mRenderer->FillPoly(theVertices, theNumVertices, theClipRect, theColor, theDrawMode, tx, ty);
		return true;
	}
	else
		return false;
}


void GPUImage::ImplFillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{

}


void GPUImage::FillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{
	if (Check3D(this))
	{
		mRenderer->FillRect(theRect, theColor, theDrawMode);
		return;
	}

	CommitBits();
	if ((mDrawToBits) || (mHasAlpha) || ((mHasTrans) && (!mFirstPixelTrans)))
	{
		MemoryImage::FillRect(theRect, theColor, theDrawMode);
		return;
	}
	
	if (!mNoLock)
		ImplFillRect(theRect,theColor,theDrawMode);

	DeleteAllNonSurfaceData();
}

void GPUImage::ImplDrawLine(double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode)
{

}

void GPUImage::DrawLine(double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode)
{
	if (Check3D(this))
	{
		mRenderer->DrawLine(theStartX, theStartY, theEndX, theEndY, theColor, theDrawMode);
		return;
	}

	if ((mDrawToBits) || (mHasAlpha) || (mHasTrans))
	{
		MemoryImage::DrawLine(theStartX, theStartY, theEndX, theEndY, theColor, theDrawMode);
		return;
	}

	if (theStartY == theEndY)
	{
		int aStartX = std::min(theStartX, theEndX);
		int aEndX = std::max(theStartX, theEndX);

		FillRect(Rect(aStartX, theStartY, aEndX - aStartX + 1, theEndY - theStartY + 1), theColor, theDrawMode);
		return;
	}
	else if (theStartX == theEndX)
	{
		int aStartY = std::min(theStartY, theEndY);
		int aEndY = std::max(theStartY, theEndY);

		FillRect(Rect(theStartX, aStartY, theEndX - theStartX + 1, aEndY - aStartY + 1), theColor, theDrawMode);
		return;
	}

	CommitBits();

	if (!mNoLock)
		ImplDrawLine(theStartX, theStartY, theEndX, theEndY, theColor, theDrawMode);

	DeleteAllNonSurfaceData();
}

void GPUImage::ImplDrawLineAA(double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode)
{
}

void GPUImage::DrawLineAA(
	double theStartX, double theStartY, double theEndX, double theEndY, const Color& theColor, int theDrawMode)
{
	if (Check3D(this))
	{
		mRenderer->DrawLine(theStartX, theStartY, theEndX, theEndY, theColor, theDrawMode);
		return;
	}

	if ((mDrawToBits) || (mHasAlpha) || (mHasTrans))
	{
		MemoryImage::DrawLine(theStartX, theStartY, theEndX, theEndY, theColor, theDrawMode);
		return;
	}

	if (theStartY == theEndY)
	{
		int aStartX = std::min(theStartX, theEndX);
		int aEndX = std::max(theStartX, theEndX);

		FillRect(Rect(aStartX, theStartY, aEndX - aStartX + 1, theEndY - theStartY + 1), theColor, theDrawMode);
		return;
	}
	else if (theStartX == theEndX)
	{
		int aStartY = std::min(theStartY, theEndY);
		int aEndY = std::max(theStartY, theEndY);

		FillRect(Rect(theStartX, aStartY, theEndX - theStartX + 1, aEndY - aStartY + 1), theColor, theDrawMode);
		return;
	}

	CommitBits();

	if (!mNoLock)
		ImplDrawLineAA(theStartX, theStartY, theEndX, theEndY, theColor, theDrawMode);
}


void GPUImage::ImplBlt(Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor, int theDrawMode)
{
}


void GPUImage::Blt(Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor, int theDrawMode)
{
	theImage->mDrawn = true;

	//if (gDebug)
	//	mApp->CopyToClipboard("+DDImage::Blt");

	DBG_ASSERTE((theColor.mRed >= 0) && (theColor.mRed <= 255));
	DBG_ASSERTE((theColor.mGreen >= 0) && (theColor.mGreen <= 255));
	DBG_ASSERTE((theColor.mBlue >= 0) && (theColor.mBlue <= 255));
	DBG_ASSERTE((theColor.mAlpha >= 0) && (theColor.mAlpha <= 255));

	CommitBits();

	if (Check3D(this))
	{
		mRenderer->Blt(theImage, theX, theY, theSrcRect, theColor, theDrawMode);
		return;
	}

	if ((mDrawToBits) || (mHasAlpha) || ((mHasTrans) && (!mFirstPixelTrans)))
	{
		MemoryImage::Blt(theImage, theX, theY, theSrcRect, theColor, theDrawMode);
		return;
	}

	if (!mNoLock)
		ImplBlt(theImage, theX, theY, theSrcRect, theColor, theDrawMode);

	DeleteAllNonSurfaceData();
}
void GPUImage::ImplBltF(Image *theImage,
					float theX,
					float theY,
					const Rect &theSrcRect,
					const Rect &theClipRect,
					const Color &theColor,
					int theDrawMode)
{

}

void GPUImage::BltF(Image *theImage,
	float theX,
	float theY,
	const Rect& theSrcRect,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode) 
{
	theImage->mDrawn = true;

	if (Check3D(this))
	{
		FRect aClipRect(theClipRect.mX, theClipRect.mY, theClipRect.mWidth, theClipRect.mHeight);
		FRect aDestRect(theX, theY, theSrcRect.mWidth, theSrcRect.mHeight);

		FRect anIntersect = aDestRect.Intersection(aClipRect);
		if (anIntersect.mWidth != aDestRect.mWidth || anIntersect.mHeight != aDestRect.mHeight)
		{
			if (anIntersect.mWidth != 0 && anIntersect.mHeight != 0)
				mRenderer->BltClipF(
					theImage, theX, theY, theSrcRect, theClipRect, theColor, theDrawMode);
		}
		else
			mRenderer->Blt(theImage, theX, theY, theSrcRect, theColor, theDrawMode, true);

		return;
	}
	else if (!mNoLock)
		ImplBltF(theImage, theX, theY, theSrcRect, theClipRect, theColor, theDrawMode);
}
void GPUImage::ImplBltRotated(Image *theImage,
	float theX,
	float theY,
	const Rect& theSrcRect,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	double theRot,
	float theRotCenterX,
	float theRotCenterY)
{

}

void GPUImage::BltRotated(Image *theImage,
	float theX,
	float theY,
	const Rect& theSrcRect,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	double theRot,
	float theRotCenterX,
	float theRotCenterY)
{
	theImage->mDrawn = true;

	if (mNoLock)
		return;

	CommitBits();

	if (Check3D(this))
	{
		mRenderer->BltRotated(theImage,
												theX,
												theY,
												theClipRect,
												theColor,
												theDrawMode,
												theRot,
												theRotCenterX,
												theRotCenterY,
												theSrcRect);
		return;
	}

	if ((mDrawToBits) || (mHasAlpha) || ((mHasTrans) && (!mFirstPixelTrans)))
	{
		MemoryImage::BltRotated(
			theImage, theX, theY, theSrcRect, theClipRect, theColor, theDrawMode, theRot, theRotCenterX, theRotCenterY);
		return;
	}

	ImplBltRotated(theImage, theX, theY, theSrcRect, theClipRect, theColor, theDrawMode, theRot, theRotCenterX, theRotCenterY);

	DeleteAllNonSurfaceData();
}
void GPUImage::ImplStretchBlt(Image *theImage,
	const Rect& theDestRect,
	const Rect& theSrcRect,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	bool fastStretch)
{

}


void GPUImage::StretchBlt(Image *theImage,
	const Rect& theDestRect,
	const Rect& theSrcRect,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	bool fastStretch)
{
	theImage->mDrawn = true;

	CommitBits();

	if (Check3D(this))
	{
		mRenderer->StretchBlt(theImage, theDestRect, theSrcRect, theClipRect, theColor, theDrawMode, fastStretch);
		return;
	}

	if ((mDrawToBits) || (mHasAlpha) || (mHasTrans))
	{
		MemoryImage::StretchBlt(theImage, theDestRect, theSrcRect, theClipRect, theColor, theDrawMode, fastStretch);
		return;
	}
	if (!mNoLock)
		ImplStretchBlt(theImage, theDestRect, theSrcRect, theClipRect, theColor, theDrawMode, fastStretch);

	DeleteAllNonSurfaceData();
}
void GPUImage::ImplBltMatrix(Image *theImage,
	float x,
	float y,
	const SexyMatrix3& theMatrix,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	const Rect& theSrcRect,
	bool blend)
{

}

void GPUImage::BltMatrix(Image *theImage,
	float x,
	float y,
	const SexyMatrix3& theMatrix,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	const Rect& theSrcRect,
	bool blend)
{
	theImage->mDrawn = true;

	if (Check3D(this))
	{
		mRenderer->BltTransformed(
			theImage, theClipRect, theColor, theDrawMode, theSrcRect, theMatrix, blend, x, y, true);
		return;
	}

	ImplBltMatrix(theImage, x, y, theMatrix, theClipRect, theColor, theDrawMode, theSrcRect, blend);

	DeleteAllNonSurfaceData();
}
void GPUImage::ImplBltTrianglesTex(Image *theTexture,
	const TriVertex theVertices[][3],
	int theNumTriangles,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	float tx,
	float ty,
	bool blend)
{
	
}

void GPUImage::BltTrianglesTex(Image *theTexture,
	const TriVertex theVertices[][3],
	int theNumTriangles,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	float tx,
	float ty,
	bool blend)
{
	theTexture->mDrawn = true;

	if (Check3D(this))
	{
		mRenderer->DrawTrianglesTex(
			theVertices, theNumTriangles, theColor, theDrawMode, theTexture, tx, ty, blend);
		return;
	}

	ImplBltTrianglesTex(theTexture, theVertices, theNumTriangles, theClipRect, theColor, theDrawMode, tx, ty, blend);

	DeleteAllNonSurfaceData();
}

void GPUImage::ImplBltMirror(
	Image* theImage, int theX, int theY, const Rect& theSrcRect, const Color& theColor, int theDrawMode)
{

}

void GPUImage::BltMirror(
	Image* theImage, int theX, int theY, const Rect& theSrcRect, const Color& theColor, int theDrawMode)
{
	DBG_ASSERTE((theColor.mRed >= 0) && (theColor.mRed <= 255));
	DBG_ASSERTE((theColor.mGreen >= 0) && (theColor.mGreen <= 255));
	DBG_ASSERTE((theColor.mBlue >= 0) && (theColor.mBlue <= 255));
	DBG_ASSERTE((theColor.mAlpha >= 0) && (theColor.mAlpha <= 255));

	CommitBits();

	if (Check3D(this))
	{
		mRenderer->BltMirror(theImage, theX, theY, theSrcRect, theColor, theDrawMode);
		return;
	}

	if (!mNoLock)
		ImplBltMirror(theImage, theX, theY, theSrcRect, theColor, theDrawMode);

	DeleteAllNonSurfaceData();
}

void GPUImage::ImplStretchBltMirror(Image *theImage,
	const Rect& theDestRectOrig,
	const Rect& theSrcRect,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	bool fastStretch)
{

}

void GPUImage::StretchBltMirror(Image *theImage,
	const Rect& theDestRectOrig,
	const Rect& theSrcRect,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	bool fastStretch)
{
	theImage->mDrawn = true;

	CommitBits();

	if (Check3D(this))
	{
		mRenderer->StretchBlt(
			theImage, theDestRectOrig, theSrcRect, theClipRect, theColor, theDrawMode, fastStretch, true);
		return;
	}

	ImplStretchBltMirror(theImage, theDestRectOrig, theSrcRect, theClipRect, theColor, theDrawMode, fastStretch);

	DeleteAllNonSurfaceData();
}

void GPUImage::ImplBltRawTexture(void* theTexture,
	int theTexWidth,
	int theTexHeight,
	const Rect& theDestRect,
	const Rect& theSrcRect,
	const Rect& theClipRect,
	const Color& theColor,
	int theDrawMode,
	bool fastStretch)
{

}

void GPUImage::BltRawTexture(void *theTexture,
							 int theTexWidth,
							 int theTexHeight,
							 const Rect &theDestRect,
							 const Rect &theSrcRect,
							 const Rect &theClipRect,
							 const Color &theColor,
							 int theDrawMode)
{
	CommitBits();

	if (Check3D(this))
	{
		mRenderer->BltRawTexture(
			theTexture, theDestRect, theSrcRect, theClipRect, theColor, theDrawMode);
		return;
	}

	ImplBltRawTexture(theTexture, theTexWidth, theTexHeight, theDestRect, theSrcRect, theClipRect, theColor, theDrawMode, true);

	DeleteAllNonSurfaceData();
}

bool GPUImage::Check3D(GPUImage *theImage)
{
	return theImage->mRenderer->mScreenImage == theImage;
}

bool GPUImage::Check3D(Image *theImage)
{
	GPUImage *anImage = dynamic_cast<GPUImage *>(theImage);
	return anImage != nullptr;
}

void GPUImage::PurgeBits()
{
	if (mSurfaceSet)
		return;

	mPurgeBits = true;

	CommitBits();

	if (mSurface != NULL)
	{
		GetBits();
		DeleteSurface();
	}
}
