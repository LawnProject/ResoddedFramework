#pragma once

#include "Common.h"
#include "GPUImage.h"
#include "SexyMatrix.h"
#include "CritSect.h"

#define MAX_VERTICES 16384

namespace Sexy
{
	enum BlendMode
	{
		BLENDMODE_NONE = 0,
		BLENDMODE_BLEND,
		BLENDMODE_BLEND_PREMULTIPLIED,
		BLENDMODE_ADD,
		BLENDMODE_ADD_PREMULTIPLIED,
	};

	enum TextureUVWrapMode
	{
		UV_WRAP,
		UV_CLAMP,
	};

	enum ImageFlags
	{
			// subdivide image into fewest possible textures (may use more memory)
		ImageFlag_MinimizeNumSubdivisions = 0x0001,
			// good to use with image strips so the entire texture isn't pulled in when drawing just a piece
		ImageFlag_Use64By64Subdivisions = 0x0002,
			// images with not too many color gradients work well in this format
		ImageFlag_UseA4R4G4B4 = 0x0004,
			// non-alpha images will be stored as R5G6B5 by default so use this option if you want a 32-bit non-alpha image
		ImageFlag_UseA8R8G8B8 = 0x0008
	};

	enum PixelFormat
	{
		PixelFormat_Unknown = 0x0000,
		PixelFormat_A8R8G8B8 = 0x0001,
		PixelFormat_A4R4G4B4 = 0x0002,
		PixelFormat_R5G6B5 = 0x0004,
		PixelFormat_Palette8 = 0x0008
	};

	enum RawPixelFormat
	{
		RAW_FORMAT_RGBA,
		RAW_FORMAT_RGB,
		RAW_FORMAT_R,
	};

	enum RenderingBackend
	{
		BACKEND_NONE = 0,
		BACKEND_OPENGL,
	};

	const std::map<RenderingBackend, std::string> gRenderBackends = 
	{
		{RenderingBackend::BACKEND_NONE, "NONE"}
		#if SEXY_USE_OPENGL
		,
		{RenderingBackend::BACKEND_OPENGL, "OPENGL"}
		#endif
	};

	struct RenderingInfo
	{
		int mTotalVideoMem;
		int mFreeVideoMem;
		int mNumTextures;
	};

	class TextureData
	{
	  public:
		//Use casting to your backend's texture format
		void *mTexData;

		int mWidth, mHeight;
		int mBitsChangedCount;
		int mTexMemSize;
		PixelFormat mPixelFormat;
		uint32_t mImageFlags;

		TextureData();
		virtual ~TextureData();

		virtual void ReleaseTextures() {}

		// cast theRendererData to a struct that contains the needed variables for your texture creation
		virtual void CreateTextures(MemoryImage *theImage, void *theRendererData) {}
		virtual void CheckCreateTextures(MemoryImage *theImage, void *theRendererData) {}
	};

	class SexyMatrix3;
	class TriVertex;
	class SexyAppBase;
	class SysFont;

	class Renderer
	{
	  public:
		SexyAppBase *mApp;
		int mWidth;
		int mHeight;
		Rect mPresentationRect;
		bool mIsWindowed;
		bool mSceneBegun;
		int mRefreshRate;
		int mMillisecondsPerFrame;
		RenderingBackend mCurrentBackend;

		typedef std::set<MemoryImage *> ImageSet;
		ImageSet mImageSet;

		//SysFonts that have to be cleaned manually
		std::set<SysFont*> mSysFonts;

		typedef std::list<SexyMatrix3> TransformStack;
		TransformStack mTransformStack;

		static std::string mErrorString;
		CritSect mCritSect;

		GPUImage *mScreenImage;

		int mRGBBits;
		uint32_t mRedMask;
		uint32_t mGreenMask;
		uint32_t mBlueMask;
		int mRedBits;
		int mGreenBits;
		int mBlueBits;
		int mRedShift;
		int mGreenShift;
		int mBlueShift;

		TextureUVWrapMode mCurrentUVWrapMode;

	  public:
		Renderer(SexyAppBase *theApp);
		virtual ~Renderer() = 0;

		virtual std::string getBackendType()
		{
			return "NONE";
		}

		virtual void Cleanup();

		virtual bool PreDraw() = 0;
		virtual void AddImage(Image *theImage);
		virtual void RemoveImage(Image *theImage);
		virtual void Remove3DData(MemoryImage *theImage) = 0;

		
		virtual bool Redraw(Rect *theClipRect) = 0;
		virtual void SetVideoOnlyDraw(bool videoOnly) = 0;

		virtual GPUImage* NewGPUImage()
		{
			return new GPUImage(this);
		}

		virtual GPUImage *GetScreenImage()
		{
			return mScreenImage;
		}

		virtual void UpdateViewport() = 0;
		virtual bool Init() = 0;

		virtual RenderingInfo GetRenderingInfo() = 0;

		virtual bool CreateImageTexture(MemoryImage *theImage) = 0;
		virtual bool RecoverBits(MemoryImage *theImage) = 0;
		virtual uint32_t *GetBitsFromTexture(void *theTexture, int theWidth, int theHeight) = 0;

		virtual void DeleteTexture(void *theTexture) = 0;
		virtual void *CreateTexture(void *thePixels, int theWidth, int theHeight, RawPixelFormat thePixelFormat, int theAlignment = 4) = 0;

		// Get a blend mode based on a provided draw mode from Graphics
		virtual BlendMode ChooseBlendMode(int theDrawMode);
		virtual void PushTransform(const SexyMatrix3 &theTransform, bool concatenate = true);
		virtual void PopTransform();

		virtual void Blt(Image *theImage,
					float theX,
					float theY,
					const Rect &theSrcRect,
					const Color &theColor,
					int theDrawMode,
					bool linearFilter = false) = 0;
		virtual void BltClipF(Image *theImage,
						float theX,
						float theY,
						const Rect &theSrcRect,
						const Rect &theClipRect,
						const Color &theColor,
						int theDrawMode) = 0;
		virtual void BltMirror(Image *theImage,
						float theX,
						float theY,
						const Rect &theSrcRect,
						const Color &theColor,
						int theDrawMode,
						bool linearFilter = false) = 0;
		virtual void StretchBlt(Image *theImage,
							const Rect &theDestRect,
							const Rect &theSrcRect,
							const Rect &theClipRect,
							const Color &theColor,
							int theDrawMode,
							bool fastStretch,
							bool mirror = false) = 0;
		virtual void BltRotated(Image *theImage,
							float theX,
							float theY,
							const Rect &theClipRect,
							const Color &theColor,
							int theDrawMode,
							double theRot,
							float theRotCenterX,
							float theRotCenterY,
							const Rect &theSrcRect) = 0;
		virtual void BltTransformed(Image *theImage,
								const Rect &theClipRect,
								const Color &theColor,
								int theDrawMode,
								const Rect &theSrcRect,
								const SexyMatrix3 &theTransform,
								bool linearFilter,
								float theX = 0,
								float theY = 0,
								bool center = false) = 0;
		virtual void DrawLine(double theStartX,
								double theStartY,
								double theEndX,
								double theEndY,
								const Color &theColor,
								int theDrawMode) = 0;
		virtual void FillRect(const Rect &theRect, const Color &theColor, int theDrawMode) = 0;
		virtual void DrawTriangle(
			const TriVertex &p1, 
			const TriVertex &p2, 
			const TriVertex &p3, 
			const Color &theColor, 
			int theDrawMode) = 0;
		virtual void DrawTriangleTex(const TriVertex &p1,
								const TriVertex &p2,
								const TriVertex &p3,
								const Color &theColor,
								int theDrawMode,
								Image *theTexture,
								bool blend = true) = 0;
		virtual void DrawTrianglesTex(const TriVertex theVertices[][3],
								int theNumTriangles,
								const Color &theColor,
								int theDrawMode,
								Image *theTexture,
								float tx = 0,
								float ty = 0,
								bool blend = true) = 0;
		virtual void DrawTrianglesTexStrip(const TriVertex theVertices[],
									int theNumTriangles,
									const Color &theColor,
									int theDrawMode,
									Image *theTexture,
									float tx = 0,
									float ty = 0,
									bool blend = true) = 0;
		virtual void FillPoly(const Point theVertices[],
						int theNumVertices,
						const Rect &theClipRect,
						const Color &theColor,
						int theDrawMode,
						int tx,
						int ty) = 0;

		virtual void BltRawTexture(void *theTexture,
							const Rect &theDestRect,
							const Rect &theSrcRect,
							const Rect &theClipRect,
							const Color &theColor,
							int theDrawMode) = 0;
	};
};