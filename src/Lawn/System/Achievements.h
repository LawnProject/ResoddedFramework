#pragma once

#include "../../ConstEnums.h"
#include "../../SexyAppFramework/Common.h"

class LawnApp;

class AchievementDefinition
{
  public:
	const SexyCharByte *mName;
	const SexyCharByte *mDescription;
};

extern AchievementDefinition gAchievementDefs[NUM_ACHIEVEMENT_TYPES];

class Achievements
{
  public:

  public:
	LawnApp *mApp;

	Achievements(LawnApp *theApp);
	~Achievements();

	static AchievementDefinition GetAchievementDefinition(AchievementID theAchievement);
	void GiveAchievement(AchievementID theAchievement, bool aForceGive);
};
