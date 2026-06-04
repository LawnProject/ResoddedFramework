#ifndef __GRIDITEM_H__
#define __GRIDITEM_H__

#include "../ConstEnums.h"

#define NUM_MOTION_TRAIL_FRAMES 12

class LawnApp;
class Board;
class Zombie;
namespace Sexy
{
class Graphics;
};

class MotionTrailFrame
{
  public:
	float mPosX;
	float mPosY;
	float mAnimTime;
};

class GridItem
{
  public:
	LawnApp *mApp;
	Board *mBoard;
	GridItemType mGridItemType;
	GridItemState mGridItemState;
	int mGridX;
	int mGridY;
	int mGridItemCounter;
	int mRenderOrder;
	bool mDead;
	float mPosX;
	float mPosY;
	float mGoalX;
	float mGoalY;
	ReanimationID mGridItemReanimID;
	ParticleSystemID mGridItemParticleID;
	ZombieType mZombieType;
	SeedType mSeedType;
	ScaryPotType mScaryPotType;
	bool mHighlighted;
	int mTransparentCounter;
	int mSunCount;
	MotionTrailFrame mMotionTrailFrames[NUM_MOTION_TRAIL_FRAMES];
	int mMotionTrailCount;

  public:
	GridItem();

	/// @brief Draw the Ladder GridType
	/// @param g Graphics object
	void DrawLadder(Sexy::Graphics *g);
	/// @brief Draw the Crater GridType
	/// @param g Graphics object
	void DrawCrater(Sexy::Graphics *g);
	/// @brief Draw the GraveStone GridType
	/// @param g Graphics object
	void DrawGraveStone(Sexy::Graphics *g);
	/// @brief Destroy the GridItem (can't be used after this)
	void GridItemDie();
	/// @brief Add the GraveStone particles
	void AddGraveStoneParticles();
	/// @brief Draw a GridItem
	/// @param g Graphics object
	void DrawGridItem(Sexy::Graphics *g);
	/// @brief Draw the GridItemOverlay
	/// @param g Graphics object
	void DrawGridItemOverlay(Sexy::Graphics *g);
	/// @brief Open the Portal GridType
	void OpenPortal();
	/// @brief Update the GridItem
	void Update();
	/// @brief Close the Portal GridType
	void ClosePortal();
	/// @brief Draw the ScaryPot GridType
	/// @param g Graphics object
	void DrawScaryPot(Sexy::Graphics *g);
	/// @brief Update the ScaryPot GridType
	void UpdateScaryPot();
	/// @brief Update the Portal GridType
	void UpdatePortal();
	/// @brief Draw the Squirrel GridType
	/// @param g Graphics object
	void DrawSquirrel(Sexy::Graphics *g);
	/// @brief Update the Rake GridType
	void UpdateRake();
	/// @brief Find a Zombie for the Rake GridType
	/// @return Zombie or nullptr if it doesn't exist
	Zombie *RakeFindZombie();
	/// @brief Draw the IZombieBrain GridType
	/// @param g Graphics object
	void DrawIZombieBrain(Sexy::Graphics *g);
	void UpdateBrain();
	/// @brief Draw the Stinky GridType
	/// @param g Graphics object
	void DrawStinky(Sexy::Graphics *g);
	/// @brief Is the Portal GridType opened
	/// @return True if the GridType is a portal and it's in the opened state
	bool IsOpenPortal();
};

#endif
