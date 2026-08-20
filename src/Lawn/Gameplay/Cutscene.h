#ifndef __CUTSCENE_H__
#define __CUTSCENE_H__

using namespace Sexy;
class LawnApp;
class Board;
class ChallengeScreen;

class CutScene
{
  public:
	LawnApp *mApp;
	Board *mBoard;
	int mCutsceneTime;
	int mSodTime;
	int mGraveStoneTime;
	int mReadySetPlantTime;
	int mFogTime;
	int mBossTime;
	int mCrazyDaveTime;
	int mLawnMowerTime;
	int mCrazyDaveDialogStart;
	bool mSeedChoosing;
	ReanimationID mZombiesWonReanimID;
	bool mPreloaded;
	bool mPlacedZombies;
	bool mPlacedLawnItems;
	int mCrazyDaveCountDown;
	int mCrazyDaveLastTalkIndex;
	bool mUpsellHideBoard;
	ChallengeScreen *mUpsellChallengeScreen;
	bool mPreUpdatingBoard;

  public:
	CutScene();
	~CutScene();
	/// @brief Begins the level introduction sequence
	void StartLevelIntro();

	/// @brief Cancels the currently playing intro sequence
	void CancelIntro();

	/// @brief Updates the board's logic state each tick
	void Update();

	/// @brief Advances all board animations by one tick
	void AnimateBoard();

	/// @brief Transitions the game into the seed/plant selection screen
	void StartSeedChooser();

	/// @brief Closes the seed chooser and transitions into gameplay
	void EndSeedChooser();

	/// @brief Linearly interpolates a position value between two points over a time range
	/// @param theTimeStart Start time of the interpolation range
	/// @param theTimeEnd End time of the interpolation range
	/// @param thePositionStart Position value at the start time
	/// @param thePositionEnd Position value at the end time
	/// @return Interpolated position value for the current time
	int CalcPosition(int theTimeStart, int theTimeEnd, int thePositionStart, int thePositionEnd);

	/// @brief Places the initial set of zombies on a street/night-type level
	void PlaceStreetZombies();

	/// @brief Spawns gravestone particle effects across the board
	void AddGraveStoneParticles();

	/// @brief Spawns a zombie of the given type at the specified grid position
	/// @param theZombieType Type of zombie to place
	/// @param theGridX Grid X coordinate to place the zombie at
	/// @param theGridY Grid Y coordinate to place the zombie at
	void PlaceAZombie(ZombieType theZombieType, int theGridX, int theGridY);

	/// @brief Checks whether a zombie type can be validly placed at a given grid spot
	/// @param theZombieType Type of zombie to check placement for
	/// @param theGridX Grid X coordinate to evaluate
	/// @param theGridY Grid Y coordinate to evaluate
	/// @param theZombieGrid 5x5 occupancy grid tracking already-placed zombies
	/// @return True if the zombie can occupy that spot, false otherwise
	bool CanZombieGoInGridSpot(ZombieType theZombieType, int theGridX, int theGridY, bool theZombieGrid[5][5]);

	/// @brief Checks whether the current survival level requires a zombie wave repick
	/// @return True if a repick is needed, false otherwise
	bool IsSurvivalRepick();

	/// @brief Checks whether the game state is past the seed chooser phase
	/// @return True if the seed chooser has already been completed, false otherwise
	bool IsAfterSeedChooser();

	/// @brief Places flower pots on the board for roof levels
	void AddFlowerPots();

	/// @brief Updates the zombies-won end sequence each tick
	void UpdateZombiesWon();

	/// @brief Initiates the zombies-won cutscene and loss sequence
	void StartZombiesWon();

	/// @brief Checks whether a zombie should be shown walking during the win cutscene
	/// @return True if the walking zombie should be visible, false otherwise
	bool ShowZombieWalking();

	/// @brief Checks whether the current cutscene has finished playing
	/// @return True if the cutscene is over, false otherwise
	bool IsCutSceneOver();

	/// @brief Handles a click event during the zombie-won cutscene sequence
	void ZombieWonClick();

	/// @brief Handles a mouse down input event on the board
	/// @param theX Screen X coordinate of the mouse press
	/// @param theY Screen Y coordinate of the mouse press
	void MouseDown(int theX, int theY);

	/// @brief Handles a key down input event on the board
	/// @param theKey The key code of the pressed key
	void KeyDown(KeyCode theKey);

	/// @brief Advances Crazy Dave's dialog to the next line or skips it entirely
	/// @param theJustSkipping True to skip the dialog immediately, false to advance normally
	void AdvanceCrazyDaveDialog(bool theJustSkipping);

	/// @brief Makes the shovel tool visible in the UI
	void ShowShovel();

	/// @brief Checks whether the player is eligible to receive a seed packet upgrade
	/// @return True if an upgrade is available, false otherwise
	bool CanGetPacketUpgrade();

	/// @brief Checks whether a specific seed packet slot is eligible for an upgrade
	/// @param theIndex Index of the seed packet slot to evaluate
	/// @return True if that slot can be upgraded, false otherwise
	bool CanGetPacketUpgrade(int theIndex);

	/// @brief Finds a valid grid position for placing a street zombie and returns it via output params
	/// @param theZombieType Type of zombie to find a position for
	/// @param theZombieGrid 5x5 grid tracking already-placed zombies
	/// @param thePosX Output receiving the chosen pixel X position
	/// @param thePosY Output receiving the chosen pixel Y position
	void FindPlaceForStreetZombies(ZombieType theZombieType, bool theZombieGrid[5][5], int &thePosX, int &thePosY);

	/// @brief Finds a valid position and places the given zombie type on the board
	/// @param theZombieType Type of zombie to place
	/// @param theZombieGrid 5x5 occupancy grid tracking already-placed zombies
	void FindAndPlaceZombie(ZombieType theZombieType, bool theZombieGrid[5][5]);

	/// @brief Checks whether the given zombie type occupies a 2x2 grid
	/// @param theZombieType The zombie type to check
	/// @return True if the zombie is 2x2 in size, false otherwise
	static bool Is2x2Zombie(ZombieType theZombieType);

	/// @brief Preloads all resources needed for the current level
	void PreloadResources();

	/// @brief Checks whether the board is still in the pre-preload state
	/// @return True if preloading has not yet begun, false otherwise
	bool IsBeforePreloading();

	/// @brief Checks whether Crazy Dave's dialog UI is currently being shown
	/// @return True if Crazy Dave is visible and talking, false otherwise
	bool IsShowingCrazyDave();

	/// @brief Checks whether the current cutscene does not scroll the camera
	/// @return True if the cutscene is non-scrolling, false otherwise
	bool IsNonScrollingCutscene();

	/// @brief Checks whether the board camera starts scrolled to the left
	/// @return True if the level begins with the camera at the left position, false otherwise
	bool IsScrolledLeftAtStart();

	/// @brief Checks whether the player is currently in the shovel tutorial segment
	/// @return True if in the shovel tutorial, false otherwise
	bool IsInShovelTutorial();

	/// @brief Places all initial Plants and GridItems on the board
	void PlaceLawnItems();

	/// @brief Checks whether the player is eligible for a second seed packet slot upgrade
	/// @return True if a second upgrade is available, false otherwise
	bool CanGetSecondPacketUpgrade();

	/// @brief Parses and returns the delay time value from the current Crazy Dave message
	/// @return Delay time in ticks extracted from the message data
	int ParseDelayTimeFromMessage();

	/// @brief Parses and returns the talk duration from the current Crazy Dave message
	/// @return Talk time in ticks extracted from the message data
	int ParseTalkTimeFromMessage();

	/// @brief Clears and resets all state associated with the upsell board
	void ClearUpsellBoard();

	/// @brief Loads and initializes the intro board layout
	void LoadIntroBoard();

	/// @brief Adds a zombie to the upsell screen at the specified position
	/// @param theZombieType Type of zombie to add to the upsell display
	/// @param thePixelX Pixel X coordinate to place the zombie at
	/// @param theGridY Grid Y row to place the zombie in
	void AddUpsellZombie(ZombieType theZombieType, int thePixelX, int theGridY);

	/// @brief Loads the pool upsell board layout
	void LoadUpsellBoardPool();

	/// @brief Loads the fog upsell board layout
	void LoadUpsellBoardFog();

	/// @brief Loads the challenge screen upsell layout
	void LoadUpsellChallengeScreen();

	/// @brief Loads the roof upsell board layout
	void LoadUpsellBoardRoof();

	/// @brief Updates the upsell screen logic each tick
	void UpdateUpsell();

	/// @brief Draws the upsell screen
	/// @param g Graphics context to draw to
	void DrawUpsell(Graphics *g);

	/// @brief Updates the intro sequence logic each tick
	void UpdateIntro();

	/// @brief Draws the intro sequence
	/// @param g Graphics context to draw to
	void DrawIntro(Graphics *g);

	/// @brief Checks whether the upsell board sequence should run for the current session
	/// @return True if the upsell board should be shown, false otherwise
	bool ShouldRunUpsellBoard();
};

#endif
