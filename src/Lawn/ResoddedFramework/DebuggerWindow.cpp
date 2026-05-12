#if LAWN_DEBUG_TOOLS

#include "DebuggerWindow.h"
#include "../Board.h"
#include "../Plant.h"
#include "../Zombie.h"
#include "../ZenGarden.h"
#include "../System/PlayerInfo.h"
#include "../Widget/ChallengeScreen.h"
#include "../System/Achievements.h"
#include "../Widget/GameSelector.h"
#include "../../Sexy.TodLib/EffectSystem.h"
#include "../../Sexy.TodLib/Attachment.h"
#include "../../Sexy.TodLib/Trail.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../Sexy.TodLib/TodParticle.h"
#include "../../SexyAppFramework/MTRand.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../Challenge.h"
#include "../../LawnApp.h"
#include "../../Sexy.TodLib/TodStringFile.h"

using namespace Sexy;

// ─────────────────────────────────────────────────────────────
// Static name helpers  (no hardcoded gameplay values)
// ─────────────────────────────────────────────────────────────
const char* DebuggerWindow::SeedTypeName(int i)
{
    if (i < 0 || i >= (int)SeedType::NUM_SEED_TYPES) return "???";
    return gPlantDefs[i].mPlantName;
}
const char* DebuggerWindow::ZombieTypeName(int i)
{
    if (i < 0 || i >= (int)ZombieType::NUM_ZOMBIE_TYPES) return "???";
    return gZombieDefs[i].mZombieName;
}
const char* DebuggerWindow::GameModeName(int m)
{
    switch ((GameMode)m)
    {
    case GAMEMODE_ADVENTURE:                     return "Adventure";
    case GAMEMODE_SURVIVAL_NORMAL_STAGE_1:       return "Surv Normal 1";
    case GAMEMODE_SURVIVAL_HARD_STAGE_1:         return "Surv Hard 1";
    case GAMEMODE_SURVIVAL_ENDLESS_STAGE_1:      return "Surv Endless 1";
    case GAMEMODE_CHALLENGE_ZEN_GARDEN:          return "Zen Garden";
    case GAMEMODE_CHALLENGE_FINAL_BOSS:          return "Final Boss";
    case GAMEMODE_TREE_OF_WISDOM:                return "Tree of Wisdom";
    default: return "Other";
    }
}
const char* DebuggerWindow::GameSceneName(int s)
{
    switch ((GameScenes)s)
    {
    case SCENE_LOADING: return "Loading";
    case SCENE_MENU:    return "Menu";
    case SCENE_PLAYING: return "Playing";
    case SCENE_AWARD:   return "Award";
    case SCENE_CREDIT:  return "Credit";
    default: return "Other";
    }
}
const char* DebuggerWindow::ZombiePhaseName(ZombiePhase p)
{
    switch (p)
    {
    case PHASE_ZOMBIE_NORMAL:   return "Normal";
    case PHASE_ZOMBIE_DYING:    return "Dying";
    case PHASE_ZOMBIE_BURNED:   return "Burned";
    case PHASE_ZOMBIE_MOWERED:  return "Mowered";
    case PHASE_DIGGER_TUNNELING:return "Tunneling";
    case PHASE_POGO_BOUNCING:   return "Pogo";
    case PHASE_BUNGEE_DIVING:   return "Bungee";
    case PHASE_DANCER_DANCING_IN: return "Dancing";
    default: return "Other";
    }
}
const char* DebuggerWindow::PlantStateName(PlantState s)
{
    switch (s)
    {
    case STATE_NOTREADY:        return "Not Ready";
    case STATE_READY:           return "Ready";
    case STATE_DOINGSPECIAL:    return "Special";
    case STATE_SQUASH_RISING:   return "Squash Rise";
    case STATE_SQUASH_FALLING:  return "Squash Fall";
    case STATE_CHOMPER_BITING:  return "Chomper Bite";
    case STATE_POTATO_ARMED:    return "Potato Armed";
    case STATE_COBCANNON_READY: return "CobReady";
    case STATE_COBCANNON_FIRING:return "CobFiring";
    default: return "Other";
    }
}
const char* DebuggerWindow::DebugTextModeName(DebugTextMode m)
{
    switch (m)
    {
    case DEBUG_TEXT_NONE:       return "None";
    case DEBUG_TEXT_ZOMBIE_SPAWN: return "Zombie Spawn";
    case DEBUG_TEXT_MUSIC:      return "Music";
    case DEBUG_TEXT_MEMORY:     return "Memory";
    case DEBUG_TEXT_COLLISION:  return "Collision";
    case DEBUG_TEXT_GRID:       return "Grid";
    default: return "Other";
    }
}
const char* DebuggerWindow::CoinTypeName(CoinType c)
{
    switch (c)
    {
    case COIN_SILVER:   return "Silver";
    case COIN_GOLD:     return "Gold";
    case COIN_DIAMOND:  return "Diamond";
    case COIN_SUN:      return "Sun (150)";
    case COIN_SMALLSUN: return "Small Sun (25)";
    case COIN_LARGESUN: return "Large Sun (50)";
    default: return "Other";
    }
}
const char* DebuggerWindow::CoinMotionName(CoinMotion m)
{
    switch (m)
    {
    case COIN_MOTION_FROM_SKY:      return "From Sky";
    case COIN_MOTION_FROM_SKY_SLOW: return "Sky Slow";
    case COIN_MOTION_FROM_PLANT:    return "From Plant";
    case COIN_MOTION_COIN:          return "Coin";
    default: return "Other";
    }
}

// ─────────────────────────────────────────────────────────────
// Constructor / Destructor
// ─────────────────────────────────────────────────────────────
DebuggerWindow::DebuggerWindow(LawnApp *theApp) : ImGuiWindow(theApp->mImGuiManager)
{
    mApp                = theApp;
    mSelectedPlant      = nullptr;
    mQuickPlantSeedIdx  = 0;
    mQuickPlantCol      = 0;
    mQuickPlantRow      = 0;
    mSpawnZombieTypeIdx = 0;
    mSpawnZombieRow     = 0;
    mWaveEditorWaveIdx  = 0;
    mAddSunAmount       = 100;
    mSpawnCoinType      = (int)COIN_GOLD;
    mSpawnCoinMotion    = (int)COIN_MOTION_COIN;
    mShowImGuiDemo      = false;
    mDebugDrawGrid      = false;
    mDebugDrawRects     = false;
    mLogAutoScroll      = true;
    mFastLoadModeIdx    = 0;
}
DebuggerWindow::~DebuggerWindow()
{
    ImGuiWindow::~ImGuiWindow();
}
void DebuggerWindow::PushLog(const std::string& msg)
{
    if (mLog.size() >= 256) mLog.pop_front();
    mLog.push_back(msg);
}

// ─────────────────────────────────────────────────────────────
// Main Update
// ─────────────────────────────────────────────────────────────
void DebuggerWindow::Update()
{
    MTRand::gIgnoreAssert = true;
    ImGui::Begin("LawnTools", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("ImGui Demo", nullptr, &mShowImGuiDemo);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    if (mShowImGuiDemo) ImGui::ShowDemoWindow(&mShowImGuiDemo);

    if (ImGui::BeginTabBar("LawnTabs"))
    {
        if (ImGui::BeginTabItem("Board"))  { DrawBoardTab();    ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Save"))   { DrawSaveFileTab(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Debug"))  { DrawDebugTab();    ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("App"))    { DrawAppTab();      ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }
    ImGui::End();
    MTRand::gIgnoreAssert = false;
}

// ─────────────────────────────────────────────────────────────
// Board tab
// ─────────────────────────────────────────────────────────────
void DebuggerWindow::DrawBoardTab()
{
    if (mApp->mBoard == nullptr) { ImGui::TextDisabled("No board active"); return; }

    if (ImGui::BeginTabBar("BoardTabs"))
    {
        if (ImGui::BeginTabItem("Cheats"))  { DrawBoardCheats();  ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Plants"))  { DrawBoardPlants();  ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Zombies")) { DrawBoardZombies(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Waves"))   { DrawBoardWaves();   ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Stats"))   { DrawBoardStats();   ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }
}

// ─────────────────────────────────────────────────────────────
// Cheats sub-tab
// ─────────────────────────────────────────────────────────────
void DebuggerWindow::DrawBoardCheats()
{
    Board* b = mApp->mBoard;

    // Sun
    ImGui::SeparatorText("Sun / Economy");
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("Sun Money", &b->mSunMoney);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##addamt", &mAddSunAmount);
    ImGui::SameLine();
    if (ImGui::Button("+Sun"))
        b->AddSunMoney(mAddSunAmount);
    ImGui::SameLine();
    if (ImGui::Button("Max")) b->mSunMoney = 9990;

    // Spawn coin
    ImGui::SetNextItemWidth(130);
    {
        // Coin type combo
        static const CoinType kCTypes[] = { COIN_SILVER, COIN_GOLD, COIN_DIAMOND, COIN_SUN, COIN_SMALLSUN, COIN_LARGESUN };
        const char* cPreview = CoinTypeName(kCTypes[mSpawnCoinType < 6 ? mSpawnCoinType : 0]);
        if (ImGui::BeginCombo("Coin Type", cPreview))
        {
            for (int i = 0; i < 6; i++)
            {
                bool sel = (mSpawnCoinType == i);
                if (ImGui::Selectable(CoinTypeName(kCTypes[i]), sel)) mSpawnCoinType = i;
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Spawn Coin"))
    {
        static const CoinType kCTypes[] = { COIN_SILVER, COIN_GOLD, COIN_DIAMOND, COIN_SUN, COIN_SMALLSUN, COIN_LARGESUN };
        b->AddCoin(400, 300, kCTypes[mSpawnCoinType], COIN_MOTION_FROM_SKY);
    }

    // Cheat modes
    ImGui::SeparatorText("Cheat Modes");
    bool mu = b->mMustacheMode;
    if (ImGui::Checkbox("Mustache", &mu))   b->SetMustacheMode(mu);
    ImGui::SameLine();
    bool sm = b->mSuperMowerMode;
    if (ImGui::Checkbox("SuperMower", &sm)) b->SetSuperMowerMode(sm);
    ImGui::SameLine();
    bool fu = b->mFutureMode;
    if (ImGui::Checkbox("Future", &fu))     b->SetFutureMode(fu);

    bool pi = b->mPinataMode;
    if (ImGui::Checkbox("Pinata", &pi))     b->SetPinataMode(pi);
    ImGui::SameLine();
    bool da = b->mDanceMode;
    if (ImGui::Checkbox("Dance", &da))      b->SetDanceMode(da);
    ImGui::SameLine();
    bool di = b->mDaisyMode;
    if (ImGui::Checkbox("Daisy", &di))      b->SetDaisyMode(di);
    ImGui::SameLine();
    bool su = b->mSukhbirMode;
    if (ImGui::Checkbox("Sukhbir", &su))    b->SetSukhbirMode(su);

    // Time / Flow controls
    ImGui::SeparatorText("Time Control");
    ImGui::Checkbox("Slow-Mo", &gSlowMo);
    ImGui::SameLine();
    ImGui::Checkbox("Fast-Mo", &gFastMo);
    ImGui::SameLine();
    bool paused = b->mPaused;
    if (ImGui::Checkbox("Paused", &paused)) b->Pause(paused);

    // Fog
    ImGui::SeparatorText("Fog");
    ImGui::Checkbox("Easy Planting", &mApp->mEasyPlantingCheat);
    if (b->StageHasFog())
    {
        ImGui::SliderFloat("Fog Offset", &b->mFogOffset, -200.0f, 800.0f);
        if (ImGui::Button("Blow All Fog")) b->mFogBlownCountDown = 1;
    }
    else ImGui::TextDisabled("(no fog on this level)");

    // Actions
    ImGui::SeparatorText("Actions");
    if (ImGui::Button("Kill All Zombies"))  b->RemoveAllZombies();
    ImGui::SameLine();
    if (ImGui::Button("End Level"))         mApp->EndLevel();
    ImGui::SameLine();
    if (ImGui::Button("Fade Out Level"))    b->FadeOutLevel();

    if (ImGui::Button("Skip to Last Wave"))
    {
        b->mCurrentWave = b->mNumWaves - 1;
        b->mZombieCountDown = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("Trigger Huge Wave")) b->mHugeWaveCountDown = 1;

    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("Bonus Mowers", &b->mBonusLawnMowersRemaining);
    ImGui::InputInt("Ice Timer Row 0", &b->mIceTimer[0]);
    ImGui::InputInt("Time Stop Ctr",   &b->mTimeStopCounter);
    ImGui::InputInt("Shake (X)",       &b->mShakeAmountX);
    ImGui::SameLine();
    ImGui::InputInt("Shake (Y)",       &b->mShakeAmountY);
    if (ImGui::Button("Do Shake")) b->ShakeBoard(b->mShakeAmountX, b->mShakeAmountY);
}

// ─────────────────────────────────────────────────────────────
// Plants sub-tab
// ─────────────────────────────────────────────────────────────
void DebuggerWindow::DrawBoardPlants()
{
    Board* b = mApp->mBoard;

    // Quick Plant
    ImGui::SeparatorText("Quick Plant");
    {
        // Seed combo
        static char sSeedFilter[64] = "";
        ImGui::SetNextItemWidth(160);
        const char* seedPreview = SeedTypeName(mQuickPlantSeedIdx);
        if (ImGui::BeginCombo("Seed##qp", seedPreview))
        {
            ImGui::InputText("Filter", sSeedFilter, sizeof(sSeedFilter));
            for (int i = 0; i < (int)SeedType::NUM_SEED_TYPES; i++)
            {
                const char* name = SeedTypeName(i);
                if (sSeedFilter[0] != '\0' && strstr(name, sSeedFilter) == nullptr) continue;
                bool sel = (mQuickPlantSeedIdx == i);
                if (ImGui::Selectable(name, sel)) mQuickPlantSeedIdx = i;
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("Col##qp", &mQuickPlantCol);
        mQuickPlantCol = std::max(0, std::min(mQuickPlantCol, MAX_GRID_SIZE_X - 1));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("Row##qp", &mQuickPlantRow);
        mQuickPlantRow = std::max(0, std::min(mQuickPlantRow, MAX_GRID_SIZE_Y - 1));
        ImGui::SameLine();
        if (ImGui::Button("Plant!"))
            b->AddPlant(mQuickPlantCol, mQuickPlantRow, (SeedType)mQuickPlantSeedIdx);
    }

    // Plant inspector table
    ImGui::SeparatorText("Plant Inspector");
    if (mApp->mBoard->mDebugSelectedPlant)
    {
        Plant* p = mApp->mBoard->mDebugSelectedPlant;
        ImGui::Text("Selected: %s  Col=%d Row=%d", SeedTypeName(p->mSeedType), p->mPlantCol, p->mRow);
        ImGui::PushID("selplant");

        int hp = p->mPlantHealth;
        if (ImGui::InputInt("HP", &hp)) p->mPlantHealth = std::max(0, hp);
        ImGui::SameLine();
        if (ImGui::Button("Full HP")) p->mPlantHealth = p->mPlantMaxHealth;

        ImGui::InputInt("Max HP", &p->mPlantMaxHealth);
        ImGui::InputInt("Launch Rate", &p->mLaunchRate);
        ImGui::InputInt("State Countdown", &p->mStateCountdown);
        ImGui::Checkbox("Is Asleep", &p->mIsAsleep);
        ImGui::SameLine();
        ImGui::Checkbox("Squished", &p->mSquished);
        ImGui::SameLine();
        ImGui::Checkbox("Dead", &p->mDead);

        if (ImGui::Button("Kill Plant"))     p->Die();
        ImGui::SameLine();
        if (ImGui::Button("Squish Plant"))   p->Squish();
        ImGui::SameLine();
        if (ImGui::Button("Do Special"))     p->DoSpecial();
        ImGui::SameLine();
        if (ImGui::Button("Deselect"))       mApp->mBoard->mDebugSelectedPlant = nullptr;
        ImGui::PopID();
        ImGui::Separator();
    }

    // Table of all plants
    ImGui::Text("Total Plants: %d", b->mPlants.mSize);
    if (ImGui::BeginTable("PlantTable", 7,
        ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg,
        ImVec2(0, 300)))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Col");
        ImGui::TableSetupColumn("Row");
        ImGui::TableSetupColumn("HP");
        ImGui::TableSetupColumn("MaxHP");
        ImGui::TableSetupColumn("State");
        ImGui::TableSetupColumn("Action");
        ImGui::TableHeadersRow();

        Plant* aPlant = nullptr;
        int idx = 0;
        while (b->IteratePlants(aPlant))
        {
            ImGui::PushID(idx++);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(SeedTypeName(aPlant->mSeedType));
            ImGui::TableSetColumnIndex(1); ImGui::Text("%d", aPlant->mPlantCol);
            ImGui::TableSetColumnIndex(2); ImGui::Text("%d", aPlant->mRow);
            ImGui::TableSetColumnIndex(3);
            {
                int hp = aPlant->mPlantHealth;
                ImGui::SetNextItemWidth(70);
                if (ImGui::InputInt("##hp", &hp)) aPlant->mPlantHealth = std::max(0, hp);
            }
            ImGui::TableSetColumnIndex(4); ImGui::Text("%d", aPlant->mPlantMaxHealth);
            ImGui::TableSetColumnIndex(5); ImGui::TextUnformatted(PlantStateName(aPlant->mState));
            ImGui::TableSetColumnIndex(6);
            if (ImGui::SmallButton("Select"))
            {
                b->mDebugSelectedPlant = aPlant;
                mSelectedPlant = aPlant;
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Kill")) aPlant->Die();
            ImGui::SameLine();
            if (ImGui::SmallButton("Heal")) aPlant->mPlantHealth = aPlant->mPlantMaxHealth;
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    ImGui::Separator();
    if (ImGui::Button("Kill ALL Plants"))
    {
        Plant* aP = nullptr;
        while (b->IteratePlants(aP)) aP->Die();
    }
    ImGui::SameLine();
    if (ImGui::Button("Heal ALL Plants"))
    {
        Plant* aP = nullptr;
        while (b->IteratePlants(aP)) aP->mPlantHealth = aP->mPlantMaxHealth;
    }
}

// ─── Zombies ─────────────────────────────────────────────────
void DebuggerWindow::DrawBoardZombies()
{
    Board* b = mApp->mBoard;

    ImGui::SeparatorText("Debug Spawn");
    {
        static char sZFilter[64] = "";
        ImGui::SetNextItemWidth(180);
        const char* zPrev = ZombieTypeName(mSpawnZombieTypeIdx);
        if (ImGui::BeginCombo("Type##zsp", zPrev))
        {
            ImGui::InputText("Filter##zf", sZFilter, sizeof(sZFilter));
            for (int i = 0; i < (int)ZombieType::NUM_ZOMBIE_TYPES; i++)
            {
                const char* n = ZombieTypeName(i);
                if (sZFilter[0] && !strstr(n, sZFilter)) continue;
                bool s = (mSpawnZombieTypeIdx == i);
                if (ImGui::Selectable(n, s)) mSpawnZombieTypeIdx = i;
                if (s) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("Row##zsp", &mSpawnZombieRow);
        mSpawnZombieRow = std::max(0, std::min(mSpawnZombieRow, MAX_GRID_SIZE_Y-1));
        ImGui::SameLine();
        if (ImGui::Button("Spawn##z"))
            b->AddZombieInRow((ZombieType)mSpawnZombieTypeIdx, mSpawnZombieRow, Zombie::ZOMBIE_WAVE_DEBUG);
        ImGui::SameLine();
        if (ImGui::Button("Kill All##z")) b->RemoveAllZombies();
    }

    // Selected zombie inspector
    ImGui::SeparatorText("Selected Zombie");
    if (b->mDebugSelectedZombie)
    {
        Zombie* z = b->mDebugSelectedZombie;
        ZombieDefinition def = GetZombieDefinition(z->mZombieType);
        ImGui::Text("%s | Row %d | Wave %d", def.mZombieName, z->mRow, z->mFromWave);
        ImGui::Text("Pos: %.1f, %.1f  Vel: %.2f", z->mPosX, z->mPosY, z->mVelX);
        ImGui::Text("Phase: %s", ZombiePhaseName(z->mZombiePhase));
        ImGui::PushID("selz");

        ImGui::InputInt("Body HP", &z->mBodyHealth);
        ImGui::SameLine();
        if (ImGui::Button("Full")) z->mBodyHealth = z->mBodyMaxHealth;
        if (z->mHelmType != HELMTYPE_NONE) ImGui::InputInt("Helm HP", &z->mHelmHealth);
        if (z->mShieldType != SHIELDTYPE_NONE) ImGui::InputInt("Shield HP", &z->mShieldHealth);

        ImGui::InputInt("Chilled", &z->mChilledCounter);
        ImGui::SameLine();
        ImGui::InputInt("Buttered", &z->mButteredCounter);
        ImGui::Checkbox("MindControl", &z->mMindControlled);
        ImGui::SameLine();
        ImGui::Checkbox("InPool", &z->mInPool);
        ImGui::SameLine();
        ImGui::Checkbox("BlowAway", &z->mBlowingAway);

        ImGui::SliderFloat("Scale", &z->mScaleZombie, 0.1f, 5.0f);
        ImGui::SliderFloat("VelX", &z->mVelX, -5.0f, 0.0f);

        if (ImGui::Button("Chill"))         z->ApplyChill(false);
        ImGui::SameLine();
        if (ImGui::Button("Butter"))        z->ApplyButter();
        ImGui::SameLine();
        if (ImGui::Button("Burn"))          z->ApplyBurn();
        ImGui::SameLine();
        if (ImGui::Button("Kill (Loot)"))   z->DieWithLoot();
        ImGui::SameLine();
        if (ImGui::Button("Kill (No Loot)"))z->DieNoLoot();
        ImGui::SameLine();
        if (ImGui::Button("Deselect"))      b->mDebugSelectedZombie = nullptr;

        ImGui::PopID();
        ImGui::Separator();
    }
    else ImGui::TextDisabled("Click a zombie in-game to select it");

    // Zombie list table
    ImGui::SeparatorText("All Zombies");
    ImGui::Text("Count: %d", b->mZombies.mSize);
    if (ImGui::BeginTable("ZTable", 6,
        ImGuiTableFlags_Borders|ImGuiTableFlags_ScrollY|ImGuiTableFlags_RowBg,
        ImVec2(0,220)))
    {
        ImGui::TableSetupScrollFreeze(0,1);
        ImGui::TableSetupColumn("Type"); ImGui::TableSetupColumn("Row");
        ImGui::TableSetupColumn("BodyHP"); ImGui::TableSetupColumn("Phase");
        ImGui::TableSetupColumn("Chill"); ImGui::TableSetupColumn("Act");
        ImGui::TableHeadersRow();
        Zombie* z = nullptr; int idx=0;
        while (b->IterateZombies(z))
        {
            ImGui::PushID(idx++);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(ZombieTypeName(z->mZombieType));
            ImGui::TableSetColumnIndex(1); ImGui::Text("%d", z->mRow);
            ImGui::TableSetColumnIndex(2);
            {
                int hp=z->mBodyHealth;
                ImGui::SetNextItemWidth(55);
                if(ImGui::InputInt("##bh",&hp)) z->mBodyHealth=hp;
            }
            ImGui::TableSetColumnIndex(3); ImGui::TextUnformatted(ZombiePhaseName(z->mZombiePhase));
            ImGui::TableSetColumnIndex(4); ImGui::Text("%d",z->mChilledCounter);
            ImGui::TableSetColumnIndex(5);
            if(ImGui::SmallButton("Sel")) b->mDebugSelectedZombie=z;
            ImGui::SameLine();
            if(ImGui::SmallButton("Kill")) z->DieWithLoot();
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

// ─── Wave editor ─────────────────────────────────────────────
void DebuggerWindow::DrawBoardWaves()
{
    Board* b = mApp->mBoard;
    ImGui::Text("Waves: %d  Current: %d  Spawned: %d",
        b->mNumWaves, b->mCurrentWave, b->mTotalSpawnedWaves);

    ImGui::InputInt("ZombieCountDown",  &b->mZombieCountDown);
    ImGui::SameLine();
    if (ImGui::Button("Trigger Next")) b->mZombieCountDown=1;

    ImGui::InputInt("HugeWaveCountDown",&b->mHugeWaveCountDown);
    ImGui::Separator();

    // Wave selector
    ImGui::SetNextItemWidth(120);
    ImGui::SliderInt("Edit Wave##we", &mWaveEditorWaveIdx, 0, b->mNumWaves-1);

    int wi = mWaveEditorWaveIdx;
    if (wi >= 0 && wi < b->mNumWaves)
    {
        ImGui::Text("Wave %d  (%d zombies)", wi, b->NumberZombiesInWave(wi));
        if (ImGui::BeginTable("WaveTable", 2,
            ImGuiTableFlags_Borders|ImGuiTableFlags_ScrollY|ImGuiTableFlags_RowBg,
            ImVec2(0,200)))
        {
            ImGui::TableSetupColumn("Slot"); ImGui::TableSetupColumn("Type");
            ImGui::TableHeadersRow();
            for (int s=0; s<MAX_ZOMBIES_IN_WAVE; s++)
            {
                ZombieType zt = b->mZombiesInWave[wi][s];
                if (zt == ZOMBIE_INVALID) break;
                ImGui::PushID(s);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", s);
                ImGui::TableSetColumnIndex(1);
                {
                    int cur=(int)zt;
                    ImGui::SetNextItemWidth(160);
                    if (ImGui::BeginCombo("##wzt", ZombieTypeName(cur)))
                    {
                        for(int t=0;t<(int)ZombieType::NUM_ZOMBIE_TYPES;t++)
                        {
                            bool sel=(cur==t);
                            if(ImGui::Selectable(ZombieTypeName(t),sel))
                                b->mZombiesInWave[wi][s]=(ZombieType)t;
                            if(sel) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::SameLine();
                    if(ImGui::SmallButton("Spawn Now"))
                        b->AddZombieInRow((ZombieType)b->mZombiesInWave[wi][s],
                            b->PickRowForNewZombie((ZombieType)b->mZombiesInWave[wi][s]),
                            Zombie::ZOMBIE_WAVE_DEBUG);
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }
}

// ─── Stats ───────────────────────────────────────────────────
void DebuggerWindow::DrawBoardStats()
{
    Board* b = mApp->mBoard;
    ImGui::SeparatorText("Game Objects");
    ImGui::Text("Plants:      %d", b->mPlants.mSize);
    ImGui::Text("Zombies:     %d", b->mZombies.mSize);
    ImGui::Text("Projectiles: %d", b->mProjectiles.mSize);
    ImGui::Text("Coins:       %d", b->mCoins.mSize);
    ImGui::Text("Mowers:      %d", b->mLawnMowers.mSize);
    ImGui::Text("Grid Items:  %d", b->mGridItems.mSize);

    ImGui::SeparatorText("Effects");
    ImGui::Text("Reanimations:    %d", mApp->mEffectSystem->mReanimationHolder->mReanimations.mSize);
    ImGui::Text("Particle Systems:%d", mApp->mEffectSystem->mParticleHolder->mParticleSystems.mSize);
    ImGui::Text("Particles:       %d", mApp->mEffectSystem->mParticleHolder->mParticles.mSize);
    ImGui::Text("Trails:          %d", mApp->mEffectSystem->mTrailHolder->mTrails.mSize);
    ImGui::Text("Attachments:     %d", mApp->mEffectSystem->mAttachmentHolder->mAttachments.mSize);

    ImGui::SeparatorText("Counters");
    ImGui::Text("Sun Money:        %d", b->mSunMoney);
    ImGui::Text("Sun Count Down:   %d", b->mSunCountDown);
    ImGui::Text("Suns Fallen:      %d", b->mNumSunsFallen);
    ImGui::Text("Wave:             %d / %d", b->mCurrentWave, b->mNumWaves);
    ImGui::Text("Total Spawned:    %d", b->mTotalSpawnedWaves);
    int activeSecs = b->mPlayTimeActiveLevel / 100;
    ImGui::Text("Play Time Active: %02d:%02d", activeSecs/60, activeSecs%60);
    ImGui::Text("Main Counter:     %d", b->mMainCounter);
    ImGui::Text("Min FPS:          %.1f", b->mMinFPS);
    ImGui::Text("Draw Count:       %d", b->mDrawCount);

    ImGui::SeparatorText("Session Stats");
    ImGui::Text("Graves Cleared:   %d", b->mGravesCleared);
    ImGui::Text("Plants Eaten:     %d", b->mPlantsEaten);
    ImGui::Text("Plants Shoveled:  %d", b->mPlantsShoveled);
    ImGui::Text("Coins Collected:  %d", b->mCoinsCollected);
    ImGui::Text("Diamonds:         %d", b->mDiamondsCollected);
    ImGui::Text("Gargantuars CC:   %d", b->mGargantuarsKillsByCornCob);
    ImGui::Text("Triggered Mowers: %d", b->mTriggeredLawnMowers);

    ImGui::Separator();
    if (ImGui::Button("Reset Session Stats"))
    {
        b->mGravesCleared=b->mPlantsEaten=b->mPlantsShoveled=0;
        b->mCoinsCollected=b->mDiamondsCollected=b->mPottedPlantsCollected=0;
        b->mChocolateCollected=b->mTriggeredLawnMowers=0;
        b->mGargantuarsKillsByCornCob=0;
    }
}

// ─── Save File ───────────────────────────────────────────────
void DebuggerWindow::DrawSaveFileTab()
{
    if (!mApp->mPlayerInfo) { ImGui::TextDisabled("No profile loaded"); return; }
    PlayerInfo* pi = mApp->mPlayerInfo;

    ImGui::SeparatorText(StrFormat("Profile: %s", pi->mName.c_str()).c_str());

    if (ImGui::BeginTabBar("SaveTabs"))
    {
        if (ImGui::BeginTabItem("Info"))
        {
            int area = std::max(1, (pi->mLevel-1)/LEVELS_PER_AREA+1);
            int sub  = pi->mLevel - (area-1)*LEVELS_PER_AREA;
            if (ImGui::InputInt("Area",  &area))   { area=std::max(1,std::min(area,ADVENTURE_AREAS+1)); pi->mLevel=(area-1)*LEVELS_PER_AREA+sub; }
            if (ImGui::InputInt("SubLevel",&sub))  { sub=std::max(1,std::min(sub,LEVELS_PER_AREA));     pi->mLevel=(area-1)*LEVELS_PER_AREA+sub; }
            ImGui::InputInt("Level ID",&pi->mLevel);
            pi->mLevel = std::max(1,std::min(pi->mLevel, NUM_LEVELS));
            if (mApp->mGameSelector) mApp->mGameSelector->mLevel=pi->mLevel;

            ImGui::InputInt("Coins", &pi->mCoins);
            ImGui::SameLine();
            if (ImGui::Button("+1M Coins")) pi->mCoins += 1000000;
            ImGui::InputInt("Finished Adventure", &pi->mFinishedAdventure);
            pi->mFinishedAdventure=std::max(0,pi->mFinishedAdventure);
            ImGui::Checkbox("Used Cheat Keys", &pi->mHasUsedCheatKeys);

            ImGui::SeparatorText("Flags");
            ImGui::Checkbox("Minigames Unlocked",  &pi->mHasUnlockedMinigames);
            ImGui::SameLine();
            ImGui::Checkbox("Puzzle Unlocked",     &pi->mHasUnlockedPuzzleMode);
            ImGui::SameLine();
            ImGui::Checkbox("Survival Unlocked",   &pi->mHasUnlockedSurvivalMode);
            ImGui::Checkbox("New MiniGame",        &pi->mHasNewMiniGame);
            ImGui::SameLine();
            ImGui::Checkbox("Has Seen Stinky",     &pi->mHasSeenStinky);
            ImGui::SameLine();
            ImGui::Checkbox("Magic Taco",          &pi->mNeedsMagicTacoReward);

            ImGui::SeparatorText("Actions");
            if (ImGui::Button("Full Unlock"))
            {
                pi->mFinishedAdventure = std::max(1, pi->mFinishedAdventure);
                pi->mHasUnlockedMinigames = pi->mHasUnlockedPuzzleMode = pi->mHasUnlockedSurvivalMode = true;
                pi->mHasNewMiniGame = pi->mHasNewScaryPotter = pi->mHasNewIZombie = pi->mHasNewSurvival = true;
                pi->mCoins += 100000;
                for (int i=0;i<NUM_ACHIEVEMENT_TYPES;i++) pi->mEarnedAchievements[i]=true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Save Now")) pi->SaveDetails();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Achievements"))
        {
            for (int i=0;i<NUM_ACHIEVEMENT_TYPES;i++)
            {
                ImGui::PushID(i);
                ImGui::SeparatorText(TodStringTranslate(gAchievementDefs[i].mName).c_str());
                bool old=pi->mEarnedAchievements[i];
                if (ImGui::Checkbox("Earned",&pi->mEarnedAchievements[i]))
                    if(!old && pi->mEarnedAchievements[i])
                        mApp->mAchievements->GiveAchievement((AchievementID)i,true);
                ImGui::SameLine();
                ImGui::Checkbox("Shown",&pi->mShownAchievements[i]);
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Records"))
        {
            for (int i=0;i<NUM_GAME_MODES-1;i++)
            {
                ImGui::PushID(i);
                ImGui::InputInt(TodStringTranslate(gChallengeDefs[i].mChallengeName).c_str(),
                                &pi->mChallengeRecords[i]);
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Purchases"))
        {
            for (int i=0;i<NUM_STORE_ITEM_MAX;i++)
            {
                ImGui::PushID(i);
                ImGui::InputInt(StrFormat("Item %d",i).c_str(),&pi->mPurchases[i]);
                ImGui::PopID();
            }
            if (ImGui::Button("Buy All"))
                for(int i=0;i<NUM_STORE_ITEM_MAX;i++) pi->mPurchases[i]=std::max(pi->mPurchases[i],1);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

// ─── Debug panel ─────────────────────────────────────────────
void DebuggerWindow::DrawDebugTab()
{
    ImGui::SeparatorText("Debug Text Mode");
    if (mApp->mBoard)
    {
        int dtm = (int)mApp->mBoard->mDebugTextMode;
        if (ImGui::BeginCombo("Mode##dtm", DebugTextModeName((DebugTextMode)dtm)))
        {
            for (int i=0;i<(int)DEBUG_TEXT_MAX;i++)
            {
                bool s=(dtm==i);
                if (ImGui::Selectable(DebugTextModeName((DebugTextMode)i),s))
                    mApp->mBoard->mDebugTextMode=(DebugTextMode)i;
                if(s) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    ImGui::SeparatorText("Overlay");
    ImGui::Checkbox("Draw Grid Overlay",  &mDebugDrawGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Draw Rect Overlay",  &mDebugDrawRects);

    ImGui::SeparatorText("Misc");
    ImGui::Checkbox("Ignore Asserts", &MTRand::gIgnoreAssert);
    ImGui::Text("SlowMo: %s  FastMo: %s  SlowMoCtr: %d",
        gSlowMo?"ON":"OFF", gFastMo?"ON":"OFF", gSlowMoCounter);

    ImGui::SeparatorText("Log");
    ImGui::Checkbox("Auto-scroll",&mLogAutoScroll);
    ImGui::SameLine();
    if (ImGui::Button("Clear")) mLog.clear();
    if (ImGui::BeginChild("LogChild", ImVec2(0,200), true))
    {
        for (auto& line : mLog) ImGui::TextUnformatted(line.c_str());
        if (mLogAutoScroll && ImGui::GetScrollY()>=ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}

// ─── App panel ───────────────────────────────────────────────
void DebuggerWindow::DrawAppTab()
{
    ImGui::SeparatorText("App State");
    ImGui::Text("Scene:     %s", GameSceneName((int)mApp->mGameScene));
    ImGui::Text("GameMode:  %d", (int)mApp->mGameMode);
    ImGui::Text("AppCounter:%d", mApp->mAppCounter);

    ImGui::SeparatorText("Cheats");
    ImGui::Checkbox("Easy Planting",  &mApp->mEasyPlantingCheat);
    ImGui::SameLine();
    ImGui::Checkbox("TodCheatKeys",   &mApp->mTodCheatKeys);
    ImGui::SameLine();
    ImGui::Checkbox("Debug Mode",     &mApp->mDebuggerEnabled);

    ImGui::SeparatorText("Fast Load");
    {
        static const GameMode kModes[] = {
            GAMEMODE_ADVENTURE,
            GAMEMODE_SURVIVAL_NORMAL_STAGE_1, GAMEMODE_SURVIVAL_HARD_STAGE_1, GAMEMODE_SURVIVAL_ENDLESS_STAGE_1,
            GAMEMODE_CHALLENGE_WAR_AND_PEAS, GAMEMODE_CHALLENGE_WALLNUT_BOWLING,
            GAMEMODE_CHALLENGE_LAST_STAND, GAMEMODE_CHALLENGE_FINAL_BOSS,
            GAMEMODE_CHALLENGE_ZEN_GARDEN, GAMEMODE_TREE_OF_WISDOM
        };
        static const char* kModeNames[] = {
            "Adventure","Surv Normal","Surv Hard","Surv Endless",
            "War&Peas","Wallnut Bowl","Last Stand","Final Boss",
            "Zen Garden","Tree Wisdom"
        };
        constexpr int kCount = 10;
        ImGui::SetNextItemWidth(180);
        if (ImGui::BeginCombo("##flmode", kModeNames[mFastLoadModeIdx]))
        {
            for (int i=0;i<kCount;i++)
            {
                bool s=(mFastLoadModeIdx==i);
                if(ImGui::Selectable(kModeNames[i],s)) mFastLoadModeIdx=i;
                if(s) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Fast Load!"))
            mApp->FastLoad(kModes[mFastLoadModeIdx]);
    }

    if (mApp->mPlayerInfo)
    {
        ImGui::SeparatorText("Profile");
        ImGui::Text("Name: %s  Coins: %d  Level: %d",
            mApp->mPlayerInfo->mName.c_str(),
            mApp->mPlayerInfo->mCoins,
            mApp->mPlayerInfo->mLevel);
    }
}

#endif