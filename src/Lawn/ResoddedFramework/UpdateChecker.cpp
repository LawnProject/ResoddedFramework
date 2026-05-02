#include "UpdateChecker.h"
#include "../../LawnApp.h"
#include <HTTPTransfer.h>
#include <sstream>

using namespace Sexy;

SexyString UpdateChecker::gUpdateHost;
Version UpdateChecker::gLatestVersion;
bool UpdateChecker::gIsOutdated = false;

UpdateCheckResult UpdateChecker::Check()
{
	HTTPTransfer aTransfer;
	aTransfer.Get(gUpdateHost);

	while (true)
	{
		int aResult = aTransfer.GetResultCode();
		bool canLeave = false;
		switch (aResult)
		{
			case HTTPTransfer::RESULT_DONE:
				canLeave = true;
				break;
			case HTTPTransfer::RESULT_NOT_FOUND:
			case HTTPTransfer::RESULT_DISCONNECTED:
			case HTTPTransfer::RESULT_CONNECT_FAIL:
			case HTTPTransfer::RESULT_SOCKET_ERROR:
			case HTTPTransfer::RESULT_INVALID_ADDR:
			case HTTPTransfer::RESULT_ABORTED:
			case HTTPTransfer::RESULT_HTTP_ERROR:
				return RESULT_FAIL;
		}
		if (canLeave)
			break;
	}

	SexyString aVersionResult = aTransfer.GetContent().substr(1); //remove the 'v' prefix from the version

	size_t pos = aVersionResult.find('.');
	gLatestVersion.mMajor = atoi(aVersionResult.substr(0, pos).c_str());

	size_t start = pos + 1;
	pos = aVersionResult.find('.', start);

	gLatestVersion.mMinor = atoi(aVersionResult.substr(start, pos - start).c_str());

	start = pos + 1;
	gLatestVersion.mPatch = atoi(aVersionResult.substr(start).c_str());

	gIsOutdated = LawnApp::gVersion.isOlderThan(gLatestVersion);
	return RESULT_SUCCESS;
}