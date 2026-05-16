#pragma once

#include "Widget.h"
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
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

		void LoadVideo(const SexyString &aPath);
		virtual void Draw(Graphics *g);
		virtual void Update();



	  private:
		void *mVideoTexture;

		int mVidStreamIndex;
		int mVidWidth;
		int mVidHeight;
		AVFormatContext *mFormatContext;
		const AVCodec *mCodec;
		AVCodecContext *mDecoder;
		AVPacket mPacket;
		AVFrame *mFrame;
		SwsContext *mSwSContext;
	};
}