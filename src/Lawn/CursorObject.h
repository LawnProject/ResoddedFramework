#ifndef __CURSOROBJECT_H__
#define __CURSOROBJECT_H__

#include "GameObject.h"

class CursorObject : public GameObject
{
  public:
	int mSeedBankIndex;
	SeedType mType;
	SeedType mImitaterType;
	CursorType mCursorType;
	CoinID mCoinID;
	PlantID mGlovePlantID;
	PlantID mDuplicatorPlantID;
	PlantID mCobCannonPlantID;
	int mHammerDownCounter;
	ReanimationID mReanimCursorID;

  public:
	CursorObject();

	/// @brief Update the object
	void Update();
	/// @brief Draw the object
	void Draw(Graphics *g);
	/// @brief Destroy the object, it can't be used after this
	void Die();
};

class CursorPreview : public GameObject
{
  public:
	int mGridX;
	int mGridY;

  public:
	CursorPreview();

	/// @brief Update the preview
	void Update();
	/// @brief Draw the preview
	void Draw(Graphics *g);
};

#endif
