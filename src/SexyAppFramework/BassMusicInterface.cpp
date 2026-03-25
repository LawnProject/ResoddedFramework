#include "BassMusicInterface.h"
#include <bass.h>
#include "..\PakLib\PakInterface.h"

using namespace Sexy;

BassMusicInfo::BassMusicInfo()
{
	mVolume = 0.0;
	mVolumeAdd = 0.0;
	mVolumeCap = 1.0;
	mStopOnFade = false;
	mHMusic = NULL;
	mHStream = NULL;
}

BassMusicInterface::BassMusicInterface(Window *theWindow)
{
	BOOL success;
	#if WIN32
	SDL_PropertiesID props = SDL_GetWindowProperties(theWindow->mInternalWindow);
	HWND aHWND = NULL;
	SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, aHWND);
	success = BASS_Init(1, 44100, 0, aHWND, NULL);
	#else
	success = BASS_Init(1, 44100, 0, NULL, NULL);
	#endif
	BASS_SetConfig(BASS_CONFIG_BUFFER, 2000);
	BASS_SetConfig(BASS_CONFIG_FLOATDSP, TRUE);

	BASS_Start();

	mMaxMusicVolume = 40;

	mMusicLoadFlags = BASS_MUSIC_LOOP | BASS_MUSIC_RAMP | BASS_MUSIC_PRESCAN;
}

BassMusicInterface::~BassMusicInterface()
{
	BASS_Stop();
	BASS_Free();
}

bool BassMusicInterface::LoadMusic(int theSongId, const std::string &theFileName)
{
	HMUSIC aHMusic = NULL;
	HSTREAM aStream = NULL;

	std::string anExt;
	int aDotPos = theFileName.find_last_of('.');
	if (aDotPos != std::string::npos)
		anExt = StringToLower(theFileName.substr(aDotPos + 1));

	PFILE *aFP = p_fopen(theFileName.c_str(), "rb");
	if (!aFP)
		return false;

	p_fseek(aFP, 0, SEEK_END);
	int aSize = p_ftell(aFP);
	p_fseek(aFP, 0, SEEK_SET);

	uint8_t *aData = new uint8_t[aSize];
	p_fread(aData, 1, aSize, aFP);
	p_fclose(aFP);

	if (anExt == "wav" || anExt == "ogg" || anExt == "mp3")
		aStream = BASS_StreamCreateFile(TRUE, aData, 0, aSize, BASS_SAMPLE_LOOP);
	else
	{
		aHMusic = BASS_MusicLoad(TRUE, aData, 0, aSize, BASS_MUSIC_LOOP | BASS_MUSIC_RAMP, 0);
		delete[] aData;
	}


	if (aHMusic == NULL && aStream == NULL)
		return false;

	BassMusicInfo aMusicInfo;
	aMusicInfo.mHMusic = aHMusic;
	aMusicInfo.mHStream = aStream;
	mMusicMap.insert(BassMusicMap::value_type(theSongId, aMusicInfo));

	return true;
}

void BassMusicInterface::PlayMusic(int theSongId, int theOffset, bool noLoop)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		aMusicInfo->mVolume = aMusicInfo->mVolumeCap;
		aMusicInfo->mVolumeAdd = 0.0;
		aMusicInfo->mStopOnFade = noLoop;
		BASS_ChannelSetAttribute(aMusicInfo->GetHandle(), BASS_ATTRIB_VOL, aMusicInfo->mVolume * 100);

		BASS_ChannelStop(aMusicInfo->GetHandle());
		if (aMusicInfo->mHMusic)
		{
			BASS_ChannelStop(aMusicInfo->mHMusic);
			BASS_ChannelSetPosition(aMusicInfo->mHMusic, MAKELONG(theOffset, 0), BASS_POS_MUSIC_ORDER);
			BASS_ChannelFlags(aMusicInfo->mHMusic, BASS_MUSIC_POSRESET | BASS_MUSIC_RAMP | (noLoop ? 0 : BASS_MUSIC_LOOP), -1);

			BASS_ChannelPlay(aMusicInfo->mHMusic, true);
		}
		else
		{
			BOOL flush = theOffset == -1 ? FALSE : TRUE;
			BASS_ChannelStop(aMusicInfo->mHStream);
			BASS_ChannelSetPosition(aMusicInfo->mHStream, 0, BASS_POS_BYTE);
			BASS_ChannelFlags(aMusicInfo->mHStream, noLoop ? 0 : BASS_MUSIC_LOOP, -1);
			BASS_ChannelPlay(aMusicInfo->mHStream, flush);
			if (theOffset > 0)
				BASS_ChannelSetPosition(aMusicInfo->mHStream, theOffset, BASS_POS_BYTE);
		}
	}
}

void BassMusicInterface::StopMusic(int theSongId)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		aMusicInfo->mVolume = 0.0;
		BASS_ChannelStop(aMusicInfo->GetHandle());
	}
}

void BassMusicInterface::StopAllMusic()
{
	BassMusicMap::iterator anItr = mMusicMap.begin();
	while (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		aMusicInfo->mVolume = 0.0;
		BASS_ChannelStop(aMusicInfo->GetHandle());
		++anItr;
	}
}

void BassMusicInterface::UnloadMusic(int theSongId)
{
	StopMusic(theSongId);

	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		if (aMusicInfo->mHStream)
			BASS_StreamFree(aMusicInfo->mHStream);
		else if (aMusicInfo->mHMusic)
			BASS_MusicFree(aMusicInfo->mHMusic);

		mMusicMap.erase(anItr);
	}
}

void BassMusicInterface::UnloadAllMusic()
{
	StopAllMusic();
	for (BassMusicMap::iterator anItr = mMusicMap.begin(); anItr != mMusicMap.end(); ++anItr)
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		if (aMusicInfo->mHStream)
			BASS_StreamFree(aMusicInfo->mHStream);
		else if (aMusicInfo->mHMusic)
			BASS_MusicFree(aMusicInfo->mHMusic);
	}
	mMusicMap.clear();
}

void BassMusicInterface::PauseMusic(int theSongId)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		BASS_ChannelPause(aMusicInfo->GetHandle());
	}
}

void BassMusicInterface::PauseAllMusic()
{
	for (BassMusicMap::iterator anItr = mMusicMap.begin(); anItr != mMusicMap.end(); ++anItr)
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		if (BASS_ChannelIsActive(aMusicInfo->GetHandle()) == BASS_ACTIVE_PLAYING)
			BASS_ChannelPause(aMusicInfo->GetHandle());
	}
}

void BassMusicInterface::ResumeAllMusic()
{
	for (BassMusicMap::iterator anItr = mMusicMap.begin(); anItr != mMusicMap.end(); ++anItr)
	{
		BassMusicInfo *aMusicInfo = &anItr->second;

		if (BASS_ChannelIsActive(aMusicInfo->GetHandle()) == BASS_ACTIVE_PAUSED)
			BASS_ChannelPlay(aMusicInfo->GetHandle(), false);
	}
}

void BassMusicInterface::ResumeMusic(int theSongId)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		BASS_ChannelPlay(aMusicInfo->GetHandle(), false);
	}
}

void BassMusicInterface::FadeIn(int theSongId, int theOffset, double theSpeed, bool noLoop)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;

		aMusicInfo->mVolumeAdd = theSpeed;
		aMusicInfo->mStopOnFade = noLoop;

		BASS_ChannelStop(aMusicInfo->GetHandle());
		BASS_ChannelSetAttribute(aMusicInfo->GetHandle(), BASS_ATTRIB_VOL, aMusicInfo->mVolume);
		if (aMusicInfo->mHMusic)
		{
			if (theOffset == -1)
				BASS_ChannelPlay(aMusicInfo->mHMusic, true);
			else
			{
				BASS_ChannelStop(aMusicInfo->mHMusic);
				BASS_ChannelSetPosition(aMusicInfo->mHMusic, MAKELONG(theOffset, 0), BASS_POS_MUSIC_ORDER);
				BASS_ChannelFlags(aMusicInfo->mHMusic, BASS_MUSIC_RAMP | (noLoop ? 0 : BASS_MUSIC_LOOP), -1);

				BASS_ChannelPlay(aMusicInfo->mHMusic, true);
			}
		}
		else
		{
			BOOL flush = theOffset == -1 ? FALSE : TRUE;
			BASS_ChannelStop(aMusicInfo->mHStream);
			BASS_ChannelSetPosition(aMusicInfo->mHStream, 0, BASS_POS_BYTE);
			BASS_ChannelFlags(aMusicInfo->mHStream, noLoop ? 0 : BASS_MUSIC_LOOP, -1);
			BASS_ChannelPlay(aMusicInfo->mHStream, flush);
			if (theOffset > 0)
				BASS_ChannelSetPosition(aMusicInfo->mHStream, theOffset, BASS_POS_BYTE);
		}
	}
}

void BassMusicInterface::FadeOut(int theSongId, bool stopSong, double theSpeed)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;

		if (aMusicInfo->mVolume != 0.0)
		{
			aMusicInfo->mVolumeAdd = -theSpeed;
		}

		aMusicInfo->mStopOnFade = stopSong;
	}
}

void BassMusicInterface::FadeOutAll(bool stopSong, double theSpeed)
{
	BassMusicMap::iterator anItr = mMusicMap.begin();
	while (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;

		aMusicInfo->mVolumeAdd = -theSpeed;
		aMusicInfo->mStopOnFade = stopSong;

		++anItr;
	}
}

void BassMusicInterface::SetVolume(double theVolume)
{
	int aVolume = (int)(theVolume * mMaxMusicVolume);

	BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, aVolume);
	BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, aVolume);
}

void BassMusicInterface::SetSongVolume(int theSongId, double theVolume)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;

		aMusicInfo->mVolume = theVolume;
		BASS_ChannelSetAttribute(aMusicInfo->GetHandle(), BASS_ATTRIB_VOL, aMusicInfo->mVolume * 100);
	}
}

void BassMusicInterface::SetSongMaxVolume(int theSongId, double theMaxVolume)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;

		aMusicInfo->mVolumeCap = theMaxVolume;
		aMusicInfo->mVolume = std::min(aMusicInfo->mVolume, theMaxVolume);
		BASS_ChannelSetAttribute(aMusicInfo->GetHandle(), BASS_ATTRIB_VOL, aMusicInfo->mVolume * 100);
	}
}

bool BassMusicInterface::IsPlaying(int theSongId)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		return BASS_ChannelIsActive(aMusicInfo->GetHandle()) == BASS_ACTIVE_PLAYING;
	}

	return false;
}

void BassMusicInterface::SetMusicAmplify(int theSongId, double theAmp)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		BASS_ChannelSetAttribute(aMusicInfo->GetHandle(), BASS_ATTRIB_MUSIC_AMPLIFY, (int)(theAmp * 100));
	}
}

void BassMusicInterface::Update()
{
	BassMusicMap::iterator anItr = mMusicMap.begin();
	while (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;

		if (aMusicInfo->mVolumeAdd != 0.0)
		{
			aMusicInfo->mVolume += aMusicInfo->mVolumeAdd;

			if (aMusicInfo->mVolume > aMusicInfo->mVolumeCap)
			{
				aMusicInfo->mVolume = aMusicInfo->mVolumeCap;
				aMusicInfo->mVolumeAdd = 0.0;
			}
			else if (aMusicInfo->mVolume < 0.0)
			{
				aMusicInfo->mVolume = 0.0;
				aMusicInfo->mVolumeAdd = 0.0;

				if (aMusicInfo->mStopOnFade)
					BASS_ChannelStop(aMusicInfo->GetHandle());
			}

			BASS_ChannelSetAttribute(aMusicInfo->GetHandle(), BASS_ATTRIB_VOL, aMusicInfo->mVolume * 100);
		}

		++anItr;
	}
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// MODs are broken up into several orders or patterns. This returns the current order a song is on.
int BassMusicInterface::GetMusicOrder(int theSongId)
{
	BassMusicMap::iterator anItr = mMusicMap.find(theSongId);
	if (anItr != mMusicMap.end())
	{
		BassMusicInfo *aMusicInfo = &anItr->second;
		int aPosition = BASS_ChannelGetPosition(aMusicInfo->GetHandle(), BASS_POS_MUSIC_ORDER);
		return aPosition;
	}
	return -1;
}