#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "../ConstEnums.h"
#include "GameObject.h"

class Plant;
class Zombie;
namespace Sexy
{
class Graphics;
};
using namespace Sexy;

class ProjectileDefinition
{
  public:
	ProjectileType mProjectileType;
	int mImageRow;
	int mDamage;
};
extern ProjectileDefinition gProjectileDefinition[NUM_PROJECTILES]; //0x69F1C0

class Projectile : public GameObject
{
  public:
	int mFrame;
	int mNumFrames;
	int mAnimCounter;
	float mPosX;
	float mPosY;
	float mPosZ;
	float mVelX;
	float mVelY;
	float mVelZ;
	float mAccZ;
	float mShadowY;
	bool mDead;
	int mAnimTicksPerFrame;
	ProjectileMotion mMotionType;
	ProjectileType mProjectileType;
	int mProjectileAge;
	int mClickBackoffCounter;
	float mRotation;
	float mRotationSpeed;
	bool mOnHighGround;
	int mDamageRangeFlags;
	int mHitTorchwoodGridX;
	AttachmentID mAttachmentID;
	float mCobTargetX;
	int mCobTargetRow;
	ZombieID mTargetZombieID;
	int mLastPortalX;

  public:
	Projectile();
	~Projectile();

	/// @brief Initialize the Projectile
	/// @param theX The X coordinate
	/// @param theY The Y coordinate
	/// @param theRenderOrder The render order to draw the Projectile on
	/// @param theRow The row to spawn the Projectile on
	/// @param theProjectileType The type of Projectile to setup
	void ProjectileInitialize(int theX, int theY, int theRenderOrder, int theRow, ProjectileType theProjectileType);
	/// @brief Update the Projectile
	void Update();
	/// @brief Draw the Projectile
	/// @param g Graphic object
	void Draw(Graphics *g);
	/// @brief Draw the Projectile's shadow
	/// @param g Graphic object
	void DrawShadow(Graphics *g);
	/// @brief Destroy the Projectile, can't be used after this
	void Die();
	/// @brief Hit the Zombie, plays sounds, create particles
	/// @return theZombie The Zombie to hit
	void DoImpact(Zombie *theZombie);
	/// @brief Update the Motion of the Projectile
	void UpdateMotion();
	/// @brief Check for a possible collision
	void CheckForCollision();
	/// @brief Try to find a collision target
	/// @return Zombie or nullptr if not found
	Zombie *FindCollisionTarget();
	/// @brief Update the lobbing motion
	void UpdateLobMotion();
	/// @brief Check for HighGround collision
	void CheckForHighGround();
	/// @brief Will the Projectile ignore the HighGround
	/// @return True if it ignores the HighGround, false if it can collide with it
	bool CantHitHighGround();
	/// @brief Deal splash damage to the Zombue
	/// @param theZombie The Zombie to hit
	void DoSplashDamage(Zombie *theZombie);
	/// @brief Get the Projectile's Definition
	/// @return Projectile Definition, asserts if it doesn't exist
	ProjectileDefinition &GetProjectileDef();
	/// @brief Get the damage flags that affect the Zombie
	/// @param theZombie The Zombie that it needs to hit
	/// @return Flags that determine that damage types are dealt (see DamageFlags enum)
	unsigned int GetDamageFlags(Zombie *theZombie);
	/// @brief Get the World-Space collision rect
	/// @return World-Space rect
	Rect GetProjectileRect();
	/// @brief Update the normal motion
	void UpdateNormalMotion();
	Plant *FindCollisionTargetPlant();
	/// @brief Convert the Projectile to a FireBall
	/// @param theGridX The Torchwood's grid X coordinate to avoid changing the Projectile again on the same grid
	void ConvertToFireball(int theGridX);
	/// @brief Convert the Projectile to a Pea
	/// @param theGridX The Torchwood's grid X coordinate to avoid changing the Projectile again on the same grid
	void ConvertToPea(int theGridX);
	/// @brief Does the Projectile deal Splash-Damage
	/// @param theZombie The Zombie to check
	/// @return True if the Projectile can deal Splash-Damage to the Zombie
	bool IsSplashDamage(Zombie *theZombie);
	/// @brief Play the impact sound of hitting the Zombie
	/// @param theZombie The Zombie to determine the correct sound
	void PlayImpactSound(Zombie *theZombie);
	/// @brief Is the Zombie able to be hit by the Splash-Damage
	/// @param theZombie The Zombie to check
	/// @return True if the Zombie is in range
	bool IsZombieHitBySplash(Zombie *theZombie);
	/// @brief Is the Pea Projectile about to hit a Torchwood
	/// @return True if the Projectile is in the Torchwood's range
	bool PeaAboutToHitTorchwood();
};

#endif
