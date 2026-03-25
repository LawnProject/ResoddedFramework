#ifndef _H_CritSect
#define _H_CritSect

#include "Common.h"
#include <mutex>

class CritSync;

namespace Sexy
{

class CritSect
{
  private:
	std::recursive_mutex mCriticalSection;
	friend class AutoCrit;

  public:
	CritSect(void);
	~CritSect(void);
};

} // namespace Sexy

#endif // _H_CritSect
