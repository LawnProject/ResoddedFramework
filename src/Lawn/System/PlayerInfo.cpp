#include "DataSync.h"
#include <fstream>
#include <filesystem>
#include "PlayerInfo.h"
#include "../LawnCommon.h"
#include "../Widget/ChallengeScreen.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../SexyAppFramework/Buffer.h"
#include "../../SexyAppFramework/SexyAppBase.h"

static int gUserVersion = 12;

PlayerInfo::PlayerInfo()
{
	Reset();
}

//0x468310
void PlayerInfo::SyncSummary(ProfileSyncer &theSync)
{
	theSync.SyncString("name", mName);
	theSync.SyncUnsignedLong("use_seq", mUseSeq);
	theSync.SyncUnsignedLong("id", mId);
}

//0x468390
void PlayerInfo::SyncDetails(ProfileSyncer &theSync)
{
	if (theSync.mReading)
	{
		Reset();
	}

	theSync.SyncInt("level", mLevel);
	theSync.SyncInt("coins", mCoins);
	theSync.SyncInt("has_finished_adventure", mFinishedAdventure);

	theSync.SyncArrayFromSize("challenge_records", mChallengeRecords, NUM_GAME_MODES);
	theSync.SyncArrayFromSize("purchases", mPurchases, NUM_STORE_ITEM_MAX);


	theSync.SyncInt("playtime_active", mPlayTimeActivePlayer);
	theSync.SyncInt("playtime_inactive", mPlayTimeInactivePlayer);
	theSync.SyncBool("used_cheat_keys", mHasUsedCheatKeys);
	theSync.SyncBool("woken_stinky", mHasWokenStinky);
	theSync.SyncInt("didnt_purchase_seed_packet_upgrade", mDidntPurchasePacketUpgrade);
	theSync.SyncLong("last_stinky_chocolate_time", mLastStinkyChocolateTime);
	theSync.SyncInt("stinky_pos_x", mStinkyPosX);
	theSync.SyncInt("stinky_pos_y", mStinkyPosY);
	theSync.SyncBool("has_unlocked_minigames", mHasUnlockedMinigames);
	theSync.SyncBool("has_unlocked_puzzle", mHasUnlockedPuzzleMode);
	theSync.SyncBool("has_new_minigame", mHasNewMiniGame);
	theSync.SyncBool("has_new_scarypotter", mHasNewScaryPotter);
	theSync.SyncBool("has_new_izombie", mHasNewIZombie);
	theSync.SyncBool("has_new_survival", mHasNewSurvival);
	theSync.SyncBool("unlocked_survival", mHasUnlockedSurvivalMode);
	theSync.SyncBool("needs_message_on_gameselector", mNeedsMessageOnGameSelector);
	theSync.SyncBool("needs_magic_taco_reward", mNeedsMagicTacoReward);
	theSync.SyncBool("seen_stinky", mHasSeenStinky);
	theSync.SyncBool("seen_upsell", mHasSeenUpsell);
	theSync.SyncBool("accepted_zombatar_tos", mAcceptedZombatarTOS);
	theSync.SyncInt("zombatar_index", mZombatarIndex);

	theSync.SyncArray("potted_plants", mPottedPlant, mNumPottedPlants);
	theSync.SyncArrayFromSize("achievements_shown", mShownAchievements, NUM_ACHIEVEMENT_TYPES);
	theSync.SyncArrayFromSize("achievements_earned", mEarnedAchievements, NUM_ACHIEVEMENT_TYPES);
	theSync.SyncArray("zombatars", mZombatars, mNumZombatars);

	TOD_ASSERT(mNumPottedPlants <= MAX_POTTED_PLANTS);
	TOD_ASSERT(mNumZombatars <= MAX_NUM_ZOMBATARS);
}

//0x469400
void PlayerInfo::LoadDetails()
{
	try
	{
		Buffer aBuffer;
		std::string aFileName = GetAppDataFolder() + StrFormat("profiles/user%d.json", mId);
		if (std::filesystem::exists(aFileName))
		{
			ProfileSyncer aSync(aFileName);
			if (aSync.mCanRead)
			{
				aSync.mReading = true;
				SyncDetails(aSync);
			}
			else
			{
				TodTraceAndLog("[LawnProject] - Couldn't read profile : %s\nResettings it", aFileName.c_str());
				Reset();
			}
		}

	}
	catch (nlohmann::json::parse_error &anError)
	{
		TodTraceAndLog("[LawnProject] - Failed to parse profile data, resetting it\n");
		TodTraceAndLog("[LawnProject] - JSON Reading Error: %s", anError.what());
		Reset();
	}
}

//0x4695F0
void PlayerInfo::SaveDetails()
{
	MkDir(GetAppDataFolder() + "profiles");
	std::string aFileName = GetAppDataFolder() + StrFormat("profiles/user%d.json", mId);
	ProfileSyncer aSync(aFileName);
	SyncDetails(aSync);
	std::ofstream outFile(aFileName);
	if (outFile)
		outFile << aSync.mJSON.dump(4);
}

//0x469810
void PlayerInfo::DeleteUserFiles()
{
	std::string aFilename = GetAppDataFolder() + StrFormat("profiles/user%d.json", mId);
	if (std::filesystem::exists(aFilename))
	{
		gSexyAppBase->EraseFile(aFilename);

		for (int i = 0; i < (int)GameMode::NUM_GAME_MODES; i++)
		{
			std::string aFileName = GetSavedGameName((GameMode)i, mId);
			gSexyAppBase->EraseFile(aFileName + ".data");
			gSexyAppBase->EraseFile(aFileName + ".schema");
		}
	}
}

//0x469940
void PlayerInfo::Reset()
{
	mLevel = 1;
	mCoins = 0;
	mFinishedAdventure = 0;
	memset(mChallengeRecords, 0, sizeof(mChallengeRecords));
	memset(mPurchases, 0, sizeof(mPurchases));
	mPlayTimeActivePlayer = 0;
	mPlayTimeInactivePlayer = 0;
	mHasUsedCheatKeys = false;
	mHasWokenStinky = false;
	mDidntPurchasePacketUpgrade = 0;
	mLastStinkyChocolateTime = 0;
	mStinkyPosX = 0;
	mStinkyPosY = 0;
	mHasUnlockedMinigames = false;
	mHasUnlockedPuzzleMode = false;
	mHasNewMiniGame = false;
	mHasNewScaryPotter = false;
	mHasNewIZombie = false;
	mHasNewSurvival = false;
	mHasUnlockedSurvivalMode = false;
	mNeedsMessageOnGameSelector = false;
	mNeedsMagicTacoReward = false;
	mHasSeenStinky = false;
	mHasSeenUpsell = false;
	mAcceptedZombatarTOS = false;
	memset(mPottedPlant, 0, sizeof(mPottedPlant));
	memset(mEarnedAchievements, 0, sizeof(mEarnedAchievements));
	memset(mShownAchievements, 0, sizeof(mShownAchievements));
	memset(mZombatars, 0, sizeof(mZombatars));
	mNumPottedPlants = 0;
	mNumZombatars = 0;
	mZombatarIndex = -1;
}

void PlayerInfo::AddCoins(int theAmount)
{
	mCoins += theAmount;
	if (mCoins > 99999)
	{
		mCoins = 99999;
	}
	else if (mCoins < 0)
	{
		mCoins = 0;
	}
}

void PlayerInfo::ResetChallengeRecord(GameMode theGameMode)
{
	int aGameMode = (int)theGameMode - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
	TOD_ASSERT(aGameMode >= 0 && aGameMode <= NUM_CHALLENGE_MODES);
	mChallengeRecords[aGameMode] = 0;
}

//0x469A00
void PottedPlant::InitializePottedPlant(SeedType theSeedType)
{
	memset(this, 0, sizeof(PottedPlant));
	mSeedType = theSeedType;
	mDrawVariation = DrawVariation::VARIATION_NORMAL;
	mLastWateredTime = 0i64;
	mFacing = (FacingDirection)RandRangeInt((int)FacingDirection::FACING_RIGHT, (int)FacingDirection::FACING_LEFT);
	mPlantAge = PottedPlantAge::PLANTAGE_SPROUT;
	mTimesFed = 0;
	mWhichZenGarden = GardenType::GARDEN_MAIN;
	mFeedingsPerGrow = RandRangeInt(3, 5);
	mPlantNeed = PottedPlantNeed::PLANTNEED_NONE;
	mLastNeedFulfilledTime = 0i64;
	mLastFertilizedTime = 0i64;
	mLastChocolateTime = 0i64;
}

ProfileSyncer::ProfileSyncer()
{
	mJSON = nlohmann::json::object();
	mReading = false;
	mCanRead = false;
}

ProfileSyncer::ProfileSyncer(const SexyString &thePath)
{
	mJSON = nlohmann::json::object();
	mReading = false;
	std::ifstream inFile(thePath);
	if (inFile)
	{
		try
		{
			inFile >> mJSON;
			mCanRead = true;
		}
		catch (...)
		{
			mCanRead = false;
		}
	}
	else
		mCanRead = false;
}

ProfileSyncer::~ProfileSyncer()
{
}

bool json_has_key(const nlohmann::json &j, const std::string &key)
{
	return j.find(key) != j.end();
}

void ProfileSyncer::SyncBool(const SexyString &theName, bool &theBool)
{
	if (mReading)
	{
		if (json_has_key(mJSON, theName) && mJSON[theName].is_boolean())
		{
			theBool = mJSON[theName].get<nlohmann::json::boolean_t>();
		}
		else
		{
			theBool = false;
			TodTraceAndLog("[LawnProject] - Missing Boolean in Save File: %s", theName.c_str());
		}
	}
	else
		mJSON[theName] = theBool;
}

void ProfileSyncer::SyncFloat(const SexyString &theName, float &theFloat)
{
	if (mReading)
	{
		if (json_has_key(mJSON, theName) && mJSON[theName].is_number_float())
		{
			theFloat = mJSON[theName].get<nlohmann::json::number_float_t>();
		}
		else
		{
			theFloat = 0.0f;
			TodTraceAndLog("[LawnProject] - Missing Float in Save File: %s", theName.c_str());
		}
	}
	else
		mJSON[theName] = theFloat;
}

void ProfileSyncer::SyncInt(const SexyString &theName, int &theInt)
{
	if (mReading)
	{
		if (json_has_key(mJSON, theName) && mJSON[theName].is_number_integer())
		{
			theInt = mJSON[theName].get<nlohmann::json::number_integer_t>();
		}
		else
		{
			theInt = 0;
			TodTraceAndLog("[LawnProject] - Missing Integer in Save File: %s", theName.c_str());
		}
	}
	else
		mJSON[theName] = theInt;
}

void ProfileSyncer::SyncUnsignedInt(const SexyString &theName, unsigned int &theInt)
{
	if (mReading)
	{
		if (json_has_key(mJSON, theName) && mJSON[theName].is_number_integer())
		{
			theInt = mJSON[theName].get<nlohmann::json::number_integer_t>();
		}
		else
		{
			theInt = 0;
			TodTraceAndLog("[LawnProject] - Missing Integer in Save File: %s", theName.c_str());
		}
	}
	else
		mJSON[theName] = theInt;
}

void ProfileSyncer::SyncLong(const SexyString &theName, long &theLong)
{
	if (mReading)
	{
		if (json_has_key(mJSON, theName) && mJSON[theName].is_number_integer())
		{
			theLong = mJSON[theName].get<nlohmann::json::number_integer_t>();
		}
		else
		{
			theLong = 0;
			TodTraceAndLog("[LawnProject] - Missing Integer in Save File: %s", theName.c_str());
		}
	}
	else
		mJSON[theName] = theLong;
}

void ProfileSyncer::SyncUnsignedLong(const SexyString &theName, unsigned long &theLong)
{
	if (mReading)
	{
		if (json_has_key(mJSON, theName) && mJSON[theName].is_number_integer())
		{
			theLong = mJSON[theName].get<nlohmann::json::number_integer_t>();
		}
		else
		{
			theLong = 0;
			TodTraceAndLog("[LawnProject] - Missing Long in Save File: %s", theName.c_str());
		}
	}
	else
		mJSON[theName] = theLong;
}

void ProfileSyncer::SyncString(const SexyString &theName, SexyString &theStr)
{
	if (mReading)
	{
		if (json_has_key(mJSON, theName) && mJSON[theName].is_string())
		{
			theStr = mJSON[theName].get<nlohmann::json::string_t>();
		}
		else
		{
			theStr = "";
			TodTraceAndLog("[LawnProject] - Missing Integer in Save File: %s", theName.c_str());
		}
	}
	else
		mJSON[theName] = theStr;
}

void to_json(nlohmann::json &j, const PottedPlant &p)
{
	j = {
		{"seed_type", (int)p.mSeedType},
		 {"garden", (int)p.mWhichZenGarden},

		 {"x", p.mX},
		 {"y", p.mY},

		 {"facing", (int)p.mFacing},

		 {"last_watered", p.mLastWateredTime},

		 {"draw_variation", (int)p.mDrawVariation},
		 {"age", (int)p.mPlantAge},

		 {"times_fed", p.mTimesFed},
		 {"feedings_per_grow", p.mFeedingsPerGrow},

		 {"plant_need", (int)p.mPlantNeed},

		 {"last_need_fulfilled_time", p.mLastNeedFulfilledTime},
		 {"last_fertilized_time", p.mLastFertilizedTime},
		 {"last_chocolate_time", p.mLastChocolateTime}
	};
}

void from_json(const nlohmann::json &j, PottedPlant &p)
{
	p.mSeedType = (SeedType)j.value("seed_type", 0);
	p.mWhichZenGarden = (GardenType)j.value("garden", 0);

	p.mX = j.value("x", 0);
	p.mY = j.value("y", 0);

	p.mFacing = (PottedPlant::FacingDirection)j.value("facing", 0);

	p.mLastWateredTime = j.value("last_watered", 0LL);

	p.mDrawVariation = (DrawVariation)j.value("draw_variation", 0);
	p.mPlantAge = (PottedPlantAge)j.value("age", 0);

	p.mTimesFed = j.value("times_fed", 0);
	p.mFeedingsPerGrow = j.value("feedings_per_grow", 0);

	p.mPlantNeed = (PottedPlantNeed)j.value("plant_need", 0);

	p.mLastNeedFulfilledTime = j.value("last_need_fulfilled_time", 0LL);
	p.mLastFertilizedTime = j.value("last_fertilized_time", 0LL);
	p.mLastChocolateTime = j.value("last_chocolate_time", 0LL);
}

void to_json(nlohmann::json &j, const Zombatar &z)
{
	j = {
		{"skin_color", (int)z.mSkinColor},

		{"clothes", z.mClothes},
		{"clothes_color", z.mClothesColor},

		{"tidbits", (int)z.mTidbits},
		{"tidbits_color", (int)z.mTidbitsColor},

		{"accessories", z.mAccessories},
		{"accessories_color", z.mAccessoriesColor},

		{"facial_hair", (int)z.mFacialHair},
		{"facial_hair_color", (int)z.mFacialHairColor},

		{"hair", (int)z.mHair},
		{"hair_color", (int)z.mHairColor},

		{"eyewear", (int)z.mEyewear},
		{"eyewear_color", (int)z.mEyewearColor},

		{"hat", (int)z.mHat},
		{"hat_color", (int)z.mHatColor},

		{"backdrop", (int)z.mBackdrop},
		{"backdrop_color", (int)z.mBackdropColor},
	};
}

void from_json(const nlohmann::json &j, Zombatar &z)
{
	z.mSkinColor = (int)j.value("skin_color", 0);

	z.mClothes = (int)j.value("clothes", -1);
	z.mClothesColor = (int)j.value("clothes_color", 17);

	z.mTidbits = (int)j.value("tidbits", -1);
	z.mTidbitsColor = (int)j.value("tidbits_color", 17);

	z.mAccessories = (int)j.value("accessories", -1);
	z.mAccessoriesColor = (int)j.value("accessories_color", 17);

	z.mFacialHair = (int)j.value("facial_hair", -1);
	z.mFacialHairColor = (int)j.value("facial_hair_color", 17);

	z.mHair = (int)j.value("hair", -1);
	z.mHairColor = (int)j.value("hair_color", 17);

	z.mEyewear = (int)j.value("eyewear", -1);
	z.mEyewearColor = (int)j.value("eyewear_color", 17);

	z.mHat = (int)j.value("hat", -1);
	z.mHatColor = (int)j.value("hat_color", 17);

	z.mBackdrop = (int)j.value("backdrop", 0);
	z.mBackdropColor = (int)j.value("backdrop_color", 17);

}

template <typename T, size_t N> void ProfileSyncer::SyncArray(const SexyString &theName, T (&theArray)[N], int &theRealSize)
{
	if (mReading)
	{
		if (json_has_key(mJSON, theName) && mJSON[theName].is_array())
		{
			const auto &aJsonArray = mJSON[theName];

			size_t aCount = std::min(aJsonArray.size(), N);
			theRealSize = aCount;
			for (size_t i = 0; i < aCount; i++)
			{
				theArray[i] = aJsonArray[i].get<T>();
			}
		}
		else
		{
			memset(theArray, 0, N);
			theRealSize = 0;
			TodTraceAndLog("[LawnProject] - Missing Array in Save File: %s", theName.c_str());
		}
	}
	else
	{
		nlohmann::json aArray = nlohmann::json::array();

		for (size_t i = 0; i < theRealSize; i++)
		{
			aArray.push_back(theArray[i]);
		}

		mJSON[theName] = std::move(aArray);
	}
}

template <typename T, size_t N> void ProfileSyncer::SyncArrayFromSize(const SexyString &theName, T (&theArray)[N], int theRealSize)
{
	if (mReading)
	{
		if (json_has_key(mJSON, theName) && mJSON[theName].is_array())
		{
			const auto &aJsonArray = mJSON[theName];

			size_t aCount = std::min(aJsonArray.size(), (size_t)theRealSize);
			for (size_t i = 0; i < aCount; i++)
			{
				theArray[i] = aJsonArray[i].get<T>();
			}
		}
		else
		{
			memset(theArray, 0, theRealSize);
			TodTraceAndLog("[LawnProject] - Missing Array in Save File: %s", theName.c_str());
		}
	}
	else
	{
		nlohmann::json aArray = nlohmann::json::array();

		for (size_t i = 0; i < theRealSize; i++)
		{
			aArray.push_back(theArray[i]);
		}

		mJSON[theName] = std::move(aArray);
	}
}