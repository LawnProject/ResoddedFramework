#ifndef __PLAYERINFO_H__
#define __PLAYERINFO_H__

#define MAX_POTTED_PLANTS 200
#define MAX_NUM_ZOMBATARS 100
#define PURCHASE_COUNT_OFFSET 1000

#include "../../ConstEnums.h"
#include <json.hpp>

class PottedPlant
{
  public:
	enum FacingDirection
	{
		FACING_RIGHT,
		FACING_LEFT
	};

  public:
	SeedType mSeedType;
	GardenType mWhichZenGarden;
	int mX;
	int mY;
	FacingDirection mFacing;

	int64_t mLastWateredTime;
	DrawVariation mDrawVariation;
	PottedPlantAge mPlantAge;
	int mTimesFed;
	int mFeedingsPerGrow;
	PottedPlantNeed mPlantNeed;

	int64_t mLastNeedFulfilledTime;
	int64_t mLastFertilizedTime;
	int64_t mLastChocolateTime;

  public:
	void InitializePottedPlant(SeedType theSeedType);
};

class Zombatar
{
  public:
	int mSkinColor;
	int mClothes;
	int mClothesColor;
	int mTidbits;
	int mTidbitsColor;
	int mAccessories;
	int mAccessoriesColor;
	int mFacialHair;
	int mFacialHairColor;
	int mHair;
	int mHairColor;
	int mEyewear;
	int mEyewearColor;
	int mHat;
	int mHatColor;
	int mBackdrop;
	int mBackdropColor;
};

class ProfileSyncer
{
  public:
	bool mReading;
	bool mCanRead;
	nlohmann::json mJSON;

  public:
	ProfileSyncer();
	ProfileSyncer(const SexyString &thePath);
	~ProfileSyncer();
	void SyncBool(const SexyString &theName, bool &theBool);
	void SyncFloat(const SexyString &theName, float &theFloat);
	void SyncInt(const SexyString &theName, int &theInt);
	void SyncUnsignedInt(const SexyString &theName, unsigned int &theInt);
	void SyncLong(const SexyString &theName, long &theLong);
	void SyncUnsignedLong(const SexyString &theName, unsigned long &theLong);
	void SyncString(const SexyString &theName, SexyString &theStr);
	template <typename T, size_t N> 
	void SyncArray(const std::string &theName, T (&theArray)[N], int &theRealSize);
	template <typename T, size_t N> 
	void SyncArrayFromSize(const std::string &theName, T (&theArray)[N], int theRealSize);
};
class PlayerInfo
{
  public:
	SexyString mName;
	unsigned long mUseSeq;
	unsigned long mId;
	int mLevel;
	int mCoins;
	int mFinishedAdventure;
	int mChallengeRecords[NUM_GAME_MODES];
	int mPurchases[NUM_STORE_ITEM_MAX];
	int mPlayTimeActivePlayer;
	int mPlayTimeInactivePlayer;
	bool mHasUsedCheatKeys;
	bool mHasWokenStinky;
	int mDidntPurchasePacketUpgrade;
	long mLastStinkyChocolateTime;
	int mStinkyPosX;
	int mStinkyPosY;
	bool mHasUnlockedMinigames;
	bool mHasUnlockedPuzzleMode;
	bool mHasNewMiniGame;
	bool mHasNewScaryPotter;
	bool mHasNewIZombie;
	bool mHasNewSurvival;
	bool mHasUnlockedSurvivalMode;
	bool mNeedsMessageOnGameSelector;
	bool mNeedsMagicTacoReward;
	bool mHasSeenStinky;	
	bool mHasSeenUpsell;
	bool mAcceptedZombatarTOS;
	int mNumPottedPlants;
	int mZombatarIndex;
	int mNumZombatars;
	PottedPlant mPottedPlant[MAX_POTTED_PLANTS];
	bool mEarnedAchievements[NUM_ACHIEVEMENT_TYPES];
	bool mShownAchievements[NUM_ACHIEVEMENT_TYPES];
	Zombatar mZombatars[MAX_NUM_ZOMBATARS];

  public:
	PlayerInfo();

	void Reset();
	/*inline*/ void AddCoins(int theAmount);
	void SyncSummary(ProfileSyncer &theSync);
	void SyncDetails(ProfileSyncer &theSync);
	void DeleteUserFiles();
	void LoadDetails();
	void SaveDetails();
	inline int GetLevel() const
	{
		return mLevel;
	}
	inline void SetLevel(int theLevel)
	{
		mLevel = theLevel;
	}
	void ResetChallengeRecord(GameMode theGameMode);
};

#endif
