// Include windows ourselves to prevent curl.h from including windows and leak macros
// IWYU pragma: begin_exports <- this suppresses clangd warning
#include "Platform.h"
// IWYU pragma: end_exports
// this comment exists to prevent formatter from reordering this include

#include "HTTPTransfer.h"
#include "SexyAppBase.h"
#include <curl/curl.h>

using namespace Sexy;

static int gCurTransferId = 1;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t totalSize = size * nmemb;
	std::string *output = static_cast<std::string *>(userp);
	output->append(static_cast<char *>(contents), totalSize);
	return totalSize;
}

static int ProgressCallback(void *clientp, curl_off_t, curl_off_t, curl_off_t, curl_off_t)
{
	HTTPTransfer *self = (HTTPTransfer *)clientp;

	if (self->mAborted || self->mExiting)
		return 1;

	return 0;
}

HTTPTransfer::HTTPTransfer()
{
	mTransferId = gCurTransferId++;
	mResult = RESULT_NOT_STARTED;
	mContentLength = 0;
	mDemoLastKnownResult = mResult;
	mThreadRunning = false;
}

HTTPTransfer::~HTTPTransfer()
{
	Abort();
	if (mThread.joinable())
		mThread.join();

	curl_global_cleanup();
}

std::string HTTPTransfer::GetAbsURL(const std::string &theBaseURL, const std::string &theRelURL)
{
	if (theRelURL.find("://") != std::string::npos)
		return theRelURL; // already absolute

	if (theBaseURL.empty())
		return theRelURL;

	std::string aBaseURL = theBaseURL;
	if (aBaseURL.back() != '/' && theRelURL.front() != '/')
		aBaseURL += "/";

	return aBaseURL + theRelURL;
}

void HTTPTransfer::Fail(int theResult)
{
	mResult = theResult;
	mExiting = true;
}

void HTTPTransfer::GetThreadProc()
{
	if (mSendStr.empty())
		GetHelper(mURL);
	else
		PostHelper(mURL, mSendStr);

	if (mAborted)
		Fail(RESULT_ABORTED);
	else if (mResult == RESULT_NOT_COMPLETED)
		mResult = RESULT_DONE;

	mThreadRunning = false;
}

void HTTPTransfer::GetThreadProcStub(void *theArg)
{
	((HTTPTransfer *)theArg)->GetThreadProc();
}

void HTTPTransfer::PrepareTransfer()
{
	Reset();

	mTransferId = gCurTransferId++;
	mExiting = false;
	mAborted = false;
	mResult = RESULT_NOT_COMPLETED;
	mContent.erase();
	mContentLength = 0;
}

void HTTPTransfer::StartTransfer()
{
	// Don't really start the transfer while in demo playing mode
	if ((gSexyAppBase != NULL) && (gSexyAppBase->mPlayingDemoBuffer))
		return;

	mThreadRunning = true;
	mThread = std::thread(&HTTPTransfer::GetThreadProc, this);
}

void HTTPTransfer::GetHelper(const std::string &theURL)
{
	PrepareTransfer();

	CURL *curl = curl_easy_init();
	if (!curl)
	{
		Fail(RESULT_HTTP_ERROR);
		return;
	}

	curl_easy_setopt(curl, CURLOPT_URL, theURL.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
	curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mContent);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		printf("CURL error: %s (%d)", curl_easy_strerror(res), (int)res);
		Fail(RESULT_HTTP_ERROR);
	}
	else
	{
		long response_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

		mResult = (response_code == 200) ? RESULT_DONE : RESULT_HTTP_ERROR;
		mContentLength = static_cast<int>(mContent.size());
	}

	curl_easy_cleanup(curl);
}

void HTTPTransfer::PostHelper(const std::string &theURL, const std::string &theParams)
{
	PrepareTransfer();

	CURL *curl = curl_easy_init();
	if (!curl)
	{
		Fail(RESULT_HTTP_ERROR);
		return;
	}

	curl_easy_setopt(curl, CURLOPT_URL, theURL.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, theParams.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
	curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mContent);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		Fail(RESULT_HTTP_ERROR);
	}
	else
	{
		long response_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		if (response_code == 200)
			mResult = RESULT_DONE;
		else if (response_code == 404)
			mResult = RESULT_NOT_FOUND;
		else
			mResult = RESULT_HTTP_ERROR;
		mContentLength = static_cast<int>(mContent.size());
	}

	curl_easy_cleanup(curl);
}

void HTTPTransfer::Get(const std::string &theURL)
{
	mSpecifiedBaseURL = "";
	mSpecifiedRelURL = theURL;
	GetHelper(theURL);
}

void HTTPTransfer::Post(const std::string &theURL, const std::string &theParams)
{
	mSpecifiedBaseURL = "";
	mSpecifiedRelURL = theURL;
	PostHelper(theURL, theParams);
}

void HTTPTransfer::Get(const std::string &theBaseURL, const std::string &theRelURL)
{
	mSpecifiedBaseURL = theBaseURL;
	mSpecifiedRelURL = theRelURL;
	mURL = GetAbsURL(theBaseURL, theRelURL);
	mSendStr.clear();
	StartTransfer();
}

void HTTPTransfer::Post(const std::string &theBaseURL, const std::string &theRelURL, const std::string &theParams)
{
	mSpecifiedBaseURL = theBaseURL;
	mSpecifiedRelURL = theRelURL;
	mURL = GetAbsURL(theBaseURL, theRelURL);
	mSendStr.clear();
	StartTransfer();
}

void HTTPTransfer::SendRequestString(const std::string &theHost, const std::string &theSendString)
{
	mSpecifiedBaseURL.erase();
	mSpecifiedRelURL.erase();
	PrepareTransfer();
	mURL = theHost;
	mSendStr = theSendString;
	StartTransfer();
}

void HTTPTransfer::Abort()
{
	mAborted = true;
	mExiting = true;
}

void HTTPTransfer::Reset()
{
	if (mThreadRunning)
	{
		Abort();
	}

	mResult = RESULT_NOT_STARTED;
	mDemoLastKnownResult = RESULT_NOT_STARTED;
	mTransferId = gCurTransferId++;
	mContent.erase();
	mExiting = false;
	mAborted = false;
}

static int aLastThreadId = 0;

void HTTPTransfer::UpdateStatus()
{
	// This will save the result data in demo recording mode and load it in (if available)
	//  in demo playback mode

	if (gSexyAppBase != NULL)
	{
		if (gSexyAppBase->mPlayingDemoBuffer)
		{
			// We only need to try to get the result if we think we are waiting for one
			if (gSexyAppBase->PrepareDemoCommand(false))
			{
				if ((!gSexyAppBase->mDemoIsShortCmd) && (gSexyAppBase->mDemoCmdNum == DEMO_HTTP_RESULT))
				{
					int anOldBufferPos = gSexyAppBase->mDemoBuffer.mReadBitPos;

					// Since we don't require a demo result entry to be here, we must verify
					//  that this is referring to us
					int aTransferId = gSexyAppBase->mDemoBuffer.ReadLong();

					if (aTransferId == mTransferId)
					{
						// We finally got our result!
						mResult = gSexyAppBase->mDemoBuffer.ReadByte();
						mContent = gSexyAppBase->mDemoBuffer.ReadString();
						mDemoLastKnownResult = mResult;
						gSexyAppBase->mDemoNeedsCommand = true;

						//TODO:
						//OutputDebugString(StrFormat("Found State Change on %s (Id %d) to %d at %d\r\n", mURL.c_str(), mTransferId, mResult, gSexyAppBase->mUpdateCount).c_str());
					}
					else
					{
						// Not us, go back
						gSexyAppBase->mDemoBuffer.mReadBitPos = anOldBufferPos;
					}
				}
			}
		}
		else if ((gSexyAppBase->mRecordingDemoBuffer) && (mResult != mDemoLastKnownResult))
		{
			//TODO:
			//OutputDebugString(StrFormat("Recording State Change on %s (Id %d) to %d at %d\r\n", mURL.c_str(), mTransferId, mResult, gSexyAppBase->mUpdateCount).c_str());

			// Write out the new result
			gSexyAppBase->WriteDemoTimingBlock();
			gSexyAppBase->mDemoBuffer.WriteNumBits(0, 1);
			gSexyAppBase->mDemoBuffer.WriteNumBits(DEMO_HTTP_RESULT, 5);
			gSexyAppBase->mDemoBuffer.WriteLong(mTransferId);
			gSexyAppBase->mDemoBuffer.WriteByte(mResult);

			// Avoid any threading issues by not allowing access to mContent while in progress
			if (mResult == RESULT_NOT_COMPLETED)
				gSexyAppBase->mDemoBuffer.WriteString("");
			else
				gSexyAppBase->mDemoBuffer.WriteString(mContent);

			mDemoLastKnownResult = mResult;
		}
	}
}

int HTTPTransfer::GetResultCode()
{
	UpdateStatus();
	return mResult;
}

std::string HTTPTransfer::GetContent()
{
	if (mResult == RESULT_NOT_COMPLETED)
		return "";

	UpdateStatus();
	return mContent;
}
