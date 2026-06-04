#pragma once

#include <string>
#include "GameObject.h"

#define MAX_MAGNET_ITEMS 5

enum PlantSubClass
{
	SUBCLASS_NORMAL = 0,
	SUBCLASS_SHOOTER = 1
};

enum PlantWeapon
{
	WEAPON_PRIMARY,
	WEAPON_SECONDARY
};

enum PlantOnBungeeState
{
	NOT_ON_BUNGEE,
	GETTING_GRABBED_BY_BUNGEE,
	RISING_WITH_BUNGEE
};

enum PlantState
{
	STATE_NOTREADY,
	STATE_READY,
	STATE_DOINGSPECIAL,
	STATE_SQUASH_LOOK,
	STATE_SQUASH_PRE_LAUNCH,
	STATE_SQUASH_RISING,
	STATE_SQUASH_FALLING,
	STATE_SQUASH_DONE_FALLING,
	STATE_GRAVEBUSTER_LANDING,
	STATE_GRAVEBUSTER_EATING,
	STATE_CHOMPER_BITING,
	STATE_CHOMPER_BITING_GOT_ONE,
	STATE_CHOMPER_BITING_MISSED,
	STATE_CHOMPER_DIGESTING,
	STATE_CHOMPER_SWALLOWING,
	STATE_POTATO_RISING,
	STATE_POTATO_ARMED,
	STATE_POTATO_MASHED,
	STATE_SPIKEWEED_ATTACKING,
	STATE_SPIKEWEED_ATTACKING_2,
	STATE_SCAREDYSHROOM_LOWERING,
	STATE_SCAREDYSHROOM_SCARED,
	STATE_SCAREDYSHROOM_RAISING,
	STATE_SUNSHROOM_SMALL,
	STATE_SUNSHROOM_GROWING,
	STATE_SUNSHROOM_BIG,
	STATE_MAGNETSHROOM_SUCKING,
	STATE_MAGNETSHROOM_CHARGING,
	STATE_BOWLING_UP,
	STATE_BOWLING_DOWN,
	STATE_CACTUS_LOW,
	STATE_CACTUS_RISING,
	STATE_CACTUS_HIGH,
	STATE_CACTUS_LOWERING,
	STATE_TANGLEKELP_GRABBING,
	STATE_COBCANNON_ARMING,
	STATE_COBCANNON_LOADING,
	STATE_COBCANNON_READY,
	STATE_COBCANNON_FIRING,
	STATE_KERNELPULT_BUTTER,
	STATE_UMBRELLA_TRIGGERED,
	STATE_UMBRELLA_REFLECTING,
	STATE_IMITATER_MORPHING,
	STATE_ZEN_GARDEN_WATERED,
	STATE_ZEN_GARDEN_NEEDY,
	STATE_ZEN_GARDEN_HAPPY,
	STATE_MARIGOLD_ENDING,
	STATE_FLOWERPOT_INVULNERABLE,
	STATE_LILYPAD_INVULNERABLE
};

enum PLANT_LAYER
{
	PLANT_LAYER_BELOW = -1,
	PLANT_LAYER_MAIN,
	PLANT_LAYER_REANIM,
	PLANT_LAYER_REANIM_HEAD,
	PLANT_LAYER_REANIM_BLINK,
	PLANT_LAYER_ON_TOP,
	NUM_PLANT_LAYERS
};

enum PLANT_ORDER
{
	PLANT_ORDER_LILYPAD,
	PLANT_ORDER_NORMAL,
	PLANT_ORDER_PUMPKIN,
	PLANT_ORDER_FLYER,
	PLANT_ORDER_CHERRYBOMB
};

enum MagnetItemType
{
	MAGNET_ITEM_NONE,
	MAGNET_ITEM_PAIL_1,
	MAGNET_ITEM_PAIL_2,
	MAGNET_ITEM_PAIL_3,
	MAGNET_ITEM_FOOTBALL_HELMET_1,
	MAGNET_ITEM_FOOTBALL_HELMET_2,
	MAGNET_ITEM_FOOTBALL_HELMET_3,
	MAGNET_ITEM_DOOR_1,
	MAGNET_ITEM_DOOR_2,
	MAGNET_ITEM_DOOR_3,
	//MAGNET_ITEM_PROPELLER,
	MAGNET_ITEM_POGO_1,
	MAGNET_ITEM_POGO_2,
	MAGNET_ITEM_POGO_3,
	MAGNET_ITEM_JACK_IN_THE_BOX,
	MAGNET_ITEM_LADDER_1,
	MAGNET_ITEM_LADDER_2,
	MAGNET_ITEM_LADDER_3,
	MAGNET_ITEM_LADDER_PLACED,
	MAGNET_ITEM_SILVER_COIN,
	MAGNET_ITEM_GOLD_COIN,
	MAGNET_ITEM_DIAMOND,
	MAGNET_ITEM_PICK_AXE
};

class MagnetItem
{
  public:
	float mPosX;
	float mPosY;
	float mDestOffsetX;
	float mDestOffsetY;
	MagnetItemType mItemType;
};

class Coin;
class Zombie;
class Reanimation;
class TodParticleSystem;

class Plant : public GameObject
{
  public:
	SeedType mSeedType;
	int mPlantCol;
	int mAnimCounter;
	int mFrame;
	int mFrameLength;
	int mNumFrames;
	PlantState mState;
	int mPlantHealth;
	int mPlantMaxHealth;
	PlantSubClass mSubclass;
	int mDisappearCountdown;
	int mDoSpecialCountdown;
	int mStateCountdown;
	int mLaunchCounter;
	int mLaunchRate;
	Rect mPlantRect;
	Rect mPlantAttackRect;
	int mTargetX;
	int mTargetY;
	int mStartRow;
	ParticleSystemID mParticleID;
	int mShootingCounter;
	ReanimationID mBodyReanimID;
	ReanimationID mHeadReanimID;
	ReanimationID mHeadReanimID2;
	ReanimationID mHeadReanimID3;
	ReanimationID mBlinkReanimID;
	ReanimationID mLightReanimID;
	ReanimationID mSleepingReanimID;
	int mBlinkCountdown;
	int mRecentlyEatenCountdown;
	int mEatenFlashCountdown;
	int mBeghouledFlashCountdown;
	float mShakeOffsetX;
	float mShakeOffsetY;
	MagnetItem mMagnetItems[MAX_MAGNET_ITEMS];
	ZombieID mTargetZombieID;
	int mWakeUpCounter;
	PlantOnBungeeState mOnBungeeState;
	SeedType mImitaterType;
	int mPottedPlantIndex;
	bool mAnimPing;
	bool mDead;
	bool mSquished;
	bool mIsAsleep;
	bool mIsOnBoard;
	bool mHighlighted;
	int mBowlingHits;

  public:
	Plant();

	/// @brief Initialize the Plant
	/// @param theGridX The Grid X Coordinate
	/// @param theGridY The Grid Y Coordinate
	/// @param theSeedType The SeedType of the Plant
	/// @param theImitaterType The actual SeedType if theSeedType == SeedType::SEED_IMITATER
	void PlantInitialize(int theGridX, int theGridY, SeedType theSeedType, SeedType theImitaterType);
	/// @brief Update the Plant
	void Update();
	/// @brief Animate the Plant
	void Animate();
	/// @brief Draw the Plant
	/// @param g Graphics object
	void Draw(Graphics *g);
	/// @brief Handle a mouse press on a Plant
	/// @param x The World-Space X
	/// @param y The World-Space Y
	/// @param theClickCount Button state flags
	void MouseDown(int x, int y, int theClickCount);
	/// @brief Trigger the Plant's Special
	void DoSpecial();
	/// @brief Fire a shot at a Zombie
	/// @param theTargetZombie [OPTIONAL] Zombie to aim at
	/// @param theRow The row to shoot in
	/// @param thePlantWeapon [OPTIONAL] The weapon to use (see PlantWeapon enum)
	void Fire(Zombie *theTargetZombie, int theRow, PlantWeapon thePlantWeapon = PlantWeapon::WEAPON_PRIMARY);
	/// @brief Try to find a zombie to aim at
	/// @param theRow The row to shoot in
	/// @param thePlantWeapon [OPTIONAL] The weapon to use (see PlantWeapon enum)
	/// @return Zombie to it will aim at, can be nullptr
	Zombie *FindTargetZombie(int theRow, PlantWeapon thePlantWeapon = PlantWeapon::WEAPON_PRIMARY);
	/// @brief Kill the Plant, can't be used anymore after this
	void Die();
	/// @brief Update production Plants
	void UpdateProductionPlant();
	/// @brief Update shooter Plants
	void UpdateShooter();
	/// @brief Try to find a zombie to aim at and shoot him
	/// @param theRow The row to shoot in
	/// @param thePlantWeapon [OPTIONAL] The weapon to use (see PlantWeapon enum)
	/// @return True if target was found
	bool FindTargetAndFire(int theRow, PlantWeapon thePlantWeapon = PlantWeapon::WEAPON_PRIMARY);
	/// @brief Launch a Treepeater shot
	void LaunchThreepeater();
	/// @brief Get the Plant's Spritesheet
	/// @param theSeedType The Plant's SeedType
	/// @return Image or nullptr if it doesn't exist
	static Image *GetImage(SeedType theSeedType);
	/// @brief Get the Plant's Sun Cost
	/// @param theSeedType The Plant's SeedType
	/// @param theImitaterType [OPTIONAL] The actual SeedType if theSeedType == SeedType::SEED_IMITATER
	/// @return The sun cost
	static int GetCost(SeedType theSeedType, SeedType theImitaterType = SeedType::SEED_NONE);
	/// @brief Get the Plant's Name
	/// @param theSeedType The Plant's SeedType
	/// @param theImitaterType [OPTIONAL] The actual SeedType if theSeedType == SeedType::SEED_IMITATER
	/// @return The pre-translated name
	static SexyString GetNameString(SeedType theSeedType, SeedType theImitaterType = SeedType::SEED_NONE);
	/// @brief Get the Plant's Tooltip Label
	/// @param theSeedType The Plant's SeedType
	/// @param theImitaterType [OPTIONAL] The actual SeedType if theSeedType == SeedType::SEED_IMITATER
	/// @return The pre-translated tooltip label
	static SexyString GetToolTip(SeedType theSeedType);
	/// @brief Get the Plant's Refresh Timer
	/// @param theSeedType The Plant's SeedType
	/// @param theImitaterType [OPTIONAL] The actual SeedType if theSeedType == SeedType::SEED_IMITATER
	/// @return The pre-translated tooltip label
	static int GetRefreshTime(SeedType theSeedType, SeedType theImitaterType = SeedType::SEED_NONE);
	/// @brief Is the Plant Nocturnal
	/// @param theSeedType The Plant's SeedType
	/// @return Is the Plant Noctural
	static bool IsNocturnal(SeedType theSeedtype);
	/// @brief Is the Plant Aquatic
	/// @param theSeedType The Plant's SeedType
	/// @return Is the Plant Aquatic
	static bool IsAquatic(SeedType theSeedType);
	/// @brief Is the Plant Flying
	/// @param theSeedType The Plant's SeedType
	/// @return Is the Plant Flying
	static bool IsFlying(SeedType theSeedtype);
	/// @brief Is the Plant Upgrade
	/// @param theSeedType The Plant's SeedType
	/// @return Is the Plant Upgrade
	static bool IsUpgrade(SeedType theSeedtype);
	/// @brief Update a Plant's abilities
	void UpdateAbilities();
	/// @brief Squish a Plant
	void Squish();
	/// @brief Do area damage in the Plant's Attack Rect
	/// @param theDamage The damage amount
	/// @param theDamageFlags The damagee flags to filter Zombies
	void DoRowAreaDamage(int theDamage, unsigned int theDamageFlags);
	/// @brief Get the Plant's Damage Ranges
	/// /// @param thePlantWeapon [OPTIONAL] The weapon to use (see PlantWeapon enum)
	/// @return Damage Range Flags (see DamageRangeFlags enum)
	int GetDamageRangeFlags(PlantWeapon thePlantWeapon = PlantWeapon::WEAPON_PRIMARY);
	/// @brief Get the Plant's Collision Rect
	/// @return Attack Rect in World Space
	Rect GetPlantRect();
	/// @brief Get the Plant's Attack Rect
	/// /// @param thePlantWeapon [OPTIONAL] The weapon to use (see PlantWeapon enum)
	/// @return Attack Rect in World Space
	Rect GetPlantAttackRect(PlantWeapon thePlantWeapon = PlantWeapon::WEAPON_PRIMARY);
	/// @brief Try to find a Zombie for a Squash to target
	/// @return Target Zombie, can be nullptr
	Zombie *FindSquashTarget();
	/// @brief Update the Squash
	void UpdateSquash();
	/// @brief Is the Plant's state not on the Ground?
	/// @return True if it's not on the ground
	bool NotOnGround();
	/// @brief Do Squash damage to all Zombies in the landing spot
	void DoSquashDamage();
	/// @brief Burn the row
	/// @param theRow The row to burn
	void BurnRow(int theRow);
	/// @brief Freeze all the Zombies on screen
	void IceZombies();
	/// @brief Blow away all flying Zombies
	/// @param theX The minimum X positions of the flying Zombies
	/// @param theRow The row to blow the flying Zombies away
	void BlowAwayFliers(int theX, int theRow);
	/// @brief Update the GraveBuster
	void UpdateGraveBuster();
	/// @brief Attach a particle system to a plant
	/// @param thePosX The X position of the particle system
	/// @param thePosY The Y position of the particle system
	/// @param theRenderPosition The render order of the particle system
	/// @param theEffect The particle effect type (see ParticleEffect)
	/// @return Particle System
	TodParticleSystem *AddAttachedParticle(int thePosX, int thePosY, int theRenderPosition, ParticleEffect theEffect);
	/// @brief Offset the coordinates by the positions of the Peashooter's (and others) Head
	/// @param theOffsetX Reference to the X position 
	/// @param theOffsetY Reference to the Y position
	void GetPeaHeadOffset(int &theOffsetX, int &theOffsetY);
	/// @brief Can the Plant make sun
	/// @return True if the Plant is a sun producer
	bool MakesSun();
	/// @brief Draw the SeedPacket preview
	/// @param g Graphics object
	/// @param theSeedType The SeedType to draw
	/// @param theDrawVariation The DrawVariation (see DrawVariation enum)
	/// @param thePosX The X position
	/// @param thePosY The Y position
	static void DrawSeedType(Graphics *g,
							 SeedType theSeedType,
							 SeedType theImitaterType,
							 DrawVariation theDrawVariation,
							 float thePosX,
							 float thePosY);
	/// @brief Kill all Plants on the same grid as the DoomShroom explosion
	void KillAllPlantsNearDoom();
	/// @brief Is the Plant on the stage's high-ground?
	/// @return True if it's on the high-ground
	bool IsOnHighGround();
	/// @brief Update the Torchwood
	void UpdateTorchwood();
	/// @brief Update the StartFruit
	void LaunchStarFruit();
	bool FindStarFruitTarget();
	/// @brief Update the Chomper
	void UpdateChomper();
	/// @brief Make the Plant Blink
	void DoBlink();
	/// @brief Update blinking animation
	void UpdateBlink();
	/// @brief Make the Plant play a specific animation
	/// @param theTrackName The animation to play
	/// @param theLoopType The loop type (see ReanimLoopType enum)
	/// @param theBlendTime The blending time between the new animation and the previous one
	/// @param theAnimRate The rate the animation plays at
	void PlayBodyReanim(const char *theTrackName, ReanimLoopType theLoopType, int theBlendTime, float theAnimRate);
	/// @brief Update the MagnetShroom
	void UpdateMagnetShroom();
	/// @brief Get the first free magnet item
	/// @return MagnetItem
	MagnetItem *GetFreeMagnetItem();
	/// @brief Draw the magnet items
	/// @param g Graphics object
	void DrawMagnetItems(Graphics *g);
	/// @brief Update the DoomShroom
	void UpdateDoomShroom();
	/// @brief Update the IceShroom
	void UpdateIceShroom();
	/// @brief Update the PotatoMine
	void UpdatePotato();
	/// @brief Calculate the render order of the Plant
	/// @return The Render Order
	int CalcRenderOrder();
	/// @brief Update the animations of the nut plants
	void AnimateNuts();
	/// @brief Set the Plant's sleep status
	/// @param theIsAsleep The state
	void SetSleeping(bool theIsAsleep);
	/// @brief Update the Shooting State
	void UpdateShooting();
	/// @brief Draw the Plant's shadow
	/// @param g Graphics object
	/// @param theOffsetX The X offset
	/// @param theOffsetY The Y offset
	void DrawShadow(Graphics *g, float theOffsetX, float theOffsetY);
	/// @brief Update the ScaredyShroom
	void UpdateScaredyShroom();
	/// @brief Get the distance to the closest Zombie
	/// @return Distance in pixels
	int DistanceToClosestZombie();
	/// @brief Update the Spikeweed
	void UpdateSpikeweed();
	/// @brief Try to attract an item from a Zombie
	/// @param theZombie The Zombie to attract an item from
	void MagnetShroomAttactItem(Zombie *theZombie);
	/// @brief Update the SunShroom
	void UpdateSunShroom();
	/// @brief Update the Bowling Wallnuts
	void UpdateBowling();
	/// @brief Update the animations of the Pumpkin
	void AnimatePumpkin();
	/// @brief Update the Blover
	void UpdateBlover();
	/// @brief Update the Cactus
	void UpdateCactus();
	/// @brief Fire a StarFruit shot
	void StarFruitFire();
	/// @brief Update the TangleKelp
	void UpdateTanglekelp();
	/// @brief Attach a blinking reanimation to the Plant's body reanimation
	/// @param theReanimBody The Plant's body
	/// @return The bliking reanimation
	Reanimation *AttachBlinkAnim(Reanimation *theReanimBody);
	/// @brief Update the color of the reanimation
	void UpdateReanimColor();
	/// @brief Can the plant be upgraded to another one
	/// @return True if it can be upgraded
	bool IsUpgradableTo(SeedType theUpgradedType);
	/// @brief Can the Plant form an upgrade
	/// @return True if it can form an upgrade
	bool IsPartOfUpgradableTo(SeedType theUpgradedType);
	/// @brief Update the CobCannon
	void UpdateCobCannon();
	/// @brief Fire a CobCannon shot
	void CobCannonFire(int theTargetX, int theTargetY);
	/// @brief Update the GoldMagnet
	void UpdateGoldMagnetShroom();
	/// @brief Is the Plant currently on the Board
	/// @return True if it belongs to the Board
	bool IsOnBoard();
	/// @brief Remove all visual effects applied to the Plant
	void RemoveEffects();
	/// @brief Update the CoffeeBean
	void UpdateCoffeeBean();
	/// @brief Update the UmbrellaLeaf
	void UpdateUmbrella();
	/// @brief End the Plant's blinking animation
	void EndBlink();
	/// @brief Update the animations of the Garlic
	void AnimateGarlic();
	/// @brief Try to find a Coin for the GoldMagnet to attract
	/// @return Coin or nullptr if it doesn't exist
	Coin *FindGoldMagnetTarget();
	/// @brief Make the Spikeweed deal damage
	void SpikeweedAttack();
	/// @brief Morph the Imitater into the imitated Plant
	void ImitaterMorph();
	/// @brief Update the Imitater
	void UpdateImitater();
	/// @brief Update the reanimation
	void UpdateReanim();
	/// @brief Deal damage to the SpikeRock
	void SpikeRockTakeDamage();
	/// @brief Is the Plant spiky?
	/// @return True if the Plant is spiky
	bool IsSpiky();
	/// @brief Preload a Plant's assets
	static void PreloadPlantResources(SeedType theSeedType);
	/// @brief Is the Plant part of the Gameplay
	bool IsInPlay();
	/// @brief ???
	void UpdateNeedsFood()
	{
		;
	}
	/// @brief Play the idle animation
	/// @param theRate The animation rate
	void PlayIdleAnim(float theRate);
	/// @brief Update the FlowerPot
	void UpdateFlowerPot();
	/// @brief Update the Lilypad
	void UpdateLilypad();
	/// @brief Try to find targets for the GoldMagnet
	void GoldMagnetFindTargets();
	/// @brief Is a GoldMagnet about to attract coins
	/// @return True if a GoldMagnet is about to attract coins
	bool IsAGoldMagnetAboutToSuck();
	/// @brief Should it draw the magnet items ontop
	/// @return True if it has an attracted item
	bool DrawMagnetItemsOnTop();
};
/// @brief Get the Plant's height offset on a grid
/// @param theBoard [OPTIONAL] The Board for specific position cases
/// @param thePlant [OPTIONAL] The Plant for specific state cases
/// @param theCol The theSeedType to check
/// @param theCol The column to check
/// @param theRow The row to check
/// @return The final Y offset
float PlantDrawHeightOffset(Board *theBoard, Plant *thePlant, SeedType theSeedType, int theCol, int theRow);
/// @brief Get the Plant's height offset on a grid
/// @param theCol The theSeedType to check
/// @param theFlowerPotScale The scale of the flowerpot
/// @return The final Y offset
float PlantFlowerPotHeightOffset(SeedType theSeedType, float theFlowerPotScale);

/// @brief Plant Definition used for setting it up in PlantInitialize
class PlantDefinition
{
  public:
	/// Plant's SeedType (see SeedType enum)
	SeedType mSeedType;
	/// Plant's Spritesheet (Beta 0.1.1.1014)
	Image **mPlantImage;
	/// Plant's ReanimationType (see ReanimationType enum)
	ReanimationType mReanimationType;
	/// Plant's Packet Sprite-Sheet Index (Unused) (Beta 0.1.1.1014)
	int mPacketIndex;
	/// Plant's Sun Cost
	int mSeedCost;
	/// Time for the SeedPacket to refresh
	int mRefreshTime;
	/// Plant's Sub-Class (see PlantSubClass enum)
	PlantSubClass mSubClass;
	/// How often does a plant do an action
	int mLaunchRate;
	/// Plant's Name Translation Key
	const SexyCharByte *mPlantName;
};
extern PlantDefinition gPlantDefs[SeedType::NUM_SEED_TYPES];

/// @brief Get the Plant Definition
/// @param theSeedType The SeedType of the Definition
/// @return The Plant Definition
PlantDefinition &GetPlantDefinition(SeedType theSeedType);
