// Complete genuine same-snapshot Title/Community configuration algorithms.
// SOURCE_ONLY_NO_NATIVE_ID: no Native/primary/physical identity assigned.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONTitleConfig.h"
using namespace WONAPI;
using namespace std;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TitleConfig::TitleConfig(bool alreadyInTitle /* = false */) :
	mAlreadyInTitle(alreadyInTitle)
{
	KeyAdd("ForceCDKey",mForceCDKey,false);
	KeyAdd("RequireCRC",mRequireCrc,false);
	KeyAdd("AllowBetaKey",mAllowBetaKey,false);
	KeyAdd("RequireCDKey",mRequireCDKey,false);
	KeyAdd("FailToGuestLogin",mFailToGuest,true);
	KeyAdd("AllowGuestLogin",mAllowGuest,false);
	KeyAdd("ConcurrentUserLogin",mConcurrentUserLogin,true);
	KeyAdd("ConcurrentCDKeyLogin",mConcurrentCDKeyLogin,false);
	KeyAdd("GetUserData",mGetUserData,false);
	KeyAdd("GetForgeableUserData",mGetForgeableUserData,false);
	KeyAdd("CDKeyForInvite",mCDKeyForInvite,false);
	KeyAdd("SaveSessionKey",mSaveSessionKey,false);
	KeyAdd("GetKeyID",mGetKeyID,false);
	KeyAdd("ASCIIAccountNames",mASCIIAccountNames,false);

	KeyAdd("NetPort",mNetPort,15111);

	KeyAdd("Network",mNetwork,NULL);
	KeyAdd("ImagePath",mImagePath,NULL);
	KeyAdd("HashString",mHashString,NULL);
	KeyAdd("KeyPrefix",mKeyPrefix,NULL);

	KeyAdd("AdminCommunityName",mAdminCommunityName,L"");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TitleConfig::Virtual0C(ConfigParser* theParser)
{
	TitleConfig *aParent = dynamic_cast<TitleConfig*>(mL38);
	if(aParent!=NULL)
		CopyKeyValsFrom(aParent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TitleConfig::Virtual04(ConfigParser* theParser, const std::string& theTitleType)
{
	if(ConfigObject::Virtual04(theParser, theTitleType))
		return true;

	if(theTitleType=="TITLEDEF")
	{
		if(mAlreadyInTitle)
		{
			theParser->SetError("You can't nest Title definitions!",false);
			return false;
		}

		TitleConfigPtr aTitleP = new TitleConfig(true);
		return ParseChild(*theParser,aTitleP.get());
	}
	else if(theTitleType=="COMMUNITY")
	{
		CommunityConfigPtr aCommunityConfigP = new CommunityConfig;
		return ParseChild(*theParser,aCommunityConfigP.get());
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TitleConfig::Virtual10(ConfigParser* theParser)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
string TitleConfig::ToString(bool includeName) const
{
	string aStr;
	if(includeName)
		aStr += mS18 +"\r\n";

	aStr += string("\tNetwork: ") + (mNetwork.empty() ? string("EMPTY") : mNetwork) + "\r\n";
	char aBuffer[20];
	sprintf(aBuffer, "%d", mNetPort);
	aStr += string("\tNetPort: ") + string(aBuffer) + "\r\n";
	aStr += string("\tForceCDKey: ") + string((mForceCDKey) ? "TRUE" : "FALSE") + "\r\n";

	aStr += string("\tImagePath: ") + (mImagePath.empty() ? string("EMPTY") : mImagePath) + "\r\n";
	aStr += string("\tHashString: ") + (mHashString.empty() ? string("EMPTY") : mHashString) + "\r\n";
	aStr += string("\tKeyPrefix: ") + (mKeyPrefix.empty() ? string("EMPTY") : mKeyPrefix) + "\r\n";

	aStr += string("\tRequireCrc: ") + string((mRequireCrc) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tAllowBetaKey: ") + string((mAllowBetaKey) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tRequireCDKey: ") + string((mRequireCDKey) ? "TRUE" : "FALSE") + "\r\n";

	aStr += string("\tFailToGuest: ") + string((mFailToGuest) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tAllowGuest: ") + string((mAllowGuest) ? "TRUE" : "FALSE") + "\r\n";
	
	aStr += string("\tConcurrentUserLogin: ") + string((mConcurrentUserLogin) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tConcurrentCDKeyLogin: ") + string((mConcurrentCDKeyLogin) ? "TRUE" : "FALSE") + "\r\n";
	
	aStr += string("\tGetUserData: ") + string((mGetUserData) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tGetForgeableUserData: ") + string((mGetForgeableUserData) ? "TRUE" : "FALSE") + "\r\n";
	
	aStr += string("\tCDKeyForInvite: ") + string((mCDKeyForInvite) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tSaveSessionKey: ") + string((mSaveSessionKey) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tGetKeyID: ") + string((mGetKeyID) ? "TRUE" : "FALSE") +"\r\n";
	aStr += string("\tASCIIAccountNames: ") + string((mASCIIAccountNames) ? "TRUE" : "FALSE") +"\r\n";

	return aStr;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CommunityConfig::CommunityConfig(bool alreadyInCommunity /* = false */) :
	TitleConfig(),
	mAlreadyInCommunity(alreadyInCommunity)
{
	RegisterIntValue("CommunityID",mCommunityID,0);
	RegisterIntListListValue("CertificateLifeSpan",mCertificateLifeSpanList,NULL,true);
	KeyAdd("NameLogin",mNameLoginStr,"Required");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CommunityConfig::CommunityConfig(const wstring& theCommunityName, unsigned long theCommunityID)
{
	Virtual18(true);
	mCommunityName = theCommunityName;
	mCommunityID = theCommunityID;
	mFailToGuest = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CommunityConfig::Virtual0C(ConfigParser* theParser)
{
	TitleConfig *aParent = dynamic_cast<TitleConfig*>(mL38);
	if(aParent!=NULL)
		CopyKeyValsFrom(aParent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CommunityConfig::Virtual04(ConfigParser* theParser, const std::string& theCommunityType)
{
	if(ConfigObject::Virtual04(theParser, theCommunityType))
		return true;

	if(theCommunityType=="TITLEDEF")
	{
		theParser->SetError("You can't have a title definition within a community!",false);
		return false;
	}
	else if(theCommunityType=="COMMUNITY")
	{
		if(mAlreadyInCommunity)
		{
			theParser->SetError("You can't nest communities!",false);
			return false;
		}

		CommunityConfigPtr aCommutiyP = new CommunityConfig(true);
		return ParseChild(*theParser,aCommutiyP.get());
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CommunityConfig::Virtual10(ConfigParser* theParser)
{
	string aNameLoginStr = StringToUpperCase(mNameLoginStr);
	if(aNameLoginStr == "REQUIRED")
		mNameLogin = NameLogin_Required;
	else if(aNameLoginStr == "OPTIONAL")
		mNameLogin = NameLogin_Optional;
	else if(aNameLoginStr == "NONE")
		mNameLogin = NameLogin_None;
	else
		mNameLogin = NameLogin_Unknown;
	
	if(mImagePath.empty())
	{
		mImagePath = "./";

		TitleConfig* anObjectP = dynamic_cast<TitleConfig*>(mL38);
		if(anObjectP)
			mImagePath += anObjectP->mS18;
		else
			mImagePath += mS18;
	}

	if(mHashString.empty())
	{
		TitleConfig* anObjectP = dynamic_cast<TitleConfig*>(mL38);
		if(anObjectP)
			mHashString = anObjectP->mS18;
		else
			mHashString = mS18;
	}

	long aTrustLevel, aDuration;
	mTrustLevelLifespanMap.clear();
	for(std::list<std::list<int> >::const_iterator anItr = mCertificateLifeSpanList.begin(); anItr != mCertificateLifeSpanList.end(); ++anItr)
	{
		std::list<int>::const_iterator anItr2 = anItr->begin();
		if(anItr2 != anItr->end())
		{
			aTrustLevel = *anItr2;
			++anItr2;
			if(anItr2 != anItr->end())
			{
				aDuration = *anItr2;
				mTrustLevelLifespanMap[aTrustLevel] = aDuration;
			}
		}
	}

	if (mCommunityName.empty())
		mCommunityName = StringToWString(mS18);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
string CommunityConfig::ToString(void) const
{
	string aStr = ((mCommunityName.empty()) ? mS18 : WStringToString(mCommunityName)) + "\r\n";

	char aBuffer[25];
	sprintf(aBuffer, "%d", mCommunityID);
	aStr += string("CommunityID: ") + string(aBuffer) + "\r\n";
	aStr += TitleConfig::ToString(false);

	// Trust level lifespan map
	aStr += string("\tNameLogin: ") + mNameLoginStr + "\r\n";
	aStr += "\tTrust Level Lifespans: [";
	if(mTrustLevelLifespanMap.empty())
		aStr += "EMPTY";
	else{
		bool isFirst = true;
		for(TRUST_LEVEL_LIFESPAN_MAP_CITR anItr = mTrustLevelLifespanMap.begin(); anItr != mTrustLevelLifespanMap.end(); ++anItr)
		{
			if(isFirst)
				isFirst = false;
			else
				aStr += ",";

			sprintf(aBuffer, "(%d,%d)", anItr->first, anItr->second);
			aStr += string(aBuffer);
		}
	}
	aStr += "]\r\n";

	return aStr;
}

