#pragma once

#include "Common.h"

namespace Sexy
{
	class Version
	{
	  public:
		Version(int theMajor, int theMinor, int thePatch);
		Version(const Version &theVersion);
		~Version();

		bool isOlderThan(const Version &theVersion);
		bool isEqual(const Version &theVersion);
		bool isNewerThan(const Version &theVersion);

	  public:
		int mMajor;
		int mMinor;
		int mPatch;
	};
}