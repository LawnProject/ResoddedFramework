#ifndef __LAWNMOWER_H__
#define __LAWNMOWER_H__

#include "../ConstEnums.h"
#include "../SexyAppFramework/Rect.h"

class LawnApp;
class Board;
class Zombie;
namespace Sexy
{
class Graphics;
};
using namespace Sexy;

class LawnMower
{
  public:
	LawnApp *mApp;
	Board *mBoard;
	float mPosX;
	float mPosY;
	int mRenderOrder;
	int mRow;
	int mAnimTicksPerFrame;
	ReanimationID mReanimID;
	int mChompCounter;
	int mRollingInCounter;
	int mSquishedCounter;
	LawnMowerState mMowerState;
	bool mDead;
	bool mVisible;
	LawnMowerType mMowerType;
	float mAltitude;
	MowerHeight mMowerHeight;
	int mLastPortalX;

  public:
	/// @brief Initialize the LawnMower
	/// @param theRow The row to spawn the LawnMower on
	void LawnMowerInitialize(int theRow);

	/// @brief Start the LawnMower
	void StartMower();

	/// @brief Update the LawnMower
	void Update();

	/// @brief Draw the LawnMower
	/// @param g Graphics object
	void Draw(Graphics *g);

	/// @brief Destroy the LawnMower, you can't use it anymore
	void Die();

	/// @brief Get the LawnMower's World-Space attack rect
	Rect GetLawnMowerAttackRect();

	/// @brief Update the LawnMower's Pool State
	void UpdatePool();

	/// @brief Mow a Zombie
	/// @param theZombie Zombie to mow down
	void MowZombie(Zombie *theZombie);

	/// @brief Squish the LawnMower
	void SquishMower();

	/// @brief Set the SuperMower mode
	/// @param theEnable The state of the SuperMower
	void EnableSuperMower(bool theEnable);
};

#endif
