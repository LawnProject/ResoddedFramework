#include "OpenALSoundInstance.h"
#include "OpenALSoundManager.h"

using namespace Sexy;


OpenALSoundInstance::OpenALSoundInstance(OpenALSoundManager* theSoundManager, ALuint theSourceSound)
{
	mSoundManagerP = theSoundManager;
	mReleased = false;
	mAutoRelease = false;
	mHasPlayed = false;
	mSourceSoundBuffer = theSourceSound;
	mSoundSource = AL_NONE;

	mBaseVolume = 1.0;
	mBasePan = 0;

	mVolume = 1.0;
	mPan = 0;

	mDefaultFrequency = 44100;

	if (mSourceSoundBuffer != AL_NONE)
	{
		alGenSources((ALuint)1, &mSoundSource);
		alSourcei(mSoundSource, AL_BUFFER, mSourceSoundBuffer);
		alGetBufferi(mSourceSoundBuffer, AL_FREQUENCY, &mDefaultFrequency);
	}

	RehupVolume();
}

OpenALSoundInstance::~OpenALSoundInstance()
{
	if (mSoundSource != AL_NONE)
		alDeleteSources(1, &mSoundSource);
	mSoundSource = AL_NONE;
}

void OpenALSoundInstance::RehupVolume()
{
	if (mSoundSource != AL_NONE)
		alSourcef(mSoundSource, AL_GAIN, mBaseVolume * mVolume * mSoundManagerP->mMasterVolume);
}

void OpenALSoundInstance::RehupPan()
{
	if (mSoundSource != AL_NONE)
	{
		alSourcei(mSoundSource, AL_SOURCE_RELATIVE, AL_TRUE);
		alSource3f(mSoundSource, AL_POSITION, mBasePan + mPan, 0, -1.0f);
	}
		
}

void OpenALSoundInstance::Release()
{
	Stop();
	mReleased = true;
}

void OpenALSoundInstance::SetVolume(double theVolume)
{
	mVolume = theVolume;
	RehupVolume();
}

void OpenALSoundInstance::SetPan(float thePosition)
{
	mPan = thePosition;
	RehupPan();
}

void OpenALSoundInstance::SetBaseVolume(double theBaseVolume)
{
	mBaseVolume = theBaseVolume;
	RehupVolume();
}

void OpenALSoundInstance::SetBasePan(float theBasePan)
{
	mBasePan = theBasePan;
	RehupPan();
}

bool OpenALSoundInstance::Play(bool looping, bool autoRelease)
{
	Stop();

	mHasPlayed = true;
	mAutoRelease = autoRelease;

	if (mSoundSource == AL_NONE)
	{
		return false;
	}

	alSourcei(mSoundSource, AL_LOOPING, looping);
	alSourcePlay(mSoundSource);

	return true;
}

void OpenALSoundInstance::Stop()
{
	if (mSoundSource != AL_NONE)
	{
		alSourceStop(mSoundSource);
		alSourcef(mSoundSource, AL_SEC_OFFSET, 0);
		mAutoRelease = false;
	}
}

void OpenALSoundInstance::AdjustPitch(double theNumSteps)
{
	if (mSoundSource != AL_NONE)
	{
		float aFrequencyMult = pow(1.0594630943592952645618252949463, theNumSteps);

		alSourcef(mSoundSource, AL_PITCH, aFrequencyMult);
	}
}

bool OpenALSoundInstance::IsPlaying()
{
	if (!mHasPlayed)
		return false;

	if (mSoundSource == AL_NONE)
		return false;

	ALint aState;
	alGetSourcei(mSoundSource, AL_SOURCE_STATE, &aState);
	return aState == AL_PLAYING;
}

bool OpenALSoundInstance::IsReleased()
{
	if ((!mReleased) && (mAutoRelease) && (mHasPlayed) && (!IsPlaying()))
		Release();

	return mReleased;
}

double OpenALSoundInstance::GetVolume()
{
	return mVolume;
}
