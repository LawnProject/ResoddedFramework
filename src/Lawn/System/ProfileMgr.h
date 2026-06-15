#ifndef __PROFILEMGR_H__
#define __PROFILEMGR_H__

#include <map>
#include "../../SexyAppFramework/Common.h"

class ProfileSyncer;
class PlayerInfo;
typedef std::pair<SexyString, PlayerInfo> ProfilePair;
typedef std::map<SexyString, PlayerInfo, Sexy::StringLessNoCase> ProfileMap;

class ProfileMgr
{
  protected:
	ProfileMap mProfileMap;
	unsigned long mNextProfileId;
	unsigned long mNextProfileUseSeq;

  protected:
	void SyncState(ProfileSyncer &theSync);
	void DeleteOldestProfile();
	inline void DeleteOldProfiles()
	{
		while (mProfileMap.size() > 200)
			DeleteOldestProfile();
	}

  public:
	bool DeleteProfile(const SexyString &theName);

  protected:
	void DeleteProfile(ProfileMap::iterator theProfile);

  public:
	ProfileMgr()
	{
		Clear();
	}
	virtual ~ProfileMgr()
	{
		;
	}

	void Clear();
	void Load();
	void Save();
	inline int GetNumProfiles() const
	{
		return mProfileMap.size();
	}
	PlayerInfo *GetProfile(const SexyString &theName);
	PlayerInfo *AddProfile(const SexyString &theName);
	PlayerInfo *GetAnyProfile();
	bool RenameProfile(const SexyString &theOldName, const SexyString &theNewName);
	inline ProfileMap &GetProfileMap()
	{
		return mProfileMap;
	}
};

#endif
