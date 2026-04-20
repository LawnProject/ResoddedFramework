#pragma once

#include <string>
#include "../../Sexy.TodLib/TodList.h"
#include "../../SexyAppFramework/Buffer.h"

class Board;
class Trail;
enum GameMode;
class Reanimation;
class TodParticleSystem;
class TodParticleEmitter;
class ReanimatorDefinition;
class TodParticleDefinition;
class TrailDefinition;
namespace Sexy
{
class Image;
}
using namespace Sexy;

struct SaveFileHeader
{
	unsigned int mMagicNumber;
	unsigned int mBuildVersion;
	unsigned int mBuildDate;
};

class SaveGameContext
{
  public:
	Buffer mBuffer;
	bool mFailed;
	bool mReading;

  public:
	inline int ByteLeftToRead()
	{
		return mBuffer.mDataBitSize - mBuffer.mReadBitPos;
	}
};

bool LawnLoadGame(Board *theBoard, const std::string &theFilePath);
bool LawnSaveGame(Board *theBoard, const std::string &theFilePath);
