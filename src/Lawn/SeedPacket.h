#ifndef __SEEDPACKET_H__
#define __SEEDPACKET_H__

#include "GameObject.h"
#include "../GameConstants.h"

constexpr const int SLOT_MACHINE_TIME = 400;
constexpr const int CONVEYOR_SPEED = 4;

class HitResult;
class SeedPacket : public GameObject
{
  public:
	int mRefreshCounter;
	int mRefreshTime;
	int mIndex;
	int mOffsetX;
	SeedType mPacketType;
	SeedType mImitaterType;
	int mSlotMachineCountDown;
	SeedType mSlotMachiningNextSeed;
	float mSlotMachiningPosition;
	bool mActive;
	bool mRefreshing;
	int mTimesUsed;

  public:
	SeedPacket();

	/// @brief Update the SeedPacket
	void Update();
	/// @brief Draw the SeedPacket
	/// @param g Graphics object
	void Draw(Graphics *g);
	/// @brief Handle a mouse pressed event
	/// @param theX The mouse click X coordinate
	/// @param theY The mouse click Y coordinate
	/// @param theClickCount Flags determining the buttons pressed
	void MouseDown(int x, int y, int theClickCount);
	/// @brief Test to see if the Packet got hit
	/// @param theX The mouse click X coordinate
	/// @param theY The mouse click Y coordinate
	/// @param theHitResult Object containing HitResult data like Object type and pointer
	bool MouseHitTest(int theX, int theY, HitResult *theHitResult);
	/// @brief Deactive the SeedPacket, making it unusable
	void Deactivate();
	/// @brief Activate the SeedPacket, making it usable
	void Activate();
	/// @brief Set the active state the SeedPacket
	/// @param theActivate The activation state
	void SetActivate(bool theActivate);
	/// @brief Pick the next Packet for the SlotMachine
	void PickNextSlotMachineSeed();
	/// @brief Handle the Packet once it was Planted
	void WasPlanted();
	/// @brief Start the SlotMachine state
	void SlotMachineStart();
	/// @brief Flash the Packet if it can be used
	void FlashIfReady();
	/// @brief Is the Packet pickable
	/// @return True if it can be picked up
	bool CanPickUp();
	/// @brief Set the SeedType of the Packet
	/// @param theSeedType The Type of the Packet
	/// @param theImitaterType The Type of the Packet if theSeedType == SeedType::SEED_IMITATER
	void SetPacketType(SeedType theSeedType, SeedType theImitaterType = SeedType::SEED_NONE);
};

class SeedBank : public GameObject
{
  public:
	int mNumPackets;
	SeedPacket mSeedPackets[SEEDBANK_MAX];
	int mCutSceneDarken;
	int mConveyorBeltCounter;
#if SEXY_USE_CONTROLLER
	int mIndexGamepad;
	float mAxisProgress;
#endif

  public:
	SeedBank();

	/// @brief Draw the SeedBank
	/// @param g Graphics object
	void Draw(Graphics *g);
	/// @brief Test to see if the SeedBank got hit
	/// @param theX The mouse click X coordinate
	/// @param theY The mouse click Y coordinate
	/// @param theHitResult Object containing HitResult data like Object type and pointer
	bool MouseHitTest(int x, int y, HitResult *theHitResult);
	/// @brief Move the SeedBank to a new position
	/// @param x The mouse click X coordinate
	/// @param y The mouse click Y coordinate
	inline void Move(int x, int y)
	{
		mX = x;
		mY = y;
	}
	/// @brief Is the point overlapping the SeedBank
	/// @param theX The mouse click X coordinate
	/// @param theY The mouse click Y coordinate
	/// @return True if the point is in the SeedBank
	bool ContainsPoint(int theX, int theY);
	/// @brief Add a new Seed to the SeedBank
	/// @param theSeedType The SeedType to add to the SeedBank
	/// @param thePlaceOnLeft [OPTIONAL] Should it be added at the left side of the SeedBank
	void AddSeed(SeedType theSeedType, bool thePlaceOnLeft = false);
	/// @brief Remove a new Seed to the SeedBank
	/// @param theIndex The index of the SeedPacket to be removed
	void RemoveSeed(int theIndex);
	/// @brief Count the number of SeedPackets on the ConvejorBelt
	/// @return The number of SeedPackets on the ConveyorBelt
	int GetNumSeedsOnConveyorBelt();
	/// @brief Count the number of SeedPackets on the ConvejorBelt
	/// @param theSeedType The SeedType to count
	/// @return The number of SeedPackets on the ConveyorBelt of a type
	int CountOfTypeOnConveyorBelt(SeedType theSeedType);
	/// @brief Update the ConvejorBelt state
	void UpdateConveyorBelt();
	/// @brief Update the width of the SeedBank
	void UpdateWidth();
	/// @brief Refresh all the Packets in the SeedBank
	void RefreshAllPackets();
};

/// @brief Draw the Seed Preview on the Packet
/// @param g Graphics object
/// @param x The Y Coordinate
/// @param y The X coordinate
/// @param theSeedType The SeedType to draw
/// @param theImitaterType The SeedType if theSeedType == SeedType::SEED_IMITATER
/// @param theOffsetX The X offset
/// @param theOffsetY The Y offset
/// @param theScale The scale of the preview
void SeedPacketDrawSeed(Graphics *g,
						float x,
						float y,
						SeedType theSeedType,
						SeedType theImitaterType,
						float theOffsetX,
						float theOffsetY,
						float theScale);

/// @brief Draw the Seed Preview on the Packet
/// @param g Graphics object
/// @param x The Y Coordinate
/// @param y The X coordinate
/// @param theSeedType The SeedType to draw
/// @param theImitaterType The SeedType if theSeedType == SeedType::SEED_IMITATER
/// @param thePercentDark How much time is left until the Packet has refreshed
/// @param theGrayness A level of grayness to be rendered ontop (0-255)
/// @param theDrawCost Should it draw the price of the Packet
/// @param theUseCurrentCost Draw the current cost of the Packet
void DrawSeedPacket(Graphics *g,
					float x,
					float y,
					SeedType theSeedType,
					SeedType theImitaterType,
					float thePercentDark,
					int theGrayness,
					bool theDrawCost,
					bool theUseCurrentCost);

#endif