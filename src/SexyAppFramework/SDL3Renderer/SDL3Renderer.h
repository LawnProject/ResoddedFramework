#if SEXY_USE_SDL3_RENDERER

#pragma once
#include "Renderer.h"
#include "SDL3Image.h"
#include <SDL3/SDL_render.h>

#include <unordered_map>
#include <vector>

namespace Sexy
{

class TriVertex;
class SexyMatrix;
class SexyAppBase;

class SDL3TextureData : public TextureData
{
  public:
	SDL_Texture *mTexturePtr;
	bool mSourceIsTarget;
	SDL_Texture *GetTexture();

	SDL3TextureData();

	virtual void ReleaseTextures();

	virtual void CreateTextures(MemoryImage *theImage, void *theRendererData);
	virtual void CheckCreateTextures(MemoryImage *theImage, void *theRendererData);
};

class SDL3Renderer : public Renderer
{

  public:

	SDL_Renderer *mBackendRenderer;
	SDL_Texture *mTargetTexture;
	static int gSDLTextureCount;
	static uint64_t gSDLUsedMemoryCount;

  public:
	SDL3Renderer(SexyAppBase *theApp);
	virtual ~SDL3Renderer();

	bool InitRendererObject();
	void ApplyBlendMode(BlendMode mode);
	void SetLinearBlend_SDL(SDL_Texture *theTexture, bool linearBlend);
	SDL_BlendMode Get_SDL_NativeBlendMode(BlendMode theBlendMode);

	virtual GPUImage *NewGPUImage()
	{
		return new SDL3Image(this);
	}

	virtual uint32_t *CaptureFrameBuffer();

	virtual void Cleanup();

	virtual bool PreDraw();
	virtual bool Redraw(Rect *theClipRect);
	virtual void SetVideoOnlyDraw(bool videoOnly);
	virtual void Remove3DData(MemoryImage *theImage);

	virtual void DeleteTexture(void *theTexture);
	virtual void *CreateTexture(void *thePixels, int theWidth, int theHeight, RawPixelFormat thePixelFormat,
								int theAlignment = 4);

	virtual RenderingInfo GetRenderingInfo();

	virtual void UpdateViewport();
	virtual bool Init();
	virtual RendererError UpdateVSync();

	virtual bool CreateImageTexture(MemoryImage *theImage);
	virtual bool RecoverBits(MemoryImage *theImage);
	virtual uint32_t *GetBitsFromTexture(void *theTexture, int theWidth, int theHeight);

	virtual void Blt(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Color &theColor,
					 int theDrawMode, bool linearFilter = false);
	virtual void BltClipF(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Rect &theClipRect,
						  const Color &theColor, int theDrawMode);
	virtual void BltMirror(Image *theImage, float theX, float theY, const Rect &theSrcRect, const Color &theColor,
						   int theDrawMode, bool linearFilter = false);
	virtual void StretchBlt(Image *theImage, const Rect &theDestRect, const Rect &theSrcRect, const Rect &theClipRect,
							const Color &theColor, int theDrawMode, bool fastStretch, bool mirror = false);
	virtual void BltRotated(Image *theImage, float theX, float theY, const Rect &theClipRect, const Color &theColor,
							int theDrawMode, double theRot, float theRotCenterX, float theRotCenterY,
							const Rect &theSrcRect);
	virtual void BltTransformed(Image *theImage, const Rect &theClipRect, const Color &theColor, int theDrawMode,
								const Rect &theSrcRect, const SexyMatrix3 &theTransform, bool linearFilter,
								float theX = 0, float theY = 0, bool center = false);
	virtual void DrawLine(double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor,
						  int theDrawMode);
	virtual void FillRect(const Rect &theRect, const Color &theColor, int theDrawMode);
	virtual void DrawTriangle(const TriVertex &p1, const TriVertex &p2, const TriVertex &p3, const Color &theColor,
							  int theDrawMode);
	virtual void DrawTriangleTex(const TriVertex &p1, const TriVertex &p2, const TriVertex &p3, const Color &theColor,
								 int theDrawMode, Image *theTexture, bool blend = true);
	virtual void DrawTrianglesTex(const TriVertex theVertices[][3], int theNumTriangles, const Color &theColor,
								  int theDrawMode, Image *theTexture, float tx = 0, float ty = 0, bool blend = true);
	virtual void DrawTrianglesTexStrip(const TriVertex theVertices[], int theNumTriangles, const Color &theColor,
									   int theDrawMode, Image *theTexture, float tx = 0, float ty = 0,
									   bool blend = true);
	virtual void FillPoly(const Point theVertices[], int theNumVertices, const Rect &theClipRect, const Color &theColor,
						  int theDrawMode, int tx, int ty);

	virtual void BltRawTexture(void *theTexture, int theTexWidth, int theTexHeight, const Rect &theDestRect,
							   const Rect &theSrcRect, const Rect &theClipRect, const Color &theColor, int theDrawMode);

	static bool TestSDL3();

	SDL_FPoint TransformToPoint(float x, float y, const SexyMatrix3 &m, float aTransX = 0, float aTransY = 0)
	{
		SDL_FPoint result;
		result.x = m.m00 * x + m.m01 * y + m.m02 + aTransX;
		result.y = m.m10 * x + m.m11 * y + m.m12 + aTransY;
		return result;
	}

	SDL_FPoint RotatePointAroundPivot(const SDL_FPoint point, const SDL_FPoint center, float angleRad)
	{
		float sinValue = sin(angleRad);
		float cosValue = cos(angleRad);

		SDL_FPoint translation = {point.x - center.x, point.y - center.y};

		SDL_FPoint rotation = {translation.x * cosValue - translation.y * sinValue,
							  translation.x * sinValue + translation.y * cosValue};

		return {rotation.x + center.x, rotation.y + center.y};
	}
};
}; // namespace Sexy
#endif