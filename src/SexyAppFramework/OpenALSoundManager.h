#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include "SoundManager.h"

namespace Sexy
{

class OpenALSoundInstance;

class OpenALSoundManager : public SoundManager
{
	friend class OpenALSoundInstance;

  protected:
	ALuint mSourceSounds[MAX_SOURCE_SOUNDS];
	std::string mSourceFileNames[MAX_SOURCE_SOUNDS];
	//LPDIRECTSOUNDBUFFER mPrimaryBuffer;
	uint32_t mSourceDataSizes[MAX_SOURCE_SOUNDS];
	double mBaseVolumes[MAX_SOURCE_SOUNDS];
	int mBasePans[MAX_SOURCE_SOUNDS];
	OpenALSoundInstance *mPlayingSounds[MAX_CHANNELS];
	double mMasterVolume;
	uint64_t mLastReleaseTick;

  protected:
	int FindFreeChannel();
	int VolumeToDB(double theVolume);
	bool LoadOGGSound(unsigned int theSfxID, const std::string &theFilename);
	bool LoadMP3Sound(unsigned int theSfxID, const std::string &theFilename);
	bool LoadFLACSound(unsigned int theSfxID, const std::string &theFilename);
	bool LoadWAVSound(unsigned int theSfxID, const std::string &theFilename);
	bool LoadAUSound(unsigned int theSfxID, const std::string &theFilename);
	bool WriteWAV(unsigned int theSfxID, const std::string &theFilename, const std::string &theDepFile);
	bool GetTheFileTime(const std::string &theDepFile, FILETIME *theFileTime);
	void ReleaseFreeChannels();

  public:
	ALCdevice* mAudioDevice;
	ALCcontext* mContext;

	OpenALSoundManager();
	virtual ~OpenALSoundManager();

	virtual bool Initialized();

	virtual bool LoadSound(unsigned int theSfxID, const std::string &theFilename);
	virtual int LoadSound(const std::string &theFilename);
	virtual void ReleaseSound(unsigned int theSfxID);

	virtual void SetVolume(double theVolume);
	virtual bool SetBaseVolume(unsigned int theSfxID, double theBaseVolume);
	virtual bool SetBasePan(unsigned int theSfxID, int theBasePan);

	virtual SoundInstance *GetSoundInstance(unsigned int theSfxID);

	virtual void ReleaseSounds();
	virtual void ReleaseChannels();

	virtual double GetMasterVolume();
	virtual void SetMasterVolume(double theVolume);

	virtual void Flush();

	virtual void SetCooperativeWindow(bool isWindowed);
	virtual void StopAllSounds();
	virtual int GetFreeSoundId();
	virtual int GetNumSounds();

	virtual std::string getErrorString(ALenum theError);
};

} // namespace Sexy