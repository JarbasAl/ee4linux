// Original-backed WONDir family; grouping provisional, no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONBuffer.h"
namespace WONAPI {
// Default protocol initialization is native-inline in AddPath/CheckResponse.

GetEntityRequest::GetEntityRequest()
{
    mFlags=0x06020837;
}
// FUNCTION: WON 0x6f0da0
GetEntityRequest::~GetEntityRequest() {}
// FUNCTION: WON 0x6f0fa0
void GetEntityRequest::Pack(WriteBuffer* message,PACKFLAG mode)
{
    DWORD flags=mFlags;
    if((flags&2)!=0 && (flags&5)!=0)
        flags|=0x10;
    message->AppendLong(flags);
    if(mode==Pack_GetDirOp)
        message->AppendShort(0);
    else if(mode==Pack_GetMultiDirOp)
        message->AppendByte(0x44);
    message->AppendWString(mPath,2);
    message->AppendShort(static_cast<short>(mDataTypes.size()));
    for(DirDataTypeSet::iterator i=mDataTypes.begin();i!=mDataTypes.end();++i)
        message->AppendString(*i,1);
}
// FUNCTION: WON 0x6f1070
void GetEntityRequest::Reset()
{
    mDirEntityList.clear();
    mDirEntityMap.clear();
}
/* DEFERRED ORIGINAL, ZERO CREDIT: ParseMultiEntityReply2f1160 and native parser
   continuation2f1190 remain external. No partial DirEntity allocation/parser is
   reconstructed here; no GetDirEntityMap lazy builder or single-entity parser.
*/
} // WONAPI

// Previous deferred note records the earlier selection; real forwarding behavior now follows.
namespace WONAPI {
// FUNCTION: WON 0x006f1160
WONStatus GetEntityRequest::ParseMultiEntityReply(const void* data,unsigned long length)
{
    mStatus=DirEntityReplyParser::ParseMultiEntityReply(data,length,mDirEntityList,NULL);
    return mStatus;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK lazy directory path grouping/cache builder.
namespace WONAPI {
const DirEntityMap& GetEntityRequest::GetDirEntityMap()
{
	if(!mDirEntityMap.empty())
		return mDirEntityMap;

	DirEntityList::iterator aListItr = mDirEntityList.begin();
	DirEntityMap::iterator aMapItr = mDirEntityMap.insert(DirEntityMap::value_type(mPath,DirEntityList())).first;

	std::wstring currentDir = mPath;
	while(aListItr!=mDirEntityList.end())
	{
		DirEntity *anEntity = aListItr->get();
		const std::wstring &aPath = anEntity->mPath;

		bool mapInsert = false;
		if((anEntity->mType=='D'))
		{
			if(aPath.empty())
				currentDir = mPath;
			else
				currentDir = aPath;
			
			if(currentDir.empty() || currentDir[currentDir.length()-1]!=L'/')
				currentDir+=L'/';

			currentDir += anEntity->mName;
			
			mapInsert = true;
		}
		else if(!aPath.empty() && aPath!=currentDir)
		{
			currentDir = aPath;
			mapInsert = true;
		}

		if(mapInsert)
			aMapItr = mDirEntityMap.insert(aMapItr,DirEntityMap::value_type(currentDir,DirEntityList()));

		aMapItr->second.push_back(anEntity);

		++aListItr;
	}

	return mDirEntityMap;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f0ed0
// (protected: virtual void * __thiscall WONAPI::GetEntityRequest::`scalar deleting destructor'(unsigned int)    [??_GGetEntityRequest@WONAPI@@MAEPAXI@Z])
// TEMPLATE: WON 0x006f0ef0
// (public: __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> > con)
