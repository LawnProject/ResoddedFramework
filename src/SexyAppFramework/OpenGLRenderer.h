#pragma once
#if SEXY_USE_OPENGL
#include "Renderer.h"
#include "GLShader.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <SDL3/SDL_video.h>
#include <OpenGLImage.h>

#include <unordered_map>
#include <vector>

namespace Sexy
{
	struct GLBlendFunc
	{
		GLenum theSrc;
		GLenum theDst;
		bool theEnableBlend = true;
	};

	const std::unordered_map<BlendMode, GLBlendFunc> blend_mode_funcs = {
	{BlendMode::BLENDMODE_BLEND, {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, true}},
	{BlendMode::BLENDMODE_ADD, {GL_SRC_ALPHA, GL_ONE, true}}};

	struct Vertex
	{
		glm::vec2 mPos;
		glm::vec2 mTexCoord;
		glm::vec4 mColor;
	};

	struct GLTextCacheEntry
	{
		GLuint textureID;
		int mWidth;
		int mHeight;
	};

	struct GLDrawCommand
	{
		GLenum mPrimitiveType;
		GLuint mTextureID;
		BlendMode mBlendMode;
		std::vector<Vertex> mVertices;
		bool mHasClipRect = false;
		Rect mClipRect;
		TextureUVWrapMode mUVWrapMode = UV_CLAMP;
	};

	struct WrappingSamplers
	{
		GLuint mWrap;
		GLuint mClamp;
	};

	class TriVertex;
	class SexyMatrix;
	class SexyAppBase;

	class OpenGLTextureData : public TextureData
	{
	  public:
		GLuint mTexID;
		bool mSourceIsFBO;
		GLuint GetTextureID();

		OpenGLTextureData();

		virtual void ReleaseTextures();

		virtual void CreateTextures(MemoryImage *theImage, void *theRendererData);
		virtual void CheckCreateTextures(MemoryImage *theImage, void *theRendererData);
	};

	class OpenGLRenderer : public Renderer
	{

	  public:
		GLuint mVAO;
		GLuint mVBO;
		SDL_GLContext mContext;
		std::vector<GLDrawCommand> mCommandBuffer;
		GLShader *mDefaultShader;
		glm::mat4 mProjection;
		WrappingSamplers mSamplers;

	  public:
		OpenGLRenderer(SexyAppBase *theApp);
		virtual ~OpenGLRenderer();

		bool InitGLContext();
		bool InitBuffers();
		void AddCommand(const GLDrawCommand& theCommand);
		void ApplyBlendMode(BlendMode mode);

		virtual std::string getBackendType()
		{
			return "OpenGL";
		}
		
		virtual GPUImage *NewGPUImage()
		{
			return new OpenGLImage(this);
		}

		virtual void Cleanup();

		virtual bool PreDraw();
		virtual bool Redraw(Rect *theClipRect);
		virtual void SetVideoOnlyDraw(bool videoOnly);
		virtual void Remove3DData(MemoryImage *theImage);

		virtual void DeleteTexture(void *theTexture);
		virtual void *CreateTexture(void *thePixels, int theWidth, int theHeight, RawPixelFormat thePixelFormat, int theAlignment = 4);

		virtual RenderingInfo GetRenderingInfo();

		virtual void UpdateViewport();
		virtual bool Init();

		virtual bool CreateImageTexture(MemoryImage *theImage);
		virtual bool RecoverBits(MemoryImage *theImage);
		virtual uint32_t *GetBitsFromTexture(void *theTexture, int theWidth, int theHeight);

		virtual void Blt(Image *theImage,
						 float theX,
						 float theY,
						 const Rect &theSrcRect,
						 const Color &theColor,
						 int theDrawMode,
						 bool linearFilter = false);
		virtual void BltClipF(Image *theImage,
							  float theX,
							  float theY,
							  const Rect &theSrcRect,
							  const Rect &theClipRect,
							  const Color &theColor,
							  int theDrawMode);
		virtual void BltMirror(Image *theImage,
							   float theX,
							   float theY,
							   const Rect &theSrcRect,
							   const Color &theColor,
							   int theDrawMode,
							   bool linearFilter = false);
		virtual void StretchBlt(Image *theImage,
								const Rect &theDestRect,
								const Rect &theSrcRect,
								const Rect &theClipRect,
								const Color &theColor,
								int theDrawMode,
								bool fastStretch,
								bool mirror = false);
		virtual void BltRotated(Image *theImage,
								float theX,
								float theY,
								const Rect &theClipRect,
								const Color &theColor,
								int theDrawMode,
								double theRot,
								float theRotCenterX,
								float theRotCenterY,
								const Rect &theSrcRect);
		virtual void BltTransformed(Image *theImage,
									const Rect &theClipRect,
									const Color &theColor,
									int theDrawMode,
									const Rect &theSrcRect,
									const SexyMatrix3 &theTransform,
									bool linearFilter,
									float theX = 0,
									float theY = 0,
									bool center = false);
		virtual void DrawLine(
			double theStartX, double theStartY, double theEndX, double theEndY, const Color &theColor, int theDrawMode);
		virtual void FillRect(const Rect &theRect, const Color &theColor, int theDrawMode);
		virtual void DrawTriangle(
			const TriVertex &p1, const TriVertex &p2, const TriVertex &p3, const Color &theColor, int theDrawMode);
		virtual void DrawTriangleTex(const TriVertex &p1,
									 const TriVertex &p2,
									 const TriVertex &p3,
									 const Color &theColor,
									 int theDrawMode,
									 Image *theTexture,
									 bool blend = true);
		virtual void DrawTrianglesTex(const TriVertex theVertices[][3],
									  int theNumTriangles,
									  const Color &theColor,
									  int theDrawMode,
									  Image *theTexture,
									  float tx = 0,
									  float ty = 0,
									  bool blend = true);
		virtual void DrawTrianglesTexStrip(const TriVertex theVertices[],
										   int theNumTriangles,
										   const Color &theColor,
										   int theDrawMode,
										   Image *theTexture,
										   float tx = 0,
										   float ty = 0,
										   bool blend = true);
		virtual void FillPoly(const Point theVertices[],
							  int theNumVertices,
							  const Rect &theClipRect,
							  const Color &theColor,
							  int theDrawMode,
							  int tx,
							  int ty);

		virtual void BltRawTexture(void *theTexture,
							  const Rect &theDestRect,
							  const Rect &theSrcRect,
							  const Rect &theClipRect,
							  const Color &theColor,
							  int theDrawMode);
		glm::vec2 TransformToPoint(float x, float y, const SexyMatrix3& m, float aTransX = 0, float aTransY = 0)
		{
			glm::vec2 result;
			result.x = m.m00 * x + m.m01 * y + m.m02 + aTransX;
			result.y = m.m10 * x + m.m11 * y + m.m12 + aTransY;
			return result;
		}

		glm::vec2 RotatePointAroundPivot(const glm::vec2 point, const glm::vec2 center, float angleRad)
		{
			float sinValue = sin(angleRad);
			float cosValue = cos(angleRad);

			glm::vec2 translation = point - center;

			glm::vec2 rotation = {translation.x * cosValue - translation.y * sinValue,
								  translation.x * sinValue + translation.y * cosValue};

			return rotation + center;
		}

	};
}; // namespace Sexy
#endif