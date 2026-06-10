#ifndef __COIN_H__
#define __COIN_H__

#include "GameObject.h"
#include "System/PlayerInfo.h"

class HitResult;
namespace Sexy
{
class Graphics;
}
using namespace Sexy;

class Coin : public GameObject
{
  public:
	float mPosX;
	float mPosY;
	float mVelX;
	float mVelY;
	float mScale;
	bool mDead;
	int mFadeCount;
	float mCollectX;
	float mCollectY;
	int mGroundY;
	int mCoinAge;
	bool mIsBeingCollected;
	int mDisappearCounter;
	CoinType mType;
	CoinMotion mCoinMotion;
	AttachmentID mAttachmentID;
	float mCollectionDistance;
	SeedType mUsableSeedType;
	PottedPlant mPottedPlantSpec;
	bool mNeedsBouncyArrow;
	bool mHasBouncyArrow;
	bool mHitGround;
	int mTimesDropped;
#if LAWN_USE_UNFINISHED_GAMEPAD_SUPPORT
	float mGamepadCollectionSpeed;
#endif

  public:
	Coin();
	~Coin();

	/// @brief Initialize the Coin
	/// @param theX The Coin X coordinate
	/// @param theY The Coin Y coordinate
	/// @param theCoinType The CoinType to use (see CoinType enum)
	/// @param theCoinMotion The Motion to spawn with (see CoinMotion enum)
	void CoinInitialize(int theX, int theY, CoinType theCoinType, CoinMotion theCoinMotion);
	/// @brief Handle a mouse pressed event
	/// @param theX The mouse click X coordinate
	/// @param theY The mouse click Y coordinate
	/// @param theClickCount Flags determining the buttons pressed
	void MouseDown(int x, int y, int theClickCount);
	/// @brief Test to see if the Coin got hit
	/// @param theX The mouse click X coordinate
	/// @param theY The mouse click Y coordinate
	/// @param theHitResult Object containing HitResult data like Object type and pointer
	bool MouseHitTest(int theX, int theY, HitResult *theHitResult);
	/// @brief Destroy the Coin, can't be used after this
	void Die();
	/// @brief Start the fade out
	void StartFade();
	/// @brief Update the Coin Object
	void Update();
	/// @brief Draw the Coin
	/// @param g Graphics object
	void Draw(Graphics *g);
	/// @brief Collect the Coin
	void Collect();
	/// @brief Get the Sun value of the Coin
	/// @return The Sun value or 0 if it's not a sun
	int GetSunValue();
	/// @brief Get the value of the CoinType
	/// @param theCoinType The CoinType to find
	/// @param The value of the Coin or 0
	static int GetCoinValue(CoinType theCoinType);
	/// @brief Update the fading timer
	void UpdateFade();
	/// @brief Update the falling state
	void UpdateFall();
	/// @brief Update the Coin's destination counters
	void ScoreCoin();
	/// @brief Update the collection state
	void UpdateCollected();
	/// @brief Get the color of the Coin based on fade-out
	/// @return The color
	Color GetColor();
	/// @brief Is the Coin considered Money
	/// @return True if the Coin is Money, false otherwise
	bool IsMoney();
	/// @brief Is the Coin a Sun Type
	/// @return True if the Coin is a sun, false otherwise
	bool IsSun();
	/// @brief Get the scale that the Sun CoinType should be at
	/// @return The scale
	float GetSunScale();
	/// @brief Get the SeedType that the Coin represents
	/// @return The SeedType determined by the current adventure level
	SeedType GetFinalSeedPacketType();
	/// @brief Is the Coin the Level Award
	/// @return True if it's the Level Award, false otherwise
	bool IsLevelAward();
	/// @brief Should a bouncy arrow appear next to the Coin (Is Present or your first Coin)
	/// @return True if the Coin needs an arrow, false otherwise
	bool CoinGetsBouncyArrow();
	/// @brief Create Coins from the current position and spread them apart in a circle and auto-collect
	/// @param theCoinType The CoinType to create
	/// @param theNumCoins The amount of coins to create
	void FanOutCoins(CoinType theCoinType, int theNumCoins);
	/// @brief Get the amount of time before the Coin dissapears
	/// @return The amount of time before the Coin disappears
	int GetDisappearTime();
	/// @brief Handle the SeedPacket dropping
	void DroppedUsableSeed();
	/// @brief Play the collecting sound of the Coin
	void PlayCollectSound();
	/// @brief Attempt to auto collect itself if the Level Award has been picked up
	void TryAutoCollectAfterLevelAward();
	/// @brief Is the Coin actually a present with advice
	/// @return True if the Coin is of a Present Type, false otherwise
	bool IsPresentWithAdvice();
#if LAWN_USE_UNFINISHED_GAMEPAD_SUPPORT
	/// @brief Handle the Gamepad Cursor overlapping the coin
	void GamepadCursorOver();
#endif
	/// @brief Play the sound of the Coin being created
	void PlayLaunchSound();
	/// @brief Play the sound of the Coin hitting the ground
	void PlayGroundSound();

	/// @brief Is the CoinType considered Money
	/// @return True if it's any type of Money, false otherwise
	static bool IsMoney(CoinType theType);
};

#endif
