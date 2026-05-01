#include "PerfTimer.h"
#include <map>
#include <SDL3/SDL_timer.h>

using namespace Sexy;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline uint64_t QueryCounters(uint64_t *out)
{
	*out = SDL_GetPerformanceCounter();
	return *out;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline uint64_t DeltaCounters(uint64_t *previous)
{
	uint64_t now = SDL_GetPerformanceCounter();
	uint64_t delta = now - *previous;
	*previous = now;
	return delta;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PerfTimer::PerfTimer()
{
	mDuration = 0;
	mStart = 0;
	mRunning = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PerfTimer::CalcDuration()
{
	uint64_t anEnd, aFreq;
	anEnd = SDL_GetPerformanceCounter();
	aFreq = SDL_GetPerformanceFrequency();
	mDuration = ((anEnd - mStart) * 1000.0) / (double)aFreq;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PerfTimer::Start()
{
	mRunning = true;
	mStart = SDL_GetPerformanceCounter();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PerfTimer::Stop()
{
	if (mRunning)
	{
		CalcDuration();
		mRunning = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
double PerfTimer::GetDuration()
{
	if (mRunning)
		CalcDuration();

	return mDuration;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct PerfInfo
{
	const char *mPerfName;
	mutable __int64 mStartTime;
	mutable __int64 mDuration;
	mutable double mMillisecondDuration;
	mutable double mLongestCall;
	mutable int mStartCount;
	mutable int mCallCount;

	PerfInfo(const char *theName)
		: mPerfName(theName), mStartTime(0), mDuration(0), mStartCount(0), mCallCount(0), mLongestCall(0)
	{
	}

	bool operator<(const PerfInfo &theInfo) const
	{
		return stricmp(mPerfName, theInfo.mPerfName) < 0;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef std::set<PerfInfo> PerfInfoSet;
static PerfInfoSet gPerfInfoSet;
static bool gPerfOn = false;
static uint64_t gStartTime;
static uint64_t gCollateTime;
double gDuration = 0;
int gStartCount = 0;
int gPerfRecordTop = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct PerfRecord
{
	const char *mName;
	uint64_t mTime;
	bool mStart;

	PerfRecord()
	{
	}
	PerfRecord(const char *theName, bool start) : mName(theName), mStart(start)
	{
		QueryCounters(&mTime);
	}
};
typedef std::vector<PerfRecord> PerfRecordVector;
PerfRecordVector gPerfRecordVector;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline void InsertPerfRecord(PerfRecord &theRecord)
{
	if (theRecord.mStart)
	{
		PerfInfoSet::iterator anItr = gPerfInfoSet.insert(PerfInfo(theRecord.mName)).first;
		anItr->mCallCount++;

		if (++anItr->mStartCount == 1)
			anItr->mStartTime = theRecord.mTime;
	}
	else
	{
		PerfInfoSet::iterator anItr = gPerfInfoSet.find(theRecord.mName);
		if (anItr != gPerfInfoSet.end())
		{
			if (--anItr->mStartCount == 0)
			{
				__int64 aDuration = theRecord.mTime - anItr->mStartTime;
				anItr->mDuration += aDuration;

				if (aDuration > anItr->mLongestCall)
					anItr->mLongestCall = (double)aDuration;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline void CollatePerfRecords()
{
	uint64_t aTime1, aTime2;
	QueryCounters(&aTime1);

	for (int i = 0; i < gPerfRecordTop; i++)
		InsertPerfRecord(gPerfRecordVector[i]);

	gPerfRecordTop = 0;
	QueryCounters(&aTime2);

	gCollateTime += aTime2 - aTime1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline void PushPerfRecord(PerfRecord &theRecord)
{
	if (gPerfRecordTop >= (int)gPerfRecordVector.size())
		gPerfRecordVector.push_back(theRecord);
	else
		gPerfRecordVector[gPerfRecordTop] = theRecord;

	++gPerfRecordTop;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SexyPerf::IsPerfOn()
{
	return gPerfOn;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SexyPerf::BeginPerf(bool measurePerfOverhead)
{
	gPerfInfoSet.clear();
	gPerfRecordTop = 0;
	gStartCount = 0;
	gCollateTime = 0;

	if (!measurePerfOverhead)
		gPerfOn = true;

	QueryCounters(&gStartTime);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SexyPerf::EndPerf()
{
	uint64_t anEndTime;
	QueryCounters(&anEndTime);

	CollatePerfRecords();

	gPerfOn = false;

	__int64 aFreq = SDL_GetPerformanceFrequency();

	gDuration = ((double)(anEndTime - gStartTime - gCollateTime)) * 1000 / aFreq;

	for (PerfInfoSet::iterator anItr = gPerfInfoSet.begin(); anItr != gPerfInfoSet.end(); ++anItr)
	{
		const PerfInfo &anInfo = *anItr;
		anInfo.mMillisecondDuration = (double)anInfo.mDuration * 1000 / aFreq;
		anInfo.mLongestCall = anInfo.mLongestCall * 1000 / aFreq;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SexyPerf::StartTiming(const char *theName)
{
	if (gPerfOn)
	{
		++gStartCount;
		PerfRecord aRecord(theName, true);
		PushPerfRecord(aRecord);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SexyPerf::StopTiming(const char *theName)
{
	if (gPerfOn)
	{
		PerfRecord aRecord(theName, false);
		PushPerfRecord(aRecord);
		if (--gStartCount == 0)
			CollatePerfRecords();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string SexyPerf::GetResults()
{
	std::string aResult;
	char aBuf[512];

	sprintf(aBuf, "Total Time: %.2f\n", gDuration);
	aResult += aBuf;
	for (PerfInfoSet::iterator anItr = gPerfInfoSet.begin(); anItr != gPerfInfoSet.end(); ++anItr)
	{
		const PerfInfo &anInfo = *anItr;
		sprintf(aBuf,
				"%s (%d calls, %%%.2f time): %.2f (%.2f avg, %.2f longest)\n",
				anInfo.mPerfName,
				anInfo.mCallCount,
				anInfo.mMillisecondDuration / gDuration * 100,
				anInfo.mMillisecondDuration,
				anInfo.mMillisecondDuration / anInfo.mCallCount,
				anInfo.mLongestCall);
		aResult += aBuf;
	}

	return aResult;
}
