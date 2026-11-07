// Native HTTP disk cache; target revision eagerly creates the cache directory.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <time.h>
#include <list>
#include <map>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONAPICore.h"
#include "ee/wonapi/WONLobbyPersistence.h"
namespace WONAPI {
// FUNCTION: WON 0x006d1260
HTTPCacheEntry::HTTPCacheEntry()
{
    SetValid(false);
    mId=0;
    mExpirationTime=0;
    mLastAccessTime=0;
}
// FUNCTION: WON 0x006d1450
void HTTPCacheEntry::ReadFromFile(FileReader& inReader)
{
    mId=inReader.ReadLong();
    mIsValid=inReader.ReadByte()!=0;
    mExpirationTime=inReader.ReadLong();
    mLastAccessTime=inReader.ReadLong();
    inReader.ReadString(mRemoteLocation,2);
    inReader.ReadString(mFileName,2);
    inReader.ReadString(mFileType,2);
    inReader.ReadString(mEntityTag,2);
}
// FUNCTION: WON 0x006d14c0
void HTTPCacheEntry::WriteToFile(FileWriter& inWriter)
{
    inWriter.WriteLong(mId);
    inWriter.WriteByte(mIsValid?1:0);
    inWriter.WriteLong(mExpirationTime);
    inWriter.WriteLong(mLastAccessTime);
    inWriter.WriteString(mRemoteLocation,2);
    inWriter.WriteString(mFileName,2);
    inWriter.WriteString(mFileType,2);
    inWriter.WriteString(mEntityTag,2);
}
// FUNCTION: WON 0x006d1670
HTTPCache::HTTPCache(const std::string& inPath,bool inFullPath)
{
    SetCachePath(inPath,inFullPath);
}
// FUNCTION: WON 0x006d1820
HTTPCache::~HTTPCache()
{
    WriteCacheMap();
}
extern const long gHTTPCacheVersion;
// FUNCTION: WON 0x006d1a10
void HTTPCache::ReadCacheMap()
{
    mCacheIdMap.clear();
    mCacheLocationMap.clear();
    mNextFreeId=1;
    FileReader reader;
    std::string path=mCachePath+"cachemap.dat";
    if (!reader.Open(path.c_str())) return;
    try {
        if (reader.ReadLong()!=gHTTPCacheVersion) return;
        int count=reader.ReadLong();
        for (int i=0;i<count;++i) {
            SmartPtr<HTTPCacheEntry> entry=new HTTPCacheEntry;
            entry->ReadFromFile(reader);
            entry->mLocalPath=mCachePath+entry->mFileName;
            mCacheIdMap[entry->mId]=entry;
            mCacheLocationMap[entry->mRemoteLocation]=entry;
        }
    } catch (FileReaderException&) {
    }
}
// FUNCTION: WON 0x006d1ca0
void HTTPCache::WriteCacheMap()
{
    FileWriter writer;
    std::string path=mCachePath+"cachemap.dat";
    if (!writer.Open(path.c_str(),false)) return;
    time_t now=time(NULL);
    unsigned long position;
    int count=0;
    CacheIdMap::iterator i;
    try {
        writer.WriteLong(gHTTPCacheVersion);
        int size=mCacheIdMap.size();
        position=writer.pos();
        writer.WriteLong(size);
        i=mCacheIdMap.begin();
        while (i!=mCacheIdMap.end()) {
            HTTPCacheEntry* entry=i->second.get();
            if (entry->IsValid() && now-entry->mLastAccessTime<1209600) {
                entry->WriteToFile(writer);
                ++count;
            } else {
                WONFile file(entry->mLocalPath);
                file.Delete(true);
            }
            ++i;
        }
        writer.SetPos(position);
        writer.WriteLong(count);
    } catch (FileWriterException&) {
    }
}
// FUNCTION: WON 0x006d1ef0
void HTTPCache::SetCachePath(const std::string& inPath,bool inFullPath)
{
    WONFile path(inPath);
    if (inFullPath) mCachePath=path.GetFullPath();
    else mCachePath=inPath;
    path.CreateAsDirectory(true);
    if (mCachePath.empty()) return;
    char last=mCachePath[mCachePath.length()-1];
    if (last!='\\' && last!='/') mCachePath+='/';
    ReadCacheMap();
}
// FUNCTION: WON 0x006d2170
int HTTPCache::GetFreeId()
{
    CacheIdMap::iterator i=mCacheIdMap.find(mNextFreeId);
    while (i!=mCacheIdMap.end()) {
        if (mNextFreeId!=i->first) break;
        ++i;
        ++mNextFreeId;
    }
    return mNextFreeId;
}
// FUNCTION: WON 0x006d2210
HTTPCacheEntry* HTTPCache::GetEntry(const std::string& inLocation)
{
    CacheLocationMap::iterator i=mCacheLocationMap.insert(CacheLocationMap::value_type(inLocation,NULL)).first;
    HTTPCacheEntry* entry=i->second.get();
    if (entry==NULL) {
        HTTPCacheEntry* fresh=new HTTPCacheEntry;
        i->second=fresh;
        entry=fresh;
        entry->mRemoteLocation=inLocation;
        entry->mId=GetFreeId();
        std::string type;
        int slash=inLocation.find_last_of('/');
        if (slash==std::string::npos) slash=inLocation.find_last_of('\\');
        if (slash!=std::string::npos) {
            int dot=inLocation.find_last_of('.');
            if (dot>slash && dot+10>=inLocation.length())
                type=inLocation.substr(dot+1);
            else type="dat";
        } else type="dat";
        char fileName[50];
        sprintf(fileName,"%06d.%s",entry->mId,type.c_str());
        entry->mFileName=fileName;
        entry->mLocalPath=mCachePath+entry->mFileName;
        WONFile file(entry->mLocalPath);
        file.Delete(true);
        mCacheIdMap[entry->mId]=entry;
    }
    entry->mLastAccessTime=time(NULL);
    return entry;
}
}

namespace WONAPI {
// FUNCTION: WON 0x006d1540
bool HTTPCacheEntry::IsValid() { return mIsValid; }
}

// Actual original four-byte static HTTPCache owner565fd4. Fixed SDK global
// declaration corroborated by native CRT4b49c4->2d1550/1560/1570/1580 graph.
static WONAPI::SmartPtr<WONAPI::HTTPCache> gGlobalCache;
namespace WONAPI {
// FUNCTION: WON 0x006d1900
HTTPCache* HTTPCache::GetGlobalCache()
{
    if(gGlobalCache.get()==NULL)
        gGlobalCache=new HTTPCache(WONAPICore::GetDefaultFileDirectory()+"_wonHTTPCache");
    return gGlobalCache.get();
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine missing session/profile/operation methods.
namespace WONAPI {
void HTTPCache::SetGlobalCache(HTTPCache* theCache)
{
	gGlobalCache = theCache;
}
}

namespace WONAPI { const long gHTTPCacheVersion=1; }
// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d1330
// (public: virtual __thiscall WONAPI::HTTPCacheEntry::~HTTPCacheEntry(void)    [??1HTTPCacheEntry@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006d1590
// (protected: virtual void * __thiscall WONAPI::HTTPCache::`scalar deleting destructor'(unsigned int)    [??_GHTTPCache@WONAPI@@MAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d1310
// (public: virtual void * __thiscall WONAPI::HTTPCacheEntry::`scalar deleting destructor'(unsigned int)    [??_GHTTPCacheEntry@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d2720
// (public: __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::SmartPtr<cla)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d15b0
// (public: __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class WONAPI::SmartPtr<class WONAP)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006da940
// (public: __thiscall std::pair<int const ,class WONAPI::SmartPtr<class WONAPI::HTTPCacheEntry> >::pair<int const ,class WONAPI::SmartPtr<class WONAPI::H)
// TEMPLATE: WON 0x006dc130
// (protected: struct std::_Tree<int,struct std::pair<int const ,class WONAPI::SmartPtr<class WONAPI::HTTPCacheEntry> >,struct std::_Mapk<int,class WONAPI)
// TEMPLATE: WON 0x006dcca0
// (public: void * __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::Smart)
// TEMPLATE: WON 0x006cff20
// (public: __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::SmartPtr<cla)
// TEMPLATE: WON 0x006d9ab0
// (public: struct std::pair<class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<cla)
// TEMPLATE: WON 0x006da3f0
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_str)
// TEMPLATE: WON 0x006da4a0
// (protected: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_)
// TEMPLATE: WON 0x006db3a0
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_str)
// TEMPLATE: WON 0x006db8c0
// (protected: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class s)
// TEMPLATE: WON 0x006d1760
// (public: __thiscall std::map<int,class WONAPI::SmartPtr<class WONAPI::HTTPCacheEntry>,struct std::less<int>,class std::allocator<struct std::pair<int c)
// TEMPLATE: WON 0x006da7a0
// (public: struct std::pair<class std::_Tree<int,struct std::pair<int const ,class WONAPI::SmartPtr<class WONAPI::HTTPCacheEntry> >,struct std::_Mapk<int)
// TEMPLATE: WON 0x006da890
// (public: class std::_Tree<int,struct std::pair<int const ,class WONAPI::SmartPtr<class WONAPI::HTTPCacheEntry> >,struct std::_Mapk<int,class WONAPI::Sm)
// TEMPLATE: WON 0x006dbdf0
// (protected: void __thiscall std::_Tree<int,struct std::pair<int const ,class WONAPI::SmartPtr<class WONAPI::HTTPCacheEntry> >,struct std::_Mapk<int,cla)
// TEMPLATE: WON 0x006dbe30
// (protected: class std::_Tree<int,struct std::pair<int const ,class WONAPI::SmartPtr<class WONAPI::HTTPCacheEntry> >,struct std::_Mapk<int,class WONAPI:)
// TEMPLATE: WON 0x006dca40
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WON)
// TEMPLATE: WON 0x006d9bb0
// (public: __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::SmartPtr<cla)
// TEMPLATE: WON 0x006d1550
// (_$E74)
// TEMPLATE: WON 0x006d1560
// (_$E71)
// TEMPLATE: WON 0x006d1570
// (_$E73)
// TEMPLATE: WON 0x006d1580
// (_$E72)
