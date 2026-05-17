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
	mVideoDecoder = nullptr;
	mFormatContext = nullptr;
	mVideoCodec = nullptr;
	mVideoTexture = nullptr;
	mSwSContext = nullptr;
	mVideoFrame = av_frame_alloc();
	mAudioFrame = av_frame_alloc();

	mSwrContext = nullptr;
	mAudioDecoder = nullptr;
	mAudioCodec = nullptr;
	mAudioStreamIndex = 0;

	mVidAudioSource = 0;
	mVidWidth = 0;
	mVidHeight = 0;
	mVidStreamIndex = 0;
	mPlayStartTick = -1;
	mFinished = false;
	mVideoStartedRendering = false;
}

VideoWidget::~VideoWidget()
{
	swr_free(&mSwrContext);
	sws_freeContext(mSwSContext);
	avformat_close_input(&mFormatContext);
	av_frame_unref(mVideoFrame);
	av_frame_unref(mAudioFrame);
	av_frame_free(&mVideoFrame);
	av_frame_free(&mAudioFrame);
	av_packet_unref(&mPacket);
	avcodec_free_context(&mVideoDecoder);
	avcodec_free_context(&mAudioDecoder);
	alSourceStop(mVidAudioSource);

	ALint aQueued = 0;
	alGetSourcei(mVidAudioSource, AL_BUFFERS_QUEUED, &aQueued);
	while (aQueued > 0)
	{
		ALuint aBuf;
		alSourceUnqueueBuffers(mVidAudioSource, 1, &aBuf);
		aQueued--;
	}

	alDeleteSources(1, &mVidAudioSource);
	alDeleteBuffers(4, mVidAudioBuffers);
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
				glDeleteTextures(1, (GLuint *)mVideoTexture); 
				delete (GLuint *)mVideoTexture;
				break;
			}
#endif
		}
		mVideoTexture = nullptr;

	}
}

void VideoWidget::Play()
{
	mPlayStartTick = SDL_GetTicks() - 100;
	alSourcePlay(mVidAudioSource);
}

bool VideoWidget::Playing()
{
	return mPlayStartTick != -1;
}

bool VideoWidget::IsFinished()
{
	return mFinished;
}

void VideoWidget::LoadVideo(const SexyString &aPath)
{
	printf("[SexyAppFramework] - Loading Video: %s\n", aPath.c_str());
	av_log_set_level(AV_LOG_ERROR);

	//Video
	avformat_open_input(&mFormatContext, aPath.c_str(), nullptr, nullptr);
	mVidStreamIndex = av_find_best_stream(mFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &mVideoCodec, 0);
	mVideoDecoder = avcodec_alloc_context3(mVideoCodec);
	mVideoDecoder->thread_count = 0;
	avcodec_parameters_to_context(mVideoDecoder, mFormatContext->streams[mVidStreamIndex]->codecpar);
	avcodec_open2(mVideoDecoder, mVideoCodec, nullptr);

	AVCodecParameters *aParameters = mFormatContext->streams[mVidStreamIndex]->codecpar;
	mVidWidth = aParameters->width;
	mVidHeight = aParameters->height;
	while (av_read_frame(mFormatContext, &mPacket) >= 0)
	{
		if (mPacket.stream_index == mVidStreamIndex)
		{
			avcodec_send_packet(mVideoDecoder,& mPacket);
			if (avcodec_receive_frame(mVideoDecoder, mVideoFrame) == 0)
				break;
		}
	}
	mSwSContext = sws_getContext(mVidWidth, mVidHeight, (AVPixelFormat)mVideoFrame->format, mVidWidth, mVidHeight, AV_PIX_FMT_RGB0, SWS_BILINEAR, nullptr, nullptr, nullptr);

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
			mVideoTexture = SDL_CreateTexture(((SDL3Renderer *)mApp->mRenderer)->mBackendRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, mVidWidth, mVidHeight);

			break;
		}
#endif
	}


	//Audio

	mAudioStreamIndex = av_find_best_stream(mFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &mAudioCodec, 0);
	mAudioDecoder = avcodec_alloc_context3(mAudioCodec);
	avcodec_parameters_to_context(mAudioDecoder, mFormatContext->streams[mAudioStreamIndex]->codecpar);
	avcodec_open2(mAudioDecoder, mAudioCodec, nullptr);

	AVChannelLayout anOutLayout = AV_CHANNEL_LAYOUT_STEREO;
	AVChannelLayout anInLayout = mAudioDecoder->ch_layout;

	swr_alloc_set_opts2(&mSwrContext, &anOutLayout, AV_SAMPLE_FMT_S16, 44100, &anInLayout, mAudioDecoder->sample_fmt, mAudioDecoder->sample_rate, 0, nullptr);
	mAudioChannels = mAudioDecoder->ch_layout.nb_channels;
	mAudioFormat = (mAudioChannels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	swr_init(mSwrContext);

	alGenSources(1, &mVidAudioSource);
	alGenBuffers(4, mVidAudioBuffers);

	for (int i = 0; i < 4; i++)
	{
		ALuint aCurrentBuffer = mVidAudioBuffers[i];
		while (av_read_frame(mFormatContext, &mPacket) >= 0)
		{
			if (mPacket.stream_index == mAudioStreamIndex)
			{
				avcodec_send_packet(mAudioDecoder, &mPacket);
				av_packet_unref(&mPacket);
				if (avcodec_receive_frame(mAudioDecoder, mAudioFrame) == 0)
				{
					uint8_t *aOutput = nullptr;
					int aLineSize = 0;
					av_samples_alloc(&aOutput, &aLineSize, mAudioChannels, mAudioFrame->nb_samples, AV_SAMPLE_FMT_S16,
									 0);
					int aNumSamples = swr_convert(mSwrContext, &aOutput, mAudioFrame->nb_samples,
												  (const uint8_t **)mAudioFrame->data, mAudioFrame->nb_samples);
					int aSize = aNumSamples * mAudioChannels * sizeof(int16_t);
					alBufferData(aCurrentBuffer, mAudioFormat, aOutput, aSize, 44100);
					alSourceQueueBuffers(mVidAudioSource, 1, &aCurrentBuffer);
					av_freep(&aOutput);
					break;
				}
			}
			else
			{
				av_packet_unref(&mPacket);
			}
		}
	}

	av_seek_frame(mFormatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
	avcodec_flush_buffers(mVideoDecoder);
	avcodec_flush_buffers(mAudioDecoder);
}

void VideoWidget::Draw(Graphics *g)
{
	if (mVideoTexture == nullptr)
		return;
	if (mVideoStartedRendering)
		g->mDestImage->BltRawTexture(mVideoTexture, mVidWidth, mVidHeight,
									 Rect(mX + g->mTransX, mY + g->mTransY, mWidth, mHeight),
									 Rect(0, 0, mVidWidth, mVidHeight), g->mClipRect, g->mColor, g->mDrawMode);
	else
	{
		g->PushState();
		g->SetColor(Color(0, 0, 0));
		g->FillRect(mX + g->mTransX, mY + g->mTransY, mWidth, mHeight);
		g->PopState();
	}
}

double VideoWidget::GetAudioClock()
{
	return (SDL_GetTicks() - mPlayStartTick) / 1000.0;
}


double VideoWidget::GetFrameTime()
{
	AVStream *aStream = mFormatContext->streams[mVidStreamIndex];
	int64_t aPts = mVideoFrame->best_effort_timestamp;
	double aTime = (double)(aPts * av_q2d(aStream->time_base));
	return aTime;
}

void VideoWidget::Update()
{
	if (mVideoTexture == nullptr || mPlayStartTick == -1)
		return;

	
	ALint aState;
	alGetSourcei(mVidAudioSource, AL_SOURCE_STATE, &aState);
	if (aState != AL_PLAYING)
	{
		alSourcePlay(mVidAudioSource);
	}

	ALint aProcessedBuffers = 0;
	alGetSourcei(mVidAudioSource, AL_BUFFERS_PROCESSED, &aProcessedBuffers);
	bool aNeedsAudio = aProcessedBuffers > 0;
	double aAudioClock = GetAudioClock();
	bool aNeedsVideo = GetFrameTime() <= aAudioClock;

	while ((aNeedsVideo || aNeedsAudio) && av_read_frame(mFormatContext, &mPacket) >= 0)
	{
		aAudioClock = GetAudioClock();

		if (mPacket.stream_index == mVidStreamIndex)
		{
			avcodec_send_packet(mVideoDecoder, &mPacket);
			if (avcodec_receive_frame(mVideoDecoder, mVideoFrame) == 0)
			{
				switch (mApp->mRenderer->mCurrentBackend)
				{
	#if SEXY_USE_SDL3_RENDERER
					case RenderingBackend::BACKEND_SDL3: {
						if (mVideoFrame->data[0] && mVideoFrame->data[1] && mVideoFrame->data[2])
						{
							SDL_UpdateYUVTexture((SDL_Texture *)mVideoTexture, nullptr, mVideoFrame->data[0],
												 mVideoFrame->linesize[0], mVideoFrame->data[1], mVideoFrame->linesize[1],
												 mVideoFrame->data[2], mVideoFrame->linesize[2]);
						}
						break;
					}
	#endif
	#if SEXY_USE_OPENGL
					case RenderingBackend::BACKEND_OPENGL: {
						uint8_t *aData = new uint8_t[mVideoFrame->width * mVideoFrame->height * 4];
						uint8_t *aDest[4] = {aData, nullptr, nullptr, nullptr};
						int aDestLineSize[4] = {mVideoFrame->width * 4, 0, 0, 0};
						sws_scale(mSwSContext, mVideoFrame->data, mVideoFrame->linesize, 0, mVideoFrame->height, aDest,
								  aDestLineSize);

						glBindTexture(GL_TEXTURE_2D, *(GLuint *)mVideoTexture);
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mVideoFrame->width, mVideoFrame->height, GL_RGBA,
										GL_UNSIGNED_BYTE, aData);
						delete[] aData;
					}
	#endif
				}

				mVideoStartedRendering = true;
				aNeedsVideo = GetFrameTime() <= aAudioClock;
			}
		}
		else if (mPacket.stream_index == mAudioStreamIndex)
		{
			avcodec_send_packet(mAudioDecoder, &mPacket);
			while (avcodec_receive_frame(mAudioDecoder, mAudioFrame) == 0)
			{
				if (aProcessedBuffers > 0)
				{
					ALuint aCurrentBuffer;
					alSourceUnqueueBuffers(mVidAudioSource, 1, &aCurrentBuffer);

					uint8_t *aOutput = nullptr;
					int aLineSize = 0;

					av_samples_alloc(&aOutput, &aLineSize, mAudioChannels, mAudioFrame->nb_samples, AV_SAMPLE_FMT_S16, 0);

					int aNumSamples = swr_convert(mSwrContext, &aOutput, mAudioFrame->nb_samples, mAudioFrame->data,
												  mAudioFrame->nb_samples);
					int aSize = aNumSamples * mAudioChannels * sizeof(int16_t);
					alBufferData(aCurrentBuffer, mAudioFormat, aOutput, aSize, 44100);
					alSourceQueueBuffers(mVidAudioSource, 1, &aCurrentBuffer);
					av_freep(&aOutput);
					aProcessedBuffers--;
					aNeedsAudio = aProcessedBuffers > 0;
				}
			}
		}

		av_packet_unref(&mPacket);

	}
	if (aNeedsVideo || aNeedsAudio)
		mFinished = true;
}
