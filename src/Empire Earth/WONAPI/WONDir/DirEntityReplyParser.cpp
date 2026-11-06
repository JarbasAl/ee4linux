// Native directory entity behavior; no runtime substitution authority.
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

// FUNCTION: WON 0x006f04f0
bool DirEntityReplyParser::OnlyDirs(DWORD inFlags)
{
    return (inFlags & 2) == 0;
}
// FUNCTION: WON 0x006f0500
bool DirEntityReplyParser::OnlyServices(DWORD inFlags)
{
    return (inFlags & 5) == 0;
}
// FUNCTION: WON 0x006f0510
WONStatus DirEntityReplyParser::ParseMultiEntityReply(const void* inData,
    unsigned long inLength, DirEntityList& outEntities, unsigned char* outSequence)
{
    int i, j, k;
    int count, dataCount, aclCount, userCount;
    unsigned char length;
    char type;
    bool isService, isDir;
    ReadBuffer message(inData, inLength);
    unsigned char header = message.ReadByte();
    unsigned short service = message.ReadShort();
    unsigned short messageType = message.ReadShort();
    if (header != 5 || service != 2 || messageType != 3)
        return (WONStatus)100063;
    short status = message.ReadShort();
    if (status < 0) return status;
    unsigned char sequence = message.ReadByte();
    if (outSequence != NULL) *outSequence = sequence;
    bool moreReplies = (sequence & 0x80) == 0;
    DWORD flags = message.ReadLong();
    bool onlyDirs = OnlyDirs(flags);
    bool onlyServices = OnlyServices(flags);
    count = (unsigned short)message.ReadShort();
    for (i = 0; i < count; ++i) {
        DirEntityPtr entity = new DirEntity;
        if (flags & 0x10) entity->mType = message.ReadByte();
        isService = entity->mType == 'S' || onlyServices;
        isDir = entity->mType == 'D' || onlyDirs;
        if (isService) {
            if (flags & 0x01000000) message.ReadWString(entity->mPath, 2);
            if (flags & 0x02000000) message.ReadWString(entity->mName, 2);
            if (flags & 0x04000000) {
                length = message.ReadByte();
                entity->mNetAddr = new ByteBuffer(message.ReadBytes(length), length, false);
            }
        } else if (isDir) {
            if (flags & 0x00010000) message.ReadWString(entity->mPath, 2);
            if (flags & 0x00020000) message.ReadWString(entity->mName, 2);
            if (flags & 0x00040000) entity->mReqUnique = message.ReadBool();
        }
        if (flags & 0x20) message.ReadWString(entity->mDisplayName, 2);
        if (flags & 0x100) entity->mLifespan = message.ReadLong();
        if (flags & 0x40) entity->mCreated = message.ReadLong();
        if (flags & 0x80) entity->mTouched = message.ReadLong();
        if (flags & 0x2000) entity->mCRC = message.ReadLong();
        if (flags & 0x8000) {
            entity->mCreateIP = message.ReadLong();
            entity->mTouchIP = message.ReadLong();
        }
        if (flags & 0x4000) {
            entity->mCreateId = message.ReadLong();
            entity->mTouchId = message.ReadLong();
        }
        if (flags & 0x800) {
            dataCount = (unsigned short)message.ReadShort();
            for (j = 0; j < dataCount; ++j) {
                DirDataObject data;
                if (flags & 0x200) message.ReadString(data.mDataType, 1);
                if (flags & 0x400) data.mData = message.ReadBuf(2);
                entity->mDataObjects.insert(entity->mDataObjects.end(),data);
            }
        }
        if (flags & 0x1000) {
            aclCount = (unsigned short)message.ReadShort();
            for (j = 0; j < aclCount; ++j) {
                type = message.ReadByte();
                userCount = (unsigned short)message.ReadShort();
                for (k = 0; k < userCount; ++k) {
                    DirACL acl;
                    acl.mType = type;
                    acl.mUser = message.ReadLong();
                    acl.mCommunity = message.ReadLong();
                    acl.mTrust = message.ReadShort();
                    entity->mACLs.push_back(acl);
                }
            }
        }
        outEntities.push_back(entity);
    }
    return moreReplies ? (WONStatus)100059 : (WONStatus)0;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: complete SDK single-entity reply parser.
#include "ee/wonapi/WONDirExtended.h"
namespace WONAPI {
WONStatus DirEntityReplyParser::ParseSingleEntityReply(const void *theMsg, unsigned long theMsgLen, DirEntityPtr &theEntity)
{
	ReadBuffer aMsg(theMsg,theMsgLen);
	unsigned char aHeaderType = aMsg.ReadByte();		// 5 = Small Header
	unsigned short aServiceType = aMsg.ReadShort();		// 2 = DirServer
	unsigned short aMessageType = aMsg.ReadShort();		// 2 = SingleEntity Reply

	if(aHeaderType!=5 || aServiceType!=2 || aMessageType!=2)
		return (WONStatus)100063;

	short aStatus = aMsg.ReadShort();
	if(aStatus<0)
		return (WONStatus)aStatus;

	DWORD flags = aMsg.ReadLong();

	DirEntityPtr anEntity = new DirEntity;

	if(flags & DIR_GF_ADDTYPE) 
		anEntity->mType = aMsg.ReadByte();

	if(flags & DIR_GF_SERVADDPATH)
		aMsg.ReadWString(anEntity->mPath,2);

	if(flags & DIR_GF_SERVADDNAME)
		aMsg.ReadWString(anEntity->mName,2);

	if(flags & DIR_GF_SERVADDNETADDR)
		anEntity->mNetAddr = aMsg.ReadBuf(1);

	if(flags & DIR_GF_ADDDISPLAYNAME)
		aMsg.ReadWString(anEntity->mDisplayName,2);

	if(flags & DIR_GF_ADDLIFESPAN)
		anEntity->mLifespan = aMsg.ReadLong();

	if(flags & DIR_GF_ADDCREATED)
		anEntity->mCreated = aMsg.ReadLong();

	if(flags & DIR_GF_ADDTOUCHED)
		anEntity->mTouched = aMsg.ReadLong();

	if(flags & DIR_GF_ADDCRC)
		anEntity->mCRC = aMsg.ReadLong();

	if(flags & DIR_GF_ADDDATAOBJECTS) // read data objects
	{
		unsigned short numDataObjects = aMsg.ReadShort();
		for(int j=0; j<numDataObjects; j++)
		{
			DirDataObject anObject;
			if(flags & DIR_GF_ADDDOTYPE)
				aMsg.ReadString(anObject.mDataType,1);

			if(flags & DIR_GF_ADDDODATA)
				anObject.mData = aMsg.ReadBuf(2);

			anEntity->mDataObjects.push_back(anObject);
		}
	}

	if(flags & DIR_GF_ADDACLS) // read ACLS
	{
		unsigned short aNumACLs = aMsg.ReadShort();
		for(int i=0; i<aNumACLs; i++)
		{
			char aType = aMsg.ReadByte();
			unsigned short aNumPermissions = aMsg.ReadShort();
			for(int j=0; j<aNumPermissions; j++)
			{
				unsigned long aUserId = aMsg.ReadLong();
				unsigned long aCommunity = aMsg.ReadLong();
				unsigned short aTrust = aMsg.ReadShort();

				DirACL anACL; anACL.mType=aType; anACL.mUser=aUserId; anACL.mCommunity=aCommunity; anACL.mTrust=aTrust; anEntity->mACLs.push_back(anACL);
			}
		}
	}
	
	theEntity = anEntity;
	return (WONStatus)0;
}
}
