#if LAWN_DEBUG_TOOLS

#include "DebuggerWindow.h"
#include "../Board.h"
#include "../ZenGarden.h"
#include "../System/PlayerInfo.h"
#include "../Widget/ChallengeScreen.h"
#include "../Widget/AchievementsWidget.h"
#include "../Widget/GameSelector.h"
#include "../Sexy.TodLib/EffectSystem.h"
#include "../Sexy.TodLib/Attachment.h"
#include "../Sexy.TodLib/Trail.h"
#include "../Sexy.TodLib/Reanimator.h"
#include "../Sexy.TodLib/TodParticle.h"
#include "../SexyAppFramework/MTRand.h"
#include "../SexyAppFramework/WidgetManager.h"
#include "../Challenge.h"
#include "../../LawnApp.h"
#include "../../Sexy.TodLib/TodStringFile.h"

using namespace Sexy;

DebuggerWindow::DebuggerWindow(LawnApp *theApp) : ImGuiWindow(theApp->mImGuiManager)
{
	mApp = theApp;
}

DebuggerWindow::~DebuggerWindow()
{
	ImGuiWindow::~ImGuiWindow();
}

void DebuggerWindow::Update()
{
	MTRand::gIgnoreAssert = true;
	ImGui::Begin("LawnTools");
	if (ImGui::BeginTabBar("LawnTabs"))
	{
		if (ImGui::BeginTabItem("Board"))
		{
			if (mApp->mBoard == nullptr)
			{
				ImGui::Text("Not currently on Board");
			}
			else
			{
				if (ImGui::BeginTabBar("BoardTabs"))
				{
					if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN)
					{
						if (ImGui::BeginTabItem("Zen Garden"))
						{
							ImGui::Text("Total Potted Plant Count: %d", mApp->mPlayerInfo->mNumPottedPlants);
							for (int aGarden = 0; aGarden < GardenType::GARDEN_TOTAL; aGarden++)
							{
								if (aGarden == GARDEN_WHEELBARROW)
									ImGui::SeparatorText("Wheelbarrow");
								else
									ImGui::SeparatorText(StrFormat("Garden %d", aGarden).c_str());
								int aGardenPlantCount = 0;
								for (int i = 0; i < mApp->mPlayerInfo->mNumPottedPlants; i++)
								{
									PottedPlant *aPottedPlant = mApp->mZenGarden->PottedPlantFromIndex(i);
									if (aPottedPlant->mWhichZenGarden == aGarden)
									{
										aGardenPlantCount++;
									}
								}
								ImGui::Text("Potten Plant Count: %d", aGardenPlantCount);

							}

							ImGui::EndTabItem();
						}
					}
					else if (mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM)
					{
						if (ImGui::BeginTabItem("Zen Garden"))
						{
							ImGui::Text("Tree Height: %d",mApp->mPlayerInfo->mChallengeRecords[mApp->GetCurrentChallengeIndex()]);

							ImGui::EndTabItem();
						}

					}
					else
					{
						if (mApp->mGameMode == GameMode::GAMEMODE_ADVENTURE)
						{
							if (ImGui::BeginTabItem("Adventure"))
							{
								ImGui::Text("Current Level: %s",  mApp->GetStageString(mApp->mBoard->mLevel).c_str());
								ImGui::SeparatorText("Waves");

								ImGui::Text("Wave Count: %d", mApp->mBoard->mNumWaves);
								ImGui::Text("Flag Count: %d",
											mApp->mBoard->mNumWaves / mApp->mBoard->GetNumWavesPerFlag());

								ImGui::Text("Current Wave: %d", mApp->mBoard->mCurrentWave);
								ImGui::Text("Current Flag: %d",
											mApp->mBoard->mCurrentWave / mApp->mBoard->GetNumWavesPerFlag());

								ImGui::EndTabItem();
							}
						}
						else
						{
							if (ImGui::BeginTabItem("Challenge"))
							{
								ImGui::Text("Current Challenge: %s",
											TodStringTranslate(mApp->GetCurrentChallengeDef().mChallengeName).c_str());
								ImGui::SeparatorText("Waves");

								ImGui::Text("Wave Count: %d", mApp->mBoard->mNumWaves);
								ImGui::Text("Flag Count: %d",
											mApp->mBoard->mNumWaves / mApp->mBoard->GetNumWavesPerFlag());

								ImGui::Text("Current Wave: %d", mApp->mBoard->mCurrentWave);
								ImGui::Text("Current Flag: %d",
											mApp->mBoard->mCurrentWave / mApp->mBoard->GetNumWavesPerFlag());

								ImGui::EndTabItem();
							}
						}
						if (ImGui::BeginTabItem("Zombies"))
						{
							if (ImGui::BeginTabBar("ZombieTabs"))
							{
								if (ImGui::BeginTabItem("Spawning"))
								{
									for (int i = ZOMBIE_NORMAL; i < NUM_ZOMBIE_TYPES; i++)
									{
										if (((ZombieType)i == ZOMBIE_YETI && mApp->CanSpawnYetis()) || ((ZombieType)i != ZOMBIE_YETI && mApp->mBoard->mZombieAllowed[i]))
											ImGui::Text(GetZombieDefinition((ZombieType)i).mZombieName);
									}

									ImGui::EndTabItem();
								}
								if (ImGui::BeginTabItem("Debug Spawn"))
								{
									for (int i = ZOMBIE_NORMAL; i < NUM_ZOMBIE_TYPES; i++)
									{
										if (ImGui::Button(GetZombieDefinition((ZombieType)i).mZombieName))
										{
											mApp->mBoard->AddZombie((ZombieType)i, Zombie::ZOMBIE_WAVE_DEBUG);
										}
									}

									ImGui::EndTabItem();
								}
								if (ImGui::BeginTabItem("Debug"))
								{

									if (mApp->mBoard->mDebugSelectedZombie)
									{
										ZombieDefinition aDefinition = GetZombieDefinition(mApp->mBoard->mDebugSelectedZombie->mZombieType);
										ImGui::Text("Currently Selected Zombie: %s", aDefinition.mZombieName);
										ImGui::SeparatorText("General Information");
										ImGui::Text("X: %d", mApp->mBoard->mDebugSelectedZombie->mX);
										ImGui::Text("Y: %d", mApp->mBoard->mDebugSelectedZombie->mY);
										ImGui::SeparatorText("Gameplay Info");

										ImGui::Text("Body Health: %d", mApp->mBoard->mDebugSelectedZombie->mBodyHealth);
										if (mApp->mBoard->mDebugSelectedZombie->mHelmType != HelmType::HELMTYPE_NONE)
										{
											ImGui::Text("Helmet Health: %d", mApp->mBoard->mDebugSelectedZombie->mHelmHealth);
										}
										if (mApp->mBoard->mDebugSelectedZombie->mShieldType != ShieldType::SHIELDTYPE_NONE)
										{
											ImGui::Text("Shield Health: %d", mApp->mBoard->mDebugSelectedZombie->mShieldHealth);
										}

										ImGui::Text("Row: %d", mApp->mBoard->mDebugSelectedZombie->mRow);
										ImGui::Text("In Pool: %s", mApp->mBoard->mDebugSelectedZombie->mInPool ? "true" : "false");
										ImGui::Text("MindControlled: %s", mApp->mBoard->mDebugSelectedZombie->mMindControlled ? "true" : "false");
									}
									ImGui::EndTabItem();
								}

								ImGui::EndTabBar();
							}

							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Stats"))
						{
							ImGui::SeparatorText("Game Objects");
							ImGui::Text("Plants: %d", mApp->mBoard->mPlants.mSize);
							ImGui::Text("Zombies: %d", mApp->mBoard->mZombies.mSize);
							ImGui::Text("Projectiles: %d", mApp->mBoard->mProjectiles.mSize);
							ImGui::Text("Coins: %d", mApp->mBoard->mCoins.mSize);
							ImGui::Text("Lawn Mowers: %d", mApp->mBoard->mLawnMowers.mSize);
							ImGui::Text("Grid Items: %d", mApp->mBoard->mGridItems.mSize);

							ImGui::SeparatorText("Generic Objects");
							ImGui::Text("Reanimations: %d", mApp->mEffectSystem->mReanimationHolder->mReanimations.mSize);

							ImGui::Text("Particle Systems: %d", mApp->mEffectSystem->mParticleHolder->mParticleSystems.mSize);
							ImGui::Text("Particles: %d", mApp->mEffectSystem->mParticleHolder->mParticles.mSize);
							ImGui::Text("Emitters: %d", mApp->mEffectSystem->mParticleHolder->mEmitters.mSize);

							ImGui::Text("Trails: %d", mApp->mEffectSystem->mTrailHolder->mTrails.mSize);

							ImGui::Text("Attachments: %d", mApp->mEffectSystem->mAttachmentHolder->mAttachments.mSize);


							ImGui::EndTabItem();
						}

					}


					ImGui::EndTabBar();
				}
			}
				
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Save File"))
		{
			if (mApp->mPlayerInfo == nullptr)
			{
				ImGui::Text("Save File is null");
			}
			else
			{
				ImGui::SeparatorText(StrFormat("Current Profile: %s", mApp->mPlayerInfo->mName.c_str()).c_str());

				if (ImGui::BeginTabBar("SaveTabs"))
				{

					if (ImGui::BeginTabItem("Info"))
					{
						int aArea = ClampInt((mApp->mPlayerInfo->mLevel - 1) / LEVELS_PER_AREA + 1, 1, ADVENTURE_AREAS + 1);
						int aSub = mApp->mPlayerInfo->mLevel - (aArea - 1) * LEVELS_PER_AREA;
						if (ImGui::InputInt("Main Level", &aArea))
						{
							aArea = ClampInt(aArea, 1, ADVENTURE_AREAS + 1);

							mApp->mPlayerInfo->mLevel = (aArea - 1) * LEVELS_PER_AREA + aSub;
						}
						if (ImGui::InputInt("Sub Level", &aSub))
						{
							aSub = ClampInt(aSub, 1, LEVELS_PER_AREA);

							mApp->mPlayerInfo->mLevel = (aArea - 1) * LEVELS_PER_AREA + aSub;
						}

						ImGui::InputInt("Level ID", &mApp->mPlayerInfo->mLevel);
						mApp->mPlayerInfo->mLevel = ClampInt(mApp->mPlayerInfo->mLevel, 1, NUM_LEVELS);
						if (mApp->mGameSelector != nullptr)
							mApp->mGameSelector->mLevel = mApp->mPlayerInfo->mLevel;
						ImGui::InputInt("Coins", &mApp->mPlayerInfo->mCoins);
						ImGui::InputInt("Finished Adventure", &mApp->mPlayerInfo->mFinishedAdventure);
						mApp->mPlayerInfo->mFinishedAdventure = ClampInt(mApp->mPlayerInfo->mFinishedAdventure, 0, INT_MAX);
						ImGui::Checkbox("Used Cheat Keys", &mApp->mPlayerInfo->mHasUsedCheatKeys);

						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Purchases"))
					{
						for (int i = 0; i < NUM_STORE_ITEM_MAX; i++)
						{
							ImGui::InputInt(StrFormat("Item %d", i).c_str(), &mApp->mPlayerInfo->mPurchases[i]);
						}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Records"))
					{
						for (int i = 0; i < NUM_GAME_MODES - 1; i++)
						{
							ImGui::InputInt(TodStringTranslate(gChallengeDefs[i].mChallengeName).c_str(), &mApp->mPlayerInfo->mChallengeRecords[i]);
						}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Achievements"))
					{
						for (int i = 0; i < NUM_ACHIEVEMENT_TYPES; i++)
						{
							ImGui::SeparatorText(TodStringTranslate(gAchievementDefs[i].mName).c_str());
							ImGui::PushID(i);
							ImGui::Checkbox("Earned", &mApp->mPlayerInfo->mEarnedAchievements[i]);
							ImGui::PopID();
							ImGui::PushID(i + NUM_ACHIEVEMENT_TYPES);
							ImGui::Checkbox("Shown", &mApp->mPlayerInfo->mShownAchievements[i]);
							ImGui::PopID();
						}

						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	
	ImGui::End();
	MTRand::gIgnoreAssert = false;
}

#endif