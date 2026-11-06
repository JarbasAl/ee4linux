// Actual SDK configuration constructor source; support0 only.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLayoutConfigs.h"
// FUNCTION: WON 0x00787fb0
PercentLayoutConfig::PercentLayoutConfig()
{
    int i;
    for(i=0;i<4;++i) refs[i]=NULL;
    for(i=0;i<8;++i) params[i]=-1;
}
// FUNCTION: WON 0x00788260
ColorSchemeConfig::ColorSchemeConfig()
{
    mColorScheme=new WONAPI::ColorScheme;
}
// FontConfig construction remains deferred: the real FontDescriptor default
// constructor zeroing mStyle/mSize is absent from the shared declaration.


/* DEFERRED / ZERO CREDIT: FontConfig::FontConfig, native38a1c0/107B.
   This full unselected draft is retained for later models at owner request.
   Actual SDK FontDescriptor default construction initializes mStyle/mSize to0;
   the current shared declaration lacks that real constructor. The active draft
   therefore omitted native stores8c/90 and is semantically incomplete.
   No field-initialization workaround or shared-header expansion is authorized.
namespace WONAPI {
// DEFERRED SUPPORT-ONLY: EXE 0x0078a1c0 (107B, no active source/metadata credit)
FontConfig::FontConfig()
{
    mHaveInitedComponents=true;
    mNeedRecalcFont=true;
}
}
*/

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00788340
// (public: virtual __thiscall ColorSchemeConfig::~ColorSchemeConfig(void)    [??1ColorSchemeConfig@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00788320
// (public: virtual void * __thiscall ColorSchemeConfig::`scalar deleting destructor'(unsigned int))
