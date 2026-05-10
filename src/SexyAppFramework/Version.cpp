#include "Version.h"

using namespace Sexy;


Version::Version() : Version(-1, -1, -1, -1)
{
}

Version::Version(int theMajor, int theMinor, int thePatch, int theBuildNumber)
{
	mMajor = theMajor;
	mMinor = theMinor;
	mPatch = thePatch;
	mBuildNumber = theBuildNumber;
}

Version::Version(const Version &theVersion) : Version(theVersion.mMajor, theVersion.mMinor, theVersion.mPatch, theVersion.mBuildNumber) {}

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
	else if (mPatch > theVersion.mPatch)
		return false;

	if (mBuildNumber != -1 && theVersion.mBuildNumber != -1) //making the build number -1 makes it ignore version checks
	{
		if (mBuildNumber < theVersion.mBuildNumber)
			return true;
	}

	return false;
}

bool Version::isEqual(const Version &theVersion)
{
	return mMajor == theVersion.mMajor && mMajor == theVersion.mMinor && mPatch == theVersion.mPatch && mBuildNumber == theVersion.mBuildNumber;
}

bool Version::isNewerThan(const Version &theVersion)
{
	return !isOlderThan(theVersion) && !isEqual(theVersion);
}

std::string Sexy::Version::toString()
{
	if (mBuildNumber == -1)
		return StrFormat("%d.%d.%d", mMajor, mMinor, mPatch);
	else
		return StrFormat("%d.%d.%d.%d", mMajor, mMinor, mPatch, mBuildNumber);
}
