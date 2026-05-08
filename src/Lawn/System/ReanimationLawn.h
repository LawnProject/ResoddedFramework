#ifndef __REANIMATORCACHE_H__
#define __REANIMATORCACHE_H__

#include "../../ConstEnums.h"
#include "../../Sexy.TodLib/TodList.h"
namespace Sexy
{
class Graphics;
class GPUImage;
}; // namespace Sexy
using namespace Sexy;

class LawnApp;
class ReanimCacheImageVariation
{
  public:
	SeedType mSeedType;
	DrawVariation mDrawVariation;
	GPUImage *mImage;
};
typedef TodList<ReanimCacheImageVariation> ImageVariationList;

class Reanimation;
class ReanimatorCache
{
  public:
	GPUImage *mPlantImages[SeedType::NUM_SEED_TYPES];
	ImageVariationList mImageVariationList;
	GPUImage *mLawnMowers[LawnMowerType::NUM_MOWER_TYPES];
	GPUImage *mZombieImages[ZombieType::NUM_CACHED_ZOMBIE_TYPES];
	LawnApp *mApp;

  public:
	//ReanimatorCache() { ReanimatorCacheInitialize(); }
	//~ReanimatorCache() { ReanimatorCacheDispose(); }

	void ReanimatorCacheInitialize();
	void ReanimatorCacheDispose();
	void DrawCachedPlant(
		Graphics *g, float thePosX, float thePosY, SeedType theSeedType, DrawVariation theDrawVariation);
	void DrawCachedMower(Graphics *g, float thePosX, float thePosY, LawnMowerType theMowerType);
	void DrawCachedZombie(Graphics *g, float thePosX, float thePosY, ZombieType theZombieType);
	GPUImage *MakeBlankGPUImage(int theWidth, int theHeight);
	GPUImage *MakeCachedPlantFrame(SeedType theSeedType, DrawVariation theDrawVariation);
	GPUImage *MakeCachedMowerFrame(LawnMowerType theMowerType);
	GPUImage *MakeCachedZombieFrame(ZombieType theZombieType);
	/*inline*/ void GetPlantImageSize(
		SeedType theSeedType, int &theOffsetX, int &theOffsetY, int &theWidth, int &theHeight);
	void DrawReanimatorFrame(Graphics *g,
							 float thePosX,
							 float thePosY,
							 ReanimationType theReanimationType,
							 const char *theTrackName,
							 DrawVariation theDrawVariation);
	void UpdateReanimationForVariation(Reanimation *theReanim, DrawVariation theDrawVariation);
};

#endif
