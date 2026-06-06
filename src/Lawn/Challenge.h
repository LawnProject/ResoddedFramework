#ifndef __CHALLENGE_H__
#define __CHALLENGE_H__

#include "../ConstEnums.h"
#include "../GameConstants.h"
#include "../Sexy.TodLib/FilterEffect.h"
#include "../SexyAppFramework/Graphics.h"

#define BEGHOULED_MAX_GRIDSIZEX 8
#define BEGHOULED_MAX_GRIDSIZEY 5
#define ART_CHALLEGE_SIZE_X 9
#define MAX_PICK_GRID_SIZE 50

constexpr const int BEGHOULED_WINNING_SCORE = 75;
constexpr const int SLOT_MACHINE_WINNING_SCORE = 2000;
constexpr const int ZOMBIQUARIUM_WINNING_SCORE = 1000;
constexpr const int I_ZOMBIE_WINNING_SCORE = 5;
constexpr const int MAX_PORTALS = 4;
constexpr const int MAX_SQUIRRELS = 7;
constexpr const int MAX_SCARY_POTS = 54;
constexpr const int STORM_FLASH_TIME = 150;

using namespace Sexy;

class LawnApp;
class Board;
class Plant;
class Zombie;
class GridItem;
class SeedPacket;
class HitResult;
struct TodWeightedGridArray;

enum BeghouledUpgrade
{
	BEGHOULED_UPGRADE_REPEATER,
	BEGHOULED_UPGRADE_FUMESHROOM,
	BEGHOULED_UPGRADE_TALLNUT,
	NUM_BEGHOULED_UPGRADES
};

struct BeghouledBoardState
{
	SeedType mSeedType[9][6];
};

class Challenge
{
  public:
	LawnApp *mApp;
	Board *mBoard;
	bool mBeghouledMouseCapture;
	int mBeghouledMouseDownX;
	int mBeghouledMouseDownY;
	bool mBeghouledEated[9][6];
	bool mBeghouledPurcasedUpgrade[NUM_BEGHOULED_UPGRADES];
	int mBeghouledMatchesThisMove;
	ChallengeState mChallengeState;
	int mChallengeStateCounter;
	int mConveyorBeltCounter;
	int mChallengeScore;
	bool mShowBowlingLine;
	SeedType mLastConveyorSeedType;
	int mSurvivalStage;
	int mSlotMachineRollCount;
	ReanimationID mReanimChallenge;
	ReanimationID mReanimClouds[6];
	int mCloudsCounter[6];
	int mChallengeGridX;
	int mChallengeGridY;
	int mScaryPotterPots;
	int mRainCounter;
	int mTreeOfWisdomTalkIndex;

  public:
	Challenge();

	/// @brief Start the challenge level and apply challenge-specific startup behavior
	void StartLevel();

	/// @brief Populate the Beghouled board ensuring at least one valid move exists
	void BeghouledPopulateBoard();

	/// @brief Copy the current board plant layout into a Beghouled board state buffer
	/// @param theBoardState Pointer to the Beghouled board state to populate
	void LoadBeghouledBoardState(BeghouledBoardState *theState);

	/// @brief Pick a random seed type for an empty Beghouled grid cell
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	/// @param theBoardState The current Beghouled board state
	/// @param theAllowMatches If true, allow picks that create immediate matches
	/// @return A seed type that does not create an invalid board state
	SeedType BeghouledPickSeed(int theGridX, int theGridY, BeghouledBoardState *theBoardState, bool theAllowMatches);

	/// @brief Check if any valid match exists on the Beghouled board
	/// @param theBoardState The current Beghouled board state
	/// @return True if at least one valid match exists on the board
	bool BeghouledBoardHasMatch(BeghouledBoardState *theBoardState);

	/// @brief Get the seed type at a given Beghouled grid position
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	/// @param theBoardState The current Beghouled board state
	/// @return The seed type at the position, or SEED_NONE if out of bounds
	SeedType BeghouledGetPlantAt(int theGridX, int theGridY, BeghouledBoardState *theBoardState);

	/// @brief Get the length of a vertical match at the given position
	/// @param theGridX The starting grid X coordinate
	/// @param theGridY The starting grid Y coordinate
	/// @param theBoardState The current Beghouled board state
	/// @return The length of the vertical match (0 if no match)
	int BeghouledVerticalMatchLength(int theGridX, int theGridY, BeghouledBoardState *theBoardState);

	/// @brief Get the length of a horizontal match at the given position
	/// @param theGridX The starting grid X coordinate
	/// @param theGridY The starting grid Y coordinate
	/// @param theBoardState The current Beghouled board state
	/// @return The length of the horizontal match (0 if no match)
	int BeghouledHorizontalMatchLength(int theGridX, int theGridY, BeghouledBoardState *theBoardState);

	/// @brief Start a Beghouled drag operation
	/// @param x The screen X coordinate where the drag started
	/// @param y The screen Y coordinate where the drag started
	void BeghouledDragStart(int x, int y);

	/// @brief Process Beghouled drag input and update plant positions
	/// @param x The current screen X coordinate
	/// @param y The current screen Y coordinate
	void BeghouledDragUpdate(int x, int y);

	inline void BeghouledDragCancel()
	{
		mBeghouledMouseCapture = false;
	}

	/// @brief Handle mouse movement for challenge-specific interactions
	/// @param x The screen X coordinate
	/// @param y The screen Y coordinate
	/// @return True if the mouse movement was handled by the challenge
	bool MouseMove(int x, int y);

	/// @brief Handle mouse down events for challenge-specific interactions
	/// @param x The screen X coordinate
	/// @param y The screen Y coordinate
	/// @param theClickCount The number of clicks
	/// @param theHitResult The hit test result
	/// @return True if the mouse down was handled by the challenge
	bool MouseDown(int x, int y, int theClickCount, HitResult *theHitResult);

	/// @brief Handle mouse up events for Beghouled drag cancellation
	/// @param x The screen X coordinate
	/// @param y The screen Y coordinate
	/// @return False (mouse up not fully handled)
	bool MouseUp(int x, int y);

	/// @brief Clear challenge-specific cursor state
	void ClearCursor();

	/// @brief Remove a horizontal match starting from the given grid position
	/// @param theGridX The starting grid X coordinate
	/// @param theGridY The starting grid Y coordinate
	/// @param theBoardState The current Beghouled board state
	void BeghouledRemoveHorizontalMatch(int theGridX, int theGridY, BeghouledBoardState *theBoardState);

	/// @brief Remove a vertical match starting from the given grid position
	/// @param theGridX The starting grid X coordinate
	/// @param theGridY The starting grid Y coordinate
	/// @param theBoardState The current Beghouled board state
	void BeghouledRemoveVerticalMatch(int theGridX, int theGridY, BeghouledBoardState *theBoardState);

	/// @brief Scan the board and remove all Beghouled matches
	/// @param theBoardState The current Beghouled board state
	void BeghouledRemoveMatches(BeghouledBoardState *theBoardState);

	/// @brief Main update function for the challenge system
	void Update();

	/// @brief Main update loop for the Beghouled challenge
	void UpdateBeghouled();
	/// @brief Update a Beghouled plant position with animation
	/// @param thePlant The plant to update
	/// @return True if the plant is still moving
	bool UpdateBeghouledPlant(Plant *thePlant);

	/// @brief Make plants fall into empty squares in the Beghouled grid
	/// @param theGridX The target grid X coordinate
	/// @param theGridY The target grid Y coordinate
	/// @param theBoardState The current Beghouled board state
	void BeghouledFallIntoSquare(int theGridX, int theGridY, BeghouledBoardState *theBoardState);

	/// @brief Make all empty Beghouled squares fill by falling plants
	/// @param theBoardState The current Beghouled board state
	void BeghouledMakePlantsFall(BeghouledBoardState *theBoardState);

	/// @brief Handle zombie eating a plant in Beghouled mode
	/// @param theZombie The zombie that ate the plant
	/// @param thePlant The plant that was eaten
	void ZombieAtePlant(Zombie *theZombie, Plant *thePlant);

	/// @brief Draw challenge-specific backdrop elements
	/// @param g The graphics context to draw with
	void DrawBackdrop(Graphics *g);

	/// @brief Draw the art challenge backdrop with ghost plant previews
	/// @param g The graphics context to draw with
	void DrawArtChallenge(Graphics *g);

	/// @brief Check if the art challenge placement criteria have been met
	/// @param theGridX The grid X coordinate of the last placed plant
	/// @param theGridY The grid Y coordinate of the last placed plant
	void CheckForCompleteArtChallenge(int theGridX, int theGridY);

	/// @brief Get the seed type required for an art challenge at a grid position
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	/// @return The required seed type, or SEED_NONE if no requirement
	SeedType GetArtChallengeSeed(int theGridX, int theGridY);

	/// @brief Called when a plant is added to the board during an art challenge
	/// @param thePlant The plant that was added
	void PlantAdded(Plant *thePlant);

	/// @brief Check if a plant can be placed at the given grid position
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	/// @param theSeedType The type of seed being planted
	/// @return A PlantingReason indicating if planting is allowed
	PlantingReason CanPlantAt(int theGridX, int theGridY, SeedType theSeedType);

	/// @brief Draw Beghouled-specific overlays (craters and twist indicator)
	/// @param g The graphics context to draw with
	void DrawBeghouled(Graphics *g);

	/// @brief Validate a Beghouled drag move between two grid cells
	/// @param theFromX The source grid X coordinate
	/// @param theFromY The source grid Y coordinate
	/// @param theToX The destination grid X coordinate
	/// @param theToY The destination grid Y coordinate
	/// @param theBoardState The current Beghouled board state
	/// @return True if the move is valid and produces a match
	bool BeghouledIsValidMove(int theFromX, int theFromY, int theToX, int theToY, BeghouledBoardState *theBoardState);

	/// @brief Check if there is at least one valid move available on the Beghouled board
	/// @param theBoardState The current Beghouled board state
	/// @return True if at least one valid move exists
	bool BeghouledCheckForPossibleMoves(BeghouledBoardState *theBoardState);

	/// @brief Check if the Beghouled board has no possible moves and needs reshuffling
	void BeghouledCheckStuckState();

	/// @brief Initialize zombie waves for survival mode based on current stage
	void InitZombieWavesSurvival();

	/// @brief Initialize zombie waves from a predefined zombie type list
	/// @param theZombieList Array of zombie types to enable
	/// @param theListLength Number of entries in theZombieList
	void InitZombieWavesFromList(ZombieType *theZombieList, int theListLength);

	/// @brief Initialize zombie waves for the current game mode
	void InitZombieWaves();

	/// @brief Get the screen rectangle for the slot machine handle
	/// @return The rectangle covering the slot machine handle
	Rect SlotMachineGetHandleRect();

	/// @brief Update the slot machine challenge
	void UpdateSlotMachine();

	/// @brief Draw the slot machine challenge UI and animation
	/// @param g The graphics context to draw with
	void DrawSlotMachine(Graphics *g);

	/// @brief Update tooltip display for challenge-specific UI elements
	/// @param theX The screen X coordinate
	/// @param theY The screen Y coordinate
	/// @return True if a tooltip was shown
	bool UpdateToolTip(int theX, int theY);

	/// @brief Handle zombie spawning logic for the Whack-a-Zombie challenge
	void WhackAZombieSpawning();

	/// @brief Determine if zombie spawning should be updated this frame
	/// @return True if zombie spawning should be updated
	bool UpdateZombieSpawning();

	/// @brief Clear a set number of craters from the Beghouled board
	/// @param theCount The number of craters to clear
	void BeghouledClearCrater(int theCount);

	/// @brief Handle mouse down for Whack-a-Zombie challenge
	/// @param theX The screen X coordinate
	/// @param theY The screen Y coordinate
	void MouseDownWhackAZombie(int theX, int theY);

	/// @brief Draw the stormy night overlay effect
	/// @param g The graphics context to draw with
	void DrawStormNight(Graphics *g);

	/// @brief Update the stormy night challenge (lightning flashes and rain)
	void UpdateStormyNight();

	/// @brief Initialize challenge-specific state based on the current game mode
	void InitLevel();

	/// @brief Spawn the next zombie wave with challenge-specific modifications
	void SpawnZombieWave();

	/// @brief Spawn a random grave on the board for Grave Danger challenge
	void GraveDangerSpawnRandomGrave();

	/// @brief Spawn a grave at the specified grid position
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	void GraveDangerSpawnGraveAt(int theGridX, int theGridY);

	/// @brief Spawn the level award (trophy/money bag) at a grid position
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	void SpawnLevelAward(int theGridX, int theGridY);

	/// @brief Score a Beghouled match and award sun/upgrades
	/// @param theGridX The grid X coordinate of the match
	/// @param theGridY The grid Y coordinate of the match
	/// @param theNumPlants The number of plants in the match
	/// @param theIsHorizontal True if the match is horizontal, false if vertical
	void BeghouledScore(int theGridX, int theGridY, int theNumPlants, bool theIsHorizontal);

	/// @brief Draw a storm flash effect overlay
	/// @param g The graphics context to draw with
	/// @param theTime The current flash timer value
	/// @param theMaxAmount The maximum flash intensity
	void DrawStormFlash(Graphics *g, int theTime, int theMaxAmount);

	/// @brief Update the raining seeds challenge
	void UpdateRainingSeeds();

	/// @brief Trigger the boss zombie entrance
	void PlayBossEnter();

	/// @brief Update the conveyor belt seed dispenser
	void UpdateConveyorBelt();

	/// @brief Initialize the Portal Combat challenge mode (setup portals)
	void PortalStart();

	/// @brief Update the Portal Combat challenge mode (move portals and teleport entities)
	void UpdatePortalCombat();

	GridItem *GetOtherPortal(GridItem *thePortal);

	/// @brief Update a portal's entity teleportation logic
	/// @param thePortal The portal grid item to update
	void UpdatePortal(GridItem *thePortal);

	/// @brief Calculate zombie spawn weight for a row in Portal Combat
	/// @param theGridY The row to calculate weight for
	/// @return The spawn weight multiplier for this row
	float PortalCombatRowSpawnWeight(int theGridY);

	/// @brief Check if a plant can target a zombie through the portal system
	/// @param thePlant The plant attempting to target
	/// @param theZombie The zombie being targeted
	/// @return True if the zombie is within the plant's portal-augmented range
	bool CanTargetZombieWithPortals(Plant *thePlant, Zombie *theZombie);

	GridItem *GetPortalToRight(int theGridX, int theGridY);

	GridItem *GetPortalAt(int theGridX, int theGridY);

	/// @brief Relocate a random portal to a new valid position
	void MoveAPortal();

	/// @brief Calculate the distance from a row to the nearest lawn mower through portals
	/// @param theGridY The row to measure from
	/// @return The portal traversal distance to the mower
	int GetPortalDistanceToMower(int theGridY);

	GridItem *GetPortalToLeft(int theGridX, int theGridY);

	/// @brief Handle clicks on Beghouled seed packets (shuffle, crater, upgrades)
	/// @param theSeedPacket The seed packet that was clicked
	void BeghouledPacketClicked(SeedPacket *theSeedPacket);

	/// @brief Shuffle all plants on the Beghouled board into new random positions
	void BeghouledShuffle();

	/// @brief Check if any craters remain on the Beghouled board
	/// @return True if at least one crater can be cleared
	bool BeghouledCanClearCrater();

	/// @brief Update the crater button activation state based on available craters
	void BeghouledUpdateCraters();

	Zombie *ZombiquariumSpawnSnorkle();

	/// @brief Handle seed packet clicks in Zombiquarium mode (buy snorkel or trophy)
	/// @param theSeedPacket The seed packet that was clicked
	void ZombiquariumPacketClicked(SeedPacket *theSeedPacket);

	/// @brief Handle mouse down in Zombiquarium mode (drop brain or feed zombie)
	/// @param x The screen X coordinate
	/// @param y The screen Y coordinate
	/// @param theClickCount The number of clicks
	void ZombiquariumMouseDown(int x, int y, int theClickCount);

	/// @brief Drop a brain item at the given screen position for Zombiquarium
	/// @param x The screen X coordinate
	/// @param y The screen Y coordinate
	void ZombiquariumDropBrain(int x, int y);

	/// @brief Main update loop for the Zombiquarium challenge
	void ZombiquariumUpdate();

	/// @brief Add wallnut plants across the board for Shovel/Wallnut Bowling modes
	void ShovelAddWallnuts();

	void ScaryPotterPlacePot(ScaryPotType theScaryPotType, ZombieType theZombieType, SeedType theSeedType, int theCount, TodWeightedGridArray *theGridArray, int theGridArrayCount);

	/// @brief Initialize Scary Potter challenge (show shovel tutorial if needed)
	void ScaryPotterStart();

	/// @brief Update the Scary Potter mallet animation and pot opening
	void ScaryPotterUpdate();

	/// @brief Open a scary pot and spawn its contents (seed, zombie, or sun)
	/// @param theScaryPot The scary pot grid item to open
	void ScaryPotterOpenPot(GridItem *theScaryPot);

	/// @brief Explode all scary pots in a 1-tile radius (Jack-in-the-Box effect)
	/// @param thePosX The screen X coordinate of the explosion
	/// @param thePosY The screen Y coordinate of the explosion
	void ScaryPotterJackExplode(int thePosX, int thePosY);

	/// @brief Check if all scary pots have been cleared and no zombies remain
	/// @return True if the Scary Potter challenge is complete
	bool ScaryPotterIsCompleted();

	/// @brief Change the type of existing scary pots (e.g., reveal leaf/zombie pots)
	/// @param thePotType The new pot state to apply
	/// @param theCount The number of pots to change
	void ScaryPotterChangePotType(GridItemState thePotType, int theCount);

	/// @brief Populate the board with scary pots for the current stage
	void ScaryPotterPopulate();

	/// @brief Exclude a column from scary pot placement grid
	/// @param theCol The column index to exclude
	/// @param theGridArray The weighted grid array to modify
	/// @param theGridArrayCount The number of entries in theGridArray
	void ScaryPotterDontPlaceInCol(int theCol, TodWeightedGridArray *theGridArray, int theGridArrayCount);

	void ScaryPotterFillColumnWithPlant(int theCol, SeedType theSeedType, TodWeightedGridArray *theGridArray, int theGridArrayCount);

	/// @brief Clear the board and advance to the next puzzle/I-Zombie stage
	void PuzzleNextStageClear();

	/// @brief Trigger the mallet animation to open a scary pot
	/// @param theScaryPot The pot grid item to open
	void ScaryPotterMalletPot(GridItem *theScaryPot);
	static ZombieType IZombieSeedTypeToZombieType(SeedType theSeedType);

	/// @brief Check if a seed type is a zombie seed usable in I-Zombie/Zombiquarium
	/// @param theSeedType The seed type to check
	/// @return True if the seed represents a zombie type
	static bool IsZombieSeedType(SeedType theSeedType);

	/// @brief Handle mouse down in I-Zombie mode (place zombie on click)
	/// @param theX The screen X coordinate
	/// @param theY The screen Y coordinate
	/// @param theClickCount The number of clicks
	void IZombieMouseDownWithZombie(int theX, int theY, int theClickCount);

	/// @brief Start the I-Zombie challenge (display eat-all-brains message)
	void IZombieStart();

	/// @brief Place multiple plants randomly in I-Zombie mode
	/// @param theSeedType The type of plant to place
	/// @param theCount The number of plants to place
	/// @param theGridY Optional row restriction (-1 for any row)
	void IZombiePlacePlants(SeedType theSeedType, int theCount, int theGridY = -1);

	/// @brief Main update loop for the I-Zombie challenge
	void IZombieUpdate();

	/// @brief Draw a plant in I-Zombie mode with a zombie-like palette
	/// @param g The graphics context to draw with
	/// @param thePlant The plant to draw
	void IZombieDrawPlant(Graphics *g, Plant *thePlant);

	/// @brief Apply a filter effect to all reanimations of a plant in I-Zombie
	/// @param thePlant The plant to apply the filter to
	/// @param theFilterEffect The filter effect to apply
	void IZombieSetPlantFilterEffect(Plant *thePlant, FilterEffect theFilterEffect);

	/// @brief Get the sun count stored inside a scary pot
	/// @param theScaryPot The scary pot grid item
	/// @return The number of suns inside the pot
	int ScaryPotterCountSunInPot(GridItem *theScaryPot);

	/// @brief Count the number of remaining scary pots on the board
	/// @return The number of scary pots currently on the board
	int ScaryPotterCountPots();

	/// @brief Initialize the I-Zombie level (place brains and starting plants)
	void IZombieInitLevel();

	/// @brief Draw the rain effect overlay
	/// @param g The graphics context to draw with
	void DrawRain(Graphics *g);

	/// @brief Draw weather effects (rain and storm)
	/// @param g The graphics context to draw with
	void DrawWeather(Graphics *g);

	/// @brief Main update loop for the Squirrel challenge (AI + score)
	void SquirrelUpdate();

	/// @brief Count the number of uncought squirrels remaining
	/// @return The number of squirrels still not caught or turned zombie
	int SquirrelCountUncaught();

	/// @brief Initialize the Squirrel challenge (place squirrels on the board)
	void SquirrelStart();

	/// @brief Handle squirrel found event (turn zombie or run away)
	/// @param theSquirrel The squirrel grid item that was found
	void SquirrelFound(GridItem *theSquirrel);

	/// @brief Make a squirrel peek out from its hiding position
	/// @param theSquirrel The squirrel grid item
	void SquirrelPeek(GridItem *theSquirrel);

	/// @brief Handle squirrel chewing animation and plant damage
	/// @param theSquirrel The squirrel grid item
	void SquirrelChew(GridItem *theSquirrel);

	/// @brief Update a single squirrel (timers, state transitions)
	/// @param theSquirrel The squirrel grid item to update
	void SquirrelUpdateOne(GridItem *theSquirrel);

	/// @brief Setup a plant for I-Zombie mode (freeze reanim, arm potato mine)
	/// @param thePlant The plant to setup
	void IZombieSetupPlant(Plant *thePlant);

	/// @brief Update rain splash effects for Raining Seeds/Stormy Night
	void UpdateRain();

	/// @brief Handle a zombie eating a brain in I-Zombie mode
	/// @param theZombie The zombie eating the brain
	/// @return True if the zombie ate a brain
	bool IZombieEatBrain(Zombie *theZombie);

	GridItem *IZombieGetBrainTarget(Zombie *theZombie);

	/// @brief Place a single plant in I-Zombie mode at a specific grid square
	/// @param theSeedType The type of plant to place
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	void IZombiePlacePlantInSquare(SeedType theSeedType, int theGridX, int theGridY = -1);

	/// @brief Advance Crazy Dave dialog text during Scary Potter levels
	void AdvanceCrazyDaveDialog();

	/// @brief Flash a Beghouled plant for visual feedback
	/// @param theFlashX The grid X to flash
	/// @param theFlashY The grid Y to flash
	/// @param theFromX The from grid X of the move
	/// @param theFromY The from grid Y of the move
	/// @param theToX The to grid X of the move
	/// @param theToY The to grid Y of the move
	void BeghouledFlashPlant(int theFlashX, int theFlashY, int theFromX, int theFromY, int theToX, int theToY);

	/// @brief Flash a valid match on the Beghouled board for hint purposes
	void BeghouledFlashAMatch();

	bool BeghouledFlashFromBoardState(BeghouledBoardState *theBoardState, int theFromX, int theFromY, int theToX, int theToY);

	/// @brief Drop sun from a sunflower when it is eaten in I-Zombie mode
	/// @param thePlant The plant that was eaten
	void IZombiePlantDropRemainingSun(Plant *thePlant);

	/// @brief Squish an I-Zombie brain with the cursor
	/// @param theBrain The brain grid item to squish
	void IZombieSquishBrain(GridItem *theBrain);

	/// @brief Score a brain collection and check for win condition
	/// @param theBrain The brain grid item that was scored
	void IZombieScoreBrain(GridItem *theBrain);

	/// @brief Update the Last Stand challenge (onslaught button and counter)
	void LastStandUpdate();

	/// @brief Place graves for the Whack-a-Zombie challenge mode
	/// @param theGraveCount Number of graves to place
	void WhackAZombiePlaceGraves(int theGraveCount);

	/// @brief Convert screen coordinates to a Beghouled twist grid square
	/// @param theX The screen X coordinate
	/// @param theY The screen Y coordinate
	/// @param theGridX Output grid X coordinate
	/// @param theGridY Output grid Y coordinate
	/// @return True if the coordinates map to a valid grid square
	bool BeghouledTwistSquareFromMouse(int theX, int theY, int &theGridX, int &theGridY);

	/// @brief Check if a 2x2 grid twist move at the given position is valid
	/// @param theGridX The X grid coordinate of the top-left cell
	/// @param theGridY The Y grid coordinate of the top-left cell
	/// @param theBoardState The current Beghouled board state
	/// @return True if the move is within bounds and all four cells contain plants
	bool BeghouledTwistValidMove(int theGridX, int theGridY, BeghouledBoardState *theBoardState);

	/// @brief Handle mouse down for Beghouled Twist mode
	/// @param x The screen X coordinate of the mouse
	/// @param y The screen Y coordinate of the mouse
	void BeghouledTwistMouseDown(int x, int y);

	/// @brief Check if twisting the 2x2 grid at the given position produces a match
	/// @param theGridX The X grid coordinate of the top-left cell
	/// @param theGridY The Y grid coordinate of the top-left cell
	/// @param theBoardState The current Beghouled board state
	/// @return True if the twist would result in a valid match
	bool BeghouledTwistMoveCausesMatch(int theGridX, int theGridY, BeghouledBoardState *theBoardState);

	/// @brief Flash plants when a twist move would create a match
	/// @param theBoardState The current Beghouled board state
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	/// @return True if a match would be created
	bool BeghouledTwistFlashMatch(BeghouledBoardState *theBoardState, int theGridX, int theGridY);

	/// @brief Cancel all active match flashing on Beghouled plants
	void BeghouledCancelMatchFlashing();
	/// @brief Start the Beghouled plants falling animation
	/// @param theState The challenge state to set (falling or moving)
	void BeghouledStartFalling(ChallengeState theState);

	/// @brief Fill all empty holes in the Beghouled board with random seeds
	/// @param theBoardState The current Beghouled board state
	/// @param theAllowMatches If true, allow placements that create immediate matches
	void BeghouledFillHoles(BeghouledBoardState *theBoardState, bool theAllowMatches);

	/// @brief Initialize the starting Beghouled board with no initial matches
	void BeghouledMakeStartBoard();

	/// @brief Create plant entities from a new Beghouled board state
	/// @param theOldBoardState The previous board state
	/// @param theNewBoardState The new board state to create plants from
	void BeghouledCreatePlants(BeghouledBoardState *theOldBoardState, BeghouledBoardState *theNewBoardState);

	/// @brief Execute stage completion (award prize or fade to next stage)
	/// @param theGridX The grid X coordinate of the completion trigger
	/// @param theGridY The grid Y coordinate of the completion trigger
	void PuzzlePhaseComplete(int theGridX, int theGridY);

	/// @brief Check if the current puzzle stage awards a prize (every N stages)
	/// @return True if this stage should award a prize instead of clearing
	bool PuzzleIsAwardStage();

	/// @brief Place a zombie on the board in I-Zombie mode
	/// @param theZombieType The type of zombie to place
	/// @param theGridX The grid X coordinate
	/// @param theGridY The grid Y coordinate
	void IZombiePlaceZombie(ZombieType theZombieType, int theGridX, int theGridY);

	/// @brief Update tutorial state for Whack-a-Zombie challenge
	void WhackAZombieUpdate();

	/// @brief Handle Last Stand stage completion (reset plants, show message)
	void LastStandCompletedStage();

	/// @brief Main update loop for Tree of Wisdom (clouds, babble timers, tool checks)
	void TreeOfWisdomUpdate();

	/// @brief Apply tree food fertilizer to the Tree of Wisdom
	void TreeOfWisdomFertilize();

	/// @brief Initialize the Tree of Wisdom (load reanimations, set initial size)
	void TreeOfWisdomInit();

	/// @brief Check if the mouse cursor is hovering over the Tree of Wisdom
	/// @param theX The screen X coordinate
	/// @param theY The screen Y coordinate
	/// @return True if the cursor is over the tree
	bool TreeOfWisdomMouseOn(int theX, int theY);

	/// @brief Get the current size of the Tree of Wisdom from player records
	/// @return The tree height/record value
	int TreeOfWisdomGetSize();

	/// @brief Draw the Tree of Wisdom (reanimations, speech bubble, height text)
	/// @param g The graphics context to draw with
	void TreeOfWisdomDraw(Graphics *g);

	/// @brief Leave Tree of Wisdom and transition to Zen Garden
	void TreeOfWisdomNextGarden();

	/// @brief Update tree food item animation and trigger grow when complete
	/// @param theZenTool The tree food grid item
	void TreeOfWisdomToolUpdate(GridItem *theZenTool);

	/// @brief Open the store screen from the Tree of Wisdom (buy tree food)
	void TreeOfWisdomOpenStore();

	/// @brief Process tree food items left on the board (grow and remove)
	void TreeOfWisdomLeave();

	/// @brief Grow the Tree of Wisdom by one level and play grow animation
	void TreeOfWisdomGrow();

	/// @brief Handle mouse click on the Tree of Wisdom (apply fertilizer)
	/// @param theMouseX The screen X coordinate
	/// @param theMouseY The screen Y coordinate
	void TreeOfWisdomTool(int theMouseX, int theMouseY);

	/// @brief Hit-test the Tree of Wisdom for mouse interaction
	/// @param theX The screen X coordinate
	/// @param theY The screen Y coordinate
	/// @param theHitResult Output hit result structure
	/// @return True if the tree was hit
	bool TreeOfWisdomHitTest(int theX, int theY, HitResult *theHitResult);

	/// @brief Trigger tree babbling state with random dialogue
	void TreeOfWisdomBabble();

	/// @brief Set the tree wisdom dialogue index based on tree size milestones
	void TreeOfWisdomGiveWisdom();

	/// @brief Set a random repeat dialogue for the Tree of Wisdom
	void TreeOfWisdomSayRepeat();

	/// @brief Check if the Tree of Wisdom can be fed fertilizer
	/// @return True if the tree can currently be fed
	bool TreeOfWisdomCanFeed();

	GridItem *GetPortalLeftRight(int theGridX, int theGridY, bool theToLeft = true);
};

extern SeedType gArtChallengeWallnut[6][9];
extern SeedType gArtChallengeSunFlower[6][9];
extern SeedType gArtChallengeStarFruit[6][9];

class ZombieAllowedLevels
{
  public:
	ZombieType mZombieType;
	int mAllowedOnLevel[50];
};
extern int gZombieWaves[NUM_LEVELS];
extern ZombieAllowedLevels gZombieAllowedLevels[NUM_ZOMBIE_TYPES];

#endif