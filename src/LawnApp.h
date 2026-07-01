#ifndef __LAWNAPP_H__
#define __LAWNAPP_H__

#include "ConstEnums.h"
#include "SexyAppFramework/SexyApp.h"

class Achievements;
class Board;
class GameSelector;
class ChallengeDefinition;
class SeedChooserScreen;
class AwardScreen;
class CreditScreen;
class TodFoley;
class PoolEffect;
class ZenGarden;
class PottedPlant;
class EffectSystem;
class TodParticleSystem;
class Reanimation;
class ReanimatorCache;
class ProfileMgr;
class PlayerInfo;
class Music;
class TitleScreen;
class PopDRMComm;
class ChallengeScreen;
class StoreScreen;
class AlmanacDialog;
class TypingCheck;
#if LAWN_DEBUG_TOOLS
class DebuggerWindow;
#endif

namespace Sexy
{
class Dialog;
class Graphics;
class ButtonWidget;
class Version;
}; // namespace Sexy

enum FoleyType;

using namespace Sexy;

typedef std::list<ButtonWidget *> ButtonList;
typedef std::list<Image *> ImageList;

class LevelStats
{
  public:
	int mUnusedLawnMowers;

  public:
	LevelStats()
	{
		Reset();
	}
	inline void Reset()
	{
		mUnusedLawnMowers = 0;
	}
};

class LawnApp : public SexyApp
{
  public:
	Board *mBoard;
	TitleScreen *mTitleScreen;
	GameSelector *mGameSelector;
	SeedChooserScreen *mSeedChooserScreen;
	AwardScreen *mAwardScreen;
	CreditScreen *mCreditScreen;
	ChallengeScreen *mChallengeScreen;
	TodFoley *mSoundSystem;
	ButtonList mControlButtonList;
	ImageList mCreatedImageList;
	std::string mReferId;
	std::string mRegisterLink;
	std::string mMod;
	bool mRegisterResourcesLoaded;
	bool mTodCheatKeys;
	GameMode mGameMode;
	GameScenes mGameScene;
	bool mLoadingZombiesThreadCompleted;
	bool mFirstTimeGameSelector;
	int mGamesPlayed;
	int mMaxExecutions;
	int mMaxPlays;
	int mMaxTime;
	bool mEasyPlantingCheat;
	PoolEffect *mPoolEffect;
	ZenGarden *mZenGarden;
	EffectSystem *mEffectSystem;
	ReanimatorCache *mReanimatorCache;
	ProfileMgr *mProfileMgr;
	PlayerInfo *mPlayerInfo;
	Achievements *mAchievements;
	LevelStats *mLastLevelStats;
	bool mCloseRequest;
	int mAppCounter;
	Music *mMusic;
	ReanimationID mCrazyDaveReanimID;
	CrazyDaveState mCrazyDaveState;
	int mCrazyDaveBlinkCounter;
	ReanimationID mCrazyDaveBlinkReanimID;
	int mCrazyDaveMessageIndex;
	SexyString mCrazyDaveMessageText;
	int mAppRandSeed;
#if SEXY_USE_DRM
	PopDRMComm *mDRM;
#endif
#if LAWN_DEBUG_TOOLS
	bool mDebuggerEnabled;
	DebuggerWindow *mDebugWindow;
#endif
	int64_t mSessionID;
	int mPlayTimeActiveSession;
	int mPlayTimeInactiveSession;
	BoardResult mBoardResult;
	bool mSawYeti;
	TypingCheck *mKonamiCheck;
	TypingCheck *mMustacheCheck;
	TypingCheck *mMoustacheCheck;
	TypingCheck *mSuperMowerCheck;
	TypingCheck *mSuperMowerCheck2;
	TypingCheck *mFutureCheck;
	TypingCheck *mPinataCheck;
	TypingCheck *mDanceCheck;
	TypingCheck *mDaisyCheck;
	TypingCheck *mSukhbirCheck;
	bool mMustacheMode;
	bool mSuperMowerMode;
	bool mFutureMode;
	bool mPinataMode;
	bool mDanceMode;
	bool mDaisyMode;
	bool mSukhbirMode;
	TrialType mTrialType;
	bool mDebugTrialLocked;
	bool mMuteSoundsForCutscene;
	bool mRunInCompileMode{};

	static Version gResoddedVersion;

  public:
	LawnApp();
	virtual ~LawnApp();

	/// @brief Close the Options Dialog
	/// @return True if it exists, false otherwise
	bool KillNewOptionsDialog();

	/// @brief Handle the gaining of the window's focus
	virtual void GotFocus();

	/// @brief Handle the window's loss of focus
	virtual void LostFocus();

	/// @brief Perform extra code post-init
	virtual void InitHook();

	/// @brief Write values from the games registry
	virtual void WriteToRegistry();

	/// @brief Read values from the games registry
	virtual void ReadFromRegistry();

	/// @brief Start the loading thread
	virtual void LoadingThreadProc();

	/// @brief Handle the end of loading
	virtual void LoadingCompleted();

	/// @brief Handle the completion of the loading thread
	virtual void LoadingThreadCompleted();

	/// @brief Handle a failed URL opening
	/// @param theURL The URL accessed
	virtual void URLOpenFailed(const std::string &theURL);

	/// @brief Handle a successful URL opening
	/// @param theURL The URL accessed
	virtual void URLOpenSucceeded(const std::string &theURL);

	/// @brief Try to open an URL in the web browser
	/// @param theURL The URL to access
	/// @param shutdownOnOpen Should we shutdown on opening
	/// @return True if we opened the URL, false otherwise
	virtual bool OpenURL(const std::string &theURL, bool shutdownOnOpen);

	/// @brief Handle the pressing of a debug key
	/// @param theKey The key pressed
	/// @return True if the key press did something, false otherwise
	virtual bool DebugKeyDown(int theKey);

	/// @brief Handle the parsing of a command parameter
	/// @param theParamName The name of the parameter
	/// @param theParamValue The value of the parameter
	virtual void HandleCmdLineParam(const std::string &theParamName, const std::string &theParamValue);

	/// @brief Confirm the user wants to quit
	void ConfirmQuit();

	/// @brief Confirm the check for a new update ??? (Unimplemented)
	void ConfirmCheckForUpdates()
	{
		;
	}

	/// @brief Check for a new update (Unimplemented)
	void CheckForUpdates()
	{
		;
	}

	/// @brief Create a profile dialog
	void DoUserDialog();

	/// @brief Handle the end of the profile dialog
	/// @param isYes Did we change the profile
	void FinishUserDialog(bool isYes);

	/// @brief Create a profile creation dialog
	void DoCreateUserDialog();

	/// @brief Create a cheat dialog
	void DoCheatDialog();

	/// @brief Handle the end of a cheat
	/// @param isYes Did we apply the cheat
	void FinishCheatDialog(bool isYes);

	/// @brief Handle the end of a profile creation
	/// @param isYes Did we accept the creation
	void FinishCreateUserDialog(bool isYes);

	/// @brief Handle a profile deletion
	/// @param theName The deleted profile name
	void DoConfirmDeleteUserDialog(const SexyString &theName);

	/// @brief Handle the end of a profile deletion
	/// @param isYes Did we accept the deletion
	void FinishConfirmDeleteUserDialog(bool isYes);

	/// @brief Handle a profile renaming
	/// @param theName The new profile name
	void DoRenameUserDialog(const SexyString &theName);

	/// @brief Handle the end of a profile renaming
	/// @param isYes Did we accept the rename
	void FinishRenameUserDialog(bool isYes);

	/// @brief Handle a profile name error
	/// @param theId The dialog's id that we originated from
	void FinishNameError(int theId);

	/// @brief Handle the restarting from a confirm selling
	void FinishRestartConfirmDialog();

	/// @brief Create the confirm selling dialog
	/// @param theMessage The message to display in the dialog
	void DoConfirmSellDialog(const SexyString &theMessage);

	/// @brief Create the confirm purchase dialog
	/// @param theMessage The message to display in the dialog
	void DoConfirmPurchaseDialog(const SexyString &theMessage);

	/// @brief Destroy Times Up Dialog
	void FinishTimesUpDialog();

	/// @brief Destroy the Board
	void KillBoard();

	/// @brief Create a new Board
	void MakeNewBoard();

	/// @brief Start the main gameplay (Destroy SeedChooser, set GameScene)
	void StartPlaying();

	/// @brief Try to load a game from the savefile
	/// @return True if it loaded successfully, false otherwise
	bool TryLoadGame();

	/// @brief Create a brand new game
	void NewGame();

	/// @brief Handle the starting of a game
	/// @param theGameMode The GameMode to start
	/// @param theLookForSavedGame Should we look for an existing savefile
	void PreNewGame(GameMode theGameMode, bool theLookForSavedGame);

	/// @brief Open the GameSelector
	void ShowGameSelector();

	/// @brief Close the GameSelector
	void KillGameSelector();

	/// @brief Open the Award Screen
	/// @param theAwardType The type of award to present (see AwardType enum)
	void ShowAwardScreen(AwardType theAwardType);

	/// @brief Close the Award Screen
	void KillAwardScreen();

	/// @brief Open the SeedChooser
	void ShowSeedChooserScreen();

	/// @brief Close the SeedChooser
	void KillSeedChooserScreen();

	/// @brief Return to the GameSelector from the Board
	void DoBackToMain();

	/// @brief Do the Board exit confirmation dialog
	void DoConfirmBackToMain();

	/// @brief Open the Options Menu
	/// @param theFromGameSelector Did we open from the GameSelector
	void DoNewOptions(bool theFromGameSelector);

	/// @brief Attempt to Register (Unimplemented)
	void DoRegister();

	/// @brief Handle the Registering error (Unimplemented)
	void DoRegisterError();

	/// @brief Can open the Registering Dialog (Unimplemented)
	/// @return True if we can register, false otherwise
	bool CanDoRegisterDialog();

	/// @brief Write the PlayerInfo to Disk
	/// @return Always true
	bool WriteCurrentUserConfig();

	/// @brief Open the Registering Dialog () (Unimplemented)
	void DoNeedRegisterDialog();

	/// @brief Open the Continue Dialog
	void DoContinueDialog();

	/// @brief Open the Pause Dialog
	void DoPauseDialog();

	/// @brief Handle the closing of modeless dialogs (Unimplemented)
	void FinishModelessDialogs();

	/// @brief Create a new dialog and focus on it
	/// @param theDialogId The ID of the dialog (determines the behavior on closing and opening)
	/// @param isModal Is the dialog a modal dialog
	/// @param theDialogHeader Text translation key for header
	/// @param theDialogLines Text translation key for the content
	/// @param theDialogFooter Text translation key for footer
	/// @param theButtonMode Button creation flags
	/// @return The created dialog
	virtual Dialog *DoDialog(int theDialogId,
	                         bool isModal,
	                         const SexyString &theDialogHeader,
	                         const SexyString &theDialogLines,
	                         const SexyString &theDialogFooter,
	                         int theButtonMode);

	/// @brief After 30 ticks create a new dialog and focus on it
	/// @param theDialogId The ID of the dialog (determines the behavior on closing and opening)
	/// @param isModal Is the dialog a modal dialog
	/// @param theDialogHeader Text translation key for header
	/// @param theDialogLines Text translation key for the content
	/// @param theDialogFooter Text translation key for footer
	/// @param theButtonMode Button creation flags
	/// @return The created dialog
	virtual Dialog *DoDialogDelay(int theDialogId,
	                              bool isModal,
	                              const SexyString &theDialogHeader,
	                              const SexyString &theDialogLines,
	                              const SexyString &theDialogFooter,
	                              int theButtonMode);

	/// @brief Shutdown the game
	virtual void Shutdown();

	/// @brief Initialize the game
	virtual void Init();

	/// @brief Start the game
	virtual void Start();

	/// @brief create a new dialog
	/// @param theDialogId The ID of the dialog (determines the behavior on closing and opening)
	/// @param isModal Is the dialog a modal dialog
	/// @param theDialogHeader Text translation key for header
	/// @param theDialogLines Text translation key for the content
	/// @param theDialogFooter Text translation key for footer
	/// @param theButtonMode Button creation flags
	/// @return The created dialog
	virtual Dialog *NewDialog(int theDialogId,
	                          bool isModal,
	                          const SexyString &theDialogHeader,
	                          const SexyString &theDialogLines,
	                          const SexyString &theDialogFooter,
	                          int theButtonMode);

	/// @brief Destroy the dialog
	/// @param theDialogId The ID of the dialog (determines the behavior on closing and opening)
	/// @return True if it was destroyed succesfully, false otherwise
	virtual bool KillDialog(int theDialogId);

	/// @brief Handle the opening of a modal dialog
	virtual void ModalOpen();

	/// @brief Handle the closing of a modal dialog
	virtual void ModalClose();

	/// @brief Execute extra code before showing the window
	virtual void PreDisplayHook();

	/// @brief Execute extra code before changing the working directory
	/// @return True if we abort the changing of the working directory, false otherwise
	virtual bool ChangeDirHook(const char *theIntendedPath);

	/// @brief Do we need to register the game (Unimplemented)
	/// @return True if the game needs to be registered, false otherwise
	virtual bool NeedRegister();

	/// @brief Update the registering info (Unimplemented)
	virtual void UpdateRegisterInfo();

	/// @brief Handle the pressing of a button
	/// @param theId The id of the button
	virtual void ButtonPress(int theId);

	/// @brief Handle the depressing of a button
	/// @param theId The id of the button
	virtual void ButtonDepress(int theId);

	/// @brief Run a frame update
	virtual void UpdateFrames();

	/// @brief Run an application update
	virtual bool UpdateApp();

	/// @brief Are we playing Adventure Mode
	/// @return True if the GameMode is Adventure, false otherwise
	bool IsAdventureMode();

	/// @brief Are we playing a Survival Mode
	/// @return True if the GameMode is a Survival Mode, false otherwise
	bool IsSurvivalMode();

	/// @brief Are we playing a Continuous Challenge
	/// @return True if the GameMode is a Continuous Challenge, false otherwise
	bool IsContinuousChallenge();

	/// @brief Are we playing a Art Challenge
	/// @return True if the GameMode is a Art Challenge, false otherwise
	bool IsArtChallenge();

	/// @brief Do we need to pause the game
	/// @return True if we need to pause the game, false otherwise
	bool NeedPauseGame();

	/// @brief Show an error that came from the ResourceManager
	/// @param doExit Should we exit the game after the error
	virtual void ShowResourceError(bool doExit = false);

	/// @brief Toggle the debug slow-motion mode
	void ToggleSlowMo();

	/// @brief Toggle the debug fast-motion mode
	void ToggleFastMo();

	/// @brief Play a foley sound
	/// @param theFoleyType The foley type to play
	void PlayFoley(FoleyType theFoleyType);

	/// @brief Play a foley sound
	/// @param theFoleyType The foley type to play
	/// @param thePitch The pitch of the foley
	void PlayFoleyPitch(FoleyType theFoleyType, float thePitch);

	/// @brief Play a sound sample
	/// @param theSoundNum The sound sample id
	void PlaySample(int theSoundNum);

	/// @brief Quickly load a GameMode
	/// @param theGameMode The GameMode to load
	void FastLoad(GameMode theGameMode);

	/// @brief Turn a level id to a String
	/// @param theLevel The level number to stringify
	/// @return String version of the level (x-y)
	static SexyString GetStageString(int theLevel);

	/// @brief Destroy the Challege Screen
	void KillChallengeScreen();

	/// @brief Open the Challege Screen at a certain page
	/// @param thePage The page to open
	void ShowChallengeScreen(ChallengePage thePage);

	/// @brief Get the definition of the current Challenge
	/// @return The current ChallengeDefinition
	ChallengeDefinition &GetCurrentChallengeDef();

	/// @brief Handle the end of a level
	void CheckForGameEnd();

	/// @brief Handle an async close request
	virtual void CloseRequestAsync();

	/// @brief Is the current Challenge without a SeedBank
	/// @brief True if we don't have a SeedBank
	bool IsChallengeWithoutSeedBank();

	/// @brief Open the Almanac on a certain entry
	/// @param theSeedType [OPTIONAL - SeedType::SEED_NONE] The SeedType to open the page
	/// @param theZombieType [OPTIONAL - ZombieType::ZOMBIE_INVALID] The ZombieType to open the page on
	AlmanacDialog *DoAlmanacDialog(SeedType theSeedType = SeedType::SEED_NONE, ZombieType theZombieType = ZombieType::ZOMBIE_INVALID);

	/// @brief Close the Almanac
	bool KillAlmanacDialog();

	/// @brief Get the amount of Seeds available
	/// @return The number of seeds
	int GetSeedsAvailable();

	/// @brief Add a new Reanimation to the EffectSystem
	/// @param theX The X coordinate of the reanimation
	/// @param theY The Y coordinate of the reanimation
	/// @param theRenderOrder The render order of the Reanimation
	/// @param theReanimationType The Reanimation to create (see ReanimationType enum)
	/// @return The created Reanimation
	Reanimation *AddReanimation(float theX, float theY, int theRenderOrder, ReanimationType theReanimationType);

	/// @brief Add a new ParticleSystem to the EffectSystem
	/// @param theX The X coordinate of the ParticleSystem
	/// @param theY The Y coordinate of the ParticleSystem
	/// @param theRenderOrder The render order of the ParticleSystem
	/// @param theEffect The ParticleSystem to create (see ParticleEffect enum)
	/// @return The created ParticleSystem
	TodParticleSystem *AddTodParticle(float theX, float theY, int theRenderOrder, ParticleEffect theEffect);

	/// @brief Get the id of a ParticleSystem from the EffectSystem
	/// @param theParticle The ParticleSystem instance
	/// @return The ParticleSystem id
	ParticleSystemID ParticleGetID(TodParticleSystem *theParticle);

	/// @brief Get the a ParticleSystem instance from an id
	/// @param theParticleID The ParticleSystem id
	/// @return The ParticleSystem instance
	TodParticleSystem *ParticleGet(ParticleSystemID theParticleID);

	/// @brief Try to get the a ParticleSystem instance from an id
	/// @param theParticleID The ParticleSystem id
	/// @return The ParticleSystem instance or nullptr if it doesn't exist
	TodParticleSystem *ParticleTryToGet(ParticleSystemID theParticleID);

	/// @brief Get the id of a Reanimation from the EffectSystem
	/// @param theReanimation The Reanimation instance
	/// @return The Reanimation id
	ReanimationID ReanimationGetID(Reanimation *theReanimation);

	/// @brief Get the a Reanimation instance from an id
	/// @param theReanimationID The Reanimation id
	/// @return The Reanimation instance
	Reanimation *ReanimationGet(ReanimationID theReanimationID);

	/// @brief Try to get the a Reanimation instance from an id
	/// @param theReanimationID The Reanimation id
	/// @return The Reanimation instance or nullptr if it doesn't exist
	Reanimation *ReanimationTryToGet(ReanimationID theReanimationID);

	/// @brief Remove a Reanimation from the EffectSystem
	/// @param theReanimationID The Reanimation id
	void RemoveReanimation(ReanimationID theReanimationID);

	/// @brief Remove a ParticleSystem from the EffectSystem
	/// @param theParticleID The ParticleSystem id
	void RemoveParticle(ParticleSystemID theParticleID);

	/// @brief Open the StoreScreen
	/// @return Created StoreScreen
	StoreScreen *ShowStoreScreen();

	/// @brief Close the StoreScreen
	void KillStoreScreen();

	/// @brief Check if we have a SeedType
	/// @param theSeedType The SeedType to check
	/// @return True if we have the SeedType, false otherwise
	bool HasSeedType(SeedType theSeedType);

	/// @brief Check if we have a SeedType
	/// @param theSeedType The SeedType to check
	/// @return True if we have the SeedType, false otherwise
	bool SeedTypeAvailable(SeedType theSeedType);

	/// @brief End the Level
	void EndLevel();

	/// @brief Are we playing an Ice Demo
	/// @return True if the game is an Ice Demo, false otherwise
	inline bool IsIceDemo()
	{
		return false;
	}

	/// @brief Is the current level CanYouDigIt
	/// @return True if the GameMode is CanYouDigIt, false otherwise
	bool IsShovelLevel();

	/// @brief Is the current level a WallnutBowling
	/// @return True if the GameMode is a WallnutBowling, false otherwise
	bool IsWallnutBowlingLevel();

	/// @brief Is the current level a x-10
	/// @return True if the GameMode is a x-10, false otherwise
	bool IsMiniBossLevel();

	/// @brief Is the current level a SlotMachine level
	/// @return True if the GameMode is a SlotMachine level, false otherwise
	bool IsSlotMachineLevel();

	/// @brief Is the current level a LittleTrouble level
	/// @return True if the GameMode is a LittleTrouble level, false otherwise
	bool IsLittleTroubleLevel();

	/// @brief Is the current level a StormyNight level
	/// @return True if the GameMode is a StormyNight level, false otherwise
	bool IsStormyNightLevel();

	/// @brief Is the current level a FinalBoss level
	/// @return True if the GameMode is a FinalBoss level, false otherwise
	bool IsFinalBossLevel();

	/// @brief Is the current level a BungeeBlitz level
	/// @return True if the GameMode is a BungeeBlitz level, false otherwise
	bool IsBungeeBlitzLevel();

	/// @brief Get the SeedType awarded in a level
	/// @param theLevel The level to check
	/// @return Awarded SeedType
	static SeedType GetAwardSeedForLevel(int theLevel);

	/// @brief Get Crazy Dave's text from a specific index
	/// @param theMessageIndex The message's index
	/// @return String containing Crazy Dave's text
	SexyString GetCrazyDaveText(int theMessageIndex);

	/// @brief Can we show the Almanac
	/// @return True if we have the Almanac, false otherwise
	bool CanShowAlmanac();

	/// @brief Is the savefile's level a light level
	/// @return True if we are on a night level, false otherwise
	bool IsNight();

	/// @brief Can we show the Store
	/// @return True if we have the Store, false otherwise
	bool CanShowStore();

	/// @brief Did we beat the Challenge
	/// @param theGameMode The GameMode to check
	/// @return True if we have beaten the Challenge, false otherwise
	bool HasBeatenChallenge(GameMode theGameMode);

	/// @brief Get a PottedPlant in the ZenGarden from a specific index
	/// @param thePottedPlantIndex The index of the PottedPlant in the Zen Garden
	/// @return PottedPlant at the index
	PottedPlant *GetPottedPlantByIndex(int thePottedPlantIndex);

	/// @brief Is the GameMode a Survival Normal
	/// @param theGameMode The GameMode to check
	/// @return True if the GameMode is a Survival Normal, false otherwise
	static bool IsSurvivalNormal(GameMode theGameMode);

	/// @brief Is the GameMode a Survival Hard
	/// @param theGameMode The GameMode to check
	/// @return True if the GameMode is a Survival Hard, false otherwise
	static bool IsSurvivalHard(GameMode theGameMode);

	/// @brief Is the GameMode a Survival Endless
	/// @param theGameMode The GameMode to check
	/// @return True if the GameMode is a Survival Endless, false otherwise
	static bool IsSurvivalEndless(GameMode theGameMode);

	/// @brief Have we finished Adventure Mode
	/// @return True if we finished Adventure atleast once, false otherwise
	bool HasFinishedAdventure();

	/// @brief Is the first time we play Adventure Mode
	/// @return True if this is our first playthrough of Adventure Mode, false otherwise
	bool IsFirstTimeAdventureMode();

	/// @brief Can we spawn the YetiZombie
	/// @return True if we meet the spawning conditions, false otherwise
	bool CanSpawnYetis();

	/// @brief Make Crazy Dave enter from the left
	void CrazyDaveEnter();

	/// @brief Update Crazy Dave
	void UpdateCrazyDave();

	/// @brief Make Crazy Dave speak a message from an index
	/// @param theMessageIndex The index of the message for him to speak
	void CrazyDaveTalkIndex(int theMessageIndex);

	/// @brief Make Crazy Dave speak a message
	/// @param theMessage The message for him to speak
	void CrazyDaveTalkMessage(const SexyString &theMessage);

	/// @brief Make Crazy Dave leave to the left
	void CrazyDaveLeave();

	/// @brief Draw Crazy Dave
	/// @param g Graphics object
	void DrawCrazyDave(Graphics *g);

	/// @brief Remove Crazy Dave's object
	void CrazyDaveDie();

	/// @brief Make Crazy Dave stop talking
	void CrazyDaveStopTalking();

	/// @brief Preloads assets for the current profile
	void PreloadForUser();

	/// @brief
	/// @return
	int GetNumPreloadingTasks();

	int LawnMessageBox(int theDialogId,
	                   const SexyString &theHeaderName,
	                   const SexyString &theLinesName,
	                   const SexyString &theButton1Name,
	                   const SexyString &theButton2Name,
	                   int theButtonMode);

	/// @brief Open Credits screen
	void ShowCreditScreen();

	/// @brief Destroy the Credits screen
	void KillCreditScreen();

	/// @brief Open the Zombatar TOS dialog
	void ShowZombatarTOS();

	/// @brief Choose a string based on the count
	/// @param theCount A value that if > 1 it chooses the plural string
	/// @param theSingular The singular version of the String
	/// @param thePlural The plural version of the String
	/// @return The String based on the count
	static SexyString Pluralize(int theCount, const SexyString &theSingular, const SexyString &thePlural);

	/// @brief Get the amount of Trophies we unlocked on a page
	/// @param thePage The page to check
	/// @return The amount of Trophies
	int GetNumTrophies(ChallengePage thePage);

	/// @brief Have we earned the Gold Sunflower Trophy
	/// @return True if we met the requirements to earn it, false otherwise
	bool EarnedGoldTrophy();

	/// @brief Is the game registered
	/// @return True if it was registered (always false)
	inline bool IsRegistered()
	{
		return false;
	}

	/// @brief Has the free-trial expired
	/// @return True if it has expired (always false)
	inline bool IsExpired()
	{
		return false;
	}

	/// @brief Is the DRM System Connected
	/// @return True if it is connected (always false)
	inline bool IsDRMConnected()
	{
		return false;
	}

	/// @brief Is the current level a Vasebreaker
	/// @return True if we are playing a Vasebreaker level, false otherwise
	bool IsScaryPotterLevel();

	/// @brief Is the GameMode an Endless IZombie
	/// @param theGameMode The GameMode to check
	/// @return True if the GameMode is an Endless IZombie, false otherwise
	static bool IsEndlessIZombie(GameMode theGameMode);

	/// @brief Is the GameMode an Endless Vasebreaker
	/// @param theGameMode The GameMode to check
	/// @return True if the GameMode is an Endless Vasebreaker, false otherwise
	static bool IsEndlessScaryPotter(GameMode theGameMode);

	/// @brief Is the current level a Squirrel Level
	/// @return True if we are playing the Squirrel Level, false otherwise
	bool IsSquirrelLevel();

	/// @brief Is the current level an IZombie
	/// @return True if we are playing an IZombie level, false otherwise
	bool IsIZombieLevel();

	/// @brief Is the current level a WhackAZombie
	/// @return True if we are playing a WhackAZombie level, false otherwise
	bool IsWhackAZombieLevel();

	/// @brief Is the current level a Puzzle
	/// @return True if we are playing a Puzzle Level, false otherwise
	bool IsPuzzleMode();

	/// @brief Is the current level a Challenge
	/// @return True if we are playing a Challenge Level, false otherwise
	bool IsChallengeMode();

	/// @brief Can we show the Zen Garden
	/// @return True if we have the Zen Garden unlocked, false otherwise
	bool CanShowZenGarden();

	/// @brief Get the String version of some Money
	/// @return Stringified version of the money
	static SexyString GetMoneyString(int theAmount);

	/// @brief Advance to Crazy Dave's next message
	/// @return True if Crazy Dave's text continued, false otherwise
	bool AdvanceCrazyDaveText();

	/// @brief End Crazy Dave's handing animation
	void CrazyDaveDoneHanding();

	/// @brief Submit beta-testing review (Unimplemented)
	/// @param theAskForComments Ask the user for additional comments
	void BetaSubmit(bool theAskForComments);

	/// @brief Record the level stats of the player for beta-testing (Unimplemented)
	void BetaRecordLevelStats();

	/// @brief Update the PlayTime counters
	void UpdatePlayTimeStats();

	/// @brief ??? (Unimplemented)
	void BetaAddFile(std::list<std::string> &theUploadFileList, std::string theFileName, std::string theShortName);

	/// @brief Can we Pause the game
	/// @return True if we can Pause, false otherwise
	bool CanPauseNow();

	/// @brief Get the trophy count needed to unlock the Gold Sunflower Trophy
	/// @return The number of trophies
	int TrophiesNeedForGoldSunflower();

	/// @brief Get the index of the current Challenge (used in savefiles)
	/// @return The index
	int GetCurrentChallengeIndex();

	/// @brief Load a resource group
	/// @param theGroupName The group to load
	/// @param theGroupAveMsToLoad The average time to load it (used for task count calculations)
	void LoadGroup(const char *theGroupName, int theGroupAveMsToLoad);

	/// @brief Trace and Load a resource group (Unimplemented)
	/// @param theGroupName The group to load
	/// @param theGroupTime The time to load it (used for task count calculations)
	/// @param theTotalGroupWeigth ???
	/// @param theTaskWeight ???
	void TraceLoadGroup(const char *theGroupName, int theGroupTime, int theTotalGroupWeigth, int theTaskWeight);

	/// @brief Stop Crazy Dave's current sounds
	void CrazyDaveStopSound();

	/// @brief Is the current game TrailStage locked
	/// @return True if we are TrialLocked, false otherwise
	bool IsTrialStageLocked();

	/// @brief End the Zen Garden tutorial
	void FinishZenGardenToturial();

	/// @brief Update the player's Profile after the end of the level
	bool UpdatePlayerProfileForFinishingLevel();

	/// @brief Does a SaveFile exists for adventure
	/// @return True if we have one, false otherwise
	bool SaveFileExists();

	/// @brief Can we use PinataMode
	/// @return True if we met the requirements, false otherwise
	bool CanDoPinataMode();

	/// @brief Can we use DanceMode
	/// @return True if we met the requirements, false otherwise
	bool CanDoDanceMode();

	/// @brief Can we use DaisyMode
	/// @return True if we met the requirements, false otherwise
	bool CanDoDaisyMode();

	/// @brief Change screen settings
	/// @param wantWindowed Should the window be windowed
	/// @param is3d Should we use High Quality/Former 3D Acceleration
	/// @param force Force these changes
	virtual void SwitchScreenMode(bool wantWindowed, bool is3d, bool force = false);

	/// @brief Center a dialog on screen
	/// @param theDialog The dialog to center
	/// @param theWidth The width to center it in
	/// @param theHeight The height to center it in
	static void CenterDialog(Dialog *theDialog, int theWidth, int theHeight);
};

/// @brief Get the current level name, used for logging
/// @return Level name
SexyString LawnGetCurrentLevelName();

/// @brief Did the game request to shutdown
/// @return True if we have requested a shutdown, false otherwise
bool LawnGetCloseRequest();

/// @brief Did we use Cheat Keys
/// @return True if we used any Cheat Keys, false otherwise
bool LawnHasUsedCheatKeys();

/// @brief External function that calls the BetaSubmit
void BetaSubmitFunc();

extern bool (*gAppCloseRequest)();
extern bool (*gAppHasUsedCheatKeys)();
extern SexyString (*gGetCurrentLevelName)();

extern bool gIsPartnerBuild;
extern bool gFastMo;
extern bool gSlowMo;
extern LawnApp *gLawnApp;
extern int gSlowMoCounter;

#endif // __LAWNAPP_H__
