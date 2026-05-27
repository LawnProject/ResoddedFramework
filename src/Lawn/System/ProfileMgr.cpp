#include "DataSync.h"
#include "ProfileMgr.h"
#include "PlayerInfo.h"
#include <fstream>
#include "../../SexyAppFramework/SexyAppBase.h"
#include "../../Sexy.TodLib/TodDebug.h"
#ifndef _WIN32
#include <strings.h> // strcasecmp
#endif

using namespace Sexy;
static int gProfileVersion = 14;

//0x46A7C0
PlayerInfo *ProfileMgr::GetAnyProfile()
{
	if (mProfileMap.size() == 0)
		return nullptr;

	PlayerInfo *aPlayerInfo = &mProfileMap.begin()->second;
	aPlayerInfo->LoadDetails();
	aPlayerInfo->mUseSeq = mNextProfileUseSeq++;
	return aPlayerInfo;
}

//0x46A7F0
void ProfileMgr::Clear()
{
	mProfileMap.clear();
	mNextProfileId = 1U;
	mNextProfileUseSeq = 1U;
}

//0x46A830
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

//0x46ABC0
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

//0x46AD80
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

//0x46AF70
bool ProfileMgr::DeleteProfile(const SexyString &theName)
{
	auto anItr = mProfileMap.find(theName);
	if (anItr == mProfileMap.end())
		return false;

	DeleteProfile(anItr);
	return true;
}

//0x46AFF0
bool ProfileMgr::RenameProfile(const SexyString &theOldName, const SexyString &theNewName)
{
	auto anOldItr = mProfileMap.find(theOldName);
	if (anOldItr == mProfileMap.end())
		return false;
	else
	{
		// 判断修改前后的用户名是否一致，一致则直接在原存档中进行修改，否则需要额外操作
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
			// 向 mProfileMap 中插入一个由新用户名及旧存档组成的对组
			auto aRet = mProfileMap.emplace(
				theNewName, anOldItr->second); // auto aRet = mProfileMap.insert({theNewName, anOldItr->second});
			// 通过返回值检测新用户名是否与原有存档重复，重复则返回 false，插入成功则继续操作
			if (!aRet.second)
				return false;
			else
			{
				// 删除 mProfileMap 中原用户名及旧存档的键值对
				mProfileMap.erase(anOldItr);
				// 修改新插入的键值对中存档的用户名
				aRet.first->second.mName = theNewName;
			}
		}
		return true;
	}
}

//0x46B1C0
void ProfileMgr::DeleteOldestProfile()
{
	if (mProfileMap.size() == 0)
		return;

	//将 mUseSeq 最小的存档的所在位置记录在 anOldest 迭代器中
	auto anOldest = mProfileMap.begin();
	for (auto anItr = anOldest; anItr != mProfileMap.end(); anItr++)
		if (anItr->second.mUseSeq < anOldest->second.mUseSeq)
			anOldest = anItr;
	//删除记录的 mUseSeq 最小的存档
	DeleteProfile(anOldest);
}

//0x46B290
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

//0x46B310
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
