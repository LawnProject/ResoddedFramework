#ifndef __BOARD_H__
#define __BOARD_H__

#include "../ConstEnums.h"
#include "../Sexy.TodLib/DataArray.h"
#include "../SexyAppFramework/Widget.h"
#include "../SexyAppFramework/ButtonListener.h"

#include "Plant.h"
#include "Zombie.h"
#include "Projectile.h"
#include "Coin.h"
#include "LawnMower.h"
#include "GridItem.h"

using namespace Sexy;

#define MAX_GRID_SIZE_X 9
#define MAX_GRID_SIZE_Y 6
#define MAX_ZOMBIES_IN_WAVE 50
#define MAX_ZOMBIE_WAVES 100
#define MAX_GRAVE_STONES MAX_GRID_SIZE_X *MAX_GRID_SIZE_Y
#define MAX_POOL_GRID_SIZE 10
#define MAX_RENDER_ITEMS 2048
#define PROGRESS_METER_COUNTER 150
#define MAX_PLANTS_SIZE 1024U
#define MAX_ZOMBIES_SIZE 1024U
#define MAX_PROJECTILE_SIZE 1024U
#define MAX_COIN_SIZE 1024U
#define MAX_LAWN_MOWER_SIZE 32U
#define MAX_GRID_ITEM_SIZE 128U

class LawnApp;
class CursorObject;
class CursorPreview;
class GameButton;
class MessageWidget;
class SeedBank;
class ToolTipWidget;
class CutScene;
class Challenge;
class Reanimation;
class DataSync;
class TodParticleSystem;
namespace Sexy
{
class Graphics;
class ButtonWidget;
class WidgetManager;
class Image;
class MTRand;
} // namespace Sexy

class HitResult
{
  public:
	void *mObject;
	GameObjectType mObjectType;
};

class RenderItem
{
  public:
	RenderObjectType mRenderObjectType;
	int mZPos;
	union {
		GameObject *mGameObject;
		Plant *mPlant;
		Zombie *mZombie;
		Coin *mCoin;
		Projectile *mProjectile;
		CursorPreview *mCursorPreview;
		TodParticleSystem *mParticleSytem;
		Reanimation *mReanimation;
		GridItem *mGridItem;
		LawnMower *mMower;
		BossPart mBossPart;
		int mBoardGridY;
	};
};
bool RenderItemSortFunc(const RenderItem &theItem1, const RenderItem &theItem2);

struct ZombiePicker
{
	int mZombieCount;
	int mZombiePoints;
	int mZombieTypeCount[NUM_ZOMBIE_TYPES];
	int mAllWavesZombieTypeCount[NUM_ZOMBIE_TYPES];
};

void ZombiePickerInitForWave(ZombiePicker *theZombiePicker);
void ZombiePickerInit(ZombiePicker *theZombiePicker);

struct PlantsOnLawn
{
	Plant *mUnderPlant;
	Plant *mPumpkinPlant;
	Plant *mFlyingPlant;
	Plant *mNormalPlant;
};

struct BungeeDropGrid
{
	TodWeightedGridArray mGridArray[MAX_GRID_SIZE_X * MAX_GRID_SIZE_Y];
	int mGridArrayCount;
};

class Board : public Widget, public ButtonListener
{
  public:
	LawnApp *mApp;
	DataArray<Zombie> mZombies;
	DataArray<Plant> mPlants;
	DataArray<Projectile> mProjectiles;
	DataArray<Coin> mCoins;
	DataArray<LawnMower> mLawnMowers;
	DataArray<GridItem> mGridItems;
	CursorObject *mCursorObject;
	CursorPreview *mCursorPreview;
	MessageWidget *mAdvice;
	SeedBank *mSeedBank;
	GameButton *mMenuButton;
	GameButton *mStoreButton;
	bool mIgnoreMouseUp;
	ToolTipWidget *mToolTip;
	Font *mDebugFont;
	CutScene *mCutScene;
	Challenge *mChallenge;

#if LAWN_DEBUG_TOOLS
	Zombie *mDebugSelectedZombie;
	Plant  *mDebugSelectedPlant;
#endif

	bool mPaused;
	GridSquareType mGridSquareType[MAX_GRID_SIZE_X][MAX_GRID_SIZE_Y];
	int mGridCelLook[MAX_GRID_SIZE_X][MAX_GRID_SIZE_Y];
	int mGridCelOffset[MAX_GRID_SIZE_X][MAX_GRID_SIZE_Y][2];
	int mGridCelFog[MAX_GRID_SIZE_X][MAX_GRID_SIZE_Y + 1];
	bool mEnableGraveStones;
	int mSpecialGraveStoneX;
	int mSpecialGraveStoneY;
	float mFogOffset;
	int mFogBlownCountDown;
	PlantRowType mPlantRow[MAX_GRID_SIZE_Y];
	int mWaveRowGotLawnMowered[MAX_GRID_SIZE_Y];
	int mBonusLawnMowersRemaining;
	int mIceMinX[MAX_GRID_SIZE_Y];
	int mIceTimer[MAX_GRID_SIZE_Y];
	ParticleSystemID mIceParticleID[MAX_GRID_SIZE_Y];
	TodSmoothArray mRowPickingArray[MAX_GRID_SIZE_Y];
	ZombieType mZombiesInWave[MAX_ZOMBIE_WAVES][MAX_ZOMBIES_IN_WAVE];
	bool mZombieAllowed[100];
	int mSunCountDown;
	int mNumSunsFallen;	
	int mShakeCounter;
	int mShakeAmountX;
	int mShakeAmountY;
	BackgroundType mBackground;
	int mLevel;
	int mSodPosition;
	int mPrevMouseX;
	int mPrevMouseY;
	int mSunMoney;
	int mNumWaves;
	int mMainCounter;
	int mEffectCounter;
	int mDrawCount;
	int mRiseFromGraveCounter;
	int mOutOfMoneyCounter;
	int mCurrentWave;
	int mTotalSpawnedWaves;
	TutorialState mTutorialState;
	ParticleSystemID mTutorialParticleID;
	int mTutorialTimer;
	int mLastBungeeWave;
	int mZombieHealthToNextWave;
	int mZombieHealthWaveStart;
	int mZombieCountDown;
	int mZombieCountDownStart;
	int mHugeWaveCountDown;
	bool mHelpDisplayed[NUM_ADVICE_TYPES];
	AdviceType mHelpIndex;
	bool mFinalBossKilled;
	bool mShowShovel;
	int mCoinBankFadeCount;
	DebugTextMode mDebugTextMode;
	bool mLevelComplete;
	int mBoardFadeOutCounter;
	int mNextSurvivalStageCounter;
	int mScoreNextMowerCounter;
	bool mLevelAwardSpawned;
	int mProgressMeterWidth;
	int mFlagRaiseCounter;
	int mIceTrapCounter;
	int mBoardRandSeed;
	ParticleSystemID mPoolSparklyParticleID;
	ReanimationID mFwooshID[MAX_GRID_SIZE_Y][12];
	int mFwooshCountDown;
	int mTimeStopCounter;
	bool mDroppedFirstCoin;
	int mFinalWaveSoundCounter;
	int mCobCannonCursorDelayCounter;
	int mCobCannonMouseX;
	int mCobCannonMouseY;
	bool mKilledYeti;
	bool mMustacheMode;
	bool mSuperMowerMode;
	bool mFutureMode;
	bool mPinataMode;
	bool mDanceMode;
	bool mDaisyMode;
	bool mSukhbirMode;
	BoardResult mPrevBoardResult;
	int mTriggeredLawnMowers;
	int mPlayTimeActiveLevel;
	int mPlayTimeInactiveLevel;
	int mMaxSunPlants;
	uint32_t mStartDrawTime;
	uint32_t mIntervalDrawTime;
	int mIntervalDrawCountStart;
	float mMinFPS;
	int mPreloadTime;
	int mGameID;
	int mGravesCleared;
	int mPlantsEaten;
	int mPlantsShoveled;
	int mPlantsPlaced;
	int mCoinsCollected;
	int mDiamondsCollected;
	int mPottedPlantsCollected;
	int mChocolateCollected;
	int mPennyPincherStreak;
	bool mDontPeaUsedPeashooter;
	bool mGroundedUsedCatapult;
	bool mGoodMorningUsedNonFungus;
	bool mNoFungusUsedFungus;
	int mGargantuarsKillsByCornCob;

#if LAWN_USE_UNFINISHED_GAMEPAD_SUPPORT
	float mGamepadX;
	float mGamepadY;
	float mVisualGamepadX;
	float mVisualGamepadY;
	bool mGamepadIgnoreChallenge;
	// Previous button states, used to detect press-once vs hold
	bool mGamepadPrevSouth;     // A
	bool mGamepadPrevEast;      // B
	bool mGamepadPrevWest;      // X
	bool mGamepadPrevNorth;     // Y
	bool mGamepadPrevStart;     // Start
	bool mGamepadPrevLShoulder; // LB
	bool mGamepadPrevRShoulder; // RB
	bool mGamepadPrevLTrigger;  // LT
	bool mGamepadPrevRTrigger;  // RT
	// D-Pad accumulators for cell-to-cell stepping (one step per press)
	float mGamepadDpadXAccum;
	float mGamepadDpadYAccum;
#endif

  public:
	Board(LawnApp *theApp);
	virtual ~Board();

	/// @brief Dispose of the Board objects manually
	void DisposeBoard();

	/// @return Total value of the Sun that is being collected
	int CountSunBeingCollected();

	/// @brief Draw all GameObjects to the Board
	/// @param g Graphics object
	void DrawGameObjects(Graphics *g);

	/// @brief Clear the cursor of any object
	void ClearCursor();

	/// @brief Does the Board have any Zombies (That aren't MindControlled)
	/// @return True if there are any non-MindControlled Zombies, false otherwise
	bool AreEnemyZombiesOnScreen();

	/// @brief Get the lower in the row
	/// @param theRow The row to check
	/// @return LawnMower or nullptr if it doesn't exist
	LawnMower *FindLawnMowerInRow(int theRow);

	/// @brief Save the game
	/// @param theFileName The name of the save file
	void SaveGame(const std::string &theFileName);

	/// @brief Load the game state from the save file
	/// @return True if it loaded successfully, false otherwise
	bool LoadGame(const std::string &theFileName);

	void InitLevel();

	/// @brief Display an advice if it wasn't displayed before
	void DisplayAdvice(const SexyString &theAdvice, MessageStyle theMessageStyle, AdviceType theHelpIndex);

	/// @brief Start the level
	void StartLevel();
	Plant *AddPlant(int theGridX, int theGridY, SeedType theSeedType, SeedType theImitaterType = SeedType::SEED_NONE);

	/// @brief Add a Projectile to the Board
	/// @param theX The X coordinate
	/// @param theY The Y coordinate
	/// @param theRenderOrder The render order to draw the Projectile on
	/// @param theRow The row to spawn the Projectile on
	/// @param theProjectileType The type of Projectile to setup
	Projectile *AddProjectile(int theX, int theY, int theRenderOrder, int theRow, ProjectileType theProjectileType);

	/// @brief Add a Coin to the Board
	/// @param theX The Coin X coordinate
	/// @param theY The Coin Y coordinate
	/// @param theCoinType The CoinType to use (see CoinType enum)
	/// @param theCoinMotion The Motion to spawn with (see CoinMotion enum)
	Coin *AddCoin(int theX, int theY, CoinType theCoinType, CoinMotion theCoinMotion);

	/// @brief Remove the SeedPacket from the Cursor
	void RefreshSeedPacketFromCursor();

	/// @return Get the ZombieType that will rise of the grave if we have theZombiePoints available
	ZombieType PickGraveRisingZombieType(int theZombiePoints);

	/// @return Get the ZombieType based on theWaveIndex and available theZombiePoints
	ZombieType PickZombieType(int theZombiePoints, int theWaveIndex, ZombiePicker *theZombiePicker);

	/// @return	Row Index for theZombieType
	int PickRowForNewZombie(ZombieType theZombieType);

	Zombie *AddZombie(ZombieType theZombieType, int theFromWave);

	/// @brief Spawn a wave of Zombies
	void SpawnZombieWave();

	/// @brief Remove all zombies from the Board
	void RemoveAllZombies();

	/// @brief Remove all zombies that are bound to cutscenes
	void RemoveCutsceneZombies();

	/// @brief Spawn all final-flag Zombies from the GraveStones
	void SpawnZombiesFromGraves();

	/// @brief Determines if a plant can be placed.
	/// @param theGridX The Grid X Coordinate
	/// @param theGridY The Grid Y Coordinate
	/// @param theSeedType The SeedType to check
	/// @return Reason for planting here (inspect PlantingReason enum)
	PlantingReason CanPlantAt(int theGridX, int theGridY, SeedType theSeedType);

	/// @brief On Mouse Movement
	virtual void MouseMove(int x, int y);

	/// @brief On Mouse Drag
	virtual void MouseDrag(int x, int y);

	/// @brief Handle a mouse down press
	virtual void MouseDown(int x, int y, int theClickCount);

	/// @brief Handle a mouse up de-press
	virtual void MouseUp(int x, int y, int theClickCount);

	/// @brief Process a keyboard character press
	/// @param theChar SexyChar that was pressed (UTF-8 friendly)
	virtual void KeyChar(SexyChar theChar);

	/// @brief Process a KeyCode press
	/// @param theKey KeyCode that has been pressed
	virtual void KeyDown(KeyCode theKey);

	/// @brief Update the widget
	virtual void Update();

	/// @brief Update the Widgets currently active (MarkDirty + BringToFront)
	void UpdateLayers();

	/// @brief Draw the Widget
	/// @param g Graphics object
	virtual void Draw(Graphics *g);

	/// @brief Draw the lawn
	/// @param g Graphics object
	void DrawBackdrop(Graphics *g);

	void AddSunMoney(int theAmount);

	/// @brief Try to take an amount of Sun from the Bank
	/// @param theAmount The amount of Sun to get
	/// @return True if there is enough Sun, false otherwise
	bool TakeSunMoney(int theAmount);

	/// @brief Can we take an amount of Sun from the Bank
	/// @param theAmount The amount of Sun to get
	/// @return True if there is enough Sun, false otherwise
	bool CanTakeSunMoney(int theAmount);

	/// @brief Pause the gameplay
	/// @param the pause state
	void Pause(bool thePause);

	/// @brief Try to save the current state to the profile's save folder
	void TryToSaveGame();

	/// @brief Can we save the game
	/// @return True if the game can be saved, false otherwise
	bool NeedSaveGame();

	/// @brief Can the row have Zombies
	/// @param theRow The row to check
	/// @return True if the row can have Zombies, false otherwise
	bool RowCanHaveZombies(int theRow);

	/// @brief Delete all arrays that the board has
	void ProcessDeleteQueue();

	/// @return True if the player chooses the SeedPackets, false otherwise
	bool ChooseSeedsOnCurrentLevel();

	/// @return The max number of plants in the SeedBank
	int GetNumSeedsInBank();

	/// @brief Is the stage during the Night
	/// @return True if the stage is during the Night, false otherwise
	bool StageIsNight();

	/// @brief Does the stage have a Pool
	/// @return True if the stage has a Pool, false otherwise
	bool StageHasPool();

	/// @brief Does the stage have 6 Rows
	/// @return True if the stage has 6 Rows, false otherwise
	bool StageHas6Rows();

	/// @brief Does the stage have Fog
	/// @return True if the stage has Fog, false otherwise
	bool StageHasFog();

	/// @brief Does the stage have GraveStones
	/// @return True if the Stage has GraveStones, false otherwise
	bool StageHasGraveStones();

	/// @return The translated Grid X
	/// @param theX The X Coordinate in World-Space
	/// @param theX The Y Coordinate in World-Space
	int PixelToGridX(int theX, int theY);

	/// @return The translated Grid Y
	/// @param theX The X Coordinate in World-Space
	/// @param theX The Y Coordinate in World-Space
	int PixelToGridY(int theX, int theY);

	/// @brief Convert Grid Coordinates to World-Space X Coordinates
	/// @param theGridX The Grid X Coordinate
	/// @param theGridX The Grid Y Coordinate
	/// @return The translated World-Space X
	int GridToPixelX(int theGridX, int theGridY);

	/// @brief Convert Grid Coordinates to World-Space Y Coordinates
	/// @param theGridX The Grid X Coordinate
	/// @param theGridX The Grid Y Coordinate
	/// @return The translated World-Space Y
	int GridToPixelY(int theGridX, int theGridY);

	/// @brief Convert World-Space to Grid X Coordinates
	/// @param theGridX The X Coordinate
	/// @param theGridX The Y Coordinate
	/// @return The translated World-Space Y
	int PixelToGridXKeepOnBoard(int theX, int theY);

	/// @brief Convert World-Space to Grid Y Coordinates
	/// @param theGridX The X Coordinate
	/// @param theGridX The Y Coordinate
	/// @return The translated World-Space Y
	int PixelToGridYKeepOnBoard(int theX, int theY);

	/// @brief Update every GameObject on the Board
	void UpdateGameObjects();

	/// @param x The X coordinate
	/// @param y The Y coordinate
	/// @param theHitResult The HitResult to fill with data
	/// @return	True if the mouse hits something
	bool MouseHitTest(int x, int y, HitResult *theHitResult);

	/// @brief Handle a mouse down press with a Plant in Cursor
	void MouseDownWithPlant(int x, int y, int theClickCount);

	/// @brief Handle a mouse down press with a tool
	void MouseDownWithTool(int x, int y, int theClickCount, CursorType theCursorType);
	
	/// @brief Can we interact with the Board buttons
	/// @return	True if we can interact
	bool CanInteractWithBoardButtons();

	/// @brief Draw the progress bar
	/// @param g Graphics object
	void DrawProgressMeter(Graphics *g);

	/// @brief Update the Board Tooltip
	void UpdateToolTip();

	/// @brief Get the Plant on a specific grid coordinate
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @param PlantPriority The Plant picking priority (see PlantPriority)
	/// @param Plant or nullptr if it doesn't exist
	Plant *GetTopPlantAt(int theGridX, int theGridY, PlantPriority thePriority);

	/// @brief Get all plants on a specific grid coordinate
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return Get an object containing the Plants on that grid space
	void GetPlantsOnLawn(int theGridX, int theGridY, PlantsOnLawn *thePlantOnLawn);

	/// @brief Count all Sunflowers on the Board
	/// @return The Sunflower count
	int CountSunFlowers();

	/// @return The SeedPacket's X coordinate in the SeedBank
	/// @param theIndex Index in SeedBank
	int GetSeedPacketPositionX(int theIndex);

	/// @brief Calculate and place all the GraveStones for the level
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @param theLevelRNG Random Number Generator instance reference
	void AddGraveStones(int theGridX, int theCount, MTRand &theLevelRNG);

	/// @return Amount of GraveStones
	int GetGraveStoneCount();

	/// @brief Make the Zombies win
	/// @param theZombie The Zombie that will walk into the house
	void ZombiesWon(Zombie *theZombie = nullptr);

	/// @brief Draw the level title
	/// @param g Graphics object
	void DrawLevel(Graphics *g);

	/// @brief Draw the Shovel
	/// @param g Graphics object
	void DrawShovel(Graphics *g);

	/// @brief Update the Zombie Spawning
	void UpdateZombieSpawning();

	/// @brief Update the natural sun spawning
	void UpdateSunSpawning();

	/// @brief Clear the specified advice
	/// @param theHelpIndex The advice type (see AdviceType enum)
	void ClearAdvice(AdviceType theHelpIndex);

	/// @return	True if theRow can have a theZombieType
	bool RowCanHaveZombieType(int theRow, ZombieType theZombieType);

	/// @brief Get the number of Zombies in a wave
	/// @param theWaveIndex The wave to count
	/// @return The number of Zombies in the wave
	int NumberZombiesInWave(int theWaveIndex);

	/// @return The total amount of health that the Zombies from theWaveIndex have
	int TotalZombiesHealthInWave(int theWaveIndex);

	/// @brief Draw the debug text based on mDebugTextMode
	/// @param g Graphics object
	void DrawDebugText(Graphics *g);

	/// @brief Draw the CoinBank
	/// @param g Graphics object
	void DrawUICoinBank(Graphics *g);

	/// @brief Show the CoinBank for a time duration
	/// @param theDuration The amount of time to keep the CoinBank on screen
	void ShowCoinBank(int theDuration = 1000);

	/// @brief Start the level fade-out sequence or go to the next stage in LastStand/IZombie/Vasebreaker/Survival
	void FadeOutLevel();

	/// @brief Draw the Fade-Out transition
	/// @param g Graphics object
	void DrawFadeOut(Graphics *g);

	/// @brief Draw the ice
	/// @param g Graphics object
	/// @param theGridY The row to draw the ice on
	void DrawIce(Graphics *g, int theGridY);

	/// @brief Is the grid space covered in Ice
	/// @return True if the grid at coordinates theGridX and theGridY is ice, false otherwise
	bool IsIceAt(int theGridX, int theGridY);

	/// @brief Get the ID of a Zombie
	/// @param theZombie The Zombie
	/// @return ZombieID
	ZombieID ZombieGetID(Zombie *theZombie);

	/// @brief Get a Zombie from it's ID
	/// @param theZombieID The ID of an Zombie
	/// @return Zombie or assert if it doesn't exist
	Zombie *ZombieGet(ZombieID theZombieID);

	/// @brief Draw the ice
	/// @param theZombieID The ID of an Zombie
	/// @return Zombie or nullptr if it doesn't exist
	Zombie *ZombieTryToGet(ZombieID theZombieID);

	/// @brief Draw the debug rectangles on the lawn based on mDebugTextMode
	/// @param g Graphics object
	void DrawDebugObjectRects(Graphics *g);

	/// @brief Update the iced rows on the Board
	void UpdateIce();

	/// @brief Get the ice's render order in a row
	/// @param theRow The reference row
	/// @return The render order
	int GetIceZPos(int theRow);

	/// @brief Can we spawn a BobsledZombie
	/// @return True if the requirments are met, false otherwise
	bool CanAddBobSled();

	/// @brief Shake the board on the X and Y axis
	/// @param theShakeAmountX The X Factor
	/// @param theShakeAmountY The Y Factor
	void ShakeBoard(int theShakeAmountX, int theShakeAmountY);

	/// @brief Count every non-triggered lawnmower
	int CountUntriggerLawnMowers();

	/// @return True if we find a new Zombie, false otherwise
	/// @param theZombie Reference to a Zombie to iterate
	bool IterateZombies(Zombie *&theZombie);

	/// @return True if we find a new Plant, false otherwise
	/// @param thePlant Reference to a Plant to iterate
	bool IteratePlants(Plant *&thePlant);
	
	/// @return True if we find a new Projectile, false otherwise
	/// @param theProjectile Reference to a Projectile to iterate
	bool IterateProjectiles(Projectile *&theProjectile);

	/// @return True if we find a new Coin, false otherwise
	/// @param theCoin Reference to a Coin to iterate
	bool IterateCoins(Coin *&theCoin);

	/// @return True if we find a new LawnMower, false otherwise
	/// @param theLawnMower Reference to a LawnMower to iterate
	bool IterateLawnMowers(LawnMower *&theLawnMower);

	/// @return True if we find a new Particle System, false otherwise
	/// @param theParticle Reference to a Particle to iterate
	bool IterateParticles(TodParticleSystem *&theParticle);

	/// @return True if we find a new Reanimation, false otherwise
	/// @param theReanimation Reference to a Reanimation to iterate
	bool IterateReanimations(Reanimation *&theReanimation);

	/// @return True if we find a new GridItem, false otherwise
	/// @param theGridItem Reference to a GridItem to iterate
	bool IterateGridItems(GridItem *&theGridItem);

	/// @brief Initiates the level's zombie waves
	/// @param theZombieType The ZombieType to add
	/// @param theRow The row to spawn the Zombie on
	/// @param theFromWave The wave the Zombie was spawned from
	/// @return The new Zombie or nullptr if failed
	Zombie *AddZombieInRow(ZombieType theZombieType, int theRow, int theFromWave);

	/// @brief Is the grid space inside the Pool
	/// @return True if the grid at coordinates are in a Pool Square, false otherwise
	bool IsPoolSquare(int theGridX, int theGridY);

	/// @brief Initiates the level's zombie waves
	void PickZombieWaves();

	/// @brief Stop all Zombie Sounds on the Board
	void StopAllZombieSounds();

	bool HasLevelAwardDropped();

	/// @brief Update the progress meter values
	void UpdateProgressMeter();

	/// @brief Draw the Bottom UI
	/// @param g Graphics object
	void DrawUIBottom(Graphics *g);

	/// @brief Draw the Top UI
	/// @param g Graphics object
	void DrawUITop(Graphics *g);

	/// @brief Get Zombie at the mouse position
	/// @param theX The X Coordinate
	/// @param theY	The Y Coordinate
	/// @return Zombie or nullptr if it doesn't exist
	Zombie *ZombieHitTest(int theMouseX, int theMouseY);

	/// @brief Kill all Plants in a radius
	/// @param theX The Circle's X Coordinate
	/// @param theY	The Circle's Y Coordinate
	/// @param theRadius The Circle's Radius
	void KillAllPlantsInRadius(int theX, int theY, int theRadius);

	/// @brief Get Pumpkin at the grid coordinate
	/// @param theGridX The grid X Coordinate
	/// @param theGridY	The grid Y Coordinate
	/// @return Pumpkin or nullptr if it doesn't exist
	Plant *GetPumpkinAt(int theGridX, int theGridY);

	/// @brief Get FlowerPot at the grid coordinate
	/// @param theGridX The grid X Coordinate
	/// @param theGridY	The grid Y Coordinate
	/// @return FlowerPot or nullptr if it doesn't exist
	Plant *GetFlowerPotAt(int theGridX, int theGridY);

	/// @brief Get UmbrellaLeaf at the grid coordinate
	/// @param theGridX The grid X Coordinate
	/// @param theGridY	The grid Y Coordinate
	/// @return UmbrellaLeaf or nullptr if it doesn't exist
	Plant *FindUmbrellaPlant(int theGridX, int theGridY);

	/// @return True if theZombieType can spawn in theLevel, false otherwise
	static bool CanZombieSpawnOnLevel(ZombieType theZombieType, int theLevel);

	bool IsZombieWaveDistributionOk();

	/// @brief Setup the Background image and rows
	void PickBackground();

	/// @brief Setup the Zombie waves
	void InitZombieWaves();

	/// @brief Setup the Survival stage
	void InitSurvivalStage();

	/// @brief Get UmbrellaLeaf at the grid coordinate
	/// @param theRenderLayer The layer to calculate from (see RenderLayer enum)
	/// @param theRow The row for reference
	/// @param theLayerOffset Additional offset for the layer
	/// @return UmbrellaLeaf or nullptr if it doesn't exist
	static int MakeRenderOrder(RenderLayer theRenderLayer, int theRow, int theLayerOffset);

	/// @brief Update the gameplay (Objects, events, etc)
	void UpdateGame();

	/// @brief Setup the Zombie waves for a specific level
	void InitZombieWavesForLevel(int theForLevel);

	/// @return Flags that determine the recommendation of the seed (see NotRecommend)
	/// @param theSeedType The SeedType to check
	unsigned int SeedNotRecommendedForLevel(SeedType theSeedType);

	/// @brief Draw the Top-Right UI
	/// @param g Graphics object
	void DrawTopRightUI(Graphics *g);

	/// @brief Draw the level Fog
	/// @param g Graphics object
	void DrawFog(Graphics *g);

	/// @brief Update the level Fog
	void UpdateFog();

	/// @brief Get the left-most column where fog can appear
	/// @return The column X grid coordinate
	int LeftFogColumn();

	/// @brief Is the specified ZombieType Pool-Only
	/// @param theZombieType The ZombieType to check
	/// @return True if the ZombieType can only appear in the Pool, false otherwise
	static bool IsZombieTypePoolOnly(ZombieType theZombieType);

	/// @brief Try to drop a loot piece
	/// @param thePosX World-Space X coordinate
	/// @param thePosY World-Space Y coordinate
	/// @param theDropFactor Rarity multiplier, higher values make drops rarer
	void DropLootPiece(int thePosX, int thePosY, int theDropFactor);

	/// @brief Update the End Sequence (Timers, LawnMower Coins)
	void UpdateLevelEndSequence();

	/// @brief Get the bottom-most LawnMower
	/// @return The LawnMower or nullptr if it doesn't exist
	LawnMower *GetBottomLawnMower();

	/// @return True if we can drop loot, false otherwise
	bool CanDropLoot();

	/// @return Get the ZombieType introduced in this level or ZombieType::ZOMBIE_INVALID if none
	ZombieType GetIntroducedZombieType();

	/// @brief Pick a random GraveStone to be special
	void PickSpecialGraveStone();

	/// @return Y Offset for the row
	/// @param thePosX X position on the row
	/// @param theRow Row to calculate with
	float GetPosYBasedOnRow(float thePosX, int theRow);

	/// @brief Announce a new incoming wave
	void NextWaveComing();

	/// @return True if a BungeeZombie is targetting this cell, false otherwise
	/// @param theGridX The Grid X Coordinate
	/// @param theGridY The Grid Y Coordinate
	bool BungeeIsTargetingCell(int theGridX, int theGridY);

	/// @brief Convert World-Space Coordinates to Grid X Coordinates for Planting
	/// @param theX The X Coordinate
	/// @param theX The Y Coordinate
	/// @param theSeedType The SeedType for specific offsets
	/// @return The translated Grid X
	int PlantingPixelToGridX(int theX, int theY, SeedType theSeedType);

	/// @brief Convert World-Space Coordinates to Grid Y Coordinates for Planting
	/// @param theX The X Coordinate
	/// @param theX The Y Coordinate
	/// @param theSeedType The SeedType for specific offsets
	/// @return The translated Grid Y
	int PlantingPixelToGridY(int theX, int theY, SeedType theSeedType);

	/// @brief Set the tutorial state
	/// @return theTutorialState The current state (see TutorialState enum)
	void SetTutorialState(TutorialState theTutorialState);

	/// @brief Do the Jalapeno Fwoosh on a row
	/// @param theRow The row to create the effect on
	void DoFwoosh(int theRow);

	/// @brief Update the Jalapeno Fwoosh instances
	void UpdateFwoosh();

	Plant *SpecialPlantHitTest(int x, int y);

	/// @brief Update the mouse objects positions
	void UpdateMousePosition();

	/// @brief Setup the Plant mouse hover
	/// @param theHitResult Object containing mouse hit information
	/// @return Plant or nullptr if none was found
	Plant *ToolHitTestHelper(HitResult *theHitResult);

	/// @brief Test if a Plant it's hovered
	/// @param theX The X coordinate
	/// @param theY The Y coordinate
	/// @return Plant or nullptr if none was found
	Plant *ToolHitTest(int theX, int theY);

	/// @brief Can the grid coordinate have a GraveStone
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return True if the grid can have a GraveStone, false otherwise
	bool CanAddGraveStoneAt(int theGridX, int theGridY);

	/// @brief Update all GridItems
	void UpdateGridItems();

	/// @brief Add a GraveStone GridItem
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return Created GraveStone
	GridItem *AddAGraveStone(int theGridX, int theGridY);

	/// @return Number of Survival Flags completed
	int GetSurvivalFlagsCompleted();

	/// @return True if the level can have a progress bar, false otherwise
	bool HasProgressMeter();

	/// @brief Update the Board's Cursor
	void UpdateCursor();

	/// @brief Update the tutorial state
	void UpdateTutorial();

	/// @return SeedType the Cursor holds currently
	SeedType GetSeedTypeInCursor();

	int CountPlantByType(SeedType theSeedType);

	/// @return True if the SeedType can be planted, false otherwise
	/// @param theSeedType The SeedType to check
	bool PlantingRequirementsMet(SeedType theSeedType);

	/// @return True if the CobCannon has the minimum requirements to be planted, false otherwise
	bool HasValidCobCannonSpot();

	/// @return True if theGridX and theGridY are valid CobCannon coordinates, false otherwise
	bool IsValidCobCannonSpot(int theGridX, int theGridY);

	/// @return True if theGridX and theGridY can form a set of CobCannon Coordinates, false otherwise
	bool IsValidCobCannonSpotHelper(int theGridX, int theGridY);

	/// @brief Shoot the CobCannon if it fits the requirements
	void MouseDownCobcannonFire(int x, int y, int theClickCount);

	/// @return std::vector of Zombies that are in the radius
	/// @param theX The Circle's X Coordinate
	/// @param theY	The Circle's Y Coordinate
	/// @param theRadius The Circle's Radius
	/// @param theRowRange Max row distance it can check
	/// @param theDamageRangeFlags Damage Flags to filter the Zombies
	std::vector<Zombie*> GetZombiesInRadius(int theRow, int theX, int theY, int theRadius, int theRowRange, int theDamageRangeFlags);

	/// @brief Kill all Zombies found in the radius
	/// @param theX The Circle's X Coordinate
	/// @param theY	The Circle's Y Coordinate
	/// @param theRadius The Circle's Radius
	/// @param theRowRange Max row distance it can check
	/// @param theDamageRangeFlags Damage Flags to filter the Zombies
	void KillAllZombiesInRadius(int theRow, int theX, int theY, int theRadius, int theRowRange, bool theBurn, int theDamageRangeFlags);

	/// @brief Get the extra width of the SeedBank
	/// @return The width in pixels
	int GetSeedBankExtraWidth();

	/// @return True if the given wave index is a flag wave, false otherwise
	bool IsFlagWave(int theWaveNumber);

	/// @brief Draw the top layer of the door
	/// @param g Graphics object
	void DrawHouseDoorTop(Graphics *g);

	/// @brief Draw the buttom layer of the door
	/// @param g Graphics object
	void DrawHouseDoorBottom(Graphics *g);

	/// @brief Get the BossZombie
	/// @return BossZombie or nullptr if it doesn't exist
	Zombie *GetBossZombie();

	/// @brief Get the amount of Gargantuars on the Board
	/// @return Amount of alive Gargantuars
	int GetLiveGargantuarCount();

	/// @brief Is the SeedBank a ConveyorBelt
	/// @return True if the level has a ConveyorBelt, false otherwise
	bool HasConveyorBeltSeedBank();

	/// @brief Does the stage have a roof
	/// @return True if the stage has a roof, false otherwise
	bool StageHasRoof();

	/// @brief Spawn the end-flag Zombies from the pool
	void SpawnZombiesFromPool();

	/// @brief Spawn all final-flag Zombies from the Sky
	void SpawnZombiesFromSky();

	/// @brief Pick-Up a specific GameObjectType tool
	void PickUpTool(GameObjectType theObjectType);

	/// @brief Displays the tutorial arrow
	void TutorialArrowShow(int theX, int theY);

	/// @brief Remove the tutorial arrow
	void TutorialArrowRemove();

	/// @return Total value of the Coins that are being collected
	int CountCoinsBeingCollected();

	/// @brief Drop a BungeeZombie with a theZombieType on the Board
	/// @param theBungeeDropGrid Struct containing weighted arrays for determining the spot
	/// @param theZombieType The ZombieType that is dropped by the Bungee
	void BungeeDropZombie(BungeeDropGrid *theBungeeDropGrid, ZombieType theZombieType);

	/// @brief Setup the Stuct for determining BungeeZombie dropping
	/// @param theBungeeDropGrid Struct that is getting initialized
	void SetupBungeeDrop(BungeeDropGrid *theBungeeDropGrid);

	/// @brief Insert a ZombieType into a specific wave
	/// @param theZombieType The ZombieType to add into the wave
	/// @param theWaveNumber The wave to insert into
	/// @param theZombiePicker The picker instance
	void PutZombieInWave(ZombieType theZombieType, int theWaveNumber, ZombiePicker *theZombiePicker);

	/// @brief Fill the wave with missing ZombieTypes
	/// @param theWaveNumber The wave to insert into
	/// @param theZombiePicker The picker instance
	void PutInMissingZombies(int theWaveNumber, ZombiePicker *theZombiePicker);

	/// @brief Get the rect of the wanted Shovel Button
	/// @return The Shovel button Rect
	Rect GetShovelButtonRect();

	/// @brief Get the rect of the wanted Zen Garden Button
	/// @param theObjectType The button to get
	/// @param theRect The rect to fill
	void GetZenButtonRect(GameObjectType theObjectType, Rect &theRect);

	/// @brief Add a Plant to the Board
	/// @param theGridX The Grid X Coordinate
	/// @param theGridY The Grid Y Coordinate
	/// @param theSeedType The SeedType of the Plant
	/// @param theImitaterType The actual SeedType if theSeedType == SeedType::SEED_IMITATER
	Plant *NewPlant(int theGridX, int theGridY, SeedType theSeedType, SeedType theImitaterType = SeedType::SEED_NONE);

	/// @brief Summon Planting Effects at set coordinates, optionally for a specific plant
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	/// @param thePlant [OPTIONAL] Plant pointer for additional offsets
	void DoPlantingEffects(int theGridX, int theGridY, Plant *thePlant);

	/// @return True if it's the final Survival Stage, false otherwise, false otherwise
	bool IsFinalSurvivalStage();

	/// @brief Save the survival score to the current profile
	void SurvivalSaveScore();

	/// @brief Count every non-dying, non-mind-controlled zombie on the Board
	int CountZombiesOnScreen();

	/// @brief Get the number of spawn waves per Survival Stage
	/// @return Number of waves per Survival Stage
	int GetNumWavesPerSurvivalStage();

	/// @return The level (or Survival/LastStand stage) random number generation seed
	int GetLevelRandSeed();

	/// @brief Add a render object for ZombieType::ZOMBIE_BOSS
	/// @param theRenderList List containing all render items
	/// @param theCurRenderItem The last index of the render list
	/// @param theBossZombie Zombie to use to determine RenderItems
	void AddBossRenderItem(RenderItem *theRenderList, int &theCurRenderItem, Zombie *theBossZombie);

	/// @brief Try to get the Crater GridItem
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return Crater or nullptr if it doesn't exist
	GridItem *GetCraterAt(int theGridX, int theGridY);

	/// @brief Try to get the GraveStone GridItem
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return GraveStone or nullptr if it doesn't exist
	GridItem *GetGraveStoneAt(int theGridX, int theGridY);

	/// @brief Try to get the Ladder GridItem
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return Ladder or nullptr if it doesn't exist
	GridItem *GetLadderAt(int theGridX, int theGridY);

	/// @brief Add a Ladder GridItem
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return Created Ladder
	GridItem *AddALadder(int theGridX, int theGridY);

	/// @brief Add a Crater GridItem
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return Created Crater
	GridItem *AddACrater(int theGridX, int theGridY);

	/// @brief Initiate the LawnMowers for the current level
	void InitLawnMowers();

	/// @brief Is a Plant currently in the Cursor
	/// @return True if we have a Plant, false otherwise
	bool IsPlantInCursor();

	/// @brief Highlight the Plants at mouse coordinates if they fit the requirements
	void HighlightPlantsForMouse(int theMouseX, int theMouseY);

	/// @brief Draw the debug rectangles on the lawn based on mDebugTextMode
	/// @param thePlant The plant for position reference
	/// @param theSize The amount of fog to remove (In pixels)
	void ClearFogAroundPlant(Plant *thePlant, int theSize);

	void RemoveParticleByType(ParticleEffect theEffectType);

	/// @brief Try to get the ScaryPot GridItem
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return ScaryPot or nullptr if it doesn't exist
	GridItem *GetScaryPotAt(int theGridX, int theGridY);

	/// @brief Save the puzzle streak to the current profile
	void PuzzleSaveStreak();

	void ClearAdviceImmediately();

	/// @return Is the current Vasebreaker Stage the final one
	/// @return True if it's the final Vasebreaker Stage, false otherwise
	bool IsFinalScaryPotterStage();

	/// @brief Force the display of an advice
	/// @param theAdvice The advice string
	/// @param theMessageStyle The Style of the Message (see MessageStyle enum)
	/// @param theHelpIndex The index of the advice (see AdviceType enum)
	void DisplayAdviceAgain(const SexyString &theAdvice,
									   MessageStyle theMessageStyle,
									   AdviceType theHelpIndex);
	
	/// @brief Try to get the Squirrel GridItem
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return Squirrel or nullptr if it doesn't exist
	GridItem *GetSquirrelAt(int theGridX, int theGridY);

	/// @brief Try to get the ZenTool GridItem
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return ZenTool or nullptr if it doesn't exist
	GridItem *GetZenToolAt(int theGridX, int theGridY);

	/// @brief Is the Plant in the Gold Wantering Can's Range
	/// @param theMouseX The X coordinate
	/// @param theMouseY The Y coordinate
	/// @param thePlant The Plant to check
	/// @return True if the Plant is in the Gold Watering Can's range, false otherwise
	bool IsPlantInGoldWateringCanRange(int theMouseX, int theMouseY, Plant *thePlant);

	/// @return True if Zombies can walk from the right, false otherwise
	bool StageHasZombieWalkInFromRight();

	/// @brief Place a rake on the Board if has one purchased
	void PlaceRake();

	/// @brief Place a rake on the Board if has one purchased
	/// @return 
	GridItem *GetRake();

	/// @brief Did Crazy Dave finish talking in Vasebreaker
	/// @return True if CrazyDave is talking during a Vasebreaker level, false otherwise
	bool IsScaryPotterDaveTalking();

	/// @brief Get the Zombie that had entered the house
	/// @return The Zombie or nullptr if there is none
	Zombie *GetWinningZombie();

	void ResetFPSStats();

	/// @brief Get the number of plants that don't have a plant on top on  the Board
	/// @param theSeedType The SeedType to count
	/// @return Number of plants that are of type theSeedType and don't have a Plant on top on the Board
	int CountEmptyPotsOrLilies(SeedType theSeedType);

	/// @brief Try to get a GridItem
	/// @param theGridItemType The type to look fore
	/// @param theGridX The X grid coordinate
	/// @param theGridY The Y grid coordinate
	/// @return GridItem or nullptr if it doesn't exist
	GridItem *GetGridItemAt(GridItemType theGridItemType, int theGridX, int theGridY);

	/// @brief Does the progress meter can have flags
	/// @return True if the progress meter can have flags, false otherwise
	bool ProgressMeterHasFlags();

	/// @brief Is the current stage the final LastStand stage
	/// @return True if the stage is the final LastStand stage, false otherwise
	bool IsLastStandFinalStage();

	/// @brief Get the number of spawn waves per flag
	/// @return Number of waves
	int GetNumWavesPerFlag();

	/// @return The SeedType's Cost
	/// @param theSeedType The SeedType to check
	/// @param theSeedType The theImitaterType to check incase theSeedType == SeedType::SEED_IMITATER
	int GetCurrentPlantCost(SeedType theSeedType, SeedType theImitaterType);

	/// @brief Does the Plant's price increase dynamically
	/// @param theSeedType The SeedType to check
	/// @return True if the Plant's price increase, false otherwise
	bool PlantUsesAcceleratedPricing(SeedType theSeedType);

	/// @brief Freeze the effects and animation on the Board for animations
	/// @param theFreeze The freeze state
	void FreezeEffectsForCutscene(bool theFreeze);

	/// @brief Load the background images from their respective groups
	void LoadBackgroundImages();

	/// @brief Can we use this GameObject (see GameObjectType enum)
	/// @param theGameObject The Type of GameObject to check
	bool CanUseGameObject(GameObjectType theGameObject);

	/// @brief Toggle the Mustache Mode
	/// @param theEnableMustache The state of the mode
	void SetMustacheMode(bool theEnableMustache);

	/// @return Amount of Coin Objects of a type
	/// @param CoinType The CoinType to count
	int CountCoinByType(CoinType theCoinType);

	/// @brief Toggle the SuperMower Mode
	/// @param theEnableSuperMower The state of the mode
	void SetSuperMowerMode(bool theEnableSuperMower);

	/// @brief Draw the ZenGarden WheelBarrow button
	/// @param g Graphics object
	/// @param theOffsetY Y offset
	void DrawZenWheelBarrowButton(Graphics *g, int theOffsetY);

	/// @brief Draw the ZenGarden buttons
	/// @param g Graphics object
	void DrawZenButtons(Graphics *g);
	void OffsetYForPlanting(int &theY, SeedType theSeedType);

	/// @brief Toggle the Future Mode
	/// @param theEnableFuture The state of the mode
	void SetDanceMode(bool theEnableDance);

	/// @brief Toggle the Future Mode
	/// @param theEnableFuture The state of the mode
	void SetFutureMode(bool theEnableFuture);

	/// @brief Toggle the Future Mode
	/// @param theEnableFuture The state of the mode
	void SetPinataMode(bool theEnablePinata);

	/// @brief Toggle the Daisy Mode
	/// @param theEnableDaisy The state of the mode
	void SetDaisyMode(bool theEnableDaisy);

	/// @brief Toggle the Suknbir Mode
	/// @param theEnableSukhbir The state of the mode
	void SetSukhbirMode(bool theEnableSukhbir);

	/// @param x The X coordinate
	/// @param y The Y coordinate
	/// @param theHitResult The HitResult to fill with data
	/// @return True if a Plant is present at coordinates, false otherwise
	bool MouseHitTestPlant(int x, int y, HitResult *theHitResult);

	Reanimation *CreateRakeReanim(float theRakeX, float theRakeY, int theRenderOrder);

	/// @brief Save to profile the values changed during gameplay
	void CompleteEndLevelSequenceForSaving();

	/// @brief Remove all zombies that can't persist a repick
	void RemoveZombiesForRepick();

	/// @brief Get the amount of GraveStones on the Board
	/// @return Number of GraveStones
	int GetGraveStonesCount();

	/// @brief Can we repick after the Survival Stage
	/// @return True if we can repick our seeds, false otherwise
	bool IsSurvivalStageWithRepick();

	/// @brief Can we repick/replant after the LastStand Stage
	/// @return True if we can repick/replant our seeds, false otherwise
	bool IsLastStandStageWithRepick();

	/// @brief Process a KeyCode for typing checks
	/// @param theKey KeyCode that has been pressed
	void DoTypingCheck(KeyCode theKey);

	/// @return Number of zombies of a type
	/// @param theZombieType The type to count
	int CountZombieByType(ZombieType theZombieType);

	/// @brief Is the specified ZombieType spawned only
	/// @param theZombieType The ZombieType to check
	/// @return True if the ZombieType can only be spawned by other Zombies, false otherwise
	static bool IsZombieTypeSpawnedOnly(ZombieType theZombieType);
};
extern bool gShownMoreSunTutorial;

/// @brief Get how much do 2 Rectangles overlap
/// @param rect1 The first Rectangle
/// @param rect2 The second Rectangle
/// @return The maximum overlap between the 2 Rectangles
int GetRectOverlap(const Rect &rect1, const Rect &rect2);

/// @brief Does the Circle overlap the Rectangle
/// @param theCircleX The circle's X coordinate
/// @param theCircleY The circle's Y coordinate
/// @param theRadius The circle's radius
/// @param theRect The Rectangle
/// @return True if the Rectangle and The Circle overlap, false otherwise
bool GetCircleRectOverlap(int theCircleX, int theCircleY, int theRadius, const Rect &theRect);

/// @brief Setup the Board's flags for the Player
void BoardInitForPlayer();

#endif // __BOARD_H__