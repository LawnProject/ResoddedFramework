#include "Version.h"

using namespace Sexy;


Version::Version() : Version(-1, -1, -1)
{
}

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
	if (mMajor < theVersion.mMajor)
		return true;
	else if (mMajor > theVersion.mMajor)
		return false;

	if (mMinor < theVersion.mMinor)
		return true;
	else if (mMinor > theVersion.mMinor)
		return false;

	if (mPatch < theVersion.mPatch)
		return true;


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
