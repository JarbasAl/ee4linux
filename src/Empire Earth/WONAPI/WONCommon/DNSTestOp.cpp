// RECONSTRUCTION: new native WONAPI class family; exact original filename/grouping unproved.
// Unmatched code/support remains original-carved; no runtime authority.
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
// R3 SCRATCH ONLY. Original DNSTestOp explicit constructor/destructor family.
// Constructor defaults host+address BEFORE assigning host in body, as2d95a0.
// FUNCTION: WON 0x006d95a0
DNSTestOp::DNSTestOp(const std::string& inHost)
{
    mHostString = inHost;
}
// FUNCTION: WON 0x006d9740
DNSTestOp::~DNSTestOp() {}
// RunHook2d97f0 remains native until actualThread1c/DNSThread20 fullgate.

}

namespace WONAPI {
// FUNCTION: WON 0x006d97f0
void DNSTestOp::Virtual04()
{
    DNSTestThread* thread=new DNSTestThread(this);
    thread->SetAutoDelete(true);
    thread->Start();
    AsyncOp::Virtual04();
}
// FUNCTION: WON 0x006d9920
void DNSTestThread::ThreadFunc()
{
    mOp->mHostAddr.SetWithDefaultPort(mOp->mHostString,0);
    if (mOp->mHostAddr.IsValid()) mOp->Fn29F220(0);
    else mOp->Fn29F220(100014);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006d9720
// (protected: virtual void * __thiscall WONAPI::DNSTestOp::`scalar deleting destructor'(unsigned int)    [??_GDNSTestOp@WONAPI@@MAEPAXI@Z])
// TEMPLATE: WON 0x006d98b0
// (public: virtual __thiscall WONAPI::DNSTestThread::~DNSTestThread(void)    [??1DNSTestThread@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006d9890
// (public: virtual void * __thiscall WONAPI::DNSTestThread::`scalar deleting destructor'(unsigned int)    [??_GDNSTestThread@WONAPI@@UAEPAXI@Z])
