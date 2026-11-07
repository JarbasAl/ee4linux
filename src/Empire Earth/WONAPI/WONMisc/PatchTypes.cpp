// Native patch-server value/serialization records. Source-only reconstruction.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006d83a0
VersionData::VersionData()
    : mConfigName(""),mVersion(""),mDescriptionURL(""),mState(VersionState_Invalid)
{
    mVersionTypeList.clear();
}
// FUNCTION: WON 0x006d8510
VersionData::~VersionData()
{
    mVersionTypeList.clear();
}
// FUNCTION: WON 0x006d8660
void VersionData::WriteToBuffer(WriteBuffer& outBuffer,DWORD inFlags)
{
    outBuffer.AppendString(mConfigName);
    outBuffer.AppendString(mVersion);
    outBuffer.AppendByte(mVersionTypeList.size());
    if (!mVersionTypeList.empty()) {
        VersionTypeList::const_iterator i=mVersionTypeList.begin();
        while (true) {
            if (i==mVersionTypeList.end()) break;
            outBuffer.AppendString(*i);
            ++i;
        }
    }
    if (inFlags&2) outBuffer.AppendString(mDescriptionURL);
    if (inFlags&1) outBuffer.AppendByte(mState);
}
// FUNCTION: WON 0x006d86f0
PatchData::PatchData()
    : mConfigName(""),mFromVersion(""),mToVersion(""),mDisplayName(""),
      mNetAddress(""),mCheckSum(0),mPatchSize(0),mHostName(""),mHostURL(""),
      mHostText(""),mHostImageURL(""),mMustVisitHost(false),mIsActive(false)
{
}
// FUNCTION: WON 0x006d8a50
PatchData::~PatchData()
{
}
// FUNCTION: WON 0x006d8bc0
void PatchData::ReadFromBuffer(ReadBuffer& inBuffer,bool inGetPatchList)
{
    if (inGetPatchList) {
        inBuffer.ReadString(mConfigName);
        inBuffer.ReadString(mFromVersion);
        inBuffer.ReadString(mToVersion);
        mIsActive=inBuffer.ReadBool();
    }
    inBuffer.ReadString(mDisplayName);
    inBuffer.ReadString(mNetAddress);
    mCheckSum=inBuffer.ReadLong();
    mPatchSize=inBuffer.ReadLong();
    inBuffer.ReadString(mHostName);
    inBuffer.ReadString(mHostURL);
    inBuffer.ReadString(mHostText);
    inBuffer.ReadString(mHostImageURL);
    mMustVisitHost=inBuffer.ReadBool();
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine missing SDK version/patch value algorithms.
namespace WONAPI {
void VersionData::AddVersionType(const std::string& theVersionType)
{ 
	// Parse for any comma seperated strings
	int lastPos = 0;
	int aCommaPos = theVersionType.find(',');
	std::string versionString;

	for (; aCommaPos != -1; aCommaPos = theVersionType.find(',',lastPos))
	{
		versionString = theVersionType.substr(lastPos, aCommaPos-lastPos);
		lastPos = aCommaPos+1;
		mVersionTypeList.push_back(versionString);
	}
	
	versionString = theVersionType.substr(lastPos, aCommaPos-lastPos);
	mVersionTypeList.push_back(versionString);
}

void VersionData::ReadFromBuffer(ReadBuffer& theBuffer)
{
	theBuffer.ReadString(mConfigName);
	theBuffer.ReadString(mVersion);
	
	int numVersionTypeStrings = theBuffer.ReadByte();
	std::string versionTypeString;

	for (int k=0; k < numVersionTypeStrings; ++k)
	{
		theBuffer.ReadString(versionTypeString);
		AddVersionType(versionTypeString);
	}
	theBuffer.ReadString(mDescriptionURL);
	mState = (VersionState)theBuffer.ReadByte();
}

bool VersionData::operator==(const VersionData& theVersion) const
{
	if (mVersion == theVersion.GetVersion() &&
		mConfigName == theVersion.GetConfigName() &&
		mDescriptionURL == theVersion.GetDescriptionURL() &&
		mState == theVersion.GetState() &&
		mVersionTypeList == theVersion.GetVersionTypeList())
	{
		return true;
	}

	return false;
}

bool VersionData::operator!=(const VersionData& theVersion) const
{
	if (mVersion		 != theVersion.GetVersion()			||
		mConfigName		 != theVersion.GetConfigName()		||
		mDescriptionURL  != theVersion.GetDescriptionURL()	||
		mState			 != theVersion.GetState()			||
		mVersionTypeList != theVersion.GetVersionTypeList()		)
	{
		return true;
	}

	return false;
}

void PatchData::WriteToBuffer(WriteBuffer& theWriteBuffer, bool bUpdateOnly)
{
	theWriteBuffer.AppendString(mConfigName);
	theWriteBuffer.AppendString(mFromVersion);
	theWriteBuffer.AppendString(mToVersion);
	theWriteBuffer.AppendByte(bUpdateOnly);
	theWriteBuffer.AppendString(mDisplayName);
	theWriteBuffer.AppendString(mNetAddress);		// patch url
	theWriteBuffer.AppendLong(mCheckSum);
	theWriteBuffer.AppendLong(mPatchSize);
	theWriteBuffer.AppendString(mHostName);
	theWriteBuffer.AppendString(mHostURL);
	theWriteBuffer.AppendString(mHostText);
	theWriteBuffer.AppendString(mHostImageURL);
	theWriteBuffer.AppendByte(mMustVisitHost);
	theWriteBuffer.AppendByte(mIsActive);
}

bool PatchData::operator==(const PatchData& thePatchData) const
{
	if (mConfigName		== thePatchData.mConfigName		&&
		mFromVersion	== thePatchData.mFromVersion	&&
		mToVersion		== thePatchData.mToVersion		&&
		mNetAddress		== thePatchData.mNetAddress		&&
		mDisplayName	== thePatchData.mDisplayName	&&
		mCheckSum		== thePatchData.mCheckSum		&&
		mPatchSize		== thePatchData.mPatchSize		&&
		mHostName		== thePatchData.mHostName		&&
		mHostURL		== thePatchData.mHostURL		&&
		mHostText		== thePatchData.mHostText		&&
		mHostImageURL	== thePatchData.mHostImageURL	&&
		mMustVisitHost	== thePatchData.mMustVisitHost	&&
		mIsActive		== thePatchData.mIsActive			)
		return true;
	else
		return false;
}

bool PatchData::operator!=(const PatchData& thePatchData) const
{
	if (mConfigName		!= thePatchData.mConfigName		||
		mFromVersion	!= thePatchData.mFromVersion	||
		mToVersion		!= thePatchData.mToVersion		||
		mNetAddress		!= thePatchData.mNetAddress		||
		mDisplayName	!= thePatchData.mDisplayName	||
		mCheckSum		!= thePatchData.mCheckSum		||
		mPatchSize		!= thePatchData.mPatchSize		||
		mHostName		!= thePatchData.mHostName		||
		mHostURL		!= thePatchData.mHostURL		||
		mHostText		!= thePatchData.mHostText		||
		mHostImageURL	!= thePatchData.mHostImageURL	||
		mMustVisitHost	!= thePatchData.mMustVisitHost	||
		mIsActive		!= thePatchData.mIsActive			)
		return true;
	else
		return false;

}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d84f0
// (protected: virtual void * __thiscall WONAPI::VersionData::`scalar deleting destructor'(unsigned int)    [??_GVersionData@WONAPI@@MAEPAXI@Z])
// TEMPLATE: WON 0x006d8a30
// (protected: virtual void * __thiscall WONAPI::PatchData::`scalar deleting destructor'(unsigned int)    [??_GPatchData@WONAPI@@MAEPAXI@Z])
