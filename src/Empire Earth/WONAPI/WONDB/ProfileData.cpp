// New native ProfileData family; original source filename/grouping provisional.

// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006cd770
ProfileData::ProfileData()
    : mBirthMonth(0),mBirthDay(0),mBirthYear(0),mCustomActionId(0)
{
}

// FUNCTION: WON 0x006cd830
ProfileData::~ProfileData() {}

// FUNCTION: WON 0x006cd9e0
void ProfileData::AddNewsletterSub(unsigned short inId,bool inSubscribe)
{
    mNewsletterSubs[inId]=inSubscribe;
}

// FUNCTION: WON 0x006cdae0
bool ProfileData::GetNewsletterSub(unsigned short inId)
{
    if (mNewsletterSubs.find(inId)==mNewsletterSubs.end()) return false;
    return mNewsletterSubs[inId];
}

// FUNCTION: WON 0x006cdc20
void ProfileData::ClearNewsletterSubs()
{
    mNewsletterSubs.clear();
}

// FUNCTION: WON 0x006cdcc0
void ProfileData::PackRequestData(WriteBuffer& inMsg,bool inIsSet)
{
    unsigned short fieldCount=0;
    unsigned long fieldCountPos=inMsg.mLength;
    inMsg.AppendShort(0);
    std::map<unsigned short,bool>::iterator it=mNewsletterSubs.begin();
    while (it!=mNewsletterSubs.end()) {
        inMsg.AppendShort(1);
        inMsg.AppendShort(inIsSet ? 3 : 2);
        inMsg.AppendShort(it->first);
        if (inIsSet) inMsg.AppendBool(it->second);
        ++fieldCount;
        ++it;
    }
    if (mCustomActionId!=0) {
        inMsg.AppendShort(2);
        unsigned long dataLen=0;
        if (mCustomActionData.get()!=NULL) dataLen=mCustomActionData->length();
        inMsg.AppendShort(dataLen+2);
        inMsg.AppendShort(mCustomActionId);
        inMsg.AppendBuffer(mCustomActionData.get(),0);
        ++fieldCount;
    }
    inMsg.AppendShort(3);
    inMsg.AppendShort(4);
    inMsg.AppendByte(mBirthMonth);
    inMsg.AppendByte(mBirthDay);
    inMsg.AppendShort(mBirthYear);
    ++fieldCount;
    inMsg.SetShort(fieldCountPos,fieldCount);
}

// FUNCTION: WON 0x006cde20
// Native hot owner ranges2cde20..2ce051 +2ce057..2ce06a =580B.
// Catch6B at2ce051 is support0; metadata/ranges/nativeinstructions retained separately.
void ProfileData::UnpackGetData(ReadBuffer& inMsg)
{
    try {
        short fieldCount=inMsg.ReadShort();
        for (int i=0;i<static_cast<unsigned short>(fieldCount);++i) {
            unsigned short field=static_cast<unsigned short>(inMsg.ReadShort());
            unsigned short length=static_cast<unsigned short>(inMsg.ReadShort());
            switch (field) {
            case 1: {
                unsigned short id=static_cast<unsigned short>(inMsg.ReadShort());
                bool& subscription=mNewsletterSubs[id];
                subscription=inMsg.ReadBool();
                break;
            }
            case 2:
                inMsg.ReadShort(); // Native consumes/discards returned action id, leaves mCustomActionId unchanged.
                mCustomActionData=new ByteBuffer(inMsg.ReadBytes(static_cast<unsigned long>(length)-2),static_cast<unsigned long>(length)-2,false);
                break;
            case 3:
                mBirthMonth=inMsg.ReadByte();
                mBirthDay=inMsg.ReadByte();
                mBirthYear=inMsg.ReadShort();
                break;
            default:
                break; // Native ignores unknown field, including its length; no invented payload skipping.
            }
        }
    }
    catch (ReadBufferException&) {
    }
}

// FUNCTION: WON 0x006ce070
void ProfileData::SetBirthDate(unsigned char inMonth,unsigned char inDay,unsigned short inYear)
{
    mBirthMonth=inMonth;
    mBirthDay=inDay;
    mBirthYear=inYear;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine missing session/profile/operation methods.
namespace WONAPI {
void ProfileData::ClearNewsletterSub(unsigned short theId)
{
	mNewsletterSubs.erase(theId);
}

void ProfileData::SetCustomAction(unsigned short theActionId, const ByteBuffer *theData)
{
	mCustomActionId = theActionId;
	mCustomActionData = const_cast<ByteBuffer*>(theData);
}

void ProfileData::ClearCustomAction()
{
	mCustomActionId = 0;
	mCustomActionData = NULL;
}

unsigned long ProfileData::GetBirthDate()
{
	return (mBirthMonth) | (mBirthDay<<8) | (mBirthYear<<16);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cd810
// (protected: virtual void * __thiscall WONAPI::ProfileData::`scalar deleting destructor'(unsigned int)    [??_GProfileData@WONAPI@@MAEPAXI@Z])
// TEMPLATE: WON 0x006cd930
// (public: __thiscall std::map<unsigned short,bool,struct std::less<unsigned short>,class std::allocator<struct std::pair<unsigned short const ,bool> > >)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ceda0
// (public: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,bool>,struct std::_Mapk<unsigned short,bool,struct std::less<unsigned s)
// TEMPLATE: WON 0x006cf250
// (protected: void __thiscall std::_Tree<unsigned short,struct std::pair<unsigned short const ,bool>,struct std::_Mapk<unsigned short,bool,struct std::le)
// TEMPLATE: WON 0x006cf590
// (protected: struct std::_Tree<unsigned short,struct std::pair<unsigned short const ,bool>,struct std::_Mapk<unsigned short,bool,struct std::less<unsign)
