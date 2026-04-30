#if LAWN_DEBUG_TOOLS

#include "DebuggerWindow.h"
#include "../Board.h"
#include "../ZenGarden.h"
#include "../Widget/ChallengeScreen.h"
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
			ImGui::Text("Set some data here");
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	
	ImGui::End();
	MTRand::gIgnoreAssert = false;
}

#endif