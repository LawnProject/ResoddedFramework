#pragma once

#include <string>
#include "../../Sexy.TodLib/TodList.h"
#include "../../Sexy.TodLib/TodParticle.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../Sexy.TodLib/Trail.h"
#include "../../Sexy.TodLib/Attachment.h"
#include "../../SexyAppFramework/Buffer.h"
#include "../Plant.h"
#include "../Projectile.h"
#include "../Coin.h"
#include "../Zombie.h"
#include "../LawnMower.h"
#include "../Board.h"
#include "../GridItem.h"
#include "../MessageWidget.h"
#include "../Challenge.h"
#include "../SeedPacket.h"
#include "../System/Music.h"
#include <fstream>

namespace Sexy
{
class Image;
}
using namespace Sexy;


class SaveContext
{
	  public:
		std::ofstream *mWriter;
		std::ifstream *mReader;
		bool mReading;
		bool mFailed;

		template <typename T> void Sync(T &theValue)
		{
			if (mReading)
				mReader->read((char *)(&theValue), sizeof(T));
			else
				mWriter->write(reinterpret_cast<const char *>(&theValue), sizeof(T));
		}

		void SyncBytes(void *theData, size_t theSize)
		{
			if (mReading)
				mReader->read(reinterpret_cast<char *>(theData), theSize);
			else
				mWriter->write(reinterpret_cast<const char *>(theData), theSize);
		}

		void SyncReanimationDef(ReanimatorDefinition *&theDefinition);
		void SyncParticleDef(TodParticleDefinition *&theDefinition);
		void SyncTrailDef(TrailDefinition *&theDefinition);
		void SyncImage(Image *&theImage);
};

bool LawnLoadGame(Board *theBoard, const std::string &theFilePath);
bool LawnSaveGame(Board *theBoard, const std::string &theFilePath);
