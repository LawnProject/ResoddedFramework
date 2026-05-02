#pragma once

#include "curl/curl.h"

#include "Common.h"

namespace Sexy
{

class HTTPTransfer
{
  public:
	enum
	{
		RESULT_DONE,
		RESULT_NOT_STARTED,
		RESULT_NOT_COMPLETED,
		RESULT_NOT_FOUND,
		RESULT_HTTP_ERROR,
		RESULT_ABORTED,
		RESULT_SOCKET_ERROR,
		RESULT_INVALID_ADDR,
		RESULT_CONNECT_FAIL,
		RESULT_DISCONNECTED
	};

	int mTransferId;
	int mSocket;
	std::string mSendStr;
	std::string mSpecifiedBaseURL;
	std::string mSpecifiedRelURL;
	std::string mURL;
	std::string mProto;

	int mContentLength;
	std::string mContent;

	bool mThreadRunning;
	bool mExiting;
	bool mAborted;
	int mResult;

	int mDemoLastKnownResult;

  private:
	std::thread mThread;

  protected:
	void PrepareTransfer();
	void StartTransfer();
	void GetHelper(const std::string &theURL);

	void PostHelper(const std::string &theURL, const std::string &theParams);

	void Fail(int theResult);
	void GetThreadProc();
	static void GetThreadProcStub(void *theArg);
	static std::string GetAbsURL(const std::string &theBaseURL, const std::string &theRelURL);

	void UpdateStatus();

  public:
	HTTPTransfer();
	virtual ~HTTPTransfer();

	void Get(const std::string &theURL);
	void Get(const std::string &theBaseURL, const std::string &theRelURL);

	void Post(const std::string &theURL, const std::string &theParams);
	void Post(const std::string &theBaseURL, const std::string &theRelURL, const std::string &theParams);

	void SendRequestString(const std::string &theHost, const std::string &theSendString);

	void Reset();
	void Abort();
	int GetResultCode();
	std::string GetContent();
};

}; // namespace Sexy
