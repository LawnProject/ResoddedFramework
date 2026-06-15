#include "ProfileMgr.h"
#include "PlayerInfo.h"
#include <fstream>
#include "../../Sexy.TodLib/TodDebug.h"
#ifndef _WIN32
#include <strings.h> // strcasecmp
#endif

using namespace Sexy;
static int gProfileVersion = 14;

PlayerInfo *ProfileMgr::GetAnyProfile()
{
	if (mProfileMap.size() == 0)
		return nullptr;

	PlayerInfo *aPlayerInfo = &mProfileMap.begin()->second;
	aPlayerInfo->LoadDetails();
	aPlayerInfo->mUseSeq = mNextProfileUseSeq++;
	return aPlayerInfo;
}

void ProfileMgr::Clear()
{
	mProfileMap.clear();
	mNextProfileId = 1U;
	mNextProfileUseSeq = 1U;
}

void ProfileMgr::SyncState(ProfileSyncer &theSync)
{
	if (theSync.mReading)
	{
		mProfileMap.clear();

		int aMaxProfileId = 0;
		int aMaxUseSeq = 0;
		auto &auUserArray = theSync.mJSON["users"];
		for (auto &anUser : auUserArray)
		{
			ProfileSyncer aUserSyncer;
			aUserSyncer.mJSON = anUser;
			aUserSyncer.mReading = true;
			PlayerInfo aProfile;
			aProfile.SyncSummary(aUserSyncer);

			if (aProfile.mId > aMaxProfileId)
				aMaxProfileId = aProfile.mId;
			if (aProfile.mUseSeq > aMaxUseSeq)
				aMaxUseSeq = aProfile.mUseSeq;

			mProfileMap[aProfile.mName] = aProfile;
		}

		mNextProfileId = aMaxProfileId + 1;
		mNextProfileUseSeq = aMaxUseSeq + 1;
	}
	else
	{
		auto &aUsers = theSync.mJSON["users"];
		aUsers = nlohmann::json::array();

		for (auto &aProfile : mProfileMap)
		{
			ProfileSyncer aUserSyncer;

			aProfile.second.SyncSummary(aUserSyncer);

			aUsers.push_back(aUserSyncer.mJSON);
		}
	}
}

void ProfileMgr::Load()
{

	try
	{
		std::string aFileName = GetAppDataFolder() + "profiles/users.json";
		ProfileSyncer aSync(aFileName);

		if (std::filesystem::exists(aFileName))
		{
			ProfileSyncer aSync(aFileName);
			if (aSync.mCanRead)
			{
				aSync.mReading = true;
				SyncState(aSync);
			}
			else
			{
				TodTraceAndLog("[LawnProject] - Couldn't read profile file : %s\nResetting it", aFileName.c_str());
				Clear();
			}
		}
	}
	catch (nlohmann::json::parse_error &anError)
	{
		TodTraceAndLog("[LawnProject] - Failed to profiles\n");
		Clear();

	}
}

void ProfileMgr::Save()
{

	MkDir(GetAppDataFolder() + "profiles");
	std::string aFileName = GetAppDataFolder() + "profiles/users.json";
	ProfileSyncer aSync(aFileName);
	SyncState(aSync);
	std::ofstream outFile(aFileName);
	if (outFile)
		outFile << aSync.mJSON.dump(4);
}

void ProfileMgr::DeleteProfile(ProfileMap::iterator theProfile)
{
	theProfile->second.DeleteUserFiles();
	mProfileMap.erase(theProfile);
}

bool ProfileMgr::DeleteProfile(const SexyString &theName)
{
	auto anItr = mProfileMap.find(theName);
	if (anItr == mProfileMap.end())
		return false;

	DeleteProfile(anItr);
	return true;
}

bool ProfileMgr::RenameProfile(const SexyString &theOldName, const SexyString &theNewName)
{
	auto anOldItr = mProfileMap.find(theOldName);
	if (anOldItr == mProfileMap.end())
		return false;
	else
	{
		if (
#ifdef _WIN32
			_stricmp(theOldName.c_str(), theNewName.c_str())
#else
			strcasecmp(theOldName.c_str(), theNewName.c_str())
#endif
			== 0)
			anOldItr->second.mName = theNewName;
		else
		{
			auto aRet = mProfileMap.emplace(theNewName, anOldItr->second);
			if (!aRet.second)
				return false;
			else
			{
				mProfileMap.erase(anOldItr);
				aRet.first->second.mName = theNewName;
			}
		}
		return true;
	}
}

void ProfileMgr::DeleteOldestProfile()
{
	if (mProfileMap.size() == 0)
		return;

	auto anOldest = mProfileMap.begin();
	for (auto anItr = anOldest; anItr != mProfileMap.end(); anItr++)
		if (anItr->second.mUseSeq < anOldest->second.mUseSeq)
			anOldest = anItr;
	DeleteProfile(anOldest);
}

PlayerInfo *ProfileMgr::GetProfile(const SexyString &theName)
{
	auto anItr = mProfileMap.find(theName);
	if (anItr != mProfileMap.end())
	{
		PlayerInfo *aProfile = &anItr->second;
		aProfile->LoadDetails();
		aProfile->mUseSeq = mNextProfileUseSeq++;
		return aProfile;
	}
	return nullptr;
}

PlayerInfo *ProfileMgr::AddProfile(const SexyString &theName)
{
	auto aRet = mProfileMap.emplace(theName, PlayerInfo());
	if (aRet.second)
	{
		PlayerInfo *aProfile = &aRet.first->second;
		aProfile->mName = theName;
		aProfile->mId = mNextProfileId++;
		aProfile->mUseSeq = mNextProfileUseSeq++;
		DeleteOldProfiles();
		return aProfile;
	}
	return nullptr;
}
