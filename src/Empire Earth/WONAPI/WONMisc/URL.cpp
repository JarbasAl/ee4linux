// Native URL parser and relative URL resolution.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <string.h>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONURL.h"
namespace WONAPI {
// FUNCTION: WON 0x006d7a20
URL::URL(const std::string& inURL)
{
    mIsAbsolute=false;
    int protocolEnd=inURL.find("://");
    if (protocolEnd!=std::string::npos) {
        std::string protocol=inURL.substr(0,protocolEnd);
        if (stricmp(protocol.c_str(),"HTTP")==0) mType=URLType_HTTP;
        else if (stricmp(protocol.c_str(),"FTP")==0) mType=URLType_FTP;
        else if (stricmp(protocol.c_str(),"FILE")==0) mType=URLType_File;
        else {
            mType=URLType_None;
            return;
        }
        mIsAbsolute=true;
        protocolEnd+=3;
        int addressEnd=inURL.find_first_of('/',protocolEnd);
        mAddress=inURL.substr(protocolEnd,addressEnd-protocolEnd);
        if (addressEnd==std::string::npos) mPath="/";
        else mPath=inURL.substr(addressEnd);
    } else {
        mType=URLType_File;
        mPath=inURL;
    }
    if (mType==URLType_File) {
        for (int i=0;i<mPath.length();++i)
            if (mPath[i]=='\\') mPath[i]='/';
    }
}
// FUNCTION: WON 0x006d7d10
std::string URL::GetPathAsLocalFilePath() const
{
    WONFile file(mPath);
    return file.GetFilePath();
}
// FUNCTION: WON 0x006d7e90
void URL::CalcRelativeTo(const URL& inBase)
{
    if (mIsAbsolute) return;
    mIsAbsolute=inBase.mIsAbsolute;
    mType=inBase.mType;
    mAddress=inBase.mAddress;
    if (mPath[0]=='#') {
        mPath=inBase.mPath+mPath;
        return;
    }
    if (!mPath.empty() && !inBase.mPath.empty() && mPath[0]!='/') {
        std::string relativePath=mPath;
        mPath=inBase.mPath;
        int slash=mPath.rfind('/');
        if (slash==std::string::npos) mPath+='/';
        else mPath=mPath.substr(0,slash+1);
        mPath+=relativePath;
    }
}
// FUNCTION: WON 0x006d8150
void URL::GetEntire(std::string& outString)
{
    outString.erase();
    switch (mType) {
    case URLType_HTTP: outString+="http://"; break;
    case URLType_FTP: outString+="ftp://"; break;
    }
    outString+=mAddress;
    outString+=mPath;
}
}
