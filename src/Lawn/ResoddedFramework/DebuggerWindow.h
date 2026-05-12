#if LAWN_DEBUG_TOOLS

#pragma once

#include <ImGui/ImGuiWindow.h>
#include "../BoardInclude.h"
#include <deque>
#include <string>

using namespace Sexy;

class LawnApp;
class Plant;
class Zombie;

class DebuggerWindow : public ImGuiWindow
{
  public:
	DebuggerWindow(LawnApp* theApp);
	~DebuggerWindow();

	virtual void Update();

	// Called from outside to push a log line
	void PushLog(const std::string& theMsg);

  private:
	// ── State ─────────────────────────────────────────────────────────────
	LawnApp*  mApp;

	// Board → Plants
	Plant*    mSelectedPlant;
	int       mQuickPlantSeedIdx;   // index into gPlantDefs[0..NUM_SEED_TYPES-1]
	int       mQuickPlantCol;
	int       mQuickPlantRow;

	// Board → Zombies
	int       mSpawnZombieTypeIdx;
	int       mSpawnZombieRow;
	int       mWaveEditorWaveIdx;

	// Board → Cheats
	int       mAddSunAmount;
	int       mSpawnCoinType;       // index into CoinType
	int       mSpawnCoinMotion;     // index into CoinMotion

	// Debug panel
	bool      mShowImGuiDemo;
	bool      mDebugDrawGrid;
	bool      mDebugDrawRects;

	// Log
	std::deque<std::string> mLog;
	bool                    mLogAutoScroll;

	// App panel
	int       mFastLoadModeIdx;

	// ── Helpers ────────────────────────────────────────────────────────────
	void DrawBoardTab();
	void DrawBoardCheats();
	void DrawBoardPlants();
	void DrawBoardZombies();
	void DrawBoardWaves();
	void DrawBoardStats();
	void DrawSaveFileTab();
	void DrawDebugTab();
	void DrawAppTab();

	static const char* SeedTypeName(int idx);
	static const char* ZombieTypeName(int idx);
	static const char* GameModeName(int mode);
	static const char* GameSceneName(int scene);
	static const char* ZombiePhaseName(ZombiePhase p);
	static const char* PlantStateName(PlantState s);
	static const char* DebugTextModeName(DebugTextMode m);
	static const char* CoinTypeName(CoinType c);
	static const char* CoinMotionName(CoinMotion m);
};


#endif