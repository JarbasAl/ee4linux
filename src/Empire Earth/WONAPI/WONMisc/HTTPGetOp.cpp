// New HTTPGet cleanup source; complete15c operation, cache remains reference-only.
// SCRATCH only, no canonical publication/capture.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <stdio.h>
#include <string.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
namespace WONAPI {
// FUNCTION: WON 0x006d7770
void HTTPGetOp::Virtual08()
{
    AsyncOpWithTracker::Virtual08();
    if (mSocket.get()!=NULL) {
        if ((mDoClose && !mOnlySend) || mSocket->mStatus!=100004) {
            mSocket->Fn2B1AD0();
            mSocket=NULL;
        }
    }
    if (!mOnlySend && mHTTPCache.get()!=NULL && mHTTPCacheEntry.get()!=NULL)
        mHTTPCache->UpdateEntry(mHTTPCacheEntry.get());
}
// FUNCTION: WON 0x006d4370
void HTTPGetOp::DoFinish()
{
    if (mHTTPStatus>304) Fn29F220(100093);
    else Fn29F220(0);
}
// FUNCTION: WON 0x006d4550
bool HTTPGetOp::CheckStatus(WONStatus inStatus)
{
    if (inStatus==0) return true;
    Fn29F220(inStatus);
    return false;
}
// FUNCTION: WON 0x006d4570
bool HTTPGetOp::ExtractStatus(const char* inStatusLine)
{
    char protocol[50]; //native local52-byte aligned region; nominal buffer size reconstructed.
    if (sscanf(inStatusLine,"%s%d",protocol,&mHTTPStatus)!=2) {
        Fn29F220(100092);
        return false;
    }
    if (mHTTPStatus>304 && mCloseConnection) {
        Fn29F220(100093);
        return false;
    }
    return true;
}
// FUNCTION: WON 0x006d45f0
bool HTTPGetOp::ExtractRedirect(const char* inLocation)
{
    const char* host=strstr(inLocation,"://");
    if (host==NULL) {
        Fn29F220(100094);
        return false;
    }
    host+=3;
    const char* path=strchr(host,'/');
    if (path==NULL) {
        mRedirectHost=host;
        mRedirectPath='/';
    } else {
        mRedirectHost.assign(host,path-host);
        mRedirectPath=path;
    }
    return true;
}
// FUNCTION: WON 0x006d7870
 time_t HTTPGetOp::GetLastModifiedTime()
{
    if (mLastModified==0) {
        if (mLocalPath.empty()) return 0;
        WONFile file(mLocalPath);
        return mLastModified=file.GetModifyTime();
    }
    return mLastModified;
}

}

// New complete native HTTP parser/date/request cohort. Existing baseline prefix retained.
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "LostDebug.h"
using std::string;
static const char* monthTable[] = {	"Jan", "Feb", "Mar",
									"Apr", "May", "Jun",
									"Jul", "Aug", "Sep",
									"Oct", "Nov", "Dec" };


static const unsigned long Jan_Value = ('J' * 0x00010000) + ('a' * 0x00000100) + 'n';
static const unsigned long JAN_Value = ('J' * 0x00010000) + ('A' * 0x00000100) + 'N';
static const unsigned long Feb_Value = ('F' * 0x00010000) + ('e' * 0x00000100) + 'b';
static const unsigned long FEB_Value = ('F' * 0x00010000) + ('E' * 0x00000100) + 'B';
static const unsigned long Mar_Value = ('M' * 0x00010000) + ('a' * 0x00000100) + 'r';
static const unsigned long MAR_Value = ('M' * 0x00010000) + ('A' * 0x00000100) + 'R';
static const unsigned long Apr_Value = ('A' * 0x00010000) + ('p' * 0x00000100) + 'r';
static const unsigned long APR_Value = ('A' * 0x00010000) + ('P' * 0x00000100) + 'R';
static const unsigned long May_Value = ('M' * 0x00010000) + ('a' * 0x00000100) + 'y';
static const unsigned long MAY_Value = ('M' * 0x00010000) + ('A' * 0x00000100) + 'Y';
static const unsigned long Jun_Value = ('J' * 0x00010000) + ('u' * 0x00000100) + 'n';
static const unsigned long JUN_Value = ('J' * 0x00010000) + ('U' * 0x00000100) + 'N';
static const unsigned long Jul_Value = ('J' * 0x00010000) + ('u' * 0x00000100) + 'l';
static const unsigned long JUL_Value = ('J' * 0x00010000) + ('U' * 0x00000100) + 'L';
static const unsigned long Aug_Value = ('A' * 0x00010000) + ('u' * 0x00000100) + 'g';
static const unsigned long AUG_Value = ('A' * 0x00010000) + ('U' * 0x00000100) + 'G';
static const unsigned long Sep_Value = ('S' * 0x00010000) + ('e' * 0x00000100) + 'p';
static const unsigned long SEP_Value = ('S' * 0x00010000) + ('E' * 0x00000100) + 'P';
static const unsigned long Oct_Value = ('O' * 0x00010000) + ('c' * 0x00000100) + 't';
static const unsigned long OCT_Value = ('O' * 0x00010000) + ('C' * 0x00000100) + 'T';
static const unsigned long Nov_Value = ('N' * 0x00010000) + ('o' * 0x00000100) + 'v';
static const unsigned long NOV_Value = ('N' * 0x00010000) + ('O' * 0x00000100) + 'V';
static const unsigned long Dec_Value = ('D' * 0x00010000) + ('e' * 0x00000100) + 'c';
static const unsigned long DEC_Value = ('D' * 0x00010000) + ('E' * 0x00000100) + 'C';

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x006d5b60
static string MakeInternetDateTime(time_t theTime)
{
	struct tm aTime = *gmtime(&theTime);
	
	string result;

	switch (aTime.tm_wday)
	{
	case 0:
		result = "Sun";
		break;
	case 1:
		result = "Mon";
		break;
	case 2:
		result = "Tue";
		break;
	case 3:
		result = "Wed";
		break;
	case 4:
		result = "Thu";
		break;
	case 5:
		result = "Fri";
		break;
//	case 6:
	default:
		result = "Sat";
		break;
	};

	result += ", ";
	result += char((aTime.tm_mday / 10) % 10) + 48;
	result += char(aTime.tm_mday % 10) + 48;
	result += " ";
	result += monthTable[aTime.tm_mon % 12];
	result += " ";
	unsigned short year = 1900 + aTime.tm_year;
	result += char((year / 1000) % 10) + 48;
	result += char((year / 100) % 10) + 48;
	result += char((year / 10) % 10) + 48;
	result += char(year % 10) + 48;
	result += " ";
	result += char((aTime.tm_hour / 10) % 10) + 48;
	result += char(aTime.tm_hour % 10) + 48;
	result += ":";
	result += char((aTime.tm_min / 10) % 10) + 48;
	result += char(aTime.tm_min % 10) + 48;
	result += ":";
	result += char((aTime.tm_sec / 10) % 10) + 48;
	result += char(aTime.tm_sec % 10) + 48;
	result += " GMT";	// claims GMT for now

	return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Potential internet date/time formats:
//
//      Sun, 06 Nov 1994 08:49:37 GMT    ; RFC 822, updated by RFC 1123
//      Sunday, 06-Nov-94 08:49:37 GMT   ; RFC 850, obsoleted by RFC 1036
//      Sun Nov  6 08:49:37 1994         ; ANSI C's asctime() format

// returns 0 on failure
// ignores time zone for now
// FUNCTION: WON 0x006d4c60
static time_t ParseInternetDateTime(const char* str)
{
	const char* tmpStr = str;

	// skip leading spaces
	for (;*tmpStr == ' '; tmpStr++)
		;

	// skip day of week
	for (;*tmpStr != ' ' ; tmpStr++)
	{
		if (!*tmpStr)
			return 0;
	}

	// skip spaces
	for (;*tmpStr == ' '; tmpStr++)
		;

	bool gotDay = false;
	int day = 0;
	int month;

	// if the next char is numeric, then we're recving a day, else we're recving a month
	if (*tmpStr >= '0' && *tmpStr <= '9')
	{
		// read day of month
		for (;*tmpStr != ' ' && *tmpStr != '-'; tmpStr++)
		{
			if (!*tmpStr)
				return 0;

			day *= 10;
			day += *tmpStr - 48;
		}

		// skip spaces or dashes
		for (;*tmpStr == ' ' || *tmpStr == '-'; tmpStr++)
			;
		gotDay = true;
	}

	// read month

	//  Cheap trick here.  Since the month must be 3 chars, and 3 bytes fits in a long, we can make a switch statement
	unsigned long dateValue  = *(tmpStr++) * 0x00010000;
	dateValue += *(tmpStr++) * 0x00000100;
	dateValue += *(tmpStr++);
	switch (dateValue)
	{
	case Jan_Value:
	case JAN_Value:
		month = 0;
		break;
	case Feb_Value:
	case FEB_Value:
		month = 1;
		break;
	case Mar_Value:
	case MAR_Value:
		month = 2;
		break;
	case Apr_Value:
	case APR_Value:
		month = 3;
		break;
	case May_Value:
	case MAY_Value:
		month = 4;
		break;
	case Jun_Value:
	case JUN_Value:
		month = 5;
		break;
	case Jul_Value:
	case JUL_Value:
		month = 6;
		break;
	case Aug_Value:
	case AUG_Value:
		month = 7;
		break;
	case Sep_Value:
	case SEP_Value:
		month = 8;
		break;
	case Oct_Value:
	case OCT_Value:
		month = 9;
		break;
	case NOV_Value:
	case Nov_Value:
		month = 10;
		break;
	case Dec_Value:
	case DEC_Value:
		month = 11;
		break;
	default:
		return 0;
	}

	// skip spaces
	for (;*tmpStr == ' ' || *tmpStr == '-'; tmpStr++)
		;

	if (!gotDay)
	{
		// read day of month

		for (;*tmpStr != ' ' && *tmpStr != '-'; tmpStr++)
		{
			if (!*tmpStr)
				return 0;

			day *= 10;
			day += *tmpStr - 48;
		}

		// skip spaces or dashes
		for (;*tmpStr == ' ' || *tmpStr == '-'; tmpStr++)
			;
	}

	int year = 0;

	bool gotYear = false;
	if (*(tmpStr+2) != ':')	// Check to see if year preceeds time
	{
		// read year

		for (;*tmpStr != ' '; tmpStr++)
		{
			if (!*tmpStr)
			{
				if (year != 0)	// year might still be good
					break;
				return 0;
			}

			year *= 10;
			year += *tmpStr - 48;
		}
		gotYear = true;
	}

	int hour = 0;
	int mins = 0;
	int sec = 0;

	// skip spaces
	for (;*tmpStr == ' '; tmpStr++)
		;

	// parse 2 digit hour
	
	if (*tmpStr < '0' || *tmpStr > '9')
		return 0;
	
	hour = (*tmpStr - 48) * 10;
	
	tmpStr++;

	if (*tmpStr < '0' || *tmpStr > '9')
		return 0;
	
	hour += *tmpStr - 48;

	// skip ':'
	tmpStr++;

	if (*tmpStr != ':')
		return 0;

	// parse 2 digit min
	tmpStr++;

	if (*tmpStr < '0' || *tmpStr > '9')
		return 0;

	mins = (*tmpStr - 48) * 10;

	tmpStr++;

	if (*tmpStr < '0' || *tmpStr > '9')
		return 0;

	mins += *tmpStr - 48;

	// skip ':'
	tmpStr++;

	if (*tmpStr != ':')
		return 0;

	// parse 2 digit seconds
	tmpStr++;

	if (*tmpStr < '0' || *tmpStr > '9')
		return 0;

	sec = (*tmpStr - 48) * 10;

	tmpStr++;

	if (*tmpStr < '0' || *tmpStr > '9')
		return 0;

	sec += *tmpStr - 48;

	tmpStr++;

	// skip spaces
	for (;*tmpStr == ' '; tmpStr++)
		;

	if (!gotYear)
	{
		// read year

		for (;*tmpStr != ' '; tmpStr++)
		{
			if (!*tmpStr)
			{
				if (year != 0)	// year might still be good
					break;
				return 0;
			}

			year *= 10;
			year += *tmpStr - 48;
		}
	}

	if (year < 100)
		year += 1900;

	struct tm timeStruct;

	timeStruct.tm_sec = sec;
	timeStruct.tm_min = mins;
	timeStruct.tm_hour = hour;
	timeStruct.tm_mday = day;
	timeStruct.tm_mon = month;
	timeStruct.tm_year = year - 1900;
	timeStruct.tm_isdst = 0;  // GMT, no DST

	// Unused
	timeStruct.tm_wday = 0;
	timeStruct.tm_yday = 0;

	// Offset result by timezone since value is in GMT.
	return (mktime(&timeStruct) - _timezone);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace WONAPI {
// FUNCTION: WON 0x006d47e0
bool HTTPGetOp::ExtractHeaderLine(const char *theHeaderLine)
{
	const char *aColon = strchr(theHeaderLine,':');
	if(aColon==NULL)
		return true;

	const int NUM_KEYS = 9;
	static const char* keys[NUM_KEYS] = 
	{
		"Content-Length",
		"Content-Type",
		"Last-Modified",
		"Location",
		"Transfer-Encoding",
		"Connection",
		"Date",
		"Expires",
		"ETag"
	};

	int i;
	for(i=0; i<NUM_KEYS; i++)
	{
		const char *aPtr = theHeaderLine;
		int aPos = 0;
		bool match = true;
		while(aPtr!=aColon)
		{
			if(toupper(*aPtr)!=toupper(keys[i][aPos]))
			{
				match = false;
				break;
			}

			aPtr++;
			aPos++;
		}

		if(match)
			break;
	}

	const char *aVal = aColon+1;
	while(*aVal==' ')
		aVal++;

	switch(i)
	{
		case 0: mContentLength = atoi(aVal); break;
		case 1: mContentType.assign(aVal); break;
		case 2: mLastModified = ParseInternetDateTime(aVal); break;
		case 3: return ExtractRedirect(aVal); 
		case 4: 
		{
			string aStr = aVal;
			StringToUpperCaseInPlace(aStr);
			if(aStr.find("CHUNKED")==0)
			{
				mContentIsChunked = true;
				mIsFirstChunk = true;
			}
			break;
		}
		case 5: 
		{
			string aStr = aVal;
			StringToUpperCaseInPlace(aStr);
			if(aStr.find("CLOSE")==0)
				mDoClose = true;

			break;
		}
		case 6: mServerTime = ParseInternetDateTime(aVal); break;
		case 7: mExpireTime = ParseInternetDateTime(aVal); break;

		case 8:
		{
			mEntityTag = aVal;
			int anEndQuotePos = mEntityTag.find('"',1);
			if(anEndQuotePos==string::npos)
				mEntityTag.erase();
			else
				mEntityTag.resize(anEndQuotePos+1);
		}
		break;
	}

	return true;
}

// FUNCTION: WON 0x006d5270
bool HTTPGetOp::SendRequest()
{
    WriteBuffer aRequest(0,true);
    char aRangeOffset[10];
    aRequest.AppendString("GET ",0);
    if (!mProxyHost.empty())
        aRequest.AppendString("http://"+mHost,0);
    aRequest.AppendString(mRemotePath+" HTTP/1.1\r\n",0);
    aRequest.AppendString("Host: "+mHost+"\r\n",0);
    if (mCloseConnection)
        aRequest.AppendString("Connection: close\r\n",0);
    else
        aRequest.AppendString("Connection: Keep-Alive\r\n",0);
    if (!mLocalPath.empty()) {
        struct _stat aStat;
        if (_stat(mLocalPath.c_str(),&aStat)==0) {
            mLocalFileSize=aStat.st_size;
            mLocalFileModifyTime=aStat.st_mtime;
            if (mLocalFileSize>0) {
                if (mDoRangeResume) {
                    if (!mEntityTag.empty())
                        aRequest.AppendString("If-Range: "+mEntityTag+"\r\n",0);
                    else
                        aRequest.AppendString("If-Range: "+MakeInternetDateTime(mLocalFileModifyTime)+"\r\n",0);
                    aRequest.AppendString("Range: bytes=",0);
                    _snprintf(aRangeOffset,10,"%d",mLocalFileSize);
                    aRequest.AppendString(aRangeOffset,0);
                    aRequest.AppendString("-\r\n",0);
                }
                else if (mHTTPCacheEntry.get()==NULL || mHTTPCacheEntry->IsValid()) {
                    if (!mEntityTag.empty())
                        aRequest.AppendString("If-None-Match: "+mEntityTag+"\r\n",0);
                    aRequest.AppendString("If-Modified-Since: "+MakeInternetDateTime(mLocalFileModifyTime)+"\r\n",0);
                }
            }
        }
    }
    aRequest.AppendString("\r\n",0);
    if (mRunMode==1) {
        mSocket->Fn2B4740(static_cast<SocketOp*>(Track(
            new SendBytesOp(aRequest.ToByteBuffer(true).get()),HTTP_Track_SendRequest)),-1);
        if (!mOnlySend)
            AsyncRecvCRMsg(HTTP_Track_RecvStatus);
        return false;
    }
    else {
        ByteBufferPtr aBytes=aRequest.ToByteBuffer(true);
        mSocket->SendBytes(aBytes.get(),TimeLeft());
    }
    return true;
}

// FUNCTION: WON 0x006d6170
void HTTPGetOp::AsyncRecvCRMsg(TrackType inType)
{
    mSocket->Fn2B4740(static_cast<SocketOp*>(Track(new RecvCRMsgOp,inType)),-1);
}
}

namespace WONAPI {
// FUNCTION: WON 0x006d5090
bool HTTPGetOp::DoConnect()
{
    if (mSocket->Fn2B08D0())
        return true;
    IPAddr anAddress;
    anAddress.SetWithDefaultPort(mProxyHost.empty()?mHost:mProxyHost,80);
    if (mRunMode==1) {
        mSocket->Fn2B4740(static_cast<SocketOp*>(Track(
            new ConnectOp(anAddress),HTTP_Track_Connect)),-1);
        return false;
    }
    return CheckStatus(mSocket->Connect(anAddress,TimeLeft()));
}

// FUNCTION: WON 0x006d6240
bool HTTPGetOp::ExtractChunkLength(const char* inLine)
{
    if (sscanf(inLine,"%x",&mChunkLength)!=1) {
        Fn29F220(100092);
        return false;
    }
    mContentLength+=mChunkLength;
    if (mChunkLength!=0)
        return true;
    if (mHTTPCacheEntry.get()!=NULL)
        mHTTPCacheEntry->SetValid(true);
    if (mLocalPath.empty())
        mContent=mContentAccumulator.ToByteBuffer(true);
    if (mRunMode==1) {
        AsyncRecvCRMsg(HTTP_Track_RecvFooter);
        return false;
    }
    for (;;) {
        ByteBufferPtr aFooter;
        if (!CheckStatus(mSocket->RecvCRMsg(aFooter,TimeLeft())))
            return false;
        if (aFooter->length()==1)
            break;
    }
    DoFinish();
    return false;
}

// FUNCTION: WON 0x006d6430
bool HTTPGetOp::RecvChunkLength()
{
    if (mIsFirstChunk)
        mContentLength=0;
    if (mRunMode==1) {
        if (!mIsFirstChunk)
            AsyncRecvCRMsg(HTTP_Track_RecvChunkCRLF);
        AsyncRecvCRMsg(HTTP_Track_RecvChunkLength);
        return false;
    }
    ByteBufferPtr aLine;
    if (!mIsFirstChunk) {
        if (!CheckStatus(mSocket->RecvCRMsg(aLine,TimeLeft())))
            return false;
    }
    if (!CheckStatus(mSocket->RecvCRMsg(aLine,TimeLeft())))
        return false;
    return ExtractChunkLength(aLine->data());
}

// FUNCTION: WON 0x006d65c0
bool HTTPGetOp::DoneRecvContent(RecvBytesOp* inOp)
{
    mIsFirstChunk=false;
    if (mLocalPath.empty() && inOp->GetStatus()==0) {
        if (mContentIsChunked)
            mContentAccumulator.AppendBuffer(inOp->GetBytes().get(),0);
        else
            mContent=inOp->GetBytes();
    }
    if (inOp->GetStatus()!=0) {
        Fn29F220(inOp->GetStatus());
        return false;
    }
    if (mContentIsChunked)
        return RecvChunkLength();
    if (mHTTPCacheEntry.get()!=NULL)
        mHTTPCacheEntry->SetValid(true);
    DoFinish();
    return false;
}

// FUNCTION: WON 0x006d6d90
void HTTPGetOp::FinishRedirect(HTTPGetOp* inOp)
{
    mLastModified=inOp->mLastModified;
    mContentType=inOp->mContentType;
    mContentLength=inOp->mContentLength;
    mContent=inOp->mContent;
    mLocalPath=inOp->mLocalPath;
    if (!inOp->mRedirectHost.empty()) {
        mRedirectHost=inOp->mRedirectHost;
        mRedirectPath=inOp->mRedirectPath;
    }
    mHTTPStatus=inOp->mHTTPStatus;
    mContentIsNew=inOp->mContentIsNew;
    mNumRedirects=inOp->mNumRedirects;
    Fn29F220(inOp->GetStatus());
}

// FUNCTION: WON 0x006d6f80
void HTTPGetOp::FollowRedirect()
{
    mTracker.KillAll();
    if (mSocket.get()!=NULL) {
        mSocket->Close();
        mSocket=NULL;
    }
    if (mNumRedirects>5) {
        Fn29F220(100095);
        return;
    }
    if (!mAutoRedirect) {
        Fn29F220(100096);
        return;
    }
    SmartPtr<HTTPGetOp> anOp=new HTTPGetOp(mRedirectHost,mRedirectPath);
    anOp->SetBinaryFile(mBinaryFile);
    anOp->SetLocalPath(mLocalPath);
    anOp->SetHTTPCache(mHTTPCache);
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(1);
    anOp->mNumRedirects=mNumRedirects+1;
    if (mRecvChunkCompletion.get()!=NULL)
        anOp->SetRecvChunkCompletion(mRecvChunkCompletion.get(),mRecvChunkSize);
    if (mRunMode==1) {
        Track(anOp.get(),HTTP_Track_Redirect);
        anOp->Run(-1);
        return;
    }
    anOp->RunBlock(TimeLeft());
    FinishRedirect(anOp.get());
}

// FUNCTION: WON 0x006d7140
void HTTPGetOp::DoneRecvHeader()
{
    if (mHTTPStatus==301 || mHTTPStatus==302) {
        FollowRedirect();
        return;
    }
    HTTPCacheEntry* anEntry=mHTTPCacheEntry.get();
    if (anEntry!=NULL) {
        if (mServerTime>0 && mExpireTime>0 && mExpireTime>mServerTime)
            anEntry->SetExpirationTime(time(NULL)+mExpireTime-mServerTime);
        if (!mEntityTag.empty())
            mHTTPCacheEntry->SetEntityTag(mEntityTag);
    }
    if (mContentIsChunked && !RecvChunkLength())
        return;
    RecvContent();
}

}
namespace WONAPI {
// FUNCTION: WON 0x006d6730
void HTTPGetOp::RecvContent()
{
    if (mHTTPStatus==304) {
        mContentIsNew=false;
        DoFinish();
        return;
    }
    bool skip=mHTTPStatus<200 || mHTTPStatus>=300;
    if (skip && mDoClose) {
        Fn29F220(100093);
        return;
    }
    for (;;) {
        unsigned long length=mContentIsChunked?mChunkLength:mContentLength;
        SmartPtr<RecvBytesOp> recv=new RecvBytesOp(length,mSocket.get());
        if (skip) recv->mB80=true;
        else {
            if (mRecvChunkCompletion.get()!=NULL)
                recv->Fn2B4F90(mRecvChunkCompletion.get(),mRecvChunkSize);
            if (mHTTPCacheEntry.get()!=NULL) mHTTPCacheEntry->SetValid(false);
            if (!mLocalPath.empty()) {
                bool append=mHTTPStatus==206 || (mContentIsChunked && !mIsFirstChunk);
                if (!append && mLocalFileSize>0 && mLocalFileSize==mContentLength &&
                    abs(mLocalFileModifyTime-mLastModified)<=1 && mDoClose) {
                    DoFinish();
                    mContentIsNew=false;
                    return;
                }
                recv->mBinary=mBinaryFile;
                recv->mPath=mLocalPath;
                recv->mAppend=append;
                recv->mFileModifyTime=mLastModified;
            }
        }
        if (mRunMode==1) {
            Track(recv.get(),HTTP_Track_RecvContent);
            recv->Run(-1);
            return;
        }
        if (recv->RunBlock(TimeLeft())) {
            if (!DoneRecvContent(recv.get())) return;
        } else {
            Fn29F220(recv->mStatus);
            return;
        }
    }
}
// FUNCTION: WON 0x006d6a50
bool HTTPGetOp::CallbackHook(AsyncOp* inOp,int inParam)
{
    if (inOp->mB39) {
        Kill(true);
        return true;
    }
    if (inOp->mStatus!=0) {
        Fn29F220(inOp->mStatus);
        return true;
    }
    switch (inParam) {
    case HTTP_Track_Connect:
        SendRequest();
        break;
    case HTTP_Track_SendRequest:
        if (mOnlySend) Fn29F220(100097);
        break;
    case HTTP_Track_RecvStatus: {
        RecvCRMsgOp* op=static_cast<RecvCRMsgOp*>(inOp);
        ByteBufferPtr line=op->GetMsg();
        if (!ExtractStatus(line->data())) break;
        AsyncRecvCRMsg(HTTP_Track_RecvHeader);
        break;
    }
    case HTTP_Track_RecvHeader: {
        RecvCRMsgOp* op=static_cast<RecvCRMsgOp*>(inOp);
        ByteBufferPtr line=op->GetMsg();
        if (line->length()==1) {
            DoneRecvHeader();
            break;
        }
        if (!ExtractHeaderLine(line->data())) break;
        AsyncRecvCRMsg(HTTP_Track_RecvHeader);
        break;
    }
    case HTTP_Track_RecvFooter: {
        RecvCRMsgOp* op=static_cast<RecvCRMsgOp*>(inOp);
        ByteBufferPtr line=op->GetMsg();
        if (line->length()==1) DoFinish();
        else AsyncRecvCRMsg(HTTP_Track_RecvFooter);
        break;
    }
    case HTTP_Track_RecvContent:
        DoneRecvContent(static_cast<RecvBytesOp*>(inOp));
        break;
    case HTTP_Track_Redirect:
        FinishRedirect(static_cast<HTTPGetOp*>(inOp));
        break;
    case HTTP_Track_RecvChunkLength: {
        RecvCRMsgOp* op=static_cast<RecvCRMsgOp*>(inOp);
        ByteBufferPtr line=op->GetMsg();
        if (ExtractChunkLength(line->data())) RecvContent();
        break;
    }
    default:
        return false;
    }
    return true;
}
}

// Compiler-generated singleton/lazy getter support below carries zero source quota.
#include "ee/wonapi/WONLobbyGUI.h"
using WONAPI::CriticalSection;
static string& GetProxyHostFileName()
{
	static string mProxyHostFileName = "_wonwebproxy.txt";
	return mProxyHostFileName;
}
static CriticalSection& GetStaticCrit()
{
	static CriticalSection aCrit;
	return aCrit;
}

namespace WONAPI {
std::string HTTPGetOp::mStaticProxyHost;
// FUNCTION: WON 0x006d3830
void HTTPGetOp::ReadProxyHostFile(bool force)
{
	static bool checked = false;
	AutoCrit aCrit(GetStaticCrit());
	if(checked && !force)
		return;

	checked = true;

	FILE *aFile = fopen(GetProxyHostFileName().c_str(),"r");
	if(aFile==NULL)
		return;

	char aProxyBuf[512];
	aProxyBuf[0] = '\0';
	fgets(aProxyBuf,500,aFile);
	fclose(aFile);

	char *aPtr = strchr(aProxyBuf,'\n');
	if(aPtr!=NULL)
		*aPtr = '\0';

	mStaticProxyHost = aProxyBuf;
}

// FUNCTION: WON 0x006d3b10
HTTPGetOp::HTTPGetOp(const std::string& inHost,const std::string& inRemotePath)
    : mHost(inHost),mRemotePath(inRemotePath),mContentAccumulator(0,true),
      mNumRedirects(0)
{
    mBinaryFile=true;
    mCloseConnection=true;
    mDoRangeResume=true;
    mAutoRedirect=true;
    mOnlyRecv=false;
    mOnlySend=false;
    GetProxyHostFileName()=WONAPICore::GetDefaultFileDirectory()+GetProxyHostFileName();
    ReadProxyHostFile(false);
}

// FUNCTION: WON 0x006d3f90
HTTPGetOp::HTTPGetOp(const std::string& inURL)
    : mContentAccumulator(0,true),mNumRedirects(0)
{
    mBinaryFile=true;
    mCloseConnection=true;
    mDoRangeResume=true;
    mAutoRedirect=true;
    mOnlyRecv=false;
    mOnlySend=false;
    GetProxyHostFileName()=WONAPICore::GetDefaultFileDirectory()+GetProxyHostFileName();
    ReadProxyHostFile(false);
    std::string aURL(inURL);
    std::string::size_type aScheme=aURL.find("://");
    if (aScheme!=std::string::npos)
        aURL=aURL.substr(aScheme+3);
    std::string::size_type aSlash=aURL.find("/");
    mHost=aURL.substr(0,aSlash);
    if (aSlash!=std::string::npos)
        mRemotePath=aURL.substr(aSlash);
    else
        mRemotePath="/";
}

// FUNCTION: WON 0x006d7330
void HTTPGetOp::Virtual04()
{
    if (!mOnlyRecv) {
        if (!mStaticProxyHost.empty())
            mProxyHost=mStaticProxyHost;
        mLastModified=0;
        mServerTime=0;
        mExpireTime=0;
        mContentType.erase();
        mContentLength=0;
        mContent=NULL;
        mDoClose=mCloseConnection;
        mHTTPStatus=0;
        mLocalFileModifyTime=0;
        mLocalFileSize=0;
        mNumRedirects=0;
        mContentIsNew=true;
        mChunkLength=0;
        mContentIsChunked=false;
        mIsFirstChunk=false;
        mHTTPCacheEntry=NULL;
        mContentAccumulator.Reset();
        if (mSocket.get()==NULL)
            mSocket=new BlockingSocket(0);
        mRedirectHost="";
        mRedirectPath="";
        if (mHTTPCache.get()!=NULL) {
            mHTTPCacheEntry=mHTTPCache->GetEntry(mHost+mRemotePath);
            if (mHTTPCacheEntry.get()!=NULL) {
                mLocalPath=mHTTPCacheEntry->GetLocalPath();
                mEntityTag=mHTTPCacheEntry->GetEntityTag();
                if (mHTTPCacheEntry->IsValid() &&
                    mHTTPCacheEntry->GetExpirationTime()>time(NULL)) {
                    mContentIsNew=false;
                    Fn29F220(0);
                    return;
                }
            }
        }
        if (!DoConnect())
            return;
        if (!SendRequest())
            return;
    }
    if (mOnlySend) {
        Fn29F220(100097);
        return;
    }
    if (mOnlyRecv && mRunMode==1) {
        AsyncRecvCRMsg(HTTP_Track_RecvStatus);
        return;
    }
    ByteBufferPtr aLine;
    if (!CheckStatus(mSocket->RecvCRMsg(aLine,TimeLeft())))
        return;
    if (!ExtractStatus(aLine->data()))
        return;
    for (;;) {
        if (!CheckStatus(mSocket->RecvCRMsg(aLine,TimeLeft())))
            return;
        if (aLine->length()==1)
            break;
        if (!ExtractHeaderLine(aLine->data()))
            return;
    }
    DoneRecvHeader();
}

}

namespace WONAPI {
// FUNCTION: WON 0x006d44e0
void HTTPGetOp::SetRecvChunkCompletion(CompletionBase<AsyncOpPtr>* completion,DWORD chunkSize)
{
    mRecvChunkCompletion=completion;
    mRecvChunkSize=chunkSize;
}
// FUNCTION: WON 0x006d77f0
void HTTPGetOp::SetSocket(BlockingSocket* socket) { mSocket=socket; }
// FUNCTION: WON 0x006d7850
BlockingSocket* HTTPGetOp::GetSocket()
{
    if(mSocket.get()==NULL) return NULL;
    if(!mSocket->Fn2B08D0()) return NULL;
    return mSocket.get();
}
}

namespace WONAPI {
// FUNCTION: WON 0x006d4390
void HTTPGetOp::SetProxy(const std::string& hostAndPort)
{
    mProxyHost=hostAndPort;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing executable methods.
namespace WONAPI {
bool HTTPGetOp::WriteProxyHostFile(const std::string &theHostAndPort)
{
	AutoCrit aCrit(GetStaticCrit());
	mStaticProxyHost = theHostAndPort;

	if(theHostAndPort.empty())
		return remove(GetProxyHostFileName().c_str())==0;

	FILE *aFile = fopen(GetProxyHostFileName().c_str(),"w");
	if(aFile==NULL)
		return false;

	fprintf(aFile,"%s\n",theHostAndPort.c_str());
	fclose(aFile);
	return true;
}

const std::string& HTTPGetOp::GetStaticProxyHost()
{
	ReadProxyHostFile(false);
	return mStaticProxyHost;
}

void HTTPGetOp::SetProxyHostFileName(std::string &theFileName) // set file name and path for proxy info file
{
	AutoCrit aCrit(GetStaticCrit());
	GetProxyHostFileName() = theFileName;
}

void HTTPGetOp::SetProxy(const std::string &theHost, unsigned short thePort)
{
	char aBuf[50];
	sprintf(aBuf,":%d",thePort);
	mProxyHost = theHost + aBuf;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006d3a70
// (_$E72    [_$E72])
// TEMPLATE: WON 0x006d3b00
// (_$E74    [_$E74])

// TEMPLATE: WON 0x006d3d30
// (HTTPGetOp scalar deleting destructor)
// TEMPLATE: WON 0x006d3d50
// (HTTPGetOp implicit destructor)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006d39c0
// (class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > & __cdecl GetProxyHostFileName(void)    [?GetProxyHostFileName)
// TEMPLATE: WON 0x006d3ac0
// (class WONAPI::CriticalSection & __cdecl GetStaticCrit(void)    [?GetStaticCrit@@YAAAVCriticalSection@WONAPI@@XZ])

// TEMPLATE: WON 0x006cfe50
// (AsyncOpTracker no-argument constructor: the inline refused here, kept out of line)
