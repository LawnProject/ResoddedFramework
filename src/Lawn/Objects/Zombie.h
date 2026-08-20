#ifndef __ZOMBIE_H__
#define __ZOMBIE_H__

#include <Objects/GameObject.h>
#include <Common/GameConstants.h>
#include <Save/PlayerInfo.h>

#define MAX_ZOMBIE_FOLLOWERS 4
#define NUM_BOBSLED_FOLLOWERS 3
#define NUM_BACKUP_DANCERS 4
#define NUM_BOSS_BUNGEES 3

constexpr const int ZOMBIE_START_RANDOM_OFFSET = 40;
constexpr const int BUNGEE_ZOMBIE_HEIGHT = 3000;
constexpr const int RENDER_GROUP_SHIELD = 1;
constexpr const int RENDER_GROUP_ARMS = 2;
constexpr const int RENDER_GROUP_OVER_SHIELD = 3;
constexpr const int RENDER_GROUP_BOSS_BACK_LEG = 4;
constexpr const int RENDER_GROUP_BOSS_FRONT_LEG = 5;
constexpr const int RENDER_GROUP_BOSS_BACK_ARM = 6;
constexpr const int RENDER_GROUP_BOSS_FIREBALL_ADDITIVE = 7;
constexpr const int RENDER_GROUP_BOSS_FIREBALL_TOP = 8;
constexpr const int RENDER_GROUP_ZOMBATAR_COSMETICS= 9;
constexpr const int ZOMBIE_LIMP_SPEED_FACTOR = 2;
constexpr const int POGO_BOUNCE_TIME = 80;
constexpr const int DOLPHIN_JUMP_TIME = 120;
constexpr const int JackInTheBoxZombieRadius = 115;
constexpr const int JackInTheBoxPlantRadius = 90;
constexpr const int BOBSLED_CRASH_TIME = 150;
constexpr const int ZOMBIE_BACKUP_DANCER_RISE_HEIGHT = -200;
constexpr const int BOSS_FLASH_HEALTH_FRACTION = 10;
constexpr const int TICKS_BETWEEN_EATS = 4;
constexpr const int DAMAGE_PER_EAT = TICKS_BETWEEN_EATS;
constexpr const float THOWN_ZOMBIE_GRAVITY = 0.05f;
constexpr const float CHILLED_SPEED_FACTOR = 0.4f;
constexpr const float CLIP_HEIGHT_LIMIT = -100.0f;
constexpr const float CLIP_HEIGHT_OFF = -200.0f;
const Color ZOMBIE_MINDCONTROLLED_COLOR = Color(128, 64, 192, 255);

enum ZombieAttackType
{
	ATTACKTYPE_CHEW,
	ATTACKTYPE_DRIVE_OVER,
	ATTACKTYPE_VAULT,
	ATTACKTYPE_LADDER
};

enum ZombieParts
{
	PART_BODY,
	PART_HEAD,
	PART_HEAD_EATING,
	PART_TONGUE,
	PART_ARM,
	PART_HAIR,
	PART_HEAD_YUCKY,
	PART_ARM_PICKAXE,
	PART_ARM_POLEVAULT,
	PART_ARM_LEASH,
	PART_ARM_FLAG,
	PART_POGO,
	PART_DIGGER
};

class ZombieDrawPosition
{
  public:
	int mHeadX;
	int mHeadY;
	int mArmY;
	float mBodyY;
	float mImageOffsetX;
	float mImageOffsetY;
	float mClipHeight;
};

class Plant;
class Reanimation;
class TodParticleSystem;
class Zombatar;
class Zombie : public GameObject
{
  public:
	enum
	{
		ZOMBIE_WAVE_DEBUG = -1,
		ZOMBIE_WAVE_CUTSCENE = -2,
		ZOMBIE_WAVE_UI = -3,
		ZOMBIE_WAVE_WINNER = -4
	};

  public:
	ZombieType mZombieType;
	ZombiePhase mZombiePhase;
	float mPosX;
	float mPosY;
	float mVelX;
	int mAnimCounter;
	int mGroanCounter;
	int mAnimTicksPerFrame;
	int mAnimFrames;
	int mFrame;
	int mPrevFrame;
	bool mVariant;
	bool mIsEating;
	int mJustGotShotCounter;
	int mShieldJustGotShotCounter;
	int mShieldRecoilCounter;
	int mZombieAge;
	ZombieHeight mZombieHeight;
	int mPhaseCounter;
	int mFromWave;
	bool mDroppedLoot;
	int mZombieFade;
	bool mFlatTires;
	int mUseLadderCol;
	int mTargetCol;
	float mAltitude;
	bool mHitUmbrella;
	Rect mZombieRect;
	Rect mZombieAttackRect;
	int mChilledCounter;
	int mButteredCounter;
	int mIceTrapCounter;
	bool mMindControlled;
	bool mBlowingAway;
	bool mHasHead;
	bool mHasArm;
	bool mHasObject;
	bool mInPool;
	bool mOnHighGround;
	bool mYuckyFace;
	int mYuckyFaceCounter;
	HelmType mHelmType;
	int mBodyHealth;
	int mBodyMaxHealth;
	int mHelmHealth;
	int mHelmMaxHealth;
	ShieldType mShieldType;
	int mShieldHealth;
	int mShieldMaxHealth;
	int mFlyingHealth;
	int mFlyingMaxHealth;
	bool mDead;
	ZombieID mRelatedZombieID;
	ZombieID mFollowerZombieID[MAX_ZOMBIE_FOLLOWERS];
	bool mPlayingSong;
	int mParticleOffsetX;
	int mParticleOffsetY;
	AttachmentID mAttachmentID;
	int mSummonCounter;
	ReanimationID mBodyReanimID;
	float mScaleZombie;
	float mVelZ;
	float mOriginalAnimRate;
	PlantID mTargetPlantID;
	int mBossMode;
	int mTargetRow;
	int mBossBungeeCounter;	
	int mBossStompCounter;
	int mBossHeadCounter;
	ReanimationID mBossFireBallReanimID;
	ReanimationID mSpecialHeadReanimID;
	int mFireballRow;
	bool mIsFireBall;
	ReanimationID mMoweredReanimID;
	int mLastPortalX;	
	bool mHasSetupZombatar;
	Zombatar mZombatar;

	ReanimationID mBaseHeadReanimID;
	ReanimationID mTidbitsReanimID;
	ReanimationID mAccessoriesReanimID;
	ReanimationID mFacialHairReanimID;
	ReanimationID mHairReanimID;
	ReanimationID mEyewearReanimID;
	ReanimationID mHatReanimID;


  public:
	Zombie();
	~Zombie();

	/// @brief Initialize the Zombie
	/// @param theRow The Zombie Row
	/// @param theType The ZombieType
	/// @param theVariant Is a variant Zombie (Toggle visual alt)
	/// @param theParentZombie [OPTIONAL] Parent Zombie (Disco, Bobsled Leader)
	/// @param theFromWave Wave from where the Zombie was spawned
	void ZombieInitialize(int theRow, ZombieType theType, bool theVariant, Zombie *theParentZombie, int theFromWave);

	/// @brief Update the Zombie's animation
	void Animate();

	/// @brief Check if prey (Plant or Hypno Zombie) was caught
	void CheckIfPreyCaught();

	/// @brief Deal eating damage to a Zombie
	/// @param theZombie The Zombie to deal eating damage
	void EatZombie(Zombie *theZombie);

	/// @brief Deal eating damage to a Plant
	/// @param thePlant The Plant to deal eating damage
	void EatPlant(Plant *thePlant);

	/// @brief Update the Zombie
	void Update();

	/// @brief Destroy the Zombie without dropping loot, can't be used after this
	void DieNoLoot();

	/// @brief Destroy the Zombie and leave loot, can't be used after this
	void DieWithLoot();

	/// @brief Draw the Zombie
	/// @param g Graphics object
	void Draw(Graphics *g);

	/// @brief Draw the sprite-sheet Zombie (Beta 0.1.1.1014)
	/// @param g Graphics object
	/// @param theDrawPos Offsets for the limbs
	void DrawZombie(Graphics *g, const ZombieDrawPosition &theDrawPos);

	/// @brief Draw the parts for the sprite-sheet Zombie (Beta 0.1.1.1014)
	/// @param g Graphics object
	/// @param theDrawPos Offsets for the limbs
	void DrawZombieWithParts(Graphics *g, const ZombieDrawPosition &theDrawPos);

	/// @brief Draw a part for the sprite-sheet Zombie (Beta 0.1.1.1014)
	/// @param g Graphics object
	/// @param theImage The sprite-sheet
	/// @param theFrame The animation frame to use
	/// @param theRow The row to draw the part on
	/// @param theDrawPos Offsets for the limbs
	void DrawZombiePart(Graphics *g, Image *theImage, int theFrame, int theRow, const ZombieDrawPosition &theDrawPos);

	/// @brief Draw the head for the sprite-sheet Zombie (Beta 0.1.1.1014)
	/// @param g Graphics object
	/// @param theDrawPos Offsets for the limbs
	/// @param theFrame The animation frame to use
	void DrawZombieHead(Graphics *g, const ZombieDrawPosition &theDrawPos, int theFrame);

	/// @brief Get the drawing offsets for the sprite-sheet Zombie (Beta 0.1.1.1014)
	/// @param theDrawPos The Offset object to edit
	void GetDrawPos(ZombieDrawPosition &theDrawPos);

	/// @brief Draw the BungeeCord for the BungeeZombie
	/// @param g Graphics object
	/// @param theOffsetX The X offset
	/// @param theOffsetY The Y offset
	void DrawBungeeCord(Graphics *g, int theOffsetX, int theOffsetY);

	/// @brief Deal damage to the Zombie
	/// @param theDamage Damage to deal
	/// @param theDamageFlags Bitmask of damage types
	void TakeDamage(int theDamage, unsigned int theDamageFlags);

	/// @brief Change the row of the Zombie
	/// @param theRow The row to switch to
	void SetRow(int theRow);

	/// @brief Calculate the Y position of the Zombie based on the row
	/// @param theRow The row for reference
	/// @return The resulted Y position
	float GetPosYBasedOnRow(int theRow);

	/// @brief Apply the chill effect onto the Zombie
	/// @param theIsIceTrap Should the Zombie freeze in place
	void ApplyChill(bool theIsIceTrap);

	/// @brief Update the BungeeZombie
	void UpdateZombieBungee();

	/// @brief Land the BungeeZombie
	void BungeeLanding();
	
	/// @brief Is the Zombie affected by these damage flags
	/// @param theDamageFlags Bitmask of damage types
	bool EffectedByDamage(unsigned int theDamageRangeFlags);

	/// @brief Pick a random speed for the Zombie
	void PickRandomSpeed();

	/// @brief Update the PolevaulterZombie
	void UpdateZombiePolevaulter();

	/// @brief Update the DolphinRiderZombie
	void UpdateZombieDolphinRider();

	/// @brief Pick a random target for the BungeeZombie
	/// @param theColumn The column the BungeeZombie can pick from
	void PickBungeeZombieTarget(int theColumn);

	/// @brief Get the amount of Sunflowers that are targeted by BungeeZombies
	/// @return The amount of targeted Sunflowers
	int CountBungeesTargetingSunFlowers();

	/// @brief Get a Plant that gets affected by an attack
	/// @param theAttackType The attack to be checked against the Plant
	/// @return The Plant that can get affected by the attack type or nullptr if it doesn't exist
	Plant *FindPlantTarget(ZombieAttackType theAttackType);

	/// @brief Squish Plants that are affected by the attack
	/// @param theAttackType The attack to be checked against the Plant
	void CheckSquish(ZombieAttackType theAttackType);

	/// @brief Make the Zombie rise from a Grave
	/// @param theCol The column from where to rise
	/// @param theRow The row from where to rise
	void RiseFromGrave(int theCol, int theRow);

	/// @brief Update the grave rising
	void UpdateZombieRiseFromGrave();

	/// @brief Update the state based on the bitmask of damage types
	void UpdateDamageStates(unsigned int theDamageFlags);

	/// @brief Update the Pool state
	void UpdateZombiePool();

	/// @brief Check if the Zombie has entered or exited the Pool
	void CheckForPool();

	/// @brief Update the HighGround state
	void UpdateZombieHighGround();

	/// @brief Check if the Zombie has entered or exited the HighGround
	void CheckForHighGround();

	/// @brief Is the Zombie positioned on the HighGround
	/// @return True if it's on HighGround, false otherwise
	bool IsOnHighGround();

	/// @brief Drop the Zombie's loot
	void DropLoot();

	/// @brief Attempt to spawn the Level Award
	bool TrySpawnLevelAward();

	/// @brief Start making Zombie sounds
	void StartZombieSound();

	/// @brief Stop making Zombie sounds
	void StopZombieSound();

	/// @brief Update the JackInABoxZombie
	void UpdateZombieJackInTheBox();

	/// @brief Update the Zombie's position
	void UpdateZombiePosition();

	/// @brief Get the Zombie's Collision Hitbox
	/// @return World-Space Rect
	Rect GetZombieRect();

	/// @brief Get the Zombie's Attack Hitbox
	/// @return World-Space Rect
	Rect GetZombieAttackRect();

	/// @brief Update the Zombie's walking
	void UpdateZombieWalking();

	/// @brief Update the BobsledZombie
	void UpdateZombieBobsled();

	/// @brief Crash the Bobsled
	void BobsledCrash();

	/// @brief Check if it's standing on a Spikeweed
	/// @return Spikeweed or nullptr if there is none
	Plant *IsStandingOnSpikeweed();

	/// @brief Check the step the Zombie took
	void CheckForZombieStep();

	/// @brief Setup the Zombatar accessories
	void SetupZombatar();

	/// @brief Update the Zombatar accessories
	void UpdateZombatar(Zombatar &aZombatar);

	/// @brief Reset the Zombatar accessories
	void ResetZombatar();

	/// @brief Override the colors of the particles to match the Zombie's color
	/// @param aParticle The particle system to update
	void OverrideParticleColor(TodParticleSystem *aParticle);
	
	/// @brief Override the scale of the particles to match the Zombie's scale
	/// @param aParticle The particle system to update
	void OverrideParticleScale(TodParticleSystem *aParticle);

	/// @brief Override the scale of the particles to match the Zombie's scale
	/// @param aParticle The particle system to update
	void PoolSplash(bool theInToPoolSound);

	/// @brief Update the Flying Zombies
	void UpdateZombieFlyer();

	/// @brief Update the Pogo Zombie
	void UpdateZombiePogo();

	/// @brief Update the NewspaperZombie
	void UpdateZombieNewspaper();

	/// @brief Land the Flying Zombie
	/// @param theDamageFlags Bitmask of damage types that caused the landing
	void LandFlyer(unsigned int theDamageFlags);

	/// @brief Update the DiggerZombie
	void UpdateZombieDigger();

	/// @brief Is the Zombie walking backwards
	/// @return True if the Zombie is walking backwards, false otherwise
	bool IsWalkingBackwards();

	/// @brief Attach a particle system to the Zombie
	/// @param thePosX The X position relative to the Zombie
	/// @param thePosY The Y positon relative to the Zombie
	/// @param theEffect The particle effect to use
	/// @return Particle System
	TodParticleSystem *AddAttachedParticle(int thePosX, int thePosY, ParticleEffect theEffect);

	/// @brief Break the PogoZombie's Pogo
	/// @param theDamageFlags Bitmask of damage types that caused the break
	void PogoBreak(unsigned int theDamageFlags);

	/// @brief Update the Zombie's Falling State
	void UpdateZombieFalling();

	/// @brief Update the DancerZombie
	void UpdateZombieDancer();

	/// @brief Summon a Backup Dancer
	/// @param theRow to spawn the Backup Dancer in
	/// @param thePosX to spawn the Backup Dancer at
	/// @return ZombieID of the summoned Zombie or ZombieID::ZOMBIEID_NULL if failed
	ZombieID SummonBackupDancer(int theRow, int thePosX);

	/// @brief Summon the Backup Dancers
	void SummonBackupDancers();

	/// @brief Get the DancerZombie's dance animation frame
	/// @return The frame of the Dancer's animation
	int GetDancerFrame();

	/// @brief Make the BungeeZombie's steal the target
	void BungeeStealTarget();

	/// @brief Make the BungeeZombie's lift the target
	void BungeeLiftTarget();

	/// @brief Update the Zombie's Garlic re-route state
	void UpdateYuckyFace();

	/// @brief Draw the Zombie's Ice Trap
	/// @param g Graphics object
	/// @param theDrawPos Offsets for the limbs
	/// @brief theFront Should it draw top version
	void DrawIceTrap(Graphics *g, const ZombieDrawPosition &theDrawPos, bool theFront);

	/// @brief Damage the Zombie and apply an IceTrap
	void HitIceTrap();

	/// @brief Returns the damage index for the Zombie's helmet
	/// @return Integer index representing the current helmet damage state
	int GetHelmDamageIndex();

	/// @brief Returns the damage index for the Zombie's shield
	/// @return Integer index representing the current shield damage state
	int GetShieldDamageIndex();

	/// @brief Draws the Zombie's Reanimation at the specified position and render group
	/// @param g Graphics context to draw to
	/// @param theDrawPos The position data used to place the Zombie on screen
	/// @param theBaseRenderGroup The base render group layer for draw ordering
	void DrawReanim(Graphics *g, const ZombieDrawPosition &theDrawPos, int theBaseRenderGroup);

	/// @brief Updates the Zombie's logic while it is in its playing/active state
	void UpdatePlaying();

	/// @brief Checks if the dancer Zombie requires additional backup dancers to spawn
	/// @return True if more backup dancers are needed, false otherwise
	bool NeedsMoreBackupDancers();

	/// @brief Converts this Zombie into a standard normal Zombie, removing any special type behavior
	void ConvertToNormalZombie();

	/// @brief Update the DancerZombie walking every tick (Unused)
	void UpdateDancerWalking()
	{
		;
	}

	/// @brief Initiates the Zombie's eating animation and attack behavior
	void StartEating();

	/// @brief Stops the Zombie's eating animation and attack behavior
	void StopEating();

	/// @brief Updates the animation speed of the Zombie based on its current state
	void UpdateAnimSpeed();

	/// @brief Shows all Reanimation tracks that match the given prefix on the specified render group
	/// @param theTrackPrefix Prefix string used to match animation track names
	/// @param theRenderGroup Render group to assign to the matched tracks
	void ReanimShowPrefix(const char *theTrackPrefix, int theRenderGroup);

	/// @brief Plays the appropriate death animation based on the damage flags received
	/// @param theDamageFlags Bitmask of damage types that caused the Zombie's death
	void PlayDeathAnim(unsigned int theDamageFlags);

	/// @brief Updates the Zombie's death sequence each tick
	void UpdateDeath();

	/// @brief Draws the Zombie's shadow beneath it
	/// @param g Graphics context to draw to
	void DrawShadow(Graphics *g);

	/// @brief Checks whether this Zombie should display a shadow
	/// @return True if the Zombie has a shadow, false otherwise
	bool HasShadow();

	/// @brief Loads and returns the Reanimation resource for the given Reanimation type
	/// @param theReanimationType The type of Reanimation to load
	/// @return Pointer to the loaded Reanimation object
	Reanimation *LoadReanim(ReanimationType theReanimationType);

	/// @brief Applies damage to a flying Zombie
	/// @param theDamage Amount of damage to apply
	/// @param theDamageFlags Bitmask of damage type flags
	/// @return Remaining damage after applying to the flying state
	int TakeFlyingDamage(int theDamage, unsigned int theDamageFlags);

	/// @brief Applies damage to the Zombie's shield
	/// @param theDamage Amount of damage to apply
	/// @param theDamageFlags Bitmask of damage type flags
	/// @return Remaining damage after the shield absorbs what it can
	int TakeShieldDamage(int theDamage, unsigned int theDamageFlags);

	/// @brief Applies damage to the Zombie's helmet
	/// @param theDamage Amount of damage to apply
	/// @param theDamageFlags Bitmask of damage type flags
	/// @return Remaining damage after the helmet absorbs what it can
	int TakeHelmDamage(int theDamage, unsigned int theDamageFlags);

	/// @brief Applies damage directly to the Zombie's body
	/// @param theDamage Amount of damage to apply
	/// @param theDamageFlags Bitmask of damage type flags
	void TakeBodyDamage(int theDamage, unsigned int theDamageFlags);

	/// @brief Attaches a shield to this Zombie and sets up the associated Reanimation
	void AttachShield();

	/// @brief Detaches and removes the Zombie's shield
	void DetachShield();

	/// @brief Updates the Zombie's Reanimation state each tick
	void UpdateReanim();

	/// @brief Retrieves the world position of a named Reanimation track
	/// @param theTrackName Name of the track to look up
	/// @param thePosX Output parameter receiving the track's X position
	/// @param thePosY Output parameter receiving the track's Y position
	void GetTrackPosition(const char *theTrackName, float &thePosX, float &thePosY);

	/// @brief Loads the default plain Zombie Reanimation data
	void LoadPlainZombieReanim();

	/// @brief Shows or hides the door-holding arm tracks on this Zombie
	/// @param theShow True to show the door arms, false to hide them
	void ShowDoorArms(bool theShow);

	/// @brief Shows a specific Reanimation track on the given render group
	/// @param theTrackName Name of the track to show
	/// @param theRenderGroup Render group to assign to the track
	void ReanimShowTrack(const char *theTrackName, int theRenderGroup);

	/// @brief Plays the appropriate sound effect for this Zombie appearing on the board
	void PlayZombieAppearSound();

	/// @brief Puts the Zombie into a mind-controlled state, switching its allegiance
	void StartMindControlled();

	/// @brief Checks whether this Zombie is currently airborne
	/// @return True if the Zombie is flying, false otherwise
	bool IsFlying();

	/// @brief Detaches and drops the Zombie's head based on the damage type received
	/// @param theDamageFlags Bitmask of damage type flags that caused the head loss
	void DropHead(unsigned int theDamageFlags);

	/// @brief Determines whether this Zombie can target a specific plant with a given attack type
	/// @param thePlant Pointer to the plant being evaluated as a target
	/// @param theAttackType The type of attack being considered
	/// @return True if the Zombie can target the plant, false otherwise
	bool CanTargetPlant(Plant *thePlant, ZombieAttackType theAttackType);

	/// @brief Updates the catapult Zombie's logic each tick
	void UpdateZombieCatapult();

	/// @brief Finds and returns the best plant target for the catapult Zombie to launch at
	/// @return Pointer to the targeted Plant, or nullptr if no valid target exists
	Plant *FindCatapultTarget();

	/// @brief Fires the catapult Zombie's projectile at the specified plant
	/// @param thePlant Pointer to the target plant to fire at
	void ZombieCatapultFire(Plant *thePlant);

	/// @brief Updates the ladder-climbing Zombie's logic each tick
	void UpdateClimbingLadder();

	/// @brief Updates the Gargantuar Zombie's logic each tick
	void UpdateZombieGargantuar();

	/// @brief Returns the damage index for the Zombie's body
	/// @return Integer index representing the current body damage state
	int GetBodyDamageIndex();

	/// @brief Applies a burn effect to this Zombie
	void ApplyBurn();

	/// @brief Updates the burn effect on the Zombie each tick
	void UpdateBurn();

	/// @brief Checks whether the Zombie is currently not in a walking state
	/// @return True if the Zombie is not walking, false otherwise
	bool ZombieNotWalking();

	/// @brief Searches for and returns a valid Zombie to target
	/// @return Pointer to the targeted Zombie, or nullptr if none found
	Zombie *FindZombieTarget();

	/// @brief Plays a named Reanimation track on this Zombie with the specified parameters
	/// @param theTrackName Name of the animation track to play
	/// @param theLoopType Whether the animation should loop or play once
	/// @param theBlendTime Duration in ticks to blend from the current animation
	/// @param theAnimRate Playback rate multiplier for the animation
	void PlayZombieReanim(const char *theTrackName, ReanimLoopType theLoopType, int theBlendTime, float theAnimRate);

	/// @brief Updates the backup dancer Zombie's logic each tick
	void UpdateZombieBackupDancer();

	/// @brief Returns the current dance phase of the dancer Zombie
	/// @return The ZombiePhase enum value representing the dancer's current phase
	ZombiePhase GetDancerPhase();

	/// @brief Checks whether the Zombie is currently moving at the reduced chilled speed
	/// @return True if moving at chilled speed, false otherwise
	bool IsMovingAtChilledSpeed();

	/// @brief Transitions the Zombie into its walk animation
	/// @param theBlendTime Duration in ticks to blend from the current animation
	void StartWalkAnim(int theBlendTime);

	/// @brief Creates and attaches a secondary Reanimation to this Zombie at the given offset
	/// @param thePosX X offset for the attached Reanimation
	/// @param thePosY Y offset for the attached Reanimation
	/// @param theReanimType Type of Reanimation to attach
	/// @return Pointer to the newly created attached Reanimation
	Reanimation *AddAttachedReanim(int thePosX, int thePosY, ReanimationType theReanimType);

	/// @brief Pulls the Zombie under the water (used by Tangle Kelp and similar)
	void DragUnder();

	/// @brief Configures the door arm track visibility on a Reanimation object
	/// @param aReanim Pointer to the Reanimation to configure
	/// @param theShow True to show door arms, false to hide them
	static void SetupDoorArms(Reanimation *aReanim, bool theShow);

	/// @brief Sets up render layer ordering for a Reanimation based on the ZombieType
	/// @param aReanim Pointer to the Reanimation to configure
	/// @param theZombieType The ZombieType used to determine layer setup
	static void SetupReanimLayers(Reanimation *aReanim, ZombieType theZombieType);

	/// @brief Checks whether the Zombie is currently on the game board
	/// @return True if the Zombie is on the board, false otherwise
	bool IsOnBoard();

	/// @brief Draws the butter immobilization effect on the Zombie
	/// @param g Graphics context to draw to
	/// @param theDrawPos The position data used to place the effect on screen
	void DrawButter(Graphics *g, const ZombieDrawPosition &theDrawPos);

	/// @brief Checks whether the Zombie is currently immobilized (eg by butter or ice)
	/// @return True if the Zombie cannot move, false otherwise
	bool IsImmobilizied();

	/// @brief Applies the butter effect to this Zombie, immobilizing it
	void ApplyButter();

	/// @brief Calculates the predicted X position of a Zombie target after a given travel time
	/// @param theTime Time in seconds to project the Zombie's position forward
	/// @return Predicted X world coordinate of the Zombie target
	float ZombieTargetLeadX(float theTime);

	/// @brief Updates the Imp Zombie's logic each tick
	void UpdateZombieImp();

	/// @brief Applies a squish kill to all zombies and plants within the specified grid square
	/// @param theX Grid X coordinate of the square to squish
	/// @param theY Grid Y coordinate of the square to squish
	/// @param theAttackType The attack type to attribute to the squish damage
	void SquishAllInSquare(int theX, int theY, ZombieAttackType theAttackType);

	/// @brief Removes any ice trap placed on or near this Zombie
	void RemoveIceTrap();

	/// @brief Checks whether the Zombie is currently bouncing on a pogo stick
	/// @return True if the Zombie is bouncing, false otherwise
	bool IsBouncingPogo();

	/// @brief Returns the bobsled team position index of this Zombie
	/// @return Integer representing the Zombie's position in the bobsled formation
	int GetBobsledPosition();

	/// @brief Draws the bobsled Reanimation relative to the Zombie, either before or after the Zombie sprite
	/// @param g Graphics context to draw to
	/// @param theDrawPos The position data used to place the bobsled on screen
	/// @param theBeforeZombie If true, draws the bobsled behind the Zombie; otherwise draws it in front
	void DrawBobsledReanim(Graphics *g, const ZombieDrawPosition &theDrawPos, bool theBeforeZombie);

	/// @brief Handles the bobsled Zombie's death, detaching from the sled
	void BobsledDie();

	/// @brief Handles the bobsled catching fire and destroying the sled
	void BobsledBurn();

	/// @brief Checks whether this bobsled Zombie is part of a team that still has its sled
	/// @return True if the sled is present, false otherwise
	bool IsBobsledTeamWithSled();

	/// @brief Checks whether this Zombie can be frozen
	/// @return True if the Zombie is susceptible to freezing, false otherwise
	bool CanBeFrozen();

	/// @brief Checks whether this Zombie can be chilled (slowed)
	/// @return True if the Zombie can be chilled, false otherwise
	bool CanBeChilled();

	/// @brief Updates the snorkel Zombie's logic each tick
	void UpdateZombieSnorkel();

	/// @brief Sets whether a specific Reanimation track should ignore the clip rectangle
	/// @param theTrackName Name of the track to configure
	/// @param theIgnoreClipRect True to disable clipping on the track, false to re-enable it
	void ReanimIgnoreClipRect(const char *theTrackName, bool theIgnoreClipRect);

	/// @brief Sets the playback rate for this Zombie's Reanimation
	/// @param theAnimRate Multiplier for the animation speed
	void SetAnimRate(float theAnimRate);

	/// @brief Applies a given animation rate to the Zombie's active Reanimation
	/// @param theAnimRate Multiplier to apply to the current animation speed
	void ApplyAnimRate(float theAnimRate);

	/// @brief Checks whether the Zombie is dead or currently in its dying sequence
	/// @return True if dead or dying, false otherwise
	bool IsDeadOrDying();

	/// @brief Draws the dancer Zombie's Reanimation at the specified position
	/// @param g Graphics context to draw to
	/// @param theDrawPos The position data used to place the Zombie on screen
	void DrawDancerReanim(Graphics *g, const ZombieDrawPosition &theDrawPos);

	/// @brief Draws the bungee Zombie's Reanimation at the specified position
	/// @param g Graphics context to draw to
	/// @param theDrawPos The position data used to place the Zombie on screen
	void DrawBungeeReanim(Graphics *g, const ZombieDrawPosition &theDrawPos);

	/// @brief Draws the target indicator for the bungee Zombie's drop zone
	/// @param g Graphics context to draw to
	void DrawBungeeTarget(Graphics *g);

	/// @brief Handles the bungee Zombie's death and retraction sequence
	void BungeeDie();

	/// @brief Handles the Zamboni Zombie's death, applying damage flags for its destruction
	/// @param theDamageFlags Bitmask of damage types that caused the death
	void ZamboniDeath(unsigned int theDamageFlags);

	/// @brief Handles the catapult Zombie's death sequence
	/// @param theDamageFlags Bitmask of damage types that caused the death
	void CatapultDeath(unsigned int theDamageFlags);

	/// @brief Sets up and validates the draw state for the Zombie-won sequence
	/// @param g Graphics context used for rendering
	/// @return True if the won draw state was set up successfully, false otherwise
	bool SetupDrawZombieWon(Graphics *g);

	/// @brief Triggers the Zombie to walk into the house, ending the game
	void WalkIntoHouse();

	/// @brief Updates the Zamboni Zombie's movement and crushing logic each tick
	void UpdateZamboni();

	/// @brief Updates the chimney Zombie's logic each tick
	void UpdateZombieChimney();

	/// @brief Updates the ladder Zombie's placement logic each tick
	void UpdateLadder();

	/// @brief Detaches and drops the Zombie's arm based on the damage received
	/// @param theDamageFlags Bitmask of damage type flags that caused the arm loss
	void DropArm(unsigned int theDamageFlags);

	/// @brief Checks whether this ZombieType is able to lose body parts
	/// @return True if body parts can be dropped, false otherwise
	bool CanLoseBodyParts();

	/// @brief Detaches and drops the Zombie's helmet
	/// @param theDamageFlags Bitmask of damage type flags that caused the helm loss
	void DropHelm(unsigned int theDamageFlags);

	/// @brief Detaches and drops the Zombie's shield
	/// @param theDamageFlags Bitmask of damage type flags that caused the shield loss
	void DropShield(unsigned int theDamageFlags);

	/// @brief Re-enables clipping on all Reanimation tracks that had it disabled
	void ReanimReenableClipping();

	/// @brief Updates the Dr Zomboss boss Zombie's logic each tick
	void UpdateBoss();

	/// @brief Plays the boss Zombie's idle animation
	void BossPlayIdle();

	/// @brief Handles the boss's RV landing sequence when it touches down
	void BossRVLanding();

	/// @brief Handles the collision contact of the boss's stomp attack
	void BossStompContact();

	/// @brief Checks whether all bungee zombies used by the boss have completed their sequences
	/// @return True if all bungees are done, false otherwise
	bool BossAreBungeesDone();

	/// @brief Spawns bungee zombies as part of a boss attack sequence
	void BossBungeeSpawn();

	/// @brief Triggers the boss's Zombie spawn attack
	void BossSpawnAttack();

	/// @brief Triggers the boss's bungee drop attack
	void BossBungeeAttack();

	/// @brief Triggers the boss's RV vehicle attack
	void BossRVAttack();

	/// @brief Handles contact events for zombies spawned by the boss
	void BossSpawnContact();

	/// @brief Triggers the bungee retraction sequence after a boss bungee attack
	void BossBungeeLeave();

	/// @brief Triggers the boss's stomp attack on the board
	void BossStompAttack();

	/// @brief Checks whether the boss can execute a stomp attack on the given row
	/// @param theRow Row index to evaluate for stomping
	/// @return True if the boss can stomp that row, false otherwise
	bool BossCanStompRow(int theRow);

	/// @brief Handles the boss Zombie's death sequence
	void BossDie();

	/// @brief Triggers the boss's head projectile attack
	void BossHeadAttack();

	/// @brief Handles the contact event when the boss's head spit projectile hits
	void BossHeadSpitContact();

	/// @brief Fires a spit projectile from the boss's head
	void BossHeadSpit();

	/// @brief Updates the boss's active fireball projectile each tick
	void UpdateBossFireball();

	/// @brief Destroys the boss's active fireball projectile
	void BossDestroyFireball();

	/// @brief Destroys any iceball projectile the boss has active in the specified row
	/// @param theRow Row index in which to destroy the iceball
	void BossDestroyIceballInRow(int theRow);

	/// @brief Removes the digger Zombie's axe, transitioning it to a normal state
	void DiggerLoseAxe();

	/// @brief Drops a Zombie from a bungee onto the board at the specified grid position
	/// @param theDroppedZombie Pointer to the Zombie being dropped
	/// @param theGridX Target grid X coordinate for the drop
	/// @param theGridY Target grid Y coordinate for the drop
	void BungeeDropZombie(Zombie *theDroppedZombie, int theGridX, int theGridY);

	/// @brief Shows or hides the yucky face reaction on the Zombie
	/// @param theShow True to show the yucky face, false to hide it
	void ShowYuckyFace(bool theShow);

	/// @brief Plays the chewing sound effect synced to the Zombie's eat animation
	void AnimateChewSound();

	/// @brief Triggers the chewing particle/visual effect synced to the Zombie's eat animation
	void AnimateChewEffect();

	/// @brief Processes any queued actions for the Zombie this tick
	void UpdateActions();

	/// @brief Checks whether the Zombie has reached the edge of the board and handles the event
	void CheckForBoardEdge();

	/// @brief Updates the Yeti Zombie's special logic each tick
	void UpdateYeti();

	/// @brief Draws a specific part of the boss Zombie
	/// @param g Graphics context to draw to
	/// @param theBossPart The specific boss body part to draw
	void DrawBossPart(Graphics *g, BossPart theBossPart);

	/// @brief Initializes and configures the boss Zombie's Reanimation tracks and layers
	void BossSetupReanim();

	/// @brief Runs over and kills plants/zombies in the Zombie's path (used by lawnmower-type zombies)
	void MowDown();

	/// @brief Updates the logic for a Zombie that has been run over by a lawnmower
	void UpdateMowered();

	/// @brief Drops the Zombie's flag prop onto the board
	void DropFlag();

	/// @brief Drops the Zombie's pole prop onto the board
	void DropPole();

	/// @brief Draws the boss Zombie's back arm Reanimation
	/// @param g Graphics context to draw to
	/// @param theDrawPos The position data used to place the arm on screen
	void DrawBossBackArm(Graphics *g, const ZombieDrawPosition &theDrawPos);

	/// @brief Preloads all graphical and audio resources for the specified ZombieType
	/// @param theZombieType The ZombieType whose resources should be preloaded
	static void PreloadZombieResources(ZombieType theZombieType);

	/// @brief Initiates the boss Zombie's death sequence
	void BossStartDeath();

	/// @brief Removes all cold/chill/freeze effects currently applied to this Zombie
	void RemoveColdEffects();

	/// @brief Triggers the particle and visual effect for the boss's head spit attack
	void BossHeadSpitEffect();

	/// @brief Draws the boss's fireball projectile at the specified position
	/// @param g Graphics context to draw to
	/// @param theDrawPos The position data used to place the fireball on screen
	void DrawBossFireBall(Graphics *g, const ZombieDrawPosition &theDrawPos);

	/// @brief Updates the PeaHead Zombotany Zombie's logic each tick
	void UpdateZombiePeaHead();

	/// @brief Updates the JalapenoHead Zombotany Zombie's logic each tick
	void UpdateZombieJalapenoHead();

	/// @brief Enables or disables the smoke particle effect on the boss Zombie
	/// @param theEnable True to enable smoke particles, false to disable them
	void ApplyBossSmokeParticles(bool theEnable);

	/// @brief Updates the Zombiquarium Zombie's logic each tick
	void UpdateZombiquarium();

	/// @brief Searches for and locks onto the closest brain target for the Zombiquarium Zombie
	/// @return True if a valid brain target was found, false otherwise
	bool ZombiquariumFindClosestBrain();

	/// @brief Updates the gatling-head Zombotany Zombie's logic each tick
	void UpdateZombieGatlingHead();

	/// @brief Updates the squash-head Zombotany Zombie's logic each tick
	void UpdateZombieSquashHead();

	/// @brief Checks whether this Zombie is a valid target for Tangle Kelp
	/// @return True if the Zombie can be targeted by Tangle Kelp, false otherwise
	bool IsTanglekelpTarget();

	/// @brief Checks whether this Zombie has a custom yucky face image defined
	/// @return True if a yucky face image exists for this Zombie, false otherwise
	bool HasYuckyFaceImage();

	/// @brief Checks whether this Zombie is a valid target for Tangle Kelp
	/// @return True if the Zombie can be targeted by Tangle Kelp, false otherwise
	bool IsTangleKelpTarget();

	/// @brief Checks whether this Zombie is immune to fire-based attacks
	/// @return True if the Zombie resists fire damage, false otherwise
	bool IsFireResistant();

	/// @brief Enables or disables the mustache accessory on this Zombie
	/// @param theEnableMustache True to show the mustache, false to hide it
	void EnableMustache(bool theEnableMustache);

	/// @brief Enables or disables the future-themed visual variant for this Zombie
	/// @param theEnableFuture True to enable future styling, false to disable it
	void EnableFuture(bool theEnableFuture);

	/// @brief Enables or disables the dancing animation mode for this Zombie
	/// @param theEnableDance True to enable dancing, false to disable it
	void EnableDance(bool theEnableDance);

	/// @brief Commands a bungee Zombie to drop a plant onto the board
	void BungeeDropPlant();

	/// @brief Removes the butter immobilization effect from this Zombie
	void RemoveButter();

	/// @brief Starts or stops the propeller hat spin animation on a balloon Zombie
	/// @param theSpinning True to start spinning, false to stop
	void BalloonPropellerHatSpin(bool theSpinning);

	/// @brief Spawns daisy flowers at the Zombie's position upon death
	void DoDaisies();

	/// @brief Checks whether the given ZombieType is capable of traversing high ground
	/// @param theZombieType The ZombieType to check
	/// @return True if the type can go on high ground, false otherwise
	static bool ZombieTypeCanGoOnHighGround(ZombieType theZombieType);

	/// @brief Checks whether the given ZombieType can enter pool lanes
	/// @param theZombieType The ZombieType to check
	/// @return True if the type can go in the pool, false otherwise
	static bool ZombieTypeCanGoInPool(ZombieType theZombieType);

	/// @brief Configures the water surface animation track for this Zombie
	/// @param theTrackName Name of the track to set up as a water track
	void SetupWaterTrack(const char *theTrackName);

	/// @brief Applies burn damage and effects to all plants and zombies in the specified row
	/// @param theRow Row index to burn
	void BurnRow(int theRow);

	/// @brief Reconfigures the Zombie's Reanimation to reflect a lost head state
	void SetupReanimForLostHead();

	/// @brief Reconfigures the Zombie's Reanimation to reflect a lost arm state
	/// @param theDamageFlags Bitmask of damage type flags that caused the arm loss
	void SetupReanimForLostArm(unsigned int theDamageFlags);

	/// @brief Checks whether the Zombie is a valid squash target, optionally ignoring a specific plant
	/// @param theExcept Plant pointer to exclude from the targeting check, or nullptr
	/// @return True if the Zombie is a valid squash target, false otherwise
	bool IsSquashTarget(Plant *theExcept);

	/// @brief Checks whether the given ZombieType is a Zombotany (plant-Zombie hybrid) type
	/// @param theZombieType The ZombieType to check
	/// @return True if the type is a Zombotany variant, false otherwise
	static bool IsZombotany(ZombieType theZombieType);
};

class ZombieDefinition
{
  public:
	ZombieType mZombieType;
	ReanimationType mReanimationType;
	int mZombieValue;
	int mStartingLevel;
	int mFirstAllowedWave;
	int mPickWeight;
	const SexyCharByte *mZombieName;
};
extern ZombieDefinition gZombieDefs[NUM_ZOMBIE_TYPES];

ZombieDefinition &GetZombieDefinition(ZombieType theZombieType);

#endif