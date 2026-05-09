#include "Achievements.h"
#include "PlayerInfo.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../Board.h"
#include "../Widget/GameSelector.h"

AchievementDefinition gAchievementDefs[NUM_ACHIEVEMENT_TYPES] = {
	{"[ACHIEVEMENT_HOME_SECURITY_TITLE]", "[ACHIEVEMENT_HOME_SECURITY_DESCRIPTION]"},
	{"[ACHIEVEMENT_NOBEL_PEAS_PRIZE_TITLE]", "[ACHIEVEMENT_NOBEL_PEAS_PRIZE_DESCRIPTION]"},
	{"[ACHIEVEMENT_BETTER_OFF_DEAD_TITLE]", "[ACHIEVEMENT_BETTER_OFF_DEAD_DESCRIPTION]"},
	{"[ACHIEVEMENT_CHINA_SHOP_TITLE]", "[ACHIEVEMENT_CHINA_SHOP_DESCRIPTION]"},
	{"[ACHIEVEMENT_SPUDOW_TITLE]", "[ACHIEVEMENT_SPUDOW_DESCRIPTION]"},
	{"[ACHIEVEMENT_EXPLODONATOR_TITLE]", "[ACHIEVEMENT_EXPLODONATOR_DESCRIPTION]"},
	{"[ACHIEVEMENT_MORTICULTURALIST_TITLE]", "[ACHIEVEMENT_MORTICULTURALIST_DESCRIPTION]"},
	{"[ACHIEVEMENT_DONT_PEA_IN_POOL_TITLE]", "[ACHIEVEMENT_DONT_PEA_IN_POOL_DESCRIPTION]"},
	{"[ACHIEVEMENT_ROLL_SOME_HEADS_TITLE]", "[ACHIEVEMENT_ROLL_SOME_HEADS_DESCRIPTION]"},
	{"[ACHIEVEMENT_GROUNDED_TITLE]", "[ACHIEVEMENT_GROUNDED_DESCRIPTION]"},
	{"[ACHIEVEMENT_ZOMBOLOGIST_TITLE]", "[ACHIEVEMENT_ZOMBOLOGIST_DESCRIPTION]"},
	{"[ACHIEVEMENT_PENNY_PINCHER_TITLE]", "[ACHIEVEMENT_PENNY_PINCHER_DESCRIPTION]"},
	{"[ACHIEVEMENT_SUNNY_DAYS_TITLE]", "[ACHIEVEMENT_SUNNY_DAYS_DESCRIPTION]"},
	{"[ACHIEVEMENT_POPCORN_PARTY_TITLE]", "[ACHIEVEMENT_POPCORN_PARTY_DESCRIPTION]"},
	{"[ACHIEVEMENT_GOOD_MORNING_TITLE]", "[ACHIEVEMENT_GOOD_MORNING_DESCRIPTION]"},
	{"[ACHIEVEMENT_NO_FUNGUS_AMONG_US_TITLE]", "[ACHIEVEMENT_NO_FUNGUS_AMONG_US_DESCRIPTION]"},
	{"[ACHIEVEMENT_BEYOND_THE_GRAVE_TITLE]", "[ACHIEVEMENT_BEYOND_THE_GRAVE_DESCRIPTION]"},
	{"[ACHIEVEMENT_IMMORTAL_TITLE]", "[ACHIEVEMENT_IMMORTAL_DESCRIPTION]"},
	{"[ACHIEVEMENT_TOWERING_WISDOM_TITLE]", "[ACHIEVEMENT_TOWERING_WISDOM_DESCRIPTION]"},
	{"[ACHIEVEMENT_MUSTACHE_MODE_TITLE]", "[ACHIEVEMENT_MUSTACHE_MODE_DESCRIPTION]"},
	{"[ACHIEVEMENT_DISCO_IS_UNDEAD_TITLE]", "[ACHIEVEMENT_DISCO_IS_UNDEAD_DESCRIPTION]"},
};

AchievementDefinition Achievements::GetAchievementDefinition(AchievementID theAchievement)
{
	return gAchievementDefs[theAchievement];
}

Achievements::Achievements(LawnApp *theApp)
{
	mApp = theApp;
}

Achievements ::~Achievements()
{

}

void Achievements::GiveAchievement(AchievementID theAchievement, bool aForceGive)
{
	if (mApp->mPlayerInfo == nullptr)
		return;
	if (!aForceGive && mApp->mPlayerInfo->mEarnedAchievements[theAchievement])
		return;

	mApp->mPlayerInfo->mEarnedAchievements[theAchievement] = true;

	if (mApp->mBoard == nullptr)
		return;

	mApp->PlaySample(Sexy::SOUND_ACHIEVEMENT);

	mApp->mBoard->DisplayAdvice(
		StrFormat("%s%s", TodStringTranslate(Achievements::GetAchievementDefinition(theAchievement).mName).c_str(), TodStringTranslate("[ACHIEVEMENT_ACHIEVED]").c_str()),
		MessageStyle::MESSAGE_STYLE_ACHIEVEMENT, AdviceType::ADVICE_NONE
	);
}

void Achievements::SyncAchievements()
{
	if (mApp->mPlayerInfo == nullptr)
		return;

	if (mApp->HasFinishedAdventure())
		GiveAchievement(AchievementID::ACHIEVEMENT_HOME_SECURITY, true);

	if (mApp->EarnedGoldTrophy())
		GiveAchievement(AchievementID::ACHIEVEMENT_NOVEL_PEAS_PRIZE, true);

	if (mApp->CanSpawnYetis())
		GiveAchievement(AchievementID::ACHIEVEMENT_ZOMBOLOGIST, true);

	if (mApp->mPlayerInfo->mChallengeRecords[GAMEMODE_TREE_OF_WISDOM - 1])
		GiveAchievement(AchievementID::ACHIEVEMENT_TOWERING_WISDOM, true);

	bool aHasAllPlants = true;
	for (int i = SeedType::SEED_PEASHOOTER; i < SeedType::NUM_SEEDS_IN_CHOOSER; i++)
	{
		if (!mApp->SeedTypeAvailable(SeedType(i)))
			aHasAllPlants = false;
	}

	if (aHasAllPlants)
		GiveAchievement(AchievementID::ACHIEVEMENT_MORTICULTURALIST, true);
}
