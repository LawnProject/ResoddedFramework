#pragma once

#include "Widget.h"
#include <AL/al.h>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

namespace Sexy
{
	class SexyAppBase;

	class VideoWidget : public Widget
	{

	  public:
		SexyAppBase *mApp;

	  public:
		VideoWidget();
		VideoWidget(SexyAppBase *theApp);
		~VideoWidget();

		void Play();
		bool Playing();
		bool IsFinished();

		void LoadVideo(const SexyString &aPath);
		virtual void Draw(Graphics *g);
		virtual void Update();

		double GetAudioClock();

		double GetFrameTime();

	  private:
		void *mVideoTexture;

		bool mFinished;
		bool mVideoStartedRendering;
		int mVidStreamIndex;
		int mVidWidth;
		int mVidHeight;
		AVFormatContext *mFormatContext;
		const AVCodec *mVideoCodec;
		AVCodecContext *mVideoDecoder;
		AVPacket mPacket;
		AVFrame *mVideoFrame;
		AVFrame *mAudioFrame;
		SwsContext *mSwSContext;

		AVCodecContext *mAudioDecoder;
		const AVCodec *mAudioCodec;
		int mAudioStreamIndex;
		SwrContext *mSwrContext;
		ALuint mVidAudioSource;
		ALuint mVidAudioBuffers[4];
		int mAudioChannels;
		ALenum mAudioFormat;
		int64_t mPlayStartTick;
		double mPrebufferDuration = 0.0;
	};
}