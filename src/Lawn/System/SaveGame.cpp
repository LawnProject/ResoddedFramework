#include "Music.h"
#include "../CursorObject.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/EffectSystem.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "SaveGame.h"

static constexpr uint32_t SAVE_VERSION = 1;

template <typename T> void SaveContext::SyncVar(T &aValue, const std::string &aName)
{
	if (mReading)
	{
		if (mSchemeEntries.find(aName) == mSchemeEntries.end())
		{
			TodTraceAndLog("Couldn't find schema entry: %s", aName.c_str());
			return;
		}
		SaveSchemeEntry anEntry = mSchemeEntries[aName];
		mBinaryReader->seekg(anEntry.mOffset);
		mBinaryReader->read(reinterpret_cast<char *>(&aValue), anEntry.mSize);
	}
	else
	{
		mSchema += StrFormat("%s|%d|%d\n", aName.c_str(), mCurrentOffset, sizeof(aValue));
		mWriter->write(reinterpret_cast<char *>(&aValue), sizeof(aValue));
		mCurrentOffset += sizeof(aValue);
	}
}

void SaveContext::SyncBytes(void *aValue, size_t aSize, const std::string &aName)
{
	if (mReading)
	{
		if (mSchemeEntries.find(aName) == mSchemeEntries.end())
		{
			TodTraceAndLog("Couldn't find schema entry: %s", aName.c_str());
			return;
		}
		SaveSchemeEntry anEntry = mSchemeEntries[aName];
		mBinaryReader->seekg(anEntry.mOffset);
		mBinaryReader->read(reinterpret_cast<char *>(aValue), anEntry.mSize);
	}
	else
	{
		mSchema += StrFormat("%s|%d|%d\n", aName.c_str(), mCurrentOffset, aSize);
		mWriter->write(reinterpret_cast<char *>(aValue), aSize);
		mCurrentOffset += aSize;
	}
}



void SaveContext::SyncReanimationDef(ReanimatorDefinition *&theDefinition, const std::string &theOwner)
{
	if (mReading)
	{
		int aReanimType;
		SyncVar(aReanimType, StrFormat("ReanimationDefinition_%s", theOwner.c_str()));
		if (aReanimType == (int)ReanimationType::REANIM_NONE)
		{
			theDefinition = nullptr;
		}
		else if (aReanimType >= 0 && aReanimType < (int)ReanimationType::NUM_REANIMS)
		{
			ReanimatorEnsureDefinitionLoaded((ReanimationType)aReanimType, true);
			theDefinition = &gReanimatorDefArray[aReanimType];
		}
		else
		{
			mFailed = true;
		}
	}
	else
	{
		int aReanimType = (int)ReanimationType::REANIM_NONE;
		for (int i = 0; i < (int)ReanimationType::NUM_REANIMS; i++)
		{
			ReanimatorDefinition *aDef = &gReanimatorDefArray[i];
			if (theDefinition == aDef)
			{
				aReanimType = i;
				break;
			}
		}
		SyncVar(aReanimType, StrFormat("ReanimationDefinition_%s", theOwner.c_str()));
	}
}

void SaveContext::SyncParticleDef(TodParticleDefinition *&theDefinition, const std::string &theOwner)
{
	if (mReading)
	{
		int aParticleType;
		SyncVar(aParticleType, StrFormat("ParticleDefinition_%s", theOwner.c_str()));
		if (aParticleType == (int)ParticleEffect::PARTICLE_NONE)
		{
			theDefinition = nullptr;
		}
		else if (aParticleType >= 0 && aParticleType < (int)ParticleEffect::NUM_PARTICLES)
		{
			theDefinition = &gParticleDefArray[aParticleType];
		}
		else
		{
			mFailed = true;
		}
	}
	else
	{
		int aParticleType = (int)ParticleEffect::PARTICLE_NONE;
		for (int i = 0; i < (int)ParticleEffect::NUM_PARTICLES; i++)
		{
			TodParticleDefinition *aDef = &gParticleDefArray[i];
			if (theDefinition == aDef)
			{
				aParticleType = i;
				break;
			}
		}
		SyncVar(aParticleType, StrFormat("ParticleDefinition_%s", theOwner.c_str()));
	}
}

void SaveContext::SyncTrailDef(TrailDefinition *&theDefinition, const std::string &theOwner)
{
	if (mReading)
	{
		int aTrailType;
		SyncVar(aTrailType, StrFormat("TrailDefinition_%s", theOwner.c_str()));
		if (aTrailType == TrailType::TRAIL_NONE)
		{
			theDefinition = nullptr;
		}
		else if (aTrailType >= 0 && aTrailType < TrailType::NUM_TRAILS)
		{
			theDefinition = &gTrailDefArray[aTrailType];
		}
		else
		{
			mFailed = true;
		}
	}
	else
	{
		int aTrailType = TrailType::TRAIL_NONE;
		for (int i = 0; i < TrailType::NUM_TRAILS; i++)
		{
			TrailDefinition *aDef = &gTrailDefArray[i];
			if (theDefinition == aDef)
			{
				aTrailType = i;
				break;
			}
		}
		SyncVar(aTrailType, StrFormat("TrailDefinition_%s", theOwner.c_str()));
	}
}

static bool IsValidLoadedImage(Image* theImage)
{
	if (theImage == nullptr)
		return false;

	std::string aPath;
	return TodFindImagePath(theImage, &aPath);
}

void SaveContext::SyncImage(Image *&theImage, const std::string &theOwner)
{
	if (mReading)
	{
		ResourceId aResID;
		SyncVar((int &)aResID, StrFormat("IMAGE_%s", theOwner.c_str()));
		if (aResID == Sexy::ResourceId::RESOURCE_ID_MAX)
		{
			theImage = nullptr;
		}
		else
		{
			Image* aImage = GetImageById(aResID);
			if (IsValidLoadedImage(aImage))
			{
				theImage = aImage;
			}
			else
			{
				theImage = nullptr;
			}
		}
	}
	else
	{
		ResourceId aResID;
		if (theImage != nullptr)
		{
			aResID = GetIdByImage(theImage);
		}
		else
		{
			aResID = Sexy::ResourceId::RESOURCE_ID_MAX;
		}
		SyncVar((int &)aResID, StrFormat("IMAGE_%s", theOwner.c_str()));
	}
}

void SyncDataIDList(TodList<unsigned int> *theDataIDList, SaveContext &theContext, TodAllocator *theAllocator, const std::string &aListOwner)
{
	try
	{
		if (theContext.mReading)
		{
			if (theDataIDList)
			{
				theDataIDList->mHead = nullptr;
				theDataIDList->mTail = nullptr;
				theDataIDList->mSize = 0;
				theDataIDList->SetAllocator(theAllocator);
			}

			int aCount;
			theContext.SyncVar(aCount, StrFormat("%s_List_Size", aListOwner.c_str()));
			for (int i = 0; i < aCount; i++)
			{
				unsigned int aDataID;
				theContext.SyncBytes(&aDataID, sizeof(aDataID), StrFormat("%s_List_Data[%d]", aListOwner.c_str(), i));
				theDataIDList->AddTail(aDataID);
			}
		}
		else
		{
			int aCount = theDataIDList->mSize;
			theContext.SyncVar(aCount, StrFormat("%s_List_Size", aListOwner.c_str()));
			int i = 0;
			for (TodListNode<unsigned int> *aNode = theDataIDList->mHead; aNode != nullptr; aNode = aNode->mNext)
			{
				unsigned int aDataID = aNode->mValue;
				theContext.SyncBytes(&aDataID, sizeof(aDataID), StrFormat("%s_List_Data[%d]", aListOwner.c_str(), i));
				i++;
			}
		}
	}
	catch (std::exception &)
	{
		return;
	}
}

void SyncParticleEmitter(TodParticleSystem *theParticleSystem, TodParticleEmitter *theParticleEmitter, SaveContext &theContext)
{
	std::string aIDStr = StrFormat("%u", theParticleSystem->mParticleHolder->mEmitters.DataArrayGetID(theParticleEmitter));
	int aEmitterDefIndex = 0;
	if (theContext.mReading)
	{
		theContext.SyncVar(aEmitterDefIndex, StrFormat("ParticleEmitterDefinition_%s", aIDStr.c_str()));
		theParticleEmitter->mParticleSystem = theParticleSystem;
		theParticleEmitter->mEmitterDef = &theParticleSystem->mParticleDef->mEmitterDefs[aEmitterDefIndex];
	}
	else
	{
		aEmitterDefIndex = (int)(theParticleEmitter->mEmitterDef - theParticleSystem->mParticleDef->mEmitterDefs);
		theContext.SyncVar(aEmitterDefIndex, StrFormat("ParticleEmitterDefinition_%s", aIDStr.c_str()));
	}

	theContext.SyncImage(theParticleEmitter->mImageOverride, StrFormat("ParticleEmitter_%s", aIDStr.c_str()));
	SyncDataIDList((TodList<unsigned int> *)&theParticleEmitter->mParticleList, theContext,
				   &theParticleSystem->mParticleHolder->mParticleListNodeAllocator, StrFormat("ParticleEmitter_%s", aIDStr.c_str()));
	for (TodListNode<ParticleID> *aNode = theParticleEmitter->mParticleList.mHead; aNode != nullptr;
		 aNode = aNode->mNext)
	{
		TodParticle *aParticle =
			theParticleSystem->mParticleHolder->mParticles.DataArrayGet((unsigned int)aNode->mValue);
		if (theContext.mReading)
		{
			aParticle->mParticleEmitter = theParticleEmitter;
		}
	}
}

void SyncParticleSystem(Board *theBoard, TodParticleSystem *theParticleSystem, SaveContext &theContext)
{
	std::string aIDStr = StrFormat("%u", theBoard->mApp->mEffectSystem->mParticleHolder->mParticleSystems.DataArrayGetID(theParticleSystem));

	theContext.SyncParticleDef(theParticleSystem->mParticleDef, StrFormat("ParticleSystem_%s", aIDStr.c_str()));
	if (theContext.mReading)
	{
		theParticleSystem->mParticleHolder = theBoard->mApp->mEffectSystem->mParticleHolder;
	}

	SyncDataIDList((TodList<unsigned int> *)&theParticleSystem->mEmitterList, theContext,
				   &theParticleSystem->mParticleHolder->mEmitterListNodeAllocator,
				   StrFormat("ParticleSystem_%s", aIDStr.c_str()));
	for (TodListNode<ParticleEmitterID> *aNode = theParticleSystem->mEmitterList.mHead; aNode != nullptr;
		 aNode = aNode->mNext)
	{
		TodParticleEmitter *aEmitter =
			theParticleSystem->mParticleHolder->mEmitters.DataArrayGet((unsigned int)aNode->mValue);
		SyncParticleEmitter(theParticleSystem, aEmitter, theContext);
	}
}

void SyncReanimation(Board *theBoard, Reanimation *theReanimation, SaveContext &theContext)
{
	std::string aIDStr = StrFormat("%u", theBoard->mApp->mEffectSystem->mReanimationHolder->mReanimations.DataArrayGetID(theReanimation));
	theContext.SyncReanimationDef(theReanimation->mDefinition, StrFormat("Reanimation_%s", aIDStr.c_str()));
	if (theContext.mReading)
	{
		theReanimation->mReanimationHolder = theBoard->mApp->mEffectSystem->mReanimationHolder;
	}

	if (theReanimation->mDefinition == nullptr)
		return;

	if (theReanimation->mDefinition->mTrackCount != 0)
	{
		int aSize = theReanimation->mDefinition->mTrackCount * sizeof(ReanimatorTrackInstance);
		if (theContext.mReading)
		{
			theReanimation->mTrackInstances = (ReanimatorTrackInstance *)FindGlobalAllocator(aSize)->Calloc(aSize);
		}
		theContext.SyncBytes(theReanimation->mTrackInstances, aSize, StrFormat("Reanimation_%s_Tracks", aIDStr.c_str()));

		for (int aTrackIndex = 0; aTrackIndex < theReanimation->mDefinition->mTrackCount; aTrackIndex++)
		{
			ReanimatorTrackInstance &aTrackInstance = theReanimation->mTrackInstances[aTrackIndex];
			theContext.SyncImage(aTrackInstance.mImageOverride, StrFormat("Reanimation_%s_Track[%d]", aIDStr.c_str(), aTrackIndex));

			if (theContext.mReading)
			{
				aTrackInstance.mBlendTransform.mText = "";
				TOD_ASSERT(aTrackInstance.mBlendTransform.mFont == nullptr);
				TOD_ASSERT(aTrackInstance.mBlendTransform.mImage == nullptr);
			}
			else
			{
				TOD_ASSERT(aTrackInstance.mBlendTransform.mText[0] == NULL);
				TOD_ASSERT(aTrackInstance.mBlendTransform.mFont == nullptr);
				TOD_ASSERT(aTrackInstance.mBlendTransform.mImage == nullptr);
			}
		}
	}
}

void SyncTrail(Board *theBoard, Trail *theTrail, SaveContext &theContext)
{
	theContext.SyncTrailDef(theTrail->mDefinition, StrFormat("Trail_%u", theBoard->mApp->mEffectSystem->mTrailHolder->mTrails.DataArrayGetID(theTrail)));
	if (theContext.mReading)
	{
		theTrail->mTrailHolder = theBoard->mApp->mEffectSystem->mTrailHolder;
	}
}

void SaveContext::LoadScheme(std::string thePath)
{
	std::ifstream aReader(thePath, std::ios::binary);
	std::string aLine;
	mSchemeEntries.clear();
	while (std::getline(aReader, aLine))
	{
		SaveSchemeEntry anEntry;
		std::vector<std::string> aSeparatedLine;
		size_t start = 0;
		size_t end = aLine.find("|");

		if (end == std::string::npos)
		{
			continue;
		}

		while (end != std::string::npos)
		{
			aSeparatedLine.push_back(aLine.substr(start, end - start));
			start = end + 1;
			end = aLine.find("|", start);
		}

		aSeparatedLine.push_back(aLine.substr(start));
		size_t aCurrentIndex = 0;
		std::string aVarName = "VAL_UNKNOWN";
		for (auto& aValue : aSeparatedLine)
		{
			switch (aCurrentIndex)
			{
				case 0:
					aVarName = aValue;
					break;
				case 1:
					sscanf(aValue.c_str(), "%zu", &anEntry.mOffset);
					break;
				case 2:
					sscanf(aValue.c_str(), "%zu", &anEntry.mSize);
					aCurrentIndex = -1;
					break;
			}
			aCurrentIndex++;
		}
		mSchemeEntries.insert({aVarName, anEntry});
	}
}


#define SYNC_VAR(name) theContext.SyncVar(name, #name)
#define SYNC_ARRAY(name) theContext.SyncBytes(name, sizeof(name), #name)
#define SYNC_CLASS(obj) theContext.SyncBytes(obj, sizeof(*obj), #obj)
#define SYNC_DATA_ARRAY(type, arr)                                                                                     \
	theContext.SyncVar(arr.mMaxUsedCount, #arr ".mMaxUsedCount");                                                      \
	theContext.SyncVar(arr.mNextKey, #arr ".mNextKey");                                                                \
	theContext.SyncVar(arr.mFreeListHead, #arr ".mFreeListHead");                                                      \
	theContext.SyncVar(arr.mSize, #arr ".mSize");                                                                      \
	theContext.SyncBytes(arr.mBlock, sizeof(DataArray<type>::DataArrayItem) * arr.mMaxUsedCount, #arr ".mBlock");

#define SYNC_GAMEOBJECT(obj) \
	theContext.SyncVar(obj->mX, #obj "->mX");                                                                        \
	theContext.SyncVar(obj->mY, #obj "->mY");                                                                        \
	theContext.SyncVar(obj->mWidth, #obj "->mWidth");                                                                \
	theContext.SyncVar(obj->mHeight, #obj "->mHeight");                                                              \
	theContext.SyncVar(obj->mVisible, #obj "->mVisible");															 \
	theContext.SyncVar(obj->mRow, #obj "->mRow");                                                                    \
	theContext.SyncVar(obj->mRenderOrder, #obj "->mRenderOrder");                                                    \

void LawnSyncGame(Board* theBoard, SaveContext &theContext)
{

	SYNC_ARRAY(theBoard->mGridSquareType);
	SYNC_ARRAY(theBoard->mGridCelLook);
	SYNC_ARRAY(theBoard->mGridCelOffset);
	SYNC_ARRAY(theBoard->mGridCelFog);

	SYNC_VAR(theBoard->mEnableGraveStones);
	SYNC_VAR(theBoard->mSpecialGraveStoneX);
	SYNC_VAR(theBoard->mSpecialGraveStoneY);
	SYNC_VAR(theBoard->mFogOffset);
	SYNC_VAR(theBoard->mFogBlownCountDown);

	SYNC_ARRAY(theBoard->mPlantRow);
	SYNC_ARRAY(theBoard->mWaveRowGotLawnMowered);

	SYNC_VAR(theBoard->mBonusLawnMowersRemaining);

	SYNC_ARRAY(theBoard->mIceMinX);
	SYNC_ARRAY(theBoard->mIceTimer);
	SYNC_ARRAY(theBoard->mIceParticleID);
	SYNC_ARRAY(theBoard->mRowPickingArray);

	SYNC_ARRAY(theBoard->mZombiesInWave);
	SYNC_ARRAY(theBoard->mZombieAllowed);

	SYNC_VAR(theBoard->mSunCountDown);
	SYNC_VAR(theBoard->mNumSunsFallen);
	SYNC_VAR(theBoard->mShakeCounter);
	SYNC_VAR(theBoard->mShakeAmountX);
	SYNC_VAR(theBoard->mShakeAmountY);

	SYNC_VAR(theBoard->mBackground);
	SYNC_VAR(theBoard->mLevel);
	SYNC_VAR(theBoard->mSodPosition);

	SYNC_VAR(theBoard->mPrevMouseX);
	SYNC_VAR(theBoard->mPrevMouseY);

	SYNC_VAR(theBoard->mSunMoney);
	SYNC_VAR(theBoard->mNumWaves);
	SYNC_VAR(theBoard->mMainCounter);
	SYNC_VAR(theBoard->mEffectCounter);
	SYNC_VAR(theBoard->mDrawCount);
	SYNC_VAR(theBoard->mRiseFromGraveCounter);
	SYNC_VAR(theBoard->mOutOfMoneyCounter);

	SYNC_VAR(theBoard->mCurrentWave);
	SYNC_VAR(theBoard->mTotalSpawnedWaves);

	SYNC_VAR(theBoard->mTutorialState);
	SYNC_VAR(theBoard->mTutorialParticleID);
	SYNC_VAR(theBoard->mTutorialTimer);

	SYNC_VAR(theBoard->mLastBungeeWave);
	SYNC_VAR(theBoard->mZombieHealthToNextWave);
	SYNC_VAR(theBoard->mZombieHealthWaveStart);

	SYNC_VAR(theBoard->mZombieCountDown);
	SYNC_VAR(theBoard->mZombieCountDownStart);
	SYNC_VAR(theBoard->mHugeWaveCountDown);

	SYNC_ARRAY(theBoard->mHelpDisplayed);
	SYNC_VAR(theBoard->mHelpIndex);

	SYNC_VAR(theBoard->mFinalBossKilled);
	SYNC_VAR(theBoard->mShowShovel);
	SYNC_VAR(theBoard->mCoinBankFadeCount);
	SYNC_VAR(theBoard->mDebugTextMode);

	SYNC_VAR(theBoard->mLevelComplete);
	SYNC_VAR(theBoard->mBoardFadeOutCounter);
	SYNC_VAR(theBoard->mNextSurvivalStageCounter);
	SYNC_VAR(theBoard->mScoreNextMowerCounter);

	SYNC_VAR(theBoard->mLevelAwardSpawned);
	SYNC_VAR(theBoard->mProgressMeterWidth);
	SYNC_VAR(theBoard->mFlagRaiseCounter);
	SYNC_VAR(theBoard->mIceTrapCounter);

	SYNC_VAR(theBoard->mBoardRandSeed);
	SYNC_VAR(theBoard->mPoolSparklyParticleID);

	SYNC_ARRAY(theBoard->mFwooshID);

	SYNC_VAR(theBoard->mFwooshCountDown);
	SYNC_VAR(theBoard->mTimeStopCounter);

	SYNC_VAR(theBoard->mDroppedFirstCoin);
	SYNC_VAR(theBoard->mFinalWaveSoundCounter);

	SYNC_VAR(theBoard->mCobCannonCursorDelayCounter);
	SYNC_VAR(theBoard->mCobCannonMouseX);
	SYNC_VAR(theBoard->mCobCannonMouseY);

	SYNC_VAR(theBoard->mKilledYeti);

	SYNC_VAR(theBoard->mMustacheMode);
	SYNC_VAR(theBoard->mSuperMowerMode);
	SYNC_VAR(theBoard->mFutureMode);
	SYNC_VAR(theBoard->mPinataMode);
	SYNC_VAR(theBoard->mDanceMode);
	SYNC_VAR(theBoard->mDaisyMode);
	SYNC_VAR(theBoard->mSukhbirMode);

	SYNC_VAR(theBoard->mPrevBoardResult);

	SYNC_VAR(theBoard->mTriggeredLawnMowers);
	SYNC_VAR(theBoard->mPlayTimeActiveLevel);
	SYNC_VAR(theBoard->mPlayTimeInactiveLevel);

	SYNC_VAR(theBoard->mMaxSunPlants);

	SYNC_VAR(theBoard->mStartDrawTime);
	SYNC_VAR(theBoard->mIntervalDrawTime);
	SYNC_VAR(theBoard->mIntervalDrawCountStart);

	SYNC_VAR(theBoard->mMinFPS);
	SYNC_VAR(theBoard->mPreloadTime);
	SYNC_VAR(theBoard->mGameID);

	SYNC_VAR(theBoard->mGravesCleared);
	SYNC_VAR(theBoard->mPlantsEaten);
	SYNC_VAR(theBoard->mPlantsShoveled);
	SYNC_VAR(theBoard->mCoinsCollected);
	SYNC_VAR(theBoard->mDiamondsCollected);
	SYNC_VAR(theBoard->mPottedPlantsCollected);
	SYNC_VAR(theBoard->mChocolateCollected);

	SYNC_VAR(theBoard->mChallenge->mBeghouledMouseCapture);
	SYNC_VAR(theBoard->mChallenge->mBeghouledMouseDownX);
	SYNC_VAR(theBoard->mChallenge->mBeghouledMouseDownY);

	SYNC_VAR(theBoard->mChallenge->mBeghouledMatchesThisMove);
	SYNC_VAR(theBoard->mChallenge->mChallengeState);
	SYNC_VAR(theBoard->mChallenge->mChallengeStateCounter);
	SYNC_VAR(theBoard->mChallenge->mConveyorBeltCounter);
	SYNC_VAR(theBoard->mChallenge->mChallengeScore);
	SYNC_VAR(theBoard->mChallenge->mShowBowlingLine);
	SYNC_VAR(theBoard->mChallenge->mLastConveyorSeedType);
	SYNC_VAR(theBoard->mChallenge->mSurvivalStage);
	SYNC_VAR(theBoard->mChallenge->mSlotMachineRollCount);
	SYNC_VAR(theBoard->mChallenge->mReanimChallenge);

	SYNC_VAR(theBoard->mChallenge->mChallengeGridX);
	SYNC_VAR(theBoard->mChallenge->mChallengeGridY);
	SYNC_VAR(theBoard->mChallenge->mScaryPotterPots);
	SYNC_VAR(theBoard->mChallenge->mRainCounter);
	SYNC_VAR(theBoard->mChallenge->mTreeOfWisdomTalkIndex);

	SYNC_ARRAY(theBoard->mChallenge->mBeghouledEated);
	SYNC_ARRAY(theBoard->mChallenge->mBeghouledPurcasedUpgrade);
	SYNC_ARRAY(theBoard->mChallenge->mReanimClouds);
	SYNC_ARRAY(theBoard->mChallenge->mCloudsCounter);

	SYNC_VAR(theBoard->mApp->mMusic->mCurMusicTune);
	SYNC_VAR(theBoard->mApp->mMusic->mCurMusicFileMain);
	SYNC_VAR(theBoard->mApp->mMusic->mCurMusicFileDrums);
	SYNC_VAR(theBoard->mApp->mMusic->mCurMusicFileHihats);
	SYNC_VAR(theBoard->mApp->mMusic->mBurstOverride);
	SYNC_VAR(theBoard->mApp->mMusic->mBaseBPM);
	SYNC_VAR(theBoard->mApp->mMusic->mBaseModSpeed);
	SYNC_VAR(theBoard->mApp->mMusic->mMusicBurstState);
	SYNC_VAR(theBoard->mApp->mMusic->mBurstStateCounter);
	SYNC_VAR(theBoard->mApp->mMusic->mMusicDrumsState);
	SYNC_VAR(theBoard->mApp->mMusic->mQueuedDrumTrackPackedOrder);
	SYNC_VAR(theBoard->mApp->mMusic->mDrumsStateCounter);
	SYNC_VAR(theBoard->mApp->mMusic->mPauseOffset);
	SYNC_VAR(theBoard->mApp->mMusic->mPauseOffsetDrums);
	SYNC_VAR(theBoard->mApp->mMusic->mPaused);
	SYNC_VAR(theBoard->mApp->mMusic->mMusicDisabled);
	SYNC_VAR(theBoard->mApp->mMusic->mFadeOutCounter);
	SYNC_VAR(theBoard->mApp->mMusic->mFadeOutDuration);
	SYNC_VAR(theBoard->mApp->mMusic->mQueuedDrumTrackPosition);

	SYNC_GAMEOBJECT(theBoard->mSeedBank);
	SYNC_VAR(theBoard->mSeedBank->mNumPackets);
	SYNC_ARRAY(theBoard->mSeedBank->mSeedPackets);
	SYNC_VAR(theBoard->mSeedBank->mCutSceneDarken);
	SYNC_VAR(theBoard->mSeedBank->mConveyorBeltCounter);
#if SEXY_USE_CONTROLLER
	SYNC_VAR(theBoard->mSeedBank->mIndexGamepad);
	SYNC_VAR(theBoard->mSeedBank->mAxisProgress);
#endif

	SYNC_ARRAY(theBoard->mAdvice->mLabel);
	SYNC_VAR(theBoard->mAdvice->mDisplayTime);
	SYNC_VAR(theBoard->mAdvice->mDuration);
	SYNC_VAR(theBoard->mAdvice->mMessageStyle);
	SYNC_ARRAY(theBoard->mAdvice->mTextReanimID);
	SYNC_VAR(theBoard->mAdvice->mReanimType);
	SYNC_VAR(theBoard->mAdvice->mSlideOffTime);
	SYNC_ARRAY(theBoard->mAdvice->mLabelNext);
	SYNC_VAR(theBoard->mAdvice->mMessageStyleNext);

	SYNC_VAR(theBoard->mCursorPreview->mGridX);
	SYNC_VAR(theBoard->mCursorPreview->mGridY);

	SYNC_VAR(theBoard->mCursorObject->mSeedBankIndex);
	SYNC_VAR(theBoard->mCursorObject->mType);
	SYNC_VAR(theBoard->mCursorObject->mImitaterType);
	SYNC_VAR(theBoard->mCursorObject->mCursorType);
	SYNC_VAR(theBoard->mCursorObject->mCoinID);
	SYNC_VAR(theBoard->mCursorObject->mGlovePlantID);
	SYNC_VAR(theBoard->mCursorObject->mDuplicatorPlantID);
	SYNC_VAR(theBoard->mCursorObject->mCobCannonPlantID);
	SYNC_VAR(theBoard->mCursorObject->mHammerDownCounter);
	SYNC_VAR(theBoard->mCursorObject->mReanimCursorID);

	SYNC_DATA_ARRAY(Zombie, theBoard->mZombies)
	SYNC_DATA_ARRAY(Plant, theBoard->mPlants)
	SYNC_DATA_ARRAY(Projectile, theBoard->mProjectiles)
	SYNC_DATA_ARRAY(Coin, theBoard->mCoins)
	SYNC_DATA_ARRAY(LawnMower, theBoard->mLawnMowers)
	SYNC_DATA_ARRAY(GridItem, theBoard->mGridItems)
	SYNC_DATA_ARRAY(TodParticleSystem, theBoard->mApp->mEffectSystem->mParticleHolder->mParticleSystems)
	SYNC_DATA_ARRAY(TodParticleEmitter, theBoard->mApp->mEffectSystem->mParticleHolder->mEmitters)
	SYNC_DATA_ARRAY(TodParticle, theBoard->mApp->mEffectSystem->mParticleHolder->mParticles)
	SYNC_DATA_ARRAY(Reanimation, theBoard->mApp->mEffectSystem->mReanimationHolder->mReanimations)
	SYNC_DATA_ARRAY(Trail, theBoard->mApp->mEffectSystem->mTrailHolder->mTrails)
	SYNC_DATA_ARRAY(Attachment, theBoard->mApp->mEffectSystem->mAttachmentHolder->mAttachments)

	TodParticleSystem *aParticle = nullptr;
	while (theBoard->mApp->mEffectSystem->mParticleHolder->mParticleSystems.IterateNext(aParticle))
	{
		SyncParticleSystem(theBoard, aParticle, theContext);
	}

	Reanimation *aReanimation = nullptr;
	while (theBoard->mApp->mEffectSystem->mReanimationHolder->mReanimations.IterateNext(aReanimation))
	{
		SyncReanimation(theBoard, aReanimation, theContext);
	}

	Trail *aTrail = nullptr;
	while (theBoard->mApp->mEffectSystem->mTrailHolder->mTrails.IterateNext(aTrail))
	{
		SyncTrail(theBoard, aTrail, theContext);
	}

	Plant *aPlant = nullptr;
	while (theBoard->mPlants.IterateNext(aPlant))
	{
		aPlant->mApp = theBoard->mApp;
		aPlant->mBoard = theBoard;
	}

	Zombie *aZombie = nullptr;
	while (theBoard->mZombies.IterateNext(aZombie))
	{
		aZombie->mApp = theBoard->mApp;
		aZombie->mBoard = theBoard;
	}

	Projectile *aProjectile = nullptr;
	while (theBoard->mProjectiles.IterateNext(aProjectile))
	{
		aProjectile->mApp = theBoard->mApp;
		aProjectile->mBoard = theBoard;
	}

	Coin *aCoin = nullptr;
	while (theBoard->mCoins.IterateNext(aCoin))
	{
		aCoin->mApp = theBoard->mApp;
		aCoin->mBoard = theBoard;
	}

	LawnMower *aLawnMower = nullptr;
	while (theBoard->mLawnMowers.IterateNext(aLawnMower))
	{
		aLawnMower->mApp = theBoard->mApp;
		aLawnMower->mBoard = theBoard;
	}

	GridItem *aGridItem = nullptr;
	while (theBoard->mGridItems.IterateNext(aGridItem))
	{
		aGridItem->mApp = theBoard->mApp;
		aGridItem->mBoard = theBoard;
	}

	if (theContext.mReading)
	{
		theBoard->mAdvice->mApp = theBoard->mApp;
		theBoard->mCursorObject->mApp = theBoard->mApp;
		theBoard->mCursorObject->mBoard = theBoard;
		theBoard->mCursorPreview->mApp = theBoard->mApp;
		theBoard->mCursorPreview->mBoard = theBoard;
		theBoard->mSeedBank->mApp = theBoard->mApp;
		theBoard->mSeedBank->mBoard = theBoard;
		for (int i = 0; i < SEEDBANK_MAX; i++)
		{
			theBoard->mSeedBank->mSeedPackets[i].mApp = theBoard->mApp;
			theBoard->mSeedBank->mSeedPackets[i].mBoard = theBoard;
		}
		theBoard->mChallenge->mApp = theBoard->mApp;
		theBoard->mChallenge->mBoard = theBoard;
		theBoard->mApp->mMusic->mApp = theBoard->mApp;
		theBoard->mApp->mMusic->mMusicInterface = theBoard->mApp->mMusicInterface;
	}

}

bool LawnLoadGame(Board *theBoard, const std::string &theFilePath)
{
	std::ifstream aReader(theFilePath + ".data", std::ios::binary);
	SaveContext theContext;
	theContext.mBinaryReader = &aReader;
	theContext.mReading = true;
	theContext.LoadScheme(theFilePath + ".schema");

	LawnSyncGame(theBoard, theContext);
	if (aReader.fail())
	{
		TodErrorMessageBox("Save file read error or truncated file", "Save File Error");
		return false;
	}
	theBoard->mApp->mGameScene = GameScenes::SCENE_PLAYING;
	return true;
}

bool LawnSaveGame(Board *theBoard, const std::string &theFilePath)
{
	std::ofstream aWriter(theFilePath + ".data", std::ios::binary);
	std::ofstream aSchemaWriter(theFilePath + ".schema", std::ios::binary);

	SaveContext theContext;
	theContext.mWriter = &aWriter;
	theContext.mReading = false;
	theContext.mCurrentOffset = 0;

	LawnSyncGame(theBoard, theContext);
	aSchemaWriter << theContext.mSchema;
	aSchemaWriter.close();
	aWriter.close();
	return true;
}
