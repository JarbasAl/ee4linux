// RECONSTRUCTION: new native WONAPI class family; exact original filename/grouping unproved.
// Unmatched code/support remains original-carved; no runtime authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// R3 scratch NEW ServerContext behaviors. Complete3c fields already canonical.
// Actual AutoCrit8 protects map/list and preserves original mutation order.
// FUNCTION: WON 0x006ac390
void ServerContext::Clear()
{
    AutoCrit guard(mLock);
    mPingMap.clear();
    mAddresses.clear();
    mB08 = true;
    mB09 = false;
}
// FUNCTION: WON 0x006ac4b0
bool ServerContext::AddAddress(const IPAddr& inAddress)
{
    if (!inAddress.IsValid()) return false;
    AutoCrit guard(mLock);
    typedef std::map<IPAddr,AddrList::iterator> AddrMap;
    std::pair<AddrMap::iterator,bool> inserted =
        mPingMap.insert(AddrMap::value_type(inAddress,mAddresses.end()));
    if (!inserted.second) return false;
    mAddresses.push_back(inAddress);
    AddrList::iterator last = mAddresses.end();
    --last;
    inserted.first->second = last;
    return true;
}


// Next ServerContext source fragment, scratch only.
// FUNCTION: WON 0x006ac980
void ServerContext::NotifyFailed(const IPAddr& inAddress)
{
    AutoCrit guard(mLock);
    std::map<IPAddr,AddrList::iterator>::iterator it=mPingMap.find(inAddress);
    if (it==mPingMap.end()) return;
    mAddresses.erase(it->second);
    mAddresses.push_back(it->first);
    AddrList::iterator last=mAddresses.end();
    --last;
    it->second=last;
}

// Next ServerContext directory source; uses only proven reference prefix, no new DirEntity.
// FUNCTION: WON 0x006acab0
bool ServerContext::AddAddressesFromDir(const DirEntityList& inEntities,const wchar_t* inNameFilter)
{
    bool added=false;
    std::wstring filter;
    if (inNameFilter!=NULL) filter=inNameFilter;
    for (DirEntityList::const_iterator it=inEntities.begin();it!=inEntities.end();++it) {
        DirEntity* entity=it->get();
        if (entity->mType=='D') continue;
        if (!filter.empty() && !(filter==entity->mName)) continue;
        if (AddAddress(entity->GetNetAddrAsIP())) added=true;
    }
    return added;
}

// Native default-shuffle random selector2b0073..2b009a, cohesive support0.
// Three-argument SDK shuffle starts at divisor2; default bundled overload starts1.
class ServerAddressShuffleRandom {
public:
    int operator()(int upperBound) {
        unsigned long range=0x7fff;
        unsigned long value=rand() & 0x7fff;
        while(range<(unsigned long)upperBound && range!=~0UL) {
            range=(range<<15)|0x7fff;
            value=(value<<15)|0x7fff;
        }
        return (int)(value%(unsigned long)upperBound);
    }
};
// FUNCTION: WON 0x006ac6d0
void ServerContext::CopyAddresses(AddrList& inCopy)
{
    AutoCrit aGuard(mLock);
    if (mB08 && !mB09) {
        std::vector<IPAddr> anAddresses;
        anAddresses.reserve(mAddresses.size());
        for (AddrList::iterator anItr=mAddresses.begin();anItr!=mAddresses.end();++anItr)
            anAddresses.push_back(*anItr);
        srand(time(NULL));
        ServerAddressShuffleRandom aRandom;
        std::random_shuffle(anAddresses.begin(),anAddresses.end(),aRandom);
        mPingMap.clear();
        mAddresses.clear();
        std::vector<IPAddr>::iterator aPos;
        aPos=anAddresses.begin();
        while(true) {
            if(!(aPos!=anAddresses.end())) break;
            mAddresses.push_back(*aPos);
            AddrList::iterator aLast=mAddresses.end();
            --aLast;
            std::map<IPAddr,AddrList::iterator>::iterator anEntry=
                mPingMap.insert(std::map<IPAddr,AddrList::iterator>::value_type(
                    *aPos,AddrList::iterator())).first;
            anEntry->second=aLast;
            ++aPos;
        }
        mB09=true;
    }
    inCopy=mAddresses;
}

}

namespace WONAPI {
// FUNCTION: WON 0x006acc00
void ServerContext::RemoveAllAddresses() { Clear(); }
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine complete owner/API address-management algorithms.
namespace WONAPI {
bool ServerContext::RemoveAddress(const IPAddr &theAddr)
{
	AutoCrit aCrit(mLock);
	std::map<IPAddr,AddrList::iterator>::iterator anItr = mPingMap.find(theAddr);
	if(anItr==mPingMap.end())
		return false;

	mAddresses.erase(anItr->second);
	mPingMap.erase(anItr);
	return true;
}

void ServerContext::CopyAddresses(AddrSet &theCopy)
{
	AutoCrit aCrit(mLock);
	theCopy.clear();
	std::map<IPAddr,AddrList::iterator>::iterator aMapItr = mPingMap.begin();
	AddrSet::iterator aSetItr = theCopy.begin();
	while(aMapItr!=mPingMap.end())
	{
		aSetItr = theCopy.insert(aSetItr,aMapItr->first);
		++aMapItr;
	}
}

bool ServerContext::AddAddressesFromDir(const DirEntityMap &theDir, const std::wstring &thePath, const wchar_t *theNameFilter)
{
	DirEntityMap::const_iterator anItr = theDir.find(thePath);
	if(anItr==theDir.end())
		return false;
	else
		return AddAddressesFromDir(anItr->second,theNameFilter);
}

bool ServerContext::AtomicClearAndAddAddressesFromDir(const DirEntityList &theDir, const wchar_t *theNameFilter)
{
	AutoCrit aCrit(mLock);
	Clear();
	return AddAddressesFromDir(theDir, theNameFilter);
}

bool ServerContext::AtomicClearAndAddAddressesFromDir(const DirEntityMap &theDir, const std::wstring &thePath, const wchar_t *theNameFilter)
{
	AutoCrit aCrit(mLock);
	Clear();
	return AddAddressesFromDir(theDir, thePath, theNameFilter);	
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006aeb30
// (public: void __thiscall std::list<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::_Insert(class std::list<class WONAPI::IPAddr,clas)
// TEMPLATE: WON 0x006aebf0
// (protected: struct std::list<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::_Node * __thiscall std::list<class WONAPI::IPAddr,class)
// TEMPLATE: WON 0x006aef70
// (public: class std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class std::list<class WONAPI::IPAddr,class std::allocator<c)
// TEMPLATE: WON 0x006aefe0
// (public: unsigned int __thiscall std::vector<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::capacity(void)const     [?capacity@?$ve)
// TEMPLATE: WON 0x006af010
// (public: unsigned int __thiscall std::vector<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::size(void)const     [?size@?$vector@VIP)
// TEMPLATE: WON 0x006af0b0
// (protected: bool __thiscall std::vector<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::_Buy(unsigned int)    [?_Buy@?$vector@VIPAdd)
// TEMPLATE: WON 0x006af120
// (protected: void __thiscall std::vector<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::_Xlen(void)const     [?_Xlen@?$vector@VIPAdd)
// TEMPLATE: WON 0x006af8c0
// (public: class std::_Ptrit<class WONAPI::IPAddr,int,class WONAPI::IPAddr *,class WONAPI::IPAddr &,class WONAPI::IPAddr *,class WONAPI::IPAddr &> __this)
// TEMPLATE: WON 0x006af8e0
// (public: class std::_Ptrit<class WONAPI::IPAddr,int,class WONAPI::IPAddr *,class WONAPI::IPAddr &,class WONAPI::IPAddr *,class WONAPI::IPAddr &> __this)
// TEMPLATE: WON 0x006af940
// (public: void __thiscall std::list<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::assign(class std::list<class WONAPI::IPAddr,class)
// TEMPLATE: WON 0x006af9f0
// (protected: class WONAPI::IPAddr * __thiscall std::vector<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::_Ucopy(class std::_Ptrit<c)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006afd60
// (void __cdecl std::fill(class std::_Ptrit<class WONAPI::IPAddr,int,class WONAPI::IPAddr *,class WONAPI::IPAddr &,class WONAPI::IPAddr *,class WONAPI::I)
// TEMPLATE: WON 0x006aff00
// (class std::_Ptrit<class WONAPI::IPAddr,int,class WONAPI::IPAddr *,class WONAPI::IPAddr &,class WONAPI::IPAddr *,class WONAPI::IPAddr &> __cdecl std::c)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006af260
// (protected: void __thiscall std::list<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::_Incsize(unsigned int)    [?_Incsize@?$list@VI)
// TEMPLATE: WON 0x006af3b0
// (public: void __thiscall std::vector<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::insert(class std::_Ptrit<class WONAPI::IPAddr,i)
// TEMPLATE: WON 0x006afa30
// (void __cdecl std::_Construct(class WONAPI::IPAddr *,class WONAPI::IPAddr const &)    [?_Construct@std@@YAXPAVIPAddr@WONAPI@@ABV23@@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006ae8f0
// (protected: void __thiscall std::vector<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::_Tidy(void)    [?_Tidy@?$vector@VIPAddr@WONA)
// TEMPLATE: WON 0x006aec70
// (protected: class std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class std::list<class WONAPI::IPAddr,class std::allocato)
// TEMPLATE: WON 0x006af040
// (public: class std::_Ptrit<class WONAPI::IPAddr,int,class WONAPI::IPAddr *,class WONAPI::IPAddr &,class WONAPI::IPAddr *,class WONAPI::IPAddr &> __this)
// TEMPLATE: WON 0x006af0f0
// (protected: void __thiscall std::vector<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::_Destroy(class WONAPI::IPAddr *,class WONAPI)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006ae580
// (public: struct std::pair<class std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class std::list<class WONAPI::IPAddr,class)
// TEMPLATE: WON 0x006aeb90
// (public: class std::list<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::iterator __thiscall std::list<class WONAPI::IPAddr,class st)
// TEMPLATE: WON 0x006afbc0
// (void __cdecl std::_Construct(struct std::pair<class WONAPI::IPAddr const ,class std::list<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAd)
// TEMPLATE: WON 0x006b00e0
// (void __cdecl std::iter_swap(class std::_Ptrit<class WONAPI::IPAddr,int,class WONAPI::IPAddr *,class WONAPI::IPAddr &,class WONAPI::IPAddr *,class WONA)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006af900
// (void __cdecl std::random_shuffle(class std::_Ptrit<class WONAPI::IPAddr,int,class WONAPI::IPAddr *,class WONAPI::IPAddr &,class WONAPI::IPAddr *,class)
// TEMPLATE: WON 0x006b0110
// (void __cdecl std::_Iter_swap(class std::_Ptrit<class WONAPI::IPAddr,int,class WONAPI::IPAddr *,class WONAPI::IPAddr &,class WONAPI::IPAddr *,class WON)
