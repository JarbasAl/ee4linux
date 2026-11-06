#ifndef EE_WONAPI_WONFTP_H
#define EE_WONAPI_WONFTP_H
#include "ee/wonapi/WONRouting.h"
// Complete genuine SDK new owner; SOURCE_ONLY_NO_NATIVE_ID, no target allocation/VT claim.



namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FTPGetOp : public AsyncOpWithTracker
{
private:
	SmartPtr<BlockingSocket> mRequestSocket;
	SmartPtr<BlockingSocket> mDataSocket;

	std::string mHost;
	std::string mRemotePath;
	std::string mLocalPath;
	std::string mUserName;
	std::string mPassword;

	std::string mActualPath;
	std::string mActualFile;

	ConstSmartPtr<ByteBuffer> mContent;

	int mFTPStatus;

	OpCompletionBasePtr mRecvChunkCompletion;
	DWORD mRecvChunkSize;

	IPAddr mDataAddr;
	bool mIsBinary;
	bool mDoResume;
	DWORD mLocalFileSize;

	void Init();

private:
	
	enum TrackType
	{
		FTP_Track_RequestConnect = 1,
		FTP_Track_Request = 2,
		FTP_Track_DataConnect = 3,
		FTP_Track_RecvContent = 4
	};

	enum RequestState
	{
		RequestState_Start,
		RequestState_User,
		RequestState_Password,
		RequestState_ChangeDir,
		RequestState_ChangeType,
		RequestState_Passive,
		RequestState_Restart,
		RequestState_Retrieve
	};

	RequestState mRequestState;

	bool ExtractStatus(const char *theStatusLine);
	bool CheckStatus(WONStatus theStatus);
	bool ParsePasv(const char *theStatusLine);

	bool DoConnect();
	void RecvLineAsync();
	bool SendRequest();
	bool HandleReply(RecvCRMsgOp *theReply);

	bool DoDataConnection();
	void RecvContent();

	void ExtractUserNameAndPasswordFromURL(std::string &theURL);

protected:
	virtual void Virtual04();
	virtual void Virtual08();
	virtual bool CallbackHook(AsyncOp *theOp, int theParam);
		

public:
	FTPGetOp(const std::string &theHost, const std::string &theRemotePath);
	FTPGetOp(const std::string &theURL, bool doExtractUserAndPassword = false);

	void SetLocalPath(const std::string &thePath) { mLocalPath = thePath; }
	const std::string& GetLocalPath() { return mLocalPath; }

	void SetRemotePath(const std::string &thePath);
	const std::string& GetRemotePath() { return mRemotePath; }

	void SetUserName(const std::string &theUserName) { mUserName = theUserName; }
	void SetPassword(const std::string &thePassword) { mPassword = thePassword; }
	void SetIsBinary(bool isBinary) { mIsBinary = isBinary; } 
	void SetDoResume(bool doResume) { mDoResume = doResume; }

	void SetRecvChunkCompletion(CompletionBase<AsyncOpPtr> *theCompletion, DWORD theChunkSize = 10000);

	ConstSmartPtr<ByteBuffer> GetContent() { return mContent; }

	int GetFTPStatus() { return mFTPStatus; }

};

typedef SmartPtr<FTPGetOp> FTPGetOpPtr;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


}; // namespace WONAPI

#endif
