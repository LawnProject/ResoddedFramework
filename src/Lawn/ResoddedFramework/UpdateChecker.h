#pragma once

#include <Common.h>
#include <Version.h>

enum UpdateCheckResult
{
	RESULT_SUCCESS,
	RESULT_INVALID_FORMAT,
	RESULT_FAIL,
};

class UpdateChecker
{
  public:
	static SexyString gUpdateHost;
	static Sexy::Version gLatestVersion;
	static bool gIsOutdated;

  public:
	static UpdateCheckResult Check();

  private:
};
