// Native async ownership and queue family, source-only.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {

inline AsyncOpTrackerMaps::EntryMap::iterator
AsyncOpTrackerMaps::erase(EntryMap::iterator itr)
{
    AsyncOpTrackerEntry* value=itr->second;
    mEntries.erase(itr++);
    mOps.erase(value->mOpItr);
    delete value;
    return itr;
}

inline AsyncOpTrackerMaps::OpMap::iterator
AsyncOpTrackerMaps::erase_val(OpMap::iterator itr)
{
    erase((itr++)->second);
    return itr;
}

static inline void InsertTrackedOp(AsyncOpTrackerMaps& maps,unsigned long id,const AsyncOpPtr& op)
{
    AsyncOpTrackerMaps::EntryMap::iterator idItr=maps.mEntries.find(id);
    if(!maps.mB18 && idItr!=maps.mEntries.end()) return;
    AsyncOpTrackerMaps::OpMap::iterator opItr=maps.mOps.find(op);
    if(!maps.mB19 && opItr!=maps.mOps.end()) return;
    opItr=maps.mOps.insert(AsyncOpTrackerMaps::OpMap::value_type(op,maps.mEntries.end()));
    AsyncOpTrackerEntry* entry=new AsyncOpTrackerEntry(op,opItr);
    opItr->second=maps.mEntries.insert(idItr,AsyncOpTrackerMaps::EntryMap::value_type(id,entry));
}

static inline bool EraseTrackedOp(AsyncOpTrackerMaps& maps,const AsyncOpPtr& op)
{
    bool erased;
    if(!maps.mB19) {
        AsyncOpTrackerMaps::OpMap::iterator opItr=maps.mOps.find(op);
        if(opItr==maps.mOps.end()) erased=false;
        else {
            maps.erase(opItr->second);
            erased=true;
        }
    } else {
        std::pair<AsyncOpTrackerMaps::OpMap::iterator,AsyncOpTrackerMaps::OpMap::iterator>
            range=maps.mOps.equal_range(op);
        if(range.first==range.second) erased=false;
        else {
            maps.erase_val(range.first,range.second);
            erased=true;
        }
    }
    return erased;
}

// FUNCTION: WON 0x0069fc80
AsyncOp* AsyncOpTracker::AddOp(AsyncOp* op,unsigned long id)
{
    AutoCrit lock(mLock);
    InsertTrackedOp(mMaps,id,op);
    return op;
}
// FUNCTION: WON 0x0069fe50
bool AsyncOpTracker::RemoveOp(AsyncOp* op)
{
    AutoCrit lock(mLock);
    AsyncOpPtr hold=op;
    bool erased=EraseTrackedOp(mMaps,hold);
    return erased;
}
// FUNCTION: WON 0x0069ffa0
void AsyncOpTracker::KillAll()
{
    AutoCrit lock(mLock);
    AsyncOpTrackerMaps::EntryMap::iterator itr=mMaps.begin();
    while(itr!=mMaps.end()) {
        AsyncOpPtr op=itr->second->mOp;
        mMaps.erase(itr++);
        op->Kill(true);
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x006a9040
AsyncOpTrackerMaps::OpMap::iterator
AsyncOpTrackerMaps::erase_val(OpMap::iterator first,OpMap::iterator last)
{
    while(first!=last)
        first=erase_val(first);
    return first;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a8e70
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,struct WONAPI::AsyncOpTrackerEntry *>,struct std::_Mmapk<unsigned long,st)
// TEMPLATE: WON 0x006a9340
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,struct WONAPI::AsyncOpTrackerEntry *>,struct std::_Mmapk<unsigned long,st)
// TEMPLATE: WON 0x006a99b0
// (protected: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,struct WONAPI::AsyncOpTrackerEntry *>,struct std::_Mmapk<unsigned long)
// TEMPLATE: WON 0x006a9250
// (public: __thiscall std::pair<unsigned long const ,struct WONAPI::AsyncOpTrackerEntry *>::pair<unsigned long const ,struct WONAPI::AsyncOpTrackerEntry )
// TEMPLATE: WON 0x006a92d0
// (public: class std::_Tree<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,struct std::pair<class WONAPI::SmartPtr<class WONAPI::AsyncOp> const ,class std)
// TEMPLATE: WON 0x006a9380
// (public: __thiscall WONAPI::AsyncOpTrackerEntry::AsyncOpTrackerEntry(class WONAPI::SmartPtr<class WONAPI::AsyncOp> const &,class std::_Tree<class WONAP)
// TEMPLATE: WON 0x006a9cb0
// (protected: class std::_Tree<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,struct std::pair<class WONAPI::SmartPtr<class WONAPI::AsyncOp> const ,class )
// TEMPLATE: WON 0x006a9140
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,struct WONAPI::AsyncOpTrackerEntry *>,struct std::_Mmapk<unsigned long,st)
// TEMPLATE: WON 0x006a9270
// ( ?? ?? :: ?? ::insert)
