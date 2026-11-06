// Actual dynamic Internet connection probe and genuine function-local DLL owner.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006b09a0
bool AsyncSocket::HasInternetConnection()
{
    typedef BOOL (WINAPI *InternetGetConnectedStateFunc)(LPDWORD,DWORD);
    static InternetGetConnectedStateFunc anInternetGetConnectedStateFunc=NULL;
    static bool haveLoaded=false;
    struct AutoUnloadLibrary
    {
        HMODULE mModule;
        AutoUnloadLibrary() { mModule=NULL; }
        ~AutoUnloadLibrary() { if(mModule!=NULL) FreeLibrary(mModule); }
    };
    static AutoUnloadLibrary anUnload;
    if(!haveLoaded)
    {
        HMODULE aModule=LoadLibraryA("wininet.dll");
        if(aModule!=NULL)
        {
            anUnload.mModule=aModule;
            anInternetGetConnectedStateFunc=(InternetGetConnectedStateFunc)GetProcAddress(aModule,"InternetGetConnectedState");
        }
        haveLoaded=true;
    }
    if(anInternetGetConnectedStateFunc!=NULL)
    {
        DWORD aFlags=0;
        return anInternetGetConnectedStateFunc(&aFlags,0)==TRUE;
    }
    else
        return true;
}
}

// Compiler-generated for this unit (names in analysis/exe/names.txt):
// SYNTHETIC: WON 0x006b0a30
// (_$E45: atexit destructor of HasInternetConnection's static anUnload)
