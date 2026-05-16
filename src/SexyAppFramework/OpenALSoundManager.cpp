#include "OpenALSoundManager.h"
#include "OpenALSoundInstance.h"
#include <SDL3/SDL.h>
#include "..\PakLib\PakInterface.h"

#ifdef USE_OGG_LIB
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#endif

using namespace Sexy;

OpenALSoundManager::OpenALSoundManager()
{
	mAudioDevice = alcOpenDevice(nullptr);
	if (!mAudioDevice)
	{
		std::string anErrorString = getErrorString(alGetError());
		printf("[SexyAppFramework] - %s\n", anErrorString.c_str());
		return;
	}

	ALCint attrs[] = {ALC_FREQUENCY, 44100, 0};
	mContext = alcCreateContext(mAudioDevice, attrs);
	if (!alcMakeContextCurrent(mContext))
	{
		std::string anErrorString = getErrorString(alGetError());
		printf("[SexyAppFramework] - %s\n", anErrorString.c_str());
		return;
	}
	for (int i = 0; i < MAX_SOURCE_SOUNDS; i++)
	{
		mSourceSounds[i] = AL_NONE;
		mBaseVolumes[i] = 1;
		mBasePans[i] = 0;
	}

	for (int i = 0; i < MAX_CHANNELS; i++)
		mPlayingSounds[i] = nullptr;

	mMasterVolume = 1.0;

	mLastReleaseTick = 0;
}

bool OpenALSoundManager::Initialized()
{
	return mAudioDevice && mContext;
}

OpenALSoundManager::~OpenALSoundManager()
{
	ReleaseChannels();
	ReleaseSounds();

	alcMakeContextCurrent(nullptr);
	if (mContext)
		alcDestroyContext(mContext);
	if (mAudioDevice)
		alcCloseDevice(mAudioDevice);
}

std::string OpenALSoundManager::getErrorString(ALenum theError)
{
	switch (theError)
	{
	case AL_OUT_OF_MEMORY:
		return "[OpenAL] - Error 40965: Out of memory";
	case AL_INVALID_OPERATION:
		return "[OpenAL] - Error 40964: Invalid Operation";
	case AL_INVALID_VALUE:
		return "[OpenAL] - Error 40963: Invalid Value";
	case AL_INVALID_ENUM:
		return "[OpenAL] - Error 40962: Invalid Enum";
	case AL_INVALID_NAME:
		return "[OpenAL] - Error 40961: Invalid Name";
	case AL_NO_ERROR:
		return "[OpenAL] - Error 0: None";
	}
	return StrFormat("[OpenAL] - Error %d: UNKNOWN", theError);
}

int OpenALSoundManager::FindFreeChannel()
{
	uint64_t aTick = SDL_GetTicks();
	if (aTick - mLastReleaseTick > 1000)
	{
		ReleaseFreeChannels();
		mLastReleaseTick = aTick;
	}

	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		if (mPlayingSounds[i] == nullptr)
			return i;

		if (mPlayingSounds[i]->IsReleased())
		{
			delete mPlayingSounds[i];
			mPlayingSounds[i] = nullptr;
			return i;
		}
	}

	return -1;
}

int Sexy::OpenALSoundManager::VolumeToDB(double theVolume)
{
	int aVol = (int)((log10(1 + theVolume * 9) - 1.0) * 2333);
	if (aVol < -2000)
		aVol = -10000;

	return aVol;
}

SoundInstance *OpenALSoundManager::GetSoundInstance(unsigned int theSfxID)
{
	if (theSfxID > MAX_SOURCE_SOUNDS || mAudioDevice == nullptr)
		return nullptr;

	int aFreeChannel = FindFreeChannel();
	if (aFreeChannel < 0)
		return nullptr;


	if (mSourceSounds[theSfxID] == AL_NONE)
		return nullptr;

	mPlayingSounds[aFreeChannel] = new OpenALSoundInstance(this, mSourceSounds[theSfxID]);

	mPlayingSounds[aFreeChannel]->SetBasePan(mBasePans[theSfxID]);
	mPlayingSounds[aFreeChannel]->SetBaseVolume(mBaseVolumes[theSfxID]);

	return mPlayingSounds[aFreeChannel];
}

#define DR_FLAC_IMPLEMENTATION
#include <dr_flac.h>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>
#include <au_reader.h>

bool OpenALSoundManager::LoadSound(unsigned int theSfxID, const std::string &theFilename)
{
	if ((theSfxID < 0) || (theSfxID >= MAX_SOURCE_SOUNDS))
		return false;

	ReleaseSound(theSfxID);

	if (!mContext)
		return true; // sounds just	won't play, but this is not treated as a failure condition

	mSourceFileNames[theSfxID] = theFilename;

	return 
		LoadWAVSound(theSfxID, theFilename + ".wav") ||
		LoadFLACSound(theSfxID, theFilename + ".flac") || 
		LoadMP3Sound(theSfxID, theFilename + ".mp3") || 
		LoadOGGSound(theSfxID, theFilename + ".ogg") ||
		LoadAUSound(theSfxID, theFilename + ".au");
}

#ifdef USE_OGG_LIB

static int p_fseek64_wrap(PFILE *f, ogg_int64_t off, int whence)
{
	if (f == NULL)
		return (-1);
	return p_fseek(f, off, whence);
}

int ov_pak_open(PFILE *f, OggVorbis_File *vf, char *initial, long ibytes)
{
	ov_callbacks callbacks = {(size_t (*)(void *, size_t, size_t, void *))p_fread,
							  (int (*)(void *, ogg_int64_t, int))p_fseek64_wrap,
							  (int (*)(void *))p_fclose,
							  (long (*)(void *))p_ftell};

	return ov_open_callbacks((void *)f, vf, initial, ibytes, callbacks);
}

bool OpenALSoundManager::LoadOGGSound(unsigned int theSfxID, const std::string &theFilename)
{
	OggVorbis_File vf;
	int current_section = 0;

	PFILE *aFile = p_fopen(theFilename.c_str(), "rb");
	if (!aFile)
		return false;

	if (ov_pak_open(aFile, &vf, NULL, 0) < 0)
	{
		p_fclose(aFile);
		return false;
	}

	vorbis_info *anInfo = ov_info(&vf, -1);

	ALenum format;
	if (anInfo->channels == 1)
		format = AL_FORMAT_MONO16;
	else if (anInfo->channels == 2)
		format = AL_FORMAT_STEREO16;
	else
	{
		ov_clear(&vf);
		p_fclose(aFile);
		return false;
	}

	ogg_int64_t aLenBytes = ov_pcm_total(&vf, -1) * anInfo->channels * sizeof(int16_t);

	char *aBuf = new char[aLenBytes];

	char *aPtr = aBuf;
	ogg_int64_t aNumBytes = aLenBytes;
	while (aNumBytes > 0)
	{
		long ret = ov_read(&vf, aPtr, std::min(aNumBytes, (int64_t)4096), 0, 2, 1, &current_section);

		if (ret == 0)
			break;
		else if (ret < 0)
		{
			// this means something is wrong
			delete[] aBuf;
			ov_clear(&vf);
			return false;
		}
		else
		{
			aPtr += ret;
			aNumBytes -= ret;
		}
	}
	ALuint aBuffer;
	alGenBuffers(1, &aBuffer);
	ogg_int64_t aDecodedBytes = aLenBytes - aNumBytes;
	alBufferData(aBuffer, format, aBuf, aDecodedBytes, anInfo->rate);

	mSourceSounds[theSfxID] = aBuffer;

	delete[] aBuf;
	ov_clear(&vf);

	return true;
}
#else
bool OpenALSoundManager::LoadOGGSound(unsigned int theSfxID, const std::string &theFilename)
{
	return false;
}
#endif

bool OpenALSoundManager::LoadMP3Sound(unsigned int theSfxID, const std::string &theFilename)
{
	int aDataSize;

	PFILE *aPakFile;

	aPakFile = p_fopen(theFilename.c_str(), "rb");

	if (aPakFile == nullptr)
		return false;

	p_fseek(aPakFile, 0, SEEK_END);
	int aSize = p_ftell(aPakFile);
	p_fseek(aPakFile, 0, SEEK_SET);
	void *aData = operator new[](aSize);
	p_fread(aData, sizeof(uint8_t), aSize, aPakFile);
	p_fclose(aPakFile);

	drmp3 aMP3;
	if (!drmp3_init_memory(&aMP3, aData, aSize, nullptr))
	{
		delete[] aData;
		return false;
	}

	aSize = aMP3.totalPCMFrameCount * aMP3.channels * sizeof(drmp3_int32);
	int16_t *pDecodedInterleavedPCMFrames = (int16_t *)malloc(aSize);
	uint64_t numberOfSamplesActuallyDecoded = drmp3_read_pcm_frames_s16(&aMP3, aMP3.totalPCMFrameCount, pDecodedInterleavedPCMFrames);

	ALuint buffer;
	alGenBuffers(1, &buffer);
	alBufferData(buffer,
				 aMP3.channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
				 pDecodedInterleavedPCMFrames,
				 aSize,
				 numberOfSamplesActuallyDecoded);

	mSourceDataSizes[theSfxID] = aSize;
	mSourceSounds[theSfxID] = buffer;

	free(pDecodedInterleavedPCMFrames);
	drmp3_uninit(&aMP3);
	delete[] aData;

	return true;
}

bool OpenALSoundManager::LoadWAVSound(unsigned int theSfxID, const std::string &theFilename)
{
	int aDataSize;

	PFILE* aPakFile;

	aPakFile = p_fopen(theFilename.c_str(), "rb");

	if (aPakFile == nullptr)
		return false;

	p_fseek(aPakFile, 0, SEEK_END);
	int aSize = p_ftell(aPakFile);
	p_fseek(aPakFile, 0, SEEK_SET);
	void *aData = operator new[](aSize);
	p_fread(aData, sizeof(char), aSize, aPakFile);
	p_fclose(aPakFile);

	drwav aWAV;
	if (!drwav_init_memory(&aWAV, aData, aSize, nullptr))
	{
		return false;
	}

	aSize = aWAV.totalPCMFrameCount * aWAV.channels * sizeof(drwav_int32);
	drwav_int32 *pDecodedInterleavedPCMFrames = (drwav_int32*)malloc(aSize);
	size_t numberOfSamplesActuallyDecoded = drwav_read_pcm_frames_s32(&aWAV, aWAV.totalPCMFrameCount, pDecodedInterleavedPCMFrames);

	ALuint buffer;
	alGenBuffers(1, &buffer);
	alBufferData(buffer,
				 aWAV.channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
				 pDecodedInterleavedPCMFrames,
				 aSize,
				 numberOfSamplesActuallyDecoded);

	mSourceDataSizes[theSfxID] = aSize;
	mSourceSounds[theSfxID] = buffer;

	free(pDecodedInterleavedPCMFrames);
	drwav_uninit(&aWAV);

	return true;
}

bool OpenALSoundManager::LoadFLACSound(unsigned int theSfxID, const std::string &theFilename)
{
	int aDataSize;

	PFILE *aPakFile;

	aPakFile = p_fopen(theFilename.c_str(), "rb");

	if (aPakFile == nullptr)
		return false;

	p_fseek(aPakFile, 0, SEEK_END);
	int aSize = p_ftell(aPakFile);
	p_fseek(aPakFile, 0, SEEK_SET);
	void *aData = operator new[](aSize);
	p_fread(aData, sizeof(char), aSize, aPakFile);
	p_fclose(aPakFile);

	drflac *aFLAC = drflac_open_memory(aData, aSize, nullptr);

	aSize = aFLAC->totalPCMFrameCount * aFLAC->channels * sizeof(drwav_int32);
	drwav_int32 *pDecodedInterleavedPCMFrames = (drflac_int32 *)malloc(aSize);
	size_t numberOfSamplesActuallyDecoded =
		drflac_read_pcm_frames_s32(aFLAC, aFLAC->totalPCMFrameCount, pDecodedInterleavedPCMFrames);

	ALuint buffer;
	alGenBuffers(1, &buffer);
	alBufferData(buffer,
				 aFLAC->channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
				 pDecodedInterleavedPCMFrames,
				 aSize,
				 numberOfSamplesActuallyDecoded);

	mSourceDataSizes[theSfxID] = aSize;
	mSourceSounds[theSfxID] = buffer;

	drflac_free(pDecodedInterleavedPCMFrames, nullptr);

	return true;
}

bool OpenALSoundManager::LoadAUSound(unsigned int theSfxID, const std::string &theFilename)
{
	PFILE *fp = p_fopen(theFilename.c_str(), "rb");
	if (!fp)
		return false;

	p_fseek(fp, 0, SEEK_END);
	size_t fileSize = p_ftell(fp);
	p_fseek(fp, 0, SEEK_SET);
	uint8_t *data = new uint8_t[fileSize];
	p_fread(data, 1, fileSize, fp);
	p_fclose(fp);

	AuFile anAUFile;
	if (!LoadAU(data, fileSize, anAUFile))
	{
		delete[] data;
		return false;
	}

	mSourceDataSizes[theSfxID] = anAUFile.mSamples.size();

	ALuint buffer;
	alGenBuffers(1, &buffer);
	alBufferData(buffer,
				 anAUFile.mChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
				 anAUFile.mSamples.data(),
				 anAUFile.mSamples.size() * sizeof(int16_t),
				 anAUFile.mSampleRate);

	mSourceSounds[theSfxID] = buffer;

	delete[] data;

	return true;
}

bool Sexy::OpenALSoundManager::WriteWAV(unsigned int theSfxID,
										const std::string &theFilename,
										const std::string &theDepFile)
{
	return false;
}

int OpenALSoundManager::LoadSound(const std::string &theFilename)
{
	int i;
	for (i = 0; i < MAX_SOURCE_SOUNDS; i++)
		if (mSourceFileNames[i] == theFilename)
			return i;

	for (i = MAX_SOURCE_SOUNDS - 1; i >= 0; i--)
	{
		if (mSourceSounds[i] == NULL)
		{
			if (!LoadSound(i, theFilename))
				return -1;
			else
				return i;
		}
	}

	return -1;
}

int OpenALSoundManager::GetFreeSoundId()
{
	for (int i = 0; i < MAX_SOURCE_SOUNDS; i++)
	{
		if (mSourceSounds[i] == NULL)
			return i;
	}

	return -1;
}

int OpenALSoundManager::GetNumSounds()
{
	int aCount = 0;
	for (int i = 0; i < MAX_SOURCE_SOUNDS; i++)
	{
		if (mSourceSounds[i] != NULL)
			aCount++;
	}

	return aCount;
}

bool OpenALSoundManager::SetBaseVolume(unsigned int theSfxID, double theBaseVolume)
{
	if ((theSfxID < 0) || (theSfxID >= MAX_SOURCE_SOUNDS))
		return false;

	mBaseVolumes[theSfxID] = theBaseVolume;
	return true;
}

bool OpenALSoundManager::SetBasePan(unsigned int theSfxID, float theBasePan)
{
	if ((theSfxID < 0) || (theSfxID >= MAX_SOURCE_SOUNDS))
		return false;

	mBasePans[theSfxID] = theBasePan;
	return true;
}

void OpenALSoundManager::ReleaseSounds()
{
	for (int i = 0; i < MAX_SOURCE_SOUNDS; i++)
		if (mSourceSounds[i] != AL_NONE)
		{
			alDeleteSources(1, &mSourceSounds[i]);
			mSourceSounds[i] = AL_NONE;
		}
}

void OpenALSoundManager::ReleaseSound(unsigned int theSfxID)
{
	if (mSourceSounds[theSfxID])
	{
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			if (mPlayingSounds[i] != NULL && mPlayingSounds[i]->mSourceSoundBuffer == mSourceSounds[theSfxID])
			{
				delete mPlayingSounds[i];
				mPlayingSounds[i] = NULL;
			}
		}
		alDeleteBuffers(1, &mSourceSounds[theSfxID]);
		mSourceSounds[theSfxID] = 0;
		mSourceFileNames[theSfxID] = "";
	}
}

void OpenALSoundManager::ReleaseChannels()
{
	for (int i = 0; i < MAX_CHANNELS; i++)
		if (mPlayingSounds[i] != nullptr)
		{
			delete mPlayingSounds[i];
			mPlayingSounds[i] = nullptr;
		}
}

bool Sexy::OpenALSoundManager::GetTheFileTime(const std::string &theDepFile, uint64_t *theFileTime)
{
	return false;
}

void OpenALSoundManager::ReleaseFreeChannels()
{
	for (int i = 0; i < MAX_CHANNELS; i++)
		if (mPlayingSounds[i] != nullptr && mPlayingSounds[i]->IsReleased())
		{
			delete mPlayingSounds[i];
			mPlayingSounds[i] = nullptr;
		}
}

void OpenALSoundManager::StopAllSounds()
{
	for (int i = 0; i < MAX_CHANNELS; i++)
		if (mPlayingSounds[i] != nullptr)
		{
			bool isAutoRelease = mPlayingSounds[i]->mAutoRelease;
			mPlayingSounds[i]->Stop();
			mPlayingSounds[i]->mAutoRelease = isAutoRelease;
		}
}

void OpenALSoundManager::SetVolume(double theVolume)
{
	mMasterVolume = theVolume;

	for (int i = 0; i < MAX_CHANNELS; i++)
		if (mPlayingSounds[i] != nullptr)
			mPlayingSounds[i]->RehupVolume();
}

void OpenALSoundManager::SetMasterVolume(double theVolume)
{
	SetVolume(theVolume);
}

double OpenALSoundManager::GetMasterVolume()
{
	return mMasterVolume;
}

void OpenALSoundManager::Flush()
{
}

void OpenALSoundManager::SetCooperativeWindow(bool isWindowed)
{
}