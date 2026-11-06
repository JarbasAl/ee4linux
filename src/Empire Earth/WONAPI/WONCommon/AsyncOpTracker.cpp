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

// FUNCTION: WON 0x0069fc80
AsyncOp* AsyncOpTracker::AddOp(AsyncOp* op,unsigned long id)
{
    AutoCrit lock(mLock);
    AsyncOpPtr hold=op;
    AsyncOpTrackerMaps::EntryMap::iterator idItr=mMaps.mEntries.find(id);
    if(!mMaps.mB18 && idItr!=mMaps.mEntries.end()) return op;
    AsyncOpTrackerMaps::OpMap::iterator opItr=mMaps.mOps.find(hold);
    if(!mMaps.mB19 && opItr!=mMaps.mOps.end()) return op;
    opItr=mMaps.mOps.insert(std::make_pair(hold,mMaps.mEntries.end()));
    AsyncOpTrackerEntry* entry=new AsyncOpTrackerEntry;
    entry->mOp=hold;
    entry->mOpItr=opItr;
    AsyncOpTrackerMaps::EntryMap::iterator inserted=
        mMaps.mEntries.insert(std::make_pair(id,entry));
    opItr->second=inserted;
    return op;
}
// FUNCTION: WON 0x0069fe50
bool AsyncOpTracker::RemoveOp(AsyncOp* op)
{
    AutoCrit lock(mLock);
    AsyncOpPtr hold=op;
    if(!mMaps.mB19) {
        AsyncOpTrackerMaps::OpMap::iterator opItr=mMaps.mOps.find(hold);
        if(opItr==mMaps.mOps.end()) return false;
        AsyncOpTrackerMaps::EntryMap::iterator idItr=opItr->second;
        AsyncOpTrackerEntry* entry=idItr->second;
        mMaps.mEntries.erase(idItr);
        mMaps.mOps.erase(entry->mOpItr);
        delete entry;
        return true;
    }
    std::pair<AsyncOpTrackerMaps::OpMap::iterator,AsyncOpTrackerMaps::OpMap::iterator>
        range=mMaps.mOps.equal_range(hold);
    if(range.first==range.second) return false;
    while(range.first!=range.second) {
        AsyncOpTrackerMaps::OpMap::iterator opItr=range.first++;
        AsyncOpTrackerMaps::EntryMap::iterator idItr=opItr->second;
        AsyncOpTrackerEntry* entry=idItr->second;
        mMaps.mEntries.erase(idItr);
        mMaps.mOps.erase(entry->mOpItr);
        delete entry;
    }
    return true;
}
// FUNCTION: WON 0x0069ffa0
void AsyncOpTracker::KillAll()
{
    AutoCrit lock(mLock);
    AsyncOpTrackerMaps::EntryMap::iterator itr=mMaps.mEntries.begin();
    while(itr!=mMaps.mEntries.end()) {
        AsyncOpTrackerEntry* entry=itr->second;
        AsyncOpPtr op=entry->mOp;
        AsyncOpTrackerMaps::EntryMap::iterator doomed=itr++;
        mMaps.mEntries.erase(doomed);
        mMaps.mOps.erase(entry->mOpItr);
        delete entry;
        op->Kill(true);
    }
}
}

// PRIVATE PREP ONLY. Requires Root review of genuine existing BiMap model API.

namespace WONAPI {
AsyncOpTrackerMaps::EntryMap::iterator
AsyncOpTrackerMaps::erase(EntryMap::iterator itr)
{
    AsyncOpTrackerEntry* value=itr->second;
    mEntries.erase(itr++);
    mOps.erase(value->mOpItr);
    delete value;
    return itr;
}

AsyncOpTrackerMaps::OpMap::iterator
AsyncOpTrackerMaps::erase_val(OpMap::iterator itr)
{
    erase((itr++)->second);
    return itr;
}

// Actual primary-range native2a9040..2a9135,245B. Support0, no global novelty claim.
AsyncOpTrackerMaps::OpMap::iterator
AsyncOpTrackerMaps::erase_val(OpMap::iterator first,OpMap::iterator last)
{
    while(first!=last)
        first=erase_val(first);
    return first;
}
}
