#include "Version.h"

using namespace Sexy;


Version::Version() : Version(0, 0, 0) {}

Version::Version(int theMajor, int theMinor, int thePatch)
{
	mMajor = theMajor;
	mMinor = theMinor;
	mPatch = thePatch;
}

Version::Version(const Version &theVersion) : Version(theVersion.mMajor, theVersion.mMinor, theVersion.mPatch) {}

Version::~Version()
{
}

bool Version::isOlderThan(const Version &theVersion)
{
	if (theVersion.mMajor < mMajor)
		return true;
	else if(theVersion.mMajor > mMajor)
		return false;

	if (theVersion.mMinor < mMinor)
		return true;
	else if(theVersion.mMinor > mMinor)
		return false;

	if (theVersion.mPatch < mPatch)
		return true;
	else if (theVersion.mPatch > mPatch)
		return false;

	return false;
}

bool Version::isEqual(const Version &theVersion)
{
	return mMajor == theVersion.mMajor && mMajor == theVersion.mMinor && mPatch == theVersion.mPatch;
}

bool Version::isNewerThan(const Version &theVersion)
{
	return !isOlderThan(theVersion) && !isEqual(theVersion);
}

std::string Sexy::Version::toString()
{
	return StrFormat("v%d.%d.%d", mMajor, mMinor, mPatch);
}
