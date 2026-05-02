#pragma once

#include <Common.h>

enum UpdateCheckResult
{
	RESULT_SUCCESS,
	RESULT_INVALID_FORMAT,
	RESULT_HTTP_ERROR,
};

class UpdateChecker
{
  public:
	static SexyString gUpdateHost;

  public:
	static UpdateCheckResult Check();

  private:
};
