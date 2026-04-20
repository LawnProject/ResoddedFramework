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
						ImGui::Text("Tree Height: %d", mApp->mPlayerInfo->mChallengeRecords[mApp->GetCurrentChallengeIndex()]);

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
							ImGui::SeparatorText("Zombie Spawning");
							for (int i = 0; i < 100; i++)
							{
								if (mApp->mBoard->mZombieAllowed[i])
								{
									ImGui::Text(GetZombieDefinition((ZombieType)i).mZombieName);
								}
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
}

#endif