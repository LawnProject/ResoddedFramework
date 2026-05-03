#include "Music.h"
#include "../CursorObject.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/EffectSystem.h"
#include "SaveGame.h"


static constexpr char SAVE_HEADER_MAGIC[8] = 
{'R', 'E', 'S', 'O', 'D', 'D', 'E', 'D'};
static constexpr uint32_t SAVE_HEADER_VERSION = 1;

#define SYNC_VARIABLE(obj, name) theContext.Sync(obj->name)
#define SYNC_ARRAY(obj, name) theContext.SyncBytes(obj->name, sizeof(obj->name))

#define SYNC_BOARD_VARIABLE(name) SYNC_VARIABLE(theBoard, name)
#define SYNC_BOARD_ARRAY(name) SYNC_ARRAY(theBoard, name)

#define SYNC_CHALLENGE_VARIABLE(name) SYNC_VARIABLE(theBoard->mChallenge, name)
#define SYNC_CHALLENGE_ARRAY(name) SYNC_ARRAY(theBoard->mChallenge, name)

#define SYNC_MUSIC_VARIABLE(name) SYNC_VARIABLE(theBoard->mApp->mMusic, name)

#define SYNC_SEEDBANK_VARIABLE(name) SYNC_VARIABLE(theBoard->mSeedBank, name)
#define SYNC_SEEDBANK_ARRAY(name) SYNC_ARRAY(theBoard->mSeedBank, name)

#define SYNC_ADVICE_VARIABLE(name) SYNC_VARIABLE(theBoard->mAdvice, name)
#define SYNC_ADVICE_ARRAY(name) SYNC_ARRAY(theBoard->mAdvice, name)

#define SYNC_CURSOR_PREVIEW_VARIABLE(name) SYNC_VARIABLE(theBoard->mCursorPreview, name)

#define SYNC_CURSOR_OBJECT_VARIABLE(name) SYNC_VARIABLE(theBoard->mCursorObject, name)

#define SYNC_DATA_ARRAY(type, arr)                                                           \
	theContext.Sync(arr.mMaxUsedCount);                                                      \
	theContext.Sync(arr.mNextKey);                                                           \
	theContext.Sync(arr.mFreeListHead);                                                      \
	theContext.Sync(arr.mSize);																 \
	theContext.SyncBytes(arr.mBlock, sizeof(DataArray<type>::DataArrayItem) * arr.mMaxUsedCount);

void SaveContext::SyncReanimationDef(ReanimatorDefinition *&theDefinition)
{
	if (mReading)
	{
		int aReanimType;
		Sync(aReanimType);
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
		Sync(aReanimType);
	}
}

void SaveContext::SyncParticleDef(TodParticleDefinition *&theDefinition)
{
	if (mReading)
	{
		int aParticleType;
		Sync(aParticleType);
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
		Sync(aParticleType);
	}
}

void SaveContext::SyncTrailDef(TrailDefinition *&theDefinition)
{
	if (mReading)
	{
		int aTrailType;
		Sync(aTrailType);
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
		Sync(aTrailType);
	}
}

void SaveContext::SyncImage(Image *&theImage)
{
	if (mReading)
	{
		ResourceId aResID;
		Sync((int &)aResID);
		if (aResID == Sexy::ResourceId::RESOURCE_ID_MAX)
		{
			theImage = nullptr;
		}
		else
		{
			theImage = GetImageById(aResID);
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
		Sync((int &)aResID);
	}
}

void SyncDataIDList(TodList<unsigned int> *theDataIDList, SaveContext &theContext, TodAllocator *theAllocator)
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
			theContext.Sync(aCount);
			for (int i = 0; i < aCount; i++)
			{
				unsigned int aDataID;
				theContext.SyncBytes(&aDataID, sizeof(aDataID));
				theDataIDList->AddTail(aDataID);
			}
		}
		else
		{
			int aCount = theDataIDList->mSize;
			theContext.Sync(aCount);
			for (TodListNode<unsigned int> *aNode = theDataIDList->mHead; aNode != nullptr; aNode = aNode->mNext)
			{
				unsigned int aDataID = aNode->mValue;
				theContext.SyncBytes(&aDataID, sizeof(aDataID));
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
	int aEmitterDefIndex = 0;
	if (theContext.mReading)
	{
		theContext.Sync(aEmitterDefIndex);
		theParticleEmitter->mParticleSystem = theParticleSystem;
		theParticleEmitter->mEmitterDef = &theParticleSystem->mParticleDef->mEmitterDefs[aEmitterDefIndex];
	}
	else
	{
		aEmitterDefIndex = (int)(theParticleEmitter->mEmitterDef - theParticleSystem->mParticleDef->mEmitterDefs);
		theContext.Sync(aEmitterDefIndex);
	}

	theContext.SyncImage(theParticleEmitter->mImageOverride);
	SyncDataIDList((TodList<unsigned int> *)&theParticleEmitter->mParticleList, theContext,
				   &theParticleSystem->mParticleHolder->mParticleListNodeAllocator);
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
	theContext.SyncParticleDef(theParticleSystem->mParticleDef);
	if (theContext.mReading)
	{
		theParticleSystem->mParticleHolder = theBoard->mApp->mEffectSystem->mParticleHolder;
	}

	SyncDataIDList((TodList<unsigned int> *)&theParticleSystem->mEmitterList, theContext,
				   &theParticleSystem->mParticleHolder->mEmitterListNodeAllocator);
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
	theContext.SyncReanimationDef(theReanimation->mDefinition);
	if (theContext.mReading)
	{
		theReanimation->mReanimationHolder = theBoard->mApp->mEffectSystem->mReanimationHolder;
	}

	if (theReanimation->mDefinition->mTrackCount != 0)
	{
		int aSize = theReanimation->mDefinition->mTrackCount * sizeof(ReanimatorTrackInstance);
		if (theContext.mReading)
		{
			theReanimation->mTrackInstances = (ReanimatorTrackInstance *)FindGlobalAllocator(aSize)->Calloc(aSize);
		}
		theContext.SyncBytes(theReanimation->mTrackInstances, aSize);

		for (int aTrackIndex = 0; aTrackIndex < theReanimation->mDefinition->mTrackCount; aTrackIndex++)
		{
			ReanimatorTrackInstance &aTrackInstance = theReanimation->mTrackInstances[aTrackIndex];
			theContext.SyncImage(aTrackInstance.mImageOverride);

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
	theContext.SyncTrailDef(theTrail->mDefinition);
	if (theContext.mReading)
	{
		theTrail->mTrailHolder = theBoard->mApp->mEffectSystem->mTrailHolder;
	}
}

void LawnSyncGame(Board* theBoard, SaveContext theContext)
{

	SYNC_BOARD_ARRAY(mGridSquareType);
	SYNC_BOARD_ARRAY(mGridCelLook);
	SYNC_BOARD_ARRAY(mGridCelOffset);
	SYNC_BOARD_ARRAY(mGridCelFog);

	SYNC_BOARD_VARIABLE(mEnableGraveStones);
	SYNC_BOARD_VARIABLE(mSpecialGraveStoneX);
	SYNC_BOARD_VARIABLE(mSpecialGraveStoneY);
	SYNC_BOARD_VARIABLE(mFogOffset);
	SYNC_BOARD_VARIABLE(mFogBlownCountDown);

	SYNC_BOARD_ARRAY(mPlantRow);
	SYNC_BOARD_ARRAY(mWaveRowGotLawnMowered);

	SYNC_BOARD_VARIABLE(mBonusLawnMowersRemaining);

	SYNC_BOARD_ARRAY(mIceMinX);
	SYNC_BOARD_ARRAY(mIceTimer);
	SYNC_BOARD_ARRAY(mIceParticleID);
	SYNC_BOARD_ARRAY(mRowPickingArray);

	SYNC_BOARD_ARRAY(mZombiesInWave);
	SYNC_BOARD_ARRAY(mZombieAllowed);

	SYNC_BOARD_VARIABLE(mSunCountDown);
	SYNC_BOARD_VARIABLE(mNumSunsFallen);
	SYNC_BOARD_VARIABLE(mShakeCounter);
	SYNC_BOARD_VARIABLE(mShakeAmountX);
	SYNC_BOARD_VARIABLE(mShakeAmountY);

	SYNC_BOARD_VARIABLE(mBackground);
	SYNC_BOARD_VARIABLE(mLevel);
	SYNC_BOARD_VARIABLE(mSodPosition);

	SYNC_BOARD_VARIABLE(mPrevMouseX);
	SYNC_BOARD_VARIABLE(mPrevMouseY);

	SYNC_BOARD_VARIABLE(mSunMoney);
	SYNC_BOARD_VARIABLE(mNumWaves);
	SYNC_BOARD_VARIABLE(mMainCounter);
	SYNC_BOARD_VARIABLE(mEffectCounter);
	SYNC_BOARD_VARIABLE(mDrawCount);
	SYNC_BOARD_VARIABLE(mRiseFromGraveCounter);
	SYNC_BOARD_VARIABLE(mOutOfMoneyCounter);

	SYNC_BOARD_VARIABLE(mCurrentWave);
	SYNC_BOARD_VARIABLE(mTotalSpawnedWaves);

	SYNC_BOARD_VARIABLE(mTutorialState);
	SYNC_BOARD_VARIABLE(mTutorialParticleID);
	SYNC_BOARD_VARIABLE(mTutorialTimer);

	SYNC_BOARD_VARIABLE(mLastBungeeWave);
	SYNC_BOARD_VARIABLE(mZombieHealthToNextWave);
	SYNC_BOARD_VARIABLE(mZombieHealthWaveStart);

	SYNC_BOARD_VARIABLE(mZombieCountDown);
	SYNC_BOARD_VARIABLE(mZombieCountDownStart);
	SYNC_BOARD_VARIABLE(mHugeWaveCountDown);

	SYNC_BOARD_ARRAY(mHelpDisplayed);
	SYNC_BOARD_VARIABLE(mHelpIndex);

	SYNC_BOARD_VARIABLE(mFinalBossKilled);
	SYNC_BOARD_VARIABLE(mShowShovel);
	SYNC_BOARD_VARIABLE(mCoinBankFadeCount);
	SYNC_BOARD_VARIABLE(mDebugTextMode);

	SYNC_BOARD_VARIABLE(mLevelComplete);
	SYNC_BOARD_VARIABLE(mBoardFadeOutCounter);
	SYNC_BOARD_VARIABLE(mNextSurvivalStageCounter);
	SYNC_BOARD_VARIABLE(mScoreNextMowerCounter);

	SYNC_BOARD_VARIABLE(mLevelAwardSpawned);
	SYNC_BOARD_VARIABLE(mProgressMeterWidth);
	SYNC_BOARD_VARIABLE(mFlagRaiseCounter);
	SYNC_BOARD_VARIABLE(mIceTrapCounter);

	SYNC_BOARD_VARIABLE(mBoardRandSeed);
	SYNC_BOARD_VARIABLE(mPoolSparklyParticleID);

	SYNC_BOARD_ARRAY(mFwooshID);

	SYNC_BOARD_VARIABLE(mFwooshCountDown);
	SYNC_BOARD_VARIABLE(mTimeStopCounter);

	SYNC_BOARD_VARIABLE(mDroppedFirstCoin);
	SYNC_BOARD_VARIABLE(mFinalWaveSoundCounter);

	SYNC_BOARD_VARIABLE(mCobCannonCursorDelayCounter);
	SYNC_BOARD_VARIABLE(mCobCannonMouseX);
	SYNC_BOARD_VARIABLE(mCobCannonMouseY);

	SYNC_BOARD_VARIABLE(mKilledYeti);

	SYNC_BOARD_VARIABLE(mMustacheMode);
	SYNC_BOARD_VARIABLE(mSuperMowerMode);
	SYNC_BOARD_VARIABLE(mFutureMode);
	SYNC_BOARD_VARIABLE(mPinataMode);
	SYNC_BOARD_VARIABLE(mDanceMode);
	SYNC_BOARD_VARIABLE(mDaisyMode);
	SYNC_BOARD_VARIABLE(mSukhbirMode);

	SYNC_BOARD_VARIABLE(mPrevBoardResult);

	SYNC_BOARD_VARIABLE(mTriggeredLawnMowers);
	SYNC_BOARD_VARIABLE(mPlayTimeActiveLevel);
	SYNC_BOARD_VARIABLE(mPlayTimeInactiveLevel);

	SYNC_BOARD_VARIABLE(mMaxSunPlants);

	SYNC_BOARD_VARIABLE(mStartDrawTime);
	SYNC_BOARD_VARIABLE(mIntervalDrawTime);
	SYNC_BOARD_VARIABLE(mIntervalDrawCountStart);

	SYNC_BOARD_VARIABLE(mMinFPS);
	SYNC_BOARD_VARIABLE(mPreloadTime);
	SYNC_BOARD_VARIABLE(mGameID);

	SYNC_BOARD_VARIABLE(mGravesCleared);
	SYNC_BOARD_VARIABLE(mPlantsEaten);
	SYNC_BOARD_VARIABLE(mPlantsShoveled);
	SYNC_BOARD_VARIABLE(mCoinsCollected);
	SYNC_BOARD_VARIABLE(mDiamondsCollected);
	SYNC_BOARD_VARIABLE(mPottedPlantsCollected);
	SYNC_BOARD_VARIABLE(mChocolateCollected);

	SYNC_CHALLENGE_VARIABLE(mBeghouledMouseCapture);
	SYNC_CHALLENGE_VARIABLE(mBeghouledMouseDownX);
	SYNC_CHALLENGE_VARIABLE(mBeghouledMouseDownY);

	SYNC_CHALLENGE_VARIABLE(mBeghouledMatchesThisMove);
	SYNC_CHALLENGE_VARIABLE(mChallengeState);
	SYNC_CHALLENGE_VARIABLE(mChallengeStateCounter);
	SYNC_CHALLENGE_VARIABLE(mConveyorBeltCounter);
	SYNC_CHALLENGE_VARIABLE(mChallengeScore);
	SYNC_CHALLENGE_VARIABLE(mShowBowlingLine);
	SYNC_CHALLENGE_VARIABLE(mLastConveyorSeedType);
	SYNC_CHALLENGE_VARIABLE(mSurvivalStage);
	SYNC_CHALLENGE_VARIABLE(mSlotMachineRollCount);
	SYNC_CHALLENGE_VARIABLE(mReanimChallenge);

	SYNC_CHALLENGE_VARIABLE(mChallengeGridX);
	SYNC_CHALLENGE_VARIABLE(mChallengeGridY);
	SYNC_CHALLENGE_VARIABLE(mScaryPotterPots);
	SYNC_CHALLENGE_VARIABLE(mRainCounter);
	SYNC_CHALLENGE_VARIABLE(mTreeOfWisdomTalkIndex);

	SYNC_CHALLENGE_ARRAY(mBeghouledEated);
	SYNC_CHALLENGE_ARRAY(mBeghouledPurcasedUpgrade);
	SYNC_CHALLENGE_ARRAY(mReanimClouds);
	SYNC_CHALLENGE_ARRAY(mCloudsCounter);

	SYNC_MUSIC_VARIABLE(mCurMusicTune);
	SYNC_MUSIC_VARIABLE(mCurMusicFileMain);
	SYNC_MUSIC_VARIABLE(mCurMusicFileDrums);
	SYNC_MUSIC_VARIABLE(mCurMusicFileHihats);
	SYNC_MUSIC_VARIABLE(mBurstOverride);
	SYNC_MUSIC_VARIABLE(mBaseBPM);
	SYNC_MUSIC_VARIABLE(mBaseModSpeed);
	SYNC_MUSIC_VARIABLE(mMusicBurstState);
	SYNC_MUSIC_VARIABLE(mBurstStateCounter);
	SYNC_MUSIC_VARIABLE(mMusicDrumsState);
	SYNC_MUSIC_VARIABLE(mQueuedDrumTrackPackedOrder);
	SYNC_MUSIC_VARIABLE(mDrumsStateCounter);
	SYNC_MUSIC_VARIABLE(mPauseOffset);
	SYNC_MUSIC_VARIABLE(mPauseOffsetDrums);
	SYNC_MUSIC_VARIABLE(mPaused);
	SYNC_MUSIC_VARIABLE(mMusicDisabled);
	SYNC_MUSIC_VARIABLE(mFadeOutCounter);
	SYNC_MUSIC_VARIABLE(mFadeOutDuration);
	SYNC_MUSIC_VARIABLE(mQueuedDrumTrackPosition);

	SYNC_SEEDBANK_VARIABLE(mX); // This is one of the only variables we need to sync that isn't defined in the class
	SYNC_SEEDBANK_VARIABLE(mWidth);
	SYNC_SEEDBANK_VARIABLE(mNumPackets);
	SYNC_SEEDBANK_ARRAY(mSeedPackets);
	SYNC_SEEDBANK_VARIABLE(mCutSceneDarken);
	SYNC_SEEDBANK_VARIABLE(mConveyorBeltCounter);

	SYNC_ADVICE_ARRAY(mLabel);
	SYNC_ADVICE_VARIABLE(mDisplayTime);
	SYNC_ADVICE_VARIABLE(mDuration);
	SYNC_ADVICE_VARIABLE(mMessageStyle);
	SYNC_ADVICE_ARRAY(mTextReanimID);
	SYNC_ADVICE_VARIABLE(mReanimType);
	SYNC_ADVICE_VARIABLE(mSlideOffTime);
	SYNC_ADVICE_ARRAY(mLabelNext);
	SYNC_ADVICE_VARIABLE(mMessageStyleNext);

	SYNC_CURSOR_PREVIEW_VARIABLE(mGridX);
	SYNC_CURSOR_PREVIEW_VARIABLE(mGridY);

	SYNC_CURSOR_OBJECT_VARIABLE(mSeedBankIndex);
	SYNC_CURSOR_OBJECT_VARIABLE(mType);
	SYNC_CURSOR_OBJECT_VARIABLE(mImitaterType);
	SYNC_CURSOR_OBJECT_VARIABLE(mCursorType);
	SYNC_CURSOR_OBJECT_VARIABLE(mCoinID);
	SYNC_CURSOR_OBJECT_VARIABLE(mGlovePlantID);
	SYNC_CURSOR_OBJECT_VARIABLE(mDuplicatorPlantID);
	SYNC_CURSOR_OBJECT_VARIABLE(mCobCannonPlantID);
	SYNC_CURSOR_OBJECT_VARIABLE(mHammerDownCounter);
	SYNC_CURSOR_OBJECT_VARIABLE(mReanimCursorID);

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

bool LawnLoadGame(Board *theBoard, const std::string &theFilePath)
{
	std::ifstream aReader(theFilePath, std::ios::binary);
	SaveContext theContext{nullptr, &aReader, false};
	theContext.mReading = true;
	char aMagic[8];
	theContext.SyncBytes(aMagic, sizeof(aMagic));
	if (memcmp(aMagic, SAVE_HEADER_MAGIC, sizeof(SAVE_HEADER_MAGIC)) != 0)
	{
		TodErrorMessageBox("Invalid Save File Magic", "Save File Error");
		return false;
	}

	uint32_t aVersion = 0;
	theContext.Sync(aVersion);
	if (aVersion != SAVE_HEADER_VERSION)
	{
		TodErrorMessageBox(StrFormat("Invalid Version: %d", aVersion).c_str(), "Save File Error");
		return false;
	}

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
	std::string aTempPath = theFilePath + ".tmp";
	std::ofstream aWriter(aTempPath, std::ios::binary);
	SaveContext theContext{&aWriter, nullptr, false};
	theContext.mReading = false;

	theContext.SyncBytes((void *)&SAVE_HEADER_MAGIC, sizeof(SAVE_HEADER_MAGIC));
	theContext.Sync(SAVE_HEADER_VERSION);

	LawnSyncGame(theBoard, theContext);

	aWriter.close();
	if (aWriter.fail())
	{
		std::remove(aTempPath.c_str());
		return false;
	}
	std::rename(aTempPath.c_str(), theFilePath.c_str());
	return true;
}
