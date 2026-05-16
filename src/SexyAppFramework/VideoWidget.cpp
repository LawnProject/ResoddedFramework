#include "VideoWidget.h"
#include "SexyAppBase.h"
#include "Renderer.h"
#include <filesystem>
#if SEXY_USE_SDL3_RENDERER
#include <SDL3/SDL_render.h>
#include "SDL3Renderer/SDL3Renderer.h"
#endif
#if SEXY_USE_OPENGL
#include <glad/glad.h>
#endif

using namespace Sexy;

VideoWidget::VideoWidget() : VideoWidget(gSexyAppBase)
{

}

VideoWidget::VideoWidget(SexyAppBase *theApp)
{
	mApp = theApp;
	mDecoder = nullptr;
	mFormatContext = nullptr;
	mCodec = nullptr;
	mVideoTexture = nullptr;
	mSwSContext = nullptr;

	mVidWidth = 0;
	mVidHeight = 0;
	mVidStreamIndex = 0;
	mFrame = av_frame_alloc();
}

VideoWidget::~VideoWidget()
{
	sws_freeContext(mSwSContext);
	avformat_close_input(&mFormatContext);
	avformat_free_context(mFormatContext);
	av_frame_unref(mFrame);
	av_packet_unref(&mPacket);
	avcodec_free_context(&mDecoder);

	if (mVideoTexture)
	{
		switch (mApp->mRenderer->mCurrentBackend)
		{
#if SEXY_USE_SDL3_RENDERER
			case RenderingBackend::BACKEND_SDL3: {
				SDL_DestroyTexture((SDL_Texture *)mVideoTexture);
				break;
			}
#endif
#if SEXY_USE_OPENGL
			case RenderingBackend::BACKEND_OPENGL: {
				delete (GLuint *)mVideoTexture;
				break;
			}
#endif
		}
		mVideoTexture = nullptr;

	}
}

void VideoWidget::LoadVideo(const SexyString &aPath)
{
	mFormatContext = nullptr;
	avformat_open_input(&mFormatContext, aPath.c_str(), nullptr, nullptr);
	mCodec = nullptr;
	mVidStreamIndex = av_find_best_stream(mFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &mCodec, 0);
	mDecoder = avcodec_alloc_context3(mCodec);
	mDecoder->thread_count = 0;
	avcodec_parameters_to_context(mDecoder, mFormatContext->streams[mVidStreamIndex]->codecpar);
	avcodec_open2(mDecoder, mCodec, nullptr);

	AVCodecParameters *aParameters = mFormatContext->streams[mVidStreamIndex]->codecpar;
	mVidWidth = aParameters->width;
	mVidHeight = aParameters->height;
	while (av_read_frame(mFormatContext, &mPacket) >= 0)
	{
		if (mPacket.stream_index == mVidStreamIndex)
		{
			avcodec_send_packet(mDecoder,& mPacket);
			if (avcodec_receive_frame(mDecoder, mFrame) == 0)
				break;
		}
	}
	mSwSContext = sws_getContext(mVidWidth, mVidHeight, (AVPixelFormat)mFrame->format, mVidWidth, mVidHeight, AV_PIX_FMT_RGB0, SWS_BILINEAR, nullptr, nullptr, nullptr);
	switch (mApp->mRenderer->mCurrentBackend)
	{
#if SEXY_USE_OPENGL
		case RenderingBackend::BACKEND_OPENGL: {
			GLuint aTexID;
			glGenTextures(1, &aTexID);
			glBindTexture(GL_TEXTURE_2D, aTexID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mVidWidth, mVidHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			mVideoTexture = new GLuint(aTexID);
			break;
		}
#endif
#if SEXY_USE_SDL3_RENDERER

		case RenderingBackend::BACKEND_SDL3: {
			mVideoTexture = SDL_CreateTexture(((SDL3Renderer *)mApp->mRenderer)->mBackendRenderer, SDL_PIXELFORMAT_IYUV,
											  SDL_TEXTUREACCESS_STREAMING, mVidWidth, mVidHeight);
			break;
		}
#endif
	}

}

void VideoWidget::Draw(Graphics *g)
{
	if (mVideoTexture == nullptr)
		return;
	g->mDestImage->BltRawTexture(mVideoTexture, mVidWidth, mVidHeight, Rect(mX + g->mTransX, mY + g->mTransY, mWidth, mHeight),
								 Rect(0, 0, mVidWidth, mVidHeight), g->mClipRect, Color(255, 255, 255, 255), g->mDrawMode);
}

void VideoWidget::Update()
{
	if (mVideoTexture == nullptr)
		return;
	if (av_read_frame(mFormatContext, &mPacket) >= 0)
	{
		if (mPacket.stream_index == mVidStreamIndex)
		{
			avcodec_send_packet(mDecoder, &mPacket);
			if (avcodec_receive_frame(mDecoder, mFrame) == 0)
			{
				switch (mApp->mRenderer->mCurrentBackend)
				{
#if SEXY_USE_SDL3_RENDERER
					case RenderingBackend::BACKEND_SDL3:
					{
						SDL_UpdateYUVTexture((SDL_Texture *)mVideoTexture, nullptr, mFrame->data[0], mFrame->linesize[0], mFrame->data[1], mFrame->linesize[1], mFrame->data[2], mFrame->linesize[2]);
						break;
					}
#endif
#if SEXY_USE_OPENGL
					case RenderingBackend::BACKEND_OPENGL:
					{
						uint8_t *aData = new uint8_t[mFrame->width * mFrame->height * 4];
						uint8_t *aDest[4] = {aData, nullptr, nullptr, nullptr};
						int aDestLineSize[4] = {mFrame->width * 4, 0, 0, 0};
						sws_scale(mSwSContext, mFrame->data, mFrame->linesize, 0, mFrame->height, aDest, aDestLineSize);

						glBindTexture(GL_TEXTURE_2D, *(GLuint *)mVideoTexture);
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mFrame->width, mFrame->height, GL_RGBA, GL_UNSIGNED_BYTE, aData);
						delete[] aData;

					}
#endif
				}
			}
			av_packet_unref(&mPacket);
		}
	}
}
