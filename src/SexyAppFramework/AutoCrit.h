#ifndef __AUTOCRIT_INCLUDED__
#define __AUTOCRIT_INCLUDED__

#include "Common.h"
#include "CritSect.h"

namespace Sexy
{

class AutoCrit
{
	std::unique_lock<std::recursive_mutex> mLock;

  public:
	AutoCrit(std::recursive_mutex theCritSec) : mLock(theCritSec)
	{
	}

	AutoCrit(CritSect &theCritSect) : mLock(theCritSect.mCriticalSection)
	{
	}

	~AutoCrit()
	{
	}
};

} // namespace Sexy

#endif //__AUTOCRIT_INCLUDED__