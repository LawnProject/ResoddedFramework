#if SEXY_USE_OPENGL

#pragma once

#include "GPUImage.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Sexy
{

class Renderer;
class OpenGLRenderer;
class SysFont;

class OpenGLImage : public GPUImage
{
  protected:
	friend class SysFont;
	void DeleteAllNonSurfaceData();

  public:
	GLuint mTexID;
	GLuint mFBO;
	OpenGLRenderer *mGLRenderer;
	
	static GLuint gOpenGLImageVAO;
	static GLuint gOpenGLImageVBO;
	glm::mat4 mProjection;


  private:
	void Init();

  public:
	bool GenerateSurface();
	void DeleteSurface();

	void *GetSurface();
	virtual void BitsChanged();

  public:
	OpenGLImage();
	OpenGLImage(Renderer *theRenderer);
	virtual ~OpenGLImage() override;

	virtual bool LockSurface();
	virtual bool UnlockSurface();
	void CreateImageBuffers();
	void PreTextureDraw();
	virtual void SetSurface(void *theSurface);

	virtual uint32_t *GetBits();

	virtual void ImplBlt(
		Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor, int theDrawMode);
	virtual void ImplBltF(Image *theImage,
						  float theX,
						  float theY,
						  const Rect &theSrcRect,
						  const Rect &theClipRect,
						  const Color &theColor,
						  int theDrawMode);
	virtual void ImplBltRotated(Image *theImage,
								float theX,
								float theY,
								const Rect &theSrcRect,
								const Rect &theClipRect,
								const Color &theColor,
								int theDrawMode,
								double theRot,
								float theRotCenterX,
								float theRotCenterY);
	virtual void ImplStretchBlt(Image *theImage,
								const Rect &theDestRect,
								const Rect &theSrcRect,
								const Rect &theClipRect,
								const Color &theColor,
								int theDrawMode,
								bool fastStretch);
	virtual void ImplBltMatrix(Image *theImage,
							   float x,
							   float y,
							   const SexyMatrix3 &theMatrix,
							   const Rect &theClipRect,
							   const Color &theColor,
							   int theDrawMode,
							   const Rect &theSrcRect,
							   bool blend);
	virtual void ImplBltTrianglesTex(Image *theTexture,
									 const TriVertex theVertices[][3],
									 int theNumTriangles,
									 const Rect &theClipRect,
									 const Color &theColor,
									 int theDrawMode,
									 float tx,
									 float ty,
									 bool blend);

	virtual void ImplBltMirror(
		Image *theImage, int theX, int theY, const Rect &theSrcRect, const Color &theColor, int theDrawMode);
	virtual void ImplStretchBltMirror(Image *theImage,
									  const Rect &theDestRectOrig,
									  const Rect &theSrcRect,
									  const Rect &theClipRect,
									  const Color &theColor,
									  int theDrawMode,
									  bool fastStretch);

	virtual void ImplBltRawTexture(void *theTexture,
								int theTexWidth,
								int theTexHeight,
								const Rect &theDestRect,
								const Rect &theSrcRect,
								const Rect &theClipRect,
								const Color &theColor,
								int theDrawMode,
								bool fastStretch);


	virtual void ImplDrawLine(
		double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode);
	virtual void ImplDrawLineAA(
		double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode);

	virtual void ImplFillRect(const Rect &theRect, const Color &theColor, int theDrawMode);
};

} // namespace Sexy

#endif